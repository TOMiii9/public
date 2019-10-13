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

#ifndef REGISTRY_H_
#define REGISTRY_H_

#include <string>
#include <array>

bool reg_load(const char *file_name);
void reg_save(const char *file_name);

void reg_set_string(const char *name, const char *value);
void reg_set_i32(const char *name, int value);
void reg_set_f32(const char *name, float value);
void reg_set_f64(const char *name, double value);

std::string          reg_get_string(const char *name, const char *default_value);
int                  reg_get_i32(const char *name, int default_value);
float                reg_get_f32(const char *name, float default_value);
double               reg_get_f64(const char *name, double default_value);
std::array<float, 3> reg_get_3f32(const char *name);
#endif
