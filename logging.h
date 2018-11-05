/******************************************************************************
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
******************************************************************************/

#ifndef LOG_H_
#define LOG_H_

#include <Windows.h>

#include <cstdio>
#include <string>
#include <mutex>
#include <fstream>
#include "timer.h"

#include "stringfmt.h"

class log
{    
protected:
    
    HighResTimer    mTimer;

    std::ofstream   mLogFile;
    std::mutex      mLogMutex;

    log()
    {
        //mLogFile.open("debug.txt");    
    }

    static log& it()
    {
        static log theInstance;
        return theInstance;
    }

    void printLine(const std::string &line)
    {
        std::lock_guard<std::mutex> lock{mLogMutex};

        if (mLogFile.is_open())
        {
            mLogFile << line;
            mLogFile.flush();
        }

        OutputDebugStringA(line.c_str());
    }

public:

    template <typename ...Args>
    static void info(const char *fmt, const Args&... args)
    {
        std::string line = {"   [INFO] " + stringf(fmt, args...)};        
        it().printLine(line);
    }

    template <typename ...Args>
    static void error(const char *fmt, const Args&... args)
    {
        std::string line = {"  [ERROR] " + stringf(fmt, args...)};
        it().printLine(line);
    }
};

#endif /*LOG_H_*/
