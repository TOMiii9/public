#include "util.h"
#include "timer.h"
#include "windows.h"
#include "memory.h"
#include "containers.h"

#include <assert.h>
#include <fcntl.h>
#include <io.h>
#include <mutex>
#include <random>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>

static High_Res_Timer        timer;
static thread_local uint32_t random_state       = 1234;
static bool                  already_panicked   = false;
static bool                  shutdown_requested = false;
static f32                   frame_time         = 0.0f;
static string                debug_info         = "";

inline uint32_t xorshift32() {
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;

    return random_state;
}

static std::mutex g_reportMutex;
// static FILE *      g_logFile             = fopen("_debug.txt", "w");
static std::string g_debug_string_buffer = "init";

void report(const char *format, ...) {
    std::lock_guard<std::mutex> theLock(g_reportMutex);

    static std::string stringBuffer;
    static bool        debuggerPresent = !!IsDebuggerPresent();
    static bool        allow_console   = false;
    static bool        console_visible = false;
    va_list            args;
    int                newLen;

    if (allow_console) {
        if (!console_visible) {
            HANDLE stdHandle;
            int    hConsole;
            FILE * fp;

            console_visible = true;

            AllocConsole();

            stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            hConsole  = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
            fp        = _fdopen(hConsole, "w");
            freopen_s(&fp, "CONOUT$", "w", stdout);
        }
    }

    va_start(args, format);
    newLen = _vscprintf(format, args) + 1;

    if (newLen > stringBuffer.capacity()) {
        stringBuffer.resize(newLen, 0);
    }

    vsprintf_s((char *)stringBuffer.c_str(), newLen, format, args);
    va_end(args);

    // printf(string_buffer.c_str());
    if (debuggerPresent) {
        OutputDebugStringA(stringBuffer.c_str());
    }

    if (console_visible) {
        printf(stringBuffer.c_str());
    }

    // fprintf_s(g_logFile, "%s", stringBuffer.c_str());
    // fflush(g_logFile);
}

string_array vector_file(const char *file_name) {
    string_array  content = {""};
    std::string   line;
    std::ifstream ifs(file_name);

    if (ifs.good()) {
        while (std::getline(ifs, line)) {
            content.emplace_back(line);
        }
    }

    return content;
}

std::string string_file(const char *file_name) {
    std::string s;
    FILE *      f;
    u64         fsize;

    fopen_s(&f, file_name, "rb");
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    s.resize(fsize, 0);
    fread(&s[0], sizeof(char), fsize, f);
    fclose(f);

    return s;
}

void split_string(const std::string &str, string_array *out, char delim) {
    std::stringstream ss(str);
    std::string       token;

    out->clear();
    while (std::getline(ss, token, delim)) {
        if (token.size()) {
            out->push_back(token);
        }
    }
}

i32 str_to_i32(const std::string &string, i32 default_value) {
    i32 return_value = default_value;

    try {
        return_value = std::stoi(string);
    } catch (...) {
    }

    return return_value;
}

f32 str_to_f32(const std::string &string, f32 default_value) {
    f32 return_value = default_value;

    try {
        return_value = std::stof(string);
    } catch (...) {
    }

    return return_value;
}

f64 get_time_micro() {
    return timer.get_time_micro();
}

i32 get_time_ms() {
    return i32(timer.get_time_ms());
}

i32 random_i32(i32 upper_bound) {
    uint32_t s = xorshift32();
    return ((s - 1) % upper_bound);
}

void random_seed(uint32_t seed) {
    random_state = seed;
}

f32 random_f32() {
    uint32_t s = xorshift32();
    return f32(s - 1) / f32(0xFFFFFFFF);
}

std::string left_pad(std::string in, i32 min_len, char padding) {
    while (in.length() < min_len) {
        in = padding + in;
    }

    return in;
}

vec3 random_unit_in_disc() {
    vec3 p;
    do {
        p = 2.0f * vec3{random_f32(), random_f32(), 0.0f} - vec3{1.0f, 1.0f, 0.0f};
    } while (vec3_dot(p, p) >= 1.0f);

    return p;
}

vec3 random_in_unit_sphere() {
    vec3 p;
    vec3 x;

    do {
        x.xyz[0] = random_f32();
        x.xyz[1] = random_f32();
        x.xyz[2] = random_f32();

        p = 2.0f * x - vec3{1.0f, 1.0f, 1.0f};
    } while (p.length_squared() >= 1.0f);

    return p;
}

void crash() {
    *((i32 *)1) = 1;
}

vec3 rgb2vec3(i32 r, i32 g, i32 b, f32 intensity) {
    return vec3(map_range(f32(r), 0.0f, 256.0f, 0.0f, 1.0f) * intensity, map_range(f32(g), 0.0f, 256.0f, 0.0f, 1.0f) * intensity,
                map_range(f32(b), 0.0f, 256.0f, 0.0f, 1.0f) * intensity);
}

std::string stringf(const char *format, ...) {
    std::string string_buffer;
    va_list     args;
    int         newLen;

    va_start(args, format);
    newLen = _vscprintf(format, args) + 1;
    va_end(args);

    string_buffer.resize(newLen, 0);

    va_start(args, format);
    vsprintf_s((char *)string_buffer.c_str(), newLen, format, args);
    va_end(args);

    string_buffer.pop_back();

    return string_buffer;
}

std::wstring string_to_wstring(const std::string &string) {
    // std::string                                            narrow = converter.to_bytes(wide_utf16_source_string);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring                                           wide = converter.from_bytes(string);

    return wide;
}

void take_screenshot(const char *file_name, Array_Of<u8> &color_buffer, i32 w, i32 h) {
    std::ofstream text_file;
    text_file.open(file_name);

    report("writing %s ...\n", file_name);
    std::string temp_str = stringf("P3\n%d %d\n255\n", w, h);
    for (i32 i = 0; i < color_buffer.size(); i += 3) {
        // RGB(normal humans) <-> BGR(Win32)
        temp_str += stringf("%d %d %d\n", color_buffer[i + 2], color_buffer[i + 1], color_buffer[i]);
    }

    text_file << temp_str;
    report("done!\n");
}

void panic(string panic_string) {
    if (already_panicked) {
        return;
    }

    already_panicked = true;
    report("\n\n****** Panic! ******\n\n%s\n", panic_string.c_str());

    MessageBoxA(0, panic_string.c_str(), "Error", MB_OK + MB_ICONERROR);

    if (!!IsDebuggerPresent()) {
        while (1) {
            report("\n!!! Debugbreak !!!\n");
            DebugBreak();
            Sleep(250);
        }
    } else {
        ExitProcess(0);
    }
}

void set_frame_time(f32 ft) {
    frame_time = ft;

    if (frame_time > 0.035f) {
        report("***** perf warning *****\n");
        frame_time = 0.035f;
    }
}

f32 get_frame_time() {
    return frame_time;
}

f32 dampen(f32 source, f32 smoothing, f32 dt) {
    return source * powf(smoothing, dt);
}

#define pow2(x) ((x) * (x))
#define pow3(x) ((x) * (x) * (x))

//
// Assuming :
//	t is a value between 0 and 1
//
f32 ease_in(f32 t, f32 offset, f32 height) {
    return height * pow3(t) + offset;
}

f32 ease_out(f32 t, f32 offset, f32 height) {
    return height * (pow3(t - 1.0f) + 1.0f) + offset;
}

f32 ease_in_out(f32 t, f32 offset, f32 height) {
    if ((t * 2.0f) < 1.0f) {
        return height / 2.0f * pow3(t * 2.0f) + offset;
    } else {
        return height / 2.0f * (pow3(t * 2.0f - 2.0f) + 2.0f) + offset;
    }
}

void check_nan(vec3 &v) {
    if (_isnanf(v[0]) || _isnanf(v[1]) || _isnanf(v[2])) {
        report("nan detected\n");
    }
}

struct Alignment_Test {
    i32 integer_1;
    i32 integer_2;
    i32 integer_3;
    u8  char_1;
};

void run_test_functions() {
    /*Linear_Allocator *frame_allocator = get_frame_allocator();
    frame_allocator->reset();

    Temp_Array<i32> int_array(64);

    for (i32 i = 1; i < 65; i++) {
        int_array.append(i);
    }

    for (i32 i = 65; i <= 128; i++) {
        int_array.append(i);
    }

    for (i32 i = 0; i < 100; i++) {
        report("i = %d\n", int_array[i]);
    }

    int_array.clear();
    frame_allocator->reset();

    for (i32 i = 1; i < 65; i++) {
        int_array.append(i);
    }

    for (i32 i = 65; i <= 128; i++) {
        int_array.append(i);
    }

    for (i32 i = 0; i < 100; i++) {
        report("i = %d\n", int_array[i]);
    }*/
}

string &get_debug_string() {
    return debug_info;
}

void set_debug_string(const string &s) {
    debug_info = s;
}

void append_debug_string(const string &s) {
    debug_info += s;
}

void request_game_shutdown() {
    shutdown_requested = true;
}

bool is_game_shutting_down() {
    return shutdown_requested;
}
