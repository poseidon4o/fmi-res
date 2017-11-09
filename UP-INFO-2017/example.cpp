#define GWINDOW_DEBUG

#include "UI.hpp"


int main()
{
    double input[1000];
    GetInput(input);

    for (int c = 0;  c < 10; c++) {
        SetRaySegment(rand() % (int)input[0], rand() % (int)input[1], rand() % (int)input[0], rand() % (int)input[1]);
        Sleep(100);
    }


    return 0;
}