#ifndef HIGHRES_TIMER_H_
#define HIGHRES_TIMER_H_

#include <Windows.h>

struct High_Res_Timer {
    LARGE_INTEGER StartingTime;
    double        Frequency;

    High_Res_Timer() {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        Frequency = double(freq.QuadPart);
        QueryPerformanceCounter(&StartingTime);
    }

    void reset() {
        QueryPerformanceCounter(&StartingTime);
    }

    double get_time_ms() {
        LARGE_INTEGER EndingTime;
        QueryPerformanceCounter(&EndingTime);
        return ((double(EndingTime.QuadPart - StartingTime.QuadPart) * 1000.0) / Frequency);
    }

    double get_time_micro() {
        LARGE_INTEGER EndingTime;
        QueryPerformanceCounter(&EndingTime);
        return ((double(EndingTime.QuadPart - StartingTime.QuadPart) * 1000000.0) / Frequency);
    }

	static High_Res_Timer* it() {
		static High_Res_Timer timer;
		return &timer;
	}

	static double msec() {
		return it()->get_time_ms();
	}

	static double nanosec() {
		return it()->get_time_ms();
	}
};

#endif
