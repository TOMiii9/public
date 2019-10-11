#ifndef UTIL_H_
#define UTIL_H_
#include <string>
#include <assert.h>

#include "vec.h"
#include "typedefs.h"

#define map_range(x, A, B, C, D) (((x - A) / (B - A)) * (D - C))

void         report(const char *format, ...);
string_array vector_file(const char *file_name);
std::string  string_file(const char *file_name);
void         split_string(const std::string &str, string_array *out, char delim = ' ');
i32          str_to_i32(const std::string &string, i32 default_value);
f32          str_to_f32(const std::string &string, f32 default_value);
f64          get_time_micro();
i32          get_time_ms();
i32          random_i32(i32 upper_bound);
f32          random_f32();
std::string  left_pad(std::string in, i32 min_len, char padding);
vec3         random_unit_in_disc();
vec3         random_in_unit_sphere();
void         random_seed(uint32_t seed);
void         crash();
vec3         rgb2vec3(i32 r, i32 g, i32 b, f32 intensity);
std::string  stringf(const char *format, ...);
std::wstring string_to_wstring(const std::string &string);
void         take_screenshot(const char *file_name, Array_Of<u8> &color_buffer, i32 w, i32 h);
void         panic(string panic_string);
void         set_frame_time(f32 ft);
f32          get_frame_time();
f32          dampen(f32 source, f32 smoothing, f32 dt);
f32          ease_in(f32 t, f32 offset, f32 height);
f32          ease_out(f32 t, f32 offset, f32 height);
f32          ease_in_out(f32 t, f32 offset, f32 height);
void         check_nan(vec3 &v);
void         run_test_functions();
string &     get_debug_string();
void         set_debug_string(const string &s);
void         append_debug_string(const string &s);
void         request_game_shutdown();
bool         is_game_shutting_down();

template <typename Type>
void array_fill_pattern(Array_Of<Type> &array, i32 n, Array_Of<Type> pattern) {
    while (n-- > 0) {
        array.insert(array.end(), pattern.begin(), pattern.end());
    }
}

template <typename Type>
void Swap(Type &a, Type &b) {
    Type tmp = a;
    a        = b;
    b        = tmp;
}

inline f32 ffmin(f32 x, f32 y) {
    return (x < y ? x : y);
}

inline f32 ffmax(f32 x, f32 y) {
    return (x > y ? x : y);
}

#define RT_UNUSED(x) \
    do {             \
        ((void)x);   \
    } while (0);

#endif
