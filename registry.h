/******************************************************************************
MIT License

Copyright(c) 2018 tom9099

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef REGISTRY_H_
#define REGISTRY_H_

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

void reg_load(const char *file_name);
void reg_save(const char *file_name);
void reg_set_string(const char *name, const char *value);
void reg_set_int32(const char *name, int value);
void reg_set_float64(const char *name, double value);

int reg_get_int32(const char *name, int default_value);


#ifdef REGISTRY_IMPLEMENTATION

namespace
{
    std::map<std::string, std::string> kv_pairs;

    void string_trim(std::string& s, const char* t = " \t\n\r\f\v")
    {
        s.erase(0, s.find_first_not_of(t));
        s.erase(s.find_last_not_of(t) + 1);
    }

    void string_split(const std::string &s, char delim, std::vector<std::string> &result) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) 
        {
            if (item.length())
            {
                result.push_back(item);
            }
        }
    }

    inline bool file_exists(const char *name) 
    {
        std::ifstream f(name);
        return f.good();
    }

    void sys_error(const char *error)
    {
        MessageBoxA(0, error, "Error", MB_OK);
        abort();
    }

}

void reg_load(const char *file_name)
{
    if (file_exists(file_name))
    {
        std::ifstream in("config.txt");
        std::string str;

        while (std::getline(in, str)) 
        {
            std::vector<std::string> tokens;

            string_trim(str);
            string_split(str, ' ', tokens);

            if (tokens.size() > 1)
            {
                std::string value;

                for (int i = 1; i < tokens.size(); i++)
                {
                    value += tokens[i];
                }

                kv_pairs[tokens[0]] = value;
            }
        }
    }
    else
    {
        // defaults
        reg_set_int32("r_threads", 1);
        reg_save("config.txt");
    }
}

void reg_save(const char *file_name)
{	
    for (auto &pair : kv_pairs)
    {
        std::ofstream f;

        f.open("config.txt");
        f << pair.first << " " << pair.second << "\n";
    }
}

void reg_set_string(const char *name, const char *value)
{
    sys_error("Not implemented reg_set_string");
}

void reg_set_int32(const char *name, int value)
{
    kv_pairs[name] = std::to_string(value);
}

void reg_set_float64(const char *name, double value)
{
    sys_error("Not implemented reg_set_float64");
}

int reg_get_int32(const char *name, int default_value)
{
    auto value = kv_pairs.find(name);
    if (value != kv_pairs.end())
    {
        int ret = atoi(value->second.c_str());
        return ret;
    }
    return default_value;
}

#endif

#endif
