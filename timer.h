#include <Windows.h>

class HighResTimer
{
protected:
    LARGE_INTEGER   StartingTime;    
    LARGE_INTEGER   EndingTime;
    double          Frequency;

public:

    HighResTimer()
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        Frequency = double(freq.QuadPart);
        QueryPerformanceCounter(&StartingTime);
    }

    void reset()
    {
        QueryPerformanceCounter(&StartingTime);
    }

    double getTime()
    {        
        QueryPerformanceCounter(&EndingTime);        
        return ((double(EndingTime.QuadPart - StartingTime.QuadPart) * 1000000.0) / Frequency);
    }

};