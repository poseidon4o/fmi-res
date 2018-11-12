#include <queue>
#include <stack>
#include <vector>
#include <bitset>
#include <algorithm>
#include <cassert>
#include <istream>
#include <sstream>
#include <iostream>
#include <random>
#include <tuple>
#include <fstream>
#define M_ASSERT(x) if(!(x)) __debugbreak;

using namespace std;

struct Request {
	int floor;
	int time;
	enum Dir { Up, Down, None } dir;
};

class Lift {
public:
	Lift(istream & inp, ostream & o)
		: m_time(0)
		, m_floor5(0)
		, m_target(0)
		, m_dir(Request::None)
		, m_doAnimation(false)
		, m_output(o) {
		// read input
		int floorCount, requestCount;
		inp >> floorCount >> requestCount;

		m_floorRequests.resize(floorCount);
		m_stopList.resize(floorCount, false);

		for (int c = 0; c < requestCount; ++c) {
			char rqType[32];
			inp >> rqType;
			Request rq;
			rq.dir = Request::None;
			if (rqType[0] == 'c') {
				// floor request
				char dirStr[8];
				inp >> dirStr;
				rq.dir = dirStr[0] == 'u' ? Request::Up : Request::Down;
			}
			inp >> rq.floor >> rq.time;
			rq.floor -= 1; // go in [0, n-1], add one when printing

			if (rq.dir != Request::None) {
				m_floorRequests[rq.floor].push(rq);
			} else {
				m_liftRequests.push(rq);
			}
		}
	}

	void doAnimation() {
		m_doAnimation = true;
	}

	void run() {
		if (m_doAnimation) {
			cout << "Animation on. You may need to resize console for more floors.\nPress enter to continue:\n";
			getchar();
		}

		print_lift();
		while (!done()) {
			tick();
			++m_time;

			if (m_dir == Request::Up) {
				m_floor5 += 1;
			} else if (m_dir == Request::Down) {
				m_floor5 -= 1;
			}
			print_lift();
		}
	}

private:
	// check if lift has anything else to do
	bool done() const {
		bool done = m_liftRequests.empty() && m_globalQue.empty() && m_dir == Request::None;
		for (int c = 0; c < m_floorRequests.size(); ++c) {
			done = done && m_floorRequests[c].empty();
		}
		return done;
	}

	// Returns where we need to go to reach toFloor
	Request::Dir getRelativeDir(int toFloor) const {
		if (m_floor5 > toFloor * 5) {
			return Request::Down;
		} else if (m_floor5 < toFloor * 5) {
			return Request::Up;
		} else {
			return Request::None;
		}
	}

	// check if the request can be compleeted with current target in mind
	bool isCompletable(const Request & rq) {
		bool okay = false;
		switch (m_dir) {
		case Request::Up: okay = m_floor5 <= rq.floor * 5; break;
		case Request::Down: okay = m_floor5 >= rq.floor * 5; break;
		}

		if (!okay) {
			return false;
		}

		if (m_dir == Request::Up) {
			return m_floor5 <= rq.floor * 5 && rq.floor <= m_target;
		} else if (m_dir == Request::Down) {
			return m_floor5 >= rq.floor * 5 && rq.floor >= m_target;
		}

		return true;
	}

	// check if request is before current time
	bool isTimeReached(const Request & rq) const {
		return rq.time <= m_time;
	}

	// check if lift is exactly at some floor
	bool getAtFloor() const {
		return m_floor5 % 5 == 0;
	}

	// get floor as int
	int getIntFloor() const {
		M_ASSERT(getAtFloor());
		return m_floor5 / 5;
	}

	void print_lift() {
		if (!m_doAnimation) {
			return;
		}

		const int floorCount = m_floorRequests.size();
		for (int c = 0; c < floorCount * 5; ++c) {
			if ((c - 1) % 5 == 0 && c / 5 >= 10) {
				continue;
			} else if (c % 5 == 0) {
				cout << c / 5;
			} else if ((c + 2) % 5 == 0 && m_stopList[(c - 3) / 5]) {
				cout << '@';
			} else if ((c + 1) % 5 == 0) {
				cout << '|';
			} else {
				cout << ' ';
			}
		}
		cout << endl;
		for (int c = 0; c < floorCount * 5; ++c) {
			if (c == m_floor5) {
				switch (m_dir) {
				case Request::Up: cout << ">"; break;
				case Request::Down: cout << "<"; break;
				case Request::None: cout << "X"; break;
				}
			} else {
				cout << " ";
			}
		}
		cout << endl;
	}

	// simulate what happens in current time
	void tick() {
		const bool atFloor = getAtFloor();

		// check for requests from inside lift
		while (!m_liftRequests.empty() && isTimeReached(m_liftRequests.front())) {
			Request & rq = m_liftRequests.front();
			if (isCompletable(rq)) {
				m_stopList[rq.floor] = true;
			} else {
				m_globalQue.push_back(rq);
			}

			m_liftRequests.pop();
		}

		// check for requests from all floors and get only completable ones
		for (int c = 0; c < m_floorRequests.size(); ++c) {
			while (!m_floorRequests[c].empty() && isTimeReached(m_floorRequests[c].front())) {
				Request & rq = m_floorRequests[c].front();

				if (isCompletable(rq)) {
					m_stopList[rq.floor] = true;
				} else {
					m_globalQue.push_back(rq);
				}

				m_floorRequests[c].pop();
			}
		}

		// check if we need to stop
		if (atFloor && m_stopList[getIntFloor()]) {
			const int floor = getIntFloor() + 1; // floors are in [1, n]
			m_output << m_time << " " << floor << " " << (m_dir == Request::Up ? "up" : "down") << endl;
			// lets clear it here
			m_stopList[getIntFloor()] = false;
		}

		// we reached target - find new one
		if (atFloor && getIntFloor() == m_target) {
			m_dir = Request::None;
			// there should be nothing in stop list since we should only stop at floors between current floor and target
			M_ASSERT(std::none_of(m_stopList.begin(), m_stopList.end(), [](auto x) { return x; }));

			// pick next request
			if (!m_globalQue.empty() && isTimeReached(m_globalQue.front())) {
				m_target = m_globalQue.front().floor;
				m_stopList[m_target] = true;
				m_dir = getRelativeDir(m_target);
				m_globalQue.pop_front();

				// get all floors that we can stop at with current target
				// and also remove these requests from global que
				auto del = std::remove_if(m_globalQue.begin(), m_globalQue.end(), [this](const Request & rq) {
					if (isTimeReached(rq) && isCompletable(rq)) {
						m_stopList[rq.floor] = true;
						return true;
					} else {
						return false;
					}
				});
				m_globalQue.erase(del, m_globalQue.end());
			} else {
				// nothing to do :(
			}
		}

		M_ASSERT(m_dir == getRelativeDir(m_target));
		M_ASSERT(m_floor5 >= 0 && m_floor5 < m_floorRequests.size() * 5);
	}

private:
	// que of requests per floor
	vector<queue<Request>> m_floorRequests;
	// requests from inside lift
	queue<Request>         m_liftRequests;
	// all request that have actually happened (request time <= current time)
	deque<Request>         m_globalQue;

	// all floors that we should stop, includes target floor
	vector<bool>           m_stopList;
	// current floor multiplied by 5 (to avoid float floor because of speed 0.2 floors/second)
	int                    m_floor5;

	// current second
	int                    m_time;
	// floor lift is going to
	int                    m_target;
	// direction, can be inferred from current floor and targed, kept for consistency checks
	Request::Dir           m_dir;

	// flag to enabl/disable verbose print
	bool                   m_doAnimation;

	ostream                &m_output;
};

stringstream generate(int floors, int requests, bool dense = false) {
	default_random_engine eng(42);
	uniform_int_distribution<int> chance(0, 100);
	uniform_int_distribution<int> rfloor(1, floors);

	stringstream str;
	str << floors << ' ' << requests << endl;
	int time = 0;
	for (int c = 0; c < requests; ++c) {
		if (dense) {
			// avg of 3 requests per floor
			if (chance(eng) < 33) {
				++time;
			}
		} else {
			// average of 1 request per 3 floors
			++time;
			if (chance(eng) < 33) {
				--c; // we skip iteration and c keeps track of request count
				continue;
			}
		}

		if (chance(eng) < 50) {
			str << "call " << (chance(eng) < 50 ? "up " : "down ") << rfloor(eng) << " " << time << endl;
		} else {
			str << "go " << rfloor(eng) << " " << time << endl;
		}
	}
	return str;
}


int main() {
	tuple<int, int, bool> arg[] = {
		make_tuple(1, 10, false),
		make_tuple(10, 1, false),
		make_tuple(10, 10, false),
		make_tuple(100, 10, false),
		make_tuple(100, 100, true),
		make_tuple(10, 1000, false),
		make_tuple(100, 1000, true),
	};

	for (auto & a : arg) {
		const string baseName = to_string(get<0>(a)) + "-" + to_string(get<1>(a)) + "-" + to_string(get<2>(a));
		const string base = "D:/hw-test/";
		const string input = base + baseName + "-input.txt";
		const string output = base + baseName + "-output.txt";
		auto inData = generate(get<0>(a), get<1>(a), get<2>(a));

		ofstream inputFile(input.c_str(), ios::trunc);
		inputFile << inData.str();
		inputFile.close();

		ofstream outputFile(output.c_str(), ios::trunc);
		Lift(inData, outputFile).run();
	}
}