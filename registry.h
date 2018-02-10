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

void reg_load();
void reg_save();
void reg_set_string(const char *name, const char *value);
void reg_set_int32(const char *name, int value);
void reg_set_float64(const char *name, double value);

#ifdef REGISTRY_IMPLEMENTATION

namespace
{
    std::map<std::string, std::string> kv_pairs;

}

void reg_load(const char *file_name)
{
	
}

void reg_save(const char *file_name)
{	

}

void reg_set_string(const char *name, const char *value)
{
	
}

void reg_set_int32(const char *name, int value)
{

}

void reg_set_float64(const char *name, double value)
{

}

#endif

#endif
