#pragma once

#define _CRT_SECURE_NO_WARNINGS

// for g++/codeblocks
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <Windows.h>
#include <Windowsx.h>
#include <tchar.h>

#include <cstdio>

#include <vector>
#include <random>
#include <cassert>

#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>

// TODO: move all cond var notifies out of locks

namespace __gameInternal {

#ifdef GWINDOW_DEBUG
    #define gassert(test) assert(test);
#else
    #define gassert(test)
#endif

/// Represents point with real coordinates
struct Point {
    double x;
    double y;
    Point(double x, double y): x(x), y(y) {}
    Point() {}
};

/// Used to avoid casts everywhere Point needs to be passed to WIN API
struct IPoint {
    int x;
    int y;
    explicit IPoint(const Point & p): x(static_cast<int>(p.x)), y(static_cast<int>(p.y)) {}
};

/// Represnts line withr eal coordinates
struct Line {
    Point from;
    Point to;
    Line(double fromX, double fromY, double toX, double toY): from(fromX, fromY), to(toX, toY) {}
    Line(Point from, Point to): from(from), to(to) {}
    Line() {}
};

/// Game settings grouped in struct
struct GameConfig {
    int width;
    int height;
    Point target;
    double targetRadius;
    int maxReflect;
    int mirrorCount;
    std::vector<Line> mirrors;

    static bool readConfig(GameConfig & cfg, const char * cfgFileName) {
        auto confFile = std::shared_ptr<FILE>(fopen(cfgFileName, "r"), [](FILE *file) {
            if (file) {
                fclose(file);
            }
        });
        gassert(confFile.get() && "Missing config file!");
        if (!confFile) {
            puts("Config file is not found, it must be in working directory!");
            return false;
        }

#define readFromCofig(dest, format)\
        if (1 != fscanf(confFile.get(), format " ", dest)) {\
            puts("Failed to read" #dest " from config.");\
            return false;\
        }

        readFromCofig(&cfg.width, "%d");
        readFromCofig(&cfg.height, "%d");

        readFromCofig(&cfg.target.x, "%lf");
        readFromCofig(&cfg.target.y, "%lf");

        readFromCofig(&cfg.targetRadius, "%lf");

        readFromCofig(&cfg.maxReflect, "%d");
        readFromCofig(&cfg.mirrorCount, "%d");

        if (cfg.mirrorCount < 0) {
            puts("Mirrors can't be less than 0");
            return false;
        }

        cfg.mirrors.resize(cfg.mirrorCount);

        for (int c = 0; c < cfg.mirrorCount; c++) {
            auto & mirror = cfg.mirrors[c];

            readFromCofig(&mirror.from.x, "%lf");
            readFromCofig(&mirror.from.y, "%lf");

            readFromCofig(&mirror.to.x, "%lf");
            readFromCofig(&mirror.to.y, "%lf");
        }

#undef readFromCofig

        return true;
    }
};

/// Prototype for UI callbacks from windows
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/// Main application window, holds game state, game config, and UI thread
/// *MUST* call waitClose before dtor!
class GWindow {
    enum UiThreadState {
        UNINITIALIZED, FAILED, RUNNING, STOPPED
    };
public:
    GWindow(const GameConfig & conf, HINSTANCE instance)
        : m_inputReady(false)
        , m_userInputIdx(0)
        , m_config(conf)
        , m_uiState(UNINITIALIZED)
        , m_handle(nullptr)
        , m_instance(instance)
    {}

    ~GWindow() {
        // we can't abandon threads!
        gassert(!m_uiThread.joinable() && "Must call .waitClose() before exit");
    }

    // init the the UI (thread and window)
    // return - true on success, false otherwise
    bool init() {
        m_uiState = UiThreadState::UNINITIALIZED;

        m_uiThread = std::thread(&GWindow::uiLoop, this);

        UiThreadState result;
        // block until UI is up - not strictly needed but just in case
        {
            std::unique_lock<std::mutex> uiThreadLock(m_uiRunningMtx);
            if (m_uiState == UiThreadState::UNINITIALIZED) {
                m_uiRunningCondVar.wait(uiThreadLock, [this]() {
                    return m_uiState != UiThreadState::UNINITIALIZED;
                });
            }
            // read inside lock
            result = m_uiState;
        }

        return result == UiThreadState::RUNNING;
    }

    // Call to fill the array with game config and user input
    // Blocks until user clicks 2 times on the screen
    // If user has closed the window before providing input, this will just return
    // without filling the provided buffer
    void readGameData(double * data) const {
        {
            std::unique_lock<std::mutex> readyLock(m_readyMtx);
            if (!m_inputReady) {
                m_readyCondVar.wait(readyLock, [this]() {
                    return this->m_inputReady;
                });
            }
        }

        std::unique_lock<std::mutex> uiLock(m_uiRunningMtx);
        // dont read anything if user closed window
        if (m_uiState != UiThreadState::RUNNING) {
            return;
        }

        // since we will
        const auto & mirrors = m_config.mirrors;
        int outIdx = 0;
        auto push = [data, &outIdx](double value) {
            data[outIdx++] = value;
        };

        push(m_config.width);
        push(m_config.height);

        push(m_config.target.x);
        push(m_config.target.y);
        push(m_config.targetRadius);

        // ray start
        push(m_userInput[0].x);
        push(m_userInput[0].y);
        // ray end
        push(m_userInput[1].x);
        push(m_userInput[1].y);

        push(m_config.maxReflect);
        push(m_config.mirrorCount);

        for (const auto & mirror : mirrors) {
            push(mirror.from.x);
            push(mirror.from.y);

            push(mirror.to.x);
            push(mirror.to.y);
        }
    }

    // Add user created line (part of the ray)
    void writeLine(Line line) {
        {
            std::lock_guard<std::mutex> lock(m_userLinesMtx);
            m_userLines.push_back(line);
        }

        {
            std::lock_guard<std::mutex> uiLock(m_uiRunningMtx);
            if (m_uiState == UiThreadState::RUNNING) {
                const auto postRes = PostMessage(m_handle, WM_PRINTCLIENT, (WPARAM)GetDC(m_handle), 0);
                gassert(postRes != 0 && "Failde to PostMessage on writeLine");
            }
        }
    }

    // Handles message for UI (paint, quit and left click)
    LRESULT handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_DESTROY: {
                {
                    std::lock_guard<std::mutex> uiLock(m_uiRunningMtx);
                    m_uiState = UiThreadState::STOPPED;
                    m_uiRunningCondVar.notify_all();
                }
                PostQuitMessage(0);
                return 0;
            }
            case WM_PRINTCLIENT: {
                HDC hdc = reinterpret_cast<HDC>(wParam);
                RECT clRect;
                GetClientRect(m_handle, &clRect);
                FillRect(hdc, &clRect, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
                drawGame(hdc);
                return 0;
            }
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);

                FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
                drawGame(hdc);

                EndPaint(hwnd, &ps);
                return 0;
            }
            case WM_LBUTTONDOWN: {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                addUserInput(xPos, yPos);
                return 0;
            }
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    // waits until the window is cloes and joins the UI thread
    void waitClose() {
        {
            std::unique_lock<std::mutex> uiLock(m_uiRunningMtx);
            if (m_uiState != UiThreadState::STOPPED) {
                m_uiRunningCondVar.wait(uiLock, [this]() {
                    return m_uiState == UiThreadState::STOPPED;
                });
            }
        }
        gassert(m_uiThread.joinable() && "UI Thread is not joinable!");
        m_uiThread.join();
    }

private:
    // register user click (ignores calls after the second one - only 2 click required)
    void addUserInput(int x, int y) {
        if (m_userInputIdx >= 2) {
            return;
        }
        m_userInput[m_userInputIdx++] = Point(x, y);
        if (m_userInputIdx == 2) {
            std::lock_guard<std::mutex> inputLock(m_readyMtx);
            m_inputReady = true;
            m_readyCondVar.notify_all();
        }
    }

    // draw all object in the window (mirrors, target, user lines)
    void drawGame(HDC hdc) const {
        auto original = SelectObject(hdc, GetStockObject(DC_PEN));
        gassert(original != nullptr && original != HGDI_ERROR && "Failed to select DC_PEN object");
        auto originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
        gassert(originalBrush != nullptr && originalBrush != HGDI_ERROR && "Failed to select DC_BRUSH object");

        auto prevColor = SetDCPenColor(hdc, RGB(0, 0, 0));
        gassert(CLR_INVALID != prevColor && "SetDCPenColor failed to set black color");
        BOOL apiR;
        // mirrors
        for (const auto & line : m_config.mirrors) {
            const IPoint from(line.from), to(line.to);
            apiR = MoveToEx(hdc, from.x, from.y, nullptr);
            gassert(apiR && "Failed MoveToEx for mirror");
            apiR = LineTo(hdc, to.x, to.y);
            gassert(apiR && "Failed to LineTo for mirror");
        }

        // target as green filled circle
        prevColor = SetDCPenColor(hdc, RGB(0, 255, 0));
        gassert(CLR_INVALID != prevColor && "SetDCPenColor failed to set red color");
        prevColor = SetDCBrushColor(hdc, RGB(0, 255, 0));
        gassert(CLR_INVALID != prevColor && "SetDCBrushColor failed to set red color");
        const IPoint topLeft(Point(m_config.target.x - m_config.targetRadius, m_config.target.y - m_config.targetRadius));
        const IPoint rightBot(Point(m_config.target.x + m_config.targetRadius, m_config.target.y + m_config.targetRadius));
        apiR = Pie(hdc,
            topLeft.x, topLeft.x, rightBot.x, rightBot.y,
            topLeft.x, topLeft.x, topLeft.x, topLeft.x);
        gassert(apiR && "Failed Pie for target");


        prevColor = SetDCPenColor(hdc, RGB(255, 0, 0));
        gassert(CLR_INVALID != prevColor && "SetDCPenColor failed to set red color");

        {
            std::lock_guard<std::mutex> inputLock(m_readyMtx);
            if (m_inputReady) {
                // ray start
                const IPoint from(m_userInput[0]), to(m_userInput[1]);
                apiR = MoveToEx(hdc, from.x, from.y, nullptr);
                gassert(apiR && "Failed MoveToEx for user line");
                apiR = LineTo(hdc, to.x, to.y);
                gassert(apiR && "Failed to LineTo for user line");
            }
        }

        // user-created ray parts
        {
            std::lock_guard<std::mutex> lineLock(m_userLinesMtx);
            for (const auto & line : m_userLines) {
                const IPoint from(line.from), to(line.to);
                apiR = MoveToEx(hdc, from.x, from.y, nullptr);
                gassert(apiR && "Failed MoveToEx for user line");
                apiR = LineTo(hdc, to.x, to.y);
                gassert(apiR && "Failed to LineTo for user line");
            }
        }

        SelectObject(hdc, original);
        SelectObject(hdc, originalBrush);
    }

    // THE ui loop, will signal when set up, so init can return
    // before returning will signal UI cond var, and input cond var
    void uiLoop() {
        using namespace std::chrono;
        bool success = true;
        const _TCHAR CLASS_NAME[] = TEXT("Dummy Window Class");

        WNDCLASS wc = {};

        wc.lpfnWndProc = WindowProc;
        wc.hInstance = m_instance;
        wc.lpszClassName = CLASS_NAME;

        auto atom = RegisterClass(&wc);
        gassert(atom && "Failed to RegisterClass UI class");

        const int w = m_config.width;
        const int h = m_config.height;

        const int clientW = GetSystemMetrics(SM_CXSCREEN);
        const int clientH = GetSystemMetrics(SM_CYSCREEN);
        const int offsetX = clientW / 2 - w / 2;
        const int offsetY = clientH / 2 - h / 2;

        m_handle = CreateWindowEx(0, CLASS_NAME, TEXT("Mirrors"),
            WS_VISIBLE | WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
            offsetX, offsetY, w, h, nullptr, nullptr, m_instance, nullptr
        );

        if (!m_handle) {
            printf("Failed to create main window %d", GetLastError());
            success = false;
        }

        {
            std::lock_guard<std::mutex> uiLock(m_uiRunningMtx);
            if (!success) {
                m_uiState = UiThreadState::FAILED;
            } else {
                m_uiState = UiThreadState::RUNNING;
            }
            m_uiRunningCondVar.notify_all();
        }

        MSG msg = {};
        BOOL getMsg;
        while ((getMsg = GetMessage(&msg, nullptr, 0, 0)) != 0) {
            if (getMsg == -1) {
                printf("GetMessage returned -1, error: %d\n", GetLastError());
            } else {
                /*auto res = */
                TranslateMessage(&msg);
                // TODO: should this return 0 most of the time!?
                // gassert(res && "Failed to TranslateMessage");
                // return value of DispatchMessage is "generally ignored"
                DispatchMessage(&msg);
            }
        }

        {
            std::lock_guard<std::mutex> uiLock(m_uiRunningMtx);
            m_uiState = UiThreadState::STOPPED;
            m_uiRunningCondVar.notify_all();
        }

        // user could be blocked in readGameData call
        {
            std::lock_guard<std::mutex> inputLock(m_readyMtx);
            m_inputReady = true;
            m_readyCondVar.notify_all();
        }
    }
private:
    Point m_userInput[2]; // user's two mouse clicks inside the window
    int m_userInputIdx; // counts the clicks of the user
    bool m_inputReady; // false until user clicks two times in game window
    mutable std::mutex m_readyMtx; // mutex protecting m_inputReady
    mutable std::condition_variable m_readyCondVar; // used to block on readGameData if user has not yet filled input (2 clicks on window)

    const GameConfig m_config; // the config for the game
    mutable std::mutex m_userLinesMtx; // used to lock user lines
    std::vector<Line> m_userLines; // the lines created by the user

    UiThreadState m_uiState; // false until UI thread is running
    mutable std::mutex m_uiRunningMtx; // protects m_uiThreadRunning
    std::condition_variable m_uiRunningCondVar; // used to block until UI thread starts running

    HWND m_handle; // the draw window handle
    std::thread m_uiThread; // the thread running the UI calls
    HINSTANCE m_instance; // the instance of the program
};

// holds pointer to the window created by the main function, used in bare functions API
static GWindow * _window;

inline LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return _window->handleMessage(hwnd, uMsg, wParam, lParam);
}

} // namespace __gameInternal

// TODO: figure out how to handle main(void) and main(int, char **)
// prototype for the user function
int _user_main();

// g++ will not link this if it is inline
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, _TCHAR *, int) {
    const char * CONFIG_FILE = "game.cfg";
    __gameInternal::GameConfig config;

    if (!__gameInternal::GameConfig::readConfig(config, CONFIG_FILE)) {
        return -1;
    }

    __gameInternal::GWindow win(config, hInstance);
    __gameInternal::_window = &win;
    if (!win.init()) {
        return -1;
    }

    int userResult = _user_main();
    win.waitClose();

    return userResult;
}

// do this so we avoid entry point linking issues
#define main _user_main


inline void GetInput(double * data) {
    __gameInternal::_window->readGameData(data);
}

inline void SetRaySegment(double fromX, double fromY, double toX, double toY) {
    __gameInternal::_window->writeLine(__gameInternal::Line(fromX, fromY, toX, toY));
}

// dont pollute user's code
#undef gassert
#undef _CRT_SECURE_NO_WARNINGS
