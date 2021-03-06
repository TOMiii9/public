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

#ifndef STRINGF_H_
#define STRINGF_H_

#include <string>
#include <assert.h>
#include <stdint.h>

struct ConcreteArgType
{
    enum InternalType
    {
        FLOAT,
        DOUBLE,
        INT32,
        INT64,
        CHAR,
        STR
    } type;

    union UValue
    {
        __int64 int64_value;
        float fl_value;
        double dbl_value;
        int int32_value;
        char chr_value;       
    };

    std::string str_value;
    UValue value;

    ConcreteArgType(float fl) : type(FLOAT)
    {  
        value.fl_value = fl;
    }

    ConcreteArgType(double dbl) : type(DOUBLE)
    {
        value.dbl_value = dbl;
    }
    
    ConcreteArgType(int i32) : type(INT32)
    {
        value.int32_value = i32;
    }
	
	ConcreteArgType(char ch) : type(CHAR)
    {
        value.chr_value = ch;
    }

    ConcreteArgType(const char *str) : type(STR)
    {
        str_value = str;
    }

    ConcreteArgType(__int64 i64) : type(INT64)
    {
        value.int64_value = i64;
    }

    ~ConcreteArgType()
    {
    }
};

template <typename Type, std::size_t N>
constexpr std::size_t arraySize(const Type (&)[N])
{
    return N;
}

template <typename ...Args>
std::string stringf(const char *fmt, const Args&... args)
{
    std::string     ret             = {};    
    size_t          size            = strlen(fmt);
    int             argCounter      = 0;
    char            *pFmt           = const_cast<char *>(fmt);        
    ConcreteArgType argArray[]      = {args...};    

    // ret.reserve(512);
    while (*pFmt)
    {
        if (*pFmt == '/' &&  pFmt[1] == '%')
        {
            if (argCounter >= arraySize(argArray))
            {
                assert("check those parameters" && 0);
                break;
            }
            
            const ConcreteArgType &arg = argArray[argCounter];
            
            switch (arg.type)
            {
            case ConcreteArgType::FLOAT:
                ret.append(std::to_string(arg.value.fl_value));                
                break;
            case ConcreteArgType::DOUBLE:
                ret.append(std::to_string(arg.value.dbl_value));
                break;
            case ConcreteArgType::INT32:
                ret.append(std::to_string(arg.value.int32_value));
                break;
            case ConcreteArgType::CHAR:
                ret.append(std::to_string(arg.value.chr_value));
                break;
            case ConcreteArgType::STR:
                ret.append(arg.str_value);
                break;
            case ConcreteArgType::INT64:
                ret.append(std::to_string(arg.value.int64_value));
                break;

            default:
                assert("unknown arg type in stringf" && 0);
                break;
            }

            pFmt++;
            argCounter++;
        }
        else if (*pFmt == '/' &&  pFmt[1] == '/')
        {
            ret += '/';            
            pFmt++;
        }
        else
        {
            ret += *pFmt;
        }
        
        pFmt++;
    }

    return ret;
}


std::string stringf(const char *fmt);

#if STRINGF_IMPLEMENTATION

std::string stringf(const char *fmt)
{
    return fmt;
}

#endif

#endif
