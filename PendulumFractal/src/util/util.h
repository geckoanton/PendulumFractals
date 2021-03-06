#pragma once
#include "../simulator/precisionType.h"

#include <thread>
#include <chrono>
#include <random>
#include <iostream>
#include <mutex>
#include <string>
#include <Windows.h>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <streambuf>
#include <sstream>

const PREDEC PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062;

//warning! may sleep longer than expected
inline void sleep(long long int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//get number of milliseconds since 1970
inline long long int getTimeSinceEpoc() {
	return(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

class FpsTracker {
public:
	FpsTracker(int t_fps) { fps = t_fps; start_time = getTimeSinceEpoc(); }
	unsigned long long nextUpdate() { return ((1000 * update_count + 1) / fps) + start_time + paused_time; }
	bool isUpdateTime() { return (getTimeSinceEpoc() > nextUpdate() && pause_start_time == 0); }
	void waitToNextUpdate() { sleep(getTimeSinceEpoc() - nextUpdate()); }
	void pause() { pause_start_time = getTimeSinceEpoc(); }
	void unpause() { paused_time += getTimeSinceEpoc() - pause_start_time; pause_start_time = 0; }
	unsigned int update_count = 0;
	unsigned long long start_time;
	unsigned long long pause_start_time = 0;
	unsigned long long paused_time = 0;
private:
	FpsTracker();
	unsigned int fps;
};

// For asking the user for input, returns what the user responds with
inline std::string askUser(std::string question) {
	std::cout << question;
	std::string answer;
	std::cin >> answer;
	return answer;
}

template<class T>
std::string replaceDots(T input) {
	std::string return_string = std::to_string(input);
	std::replace(return_string.begin(), return_string.end(), '.', ',');
	return return_string;
}

// For deciding where you want to output things
class PrintOutput : public std::ostream {
public:
	PrintOutput(std::string t_output_file = "") : std::ostream(NULL), output_file(t_output_file) {}


	template<class T>
	PrintOutput& operator<<(const T& obj) {
		// write obj to stream
		stream << obj;
		return *this;
	}
	void printOnScreen() {
		// Print what is in stream to the console
		std::cout << stream.str();
		// Print to file
		if (output_file != "") {
			std::ofstream outfile;
			outfile.open(output_file, std::ios_base::app); // append instead of overwrite
			outfile << stream.str();
		}
		// Remove everything in stream
		stream.str("");
	}
	std::string output_file;
private:
	std::ostringstream stream = std::ostringstream();
};


namespace rand_ {
	// Better random number generator
	typedef std::mt19937 rng_type_32;
	typedef std::mt19937_64 rng_type_64;

	// Local thread random generation
	thread_local static std::random_device thread_rd;
	thread_local static rng_type_64 thread_rng(thread_rd());

	inline void localThreadSetSeed(unsigned long long seed) {
		thread_rng.seed(seed);
	}
}

#define rand__randomNumber rand_::thread_rng()
#define rand__seedUsingCurrentTime rand_::localThreadSetSeed(getTimeSinceEpoc())

thread_local extern double added_angle_x;
thread_local extern double added_angle_y;

thread_local extern int shifted_graph_x;
thread_local extern int shifted_graph_y;