#include "util.h"
#include "timer.h"
#include "windows.h"

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

// static FILE *         g_logFile             = fopen("_debug.txt", "w");
thread_local uint32_t random_state = 1234;
static High_Res_Timer timer;
static bool           already_panicked = false;
static std::mutex     g_reportMutex;
static std::string    g_debug_string_buffer = "init";
static HWND           con_handle            = 0;

inline uint32_t xorshift32() {
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;

    return random_state;
}

void SetConsoleHistoryWndHandle(HWND con) {
    con_handle = con;
}

void report(const char *format, ...) {
    std::lock_guard<std::mutex> theLock(g_reportMutex);

    static char *string_buffer          = NULL;
    static i32   string_buffer_capacity = 0;
    static bool  debugger_present       = !!IsDebuggerPresent();
    int          new_len;

    va_list args1;
    va_start(args1, format);
    new_len = _vscprintf(format, args1) + 1;
    va_end(args1);

    if (new_len > string_buffer_capacity) {
        string_buffer_capacity = new_len;
        string_buffer          = (char *)realloc(string_buffer, string_buffer_capacity);
    }

    va_start(args1, format);
    vsprintf_s(string_buffer, new_len, format, args1);
    va_end(args1);

    if (debugger_present) {
        OutputDebugStringA(string_buffer);
    }

    std::string tmp;
    char *      p = string_buffer;
    while (*p) {
        if (*p == '\n') {
            tmp += "\r\n";
        } else {
            tmp += *p;
        }
        p++;
    }

    if (con_handle) {
        SendMessageA(con_handle, EM_SETSEL, 0, -1);
        SendMessageA(con_handle, EM_SETSEL, -1, -1);
        SendMessageA(con_handle, EM_LINESCROLL, 0, 0xffff);
        SendMessageA(con_handle, EM_SCROLLCARET, 0, 0);
        SendMessageA(con_handle, EM_REPLACESEL, 0, (LPARAM)tmp.c_str());
    }

    // fprintf_s(g_logFile, "%s", stringBuffer.c_str());
    // fflush(g_logFile);
}

String_Array VectorFile(const char *file_name) {
    String_Array  content = {""};
    std::string   line;
    std::ifstream ifs(file_name);

    if (ifs.good()) {
        while (std::getline(ifs, line)) {
            content.emplace_back(line);
        }
    }

    return content;
}

std::string StringFile(const char *file_name) {
#if 1
    std::ifstream ifs(file_name);
    if (!ifs.good()) {
        report("%s could not be found!\n", file_name);
        return "";
    }

    return std::string{(std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())};
#else
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
#endif
}

void SplitString(const std::string &str, String_Array *out, char delim) {
    std::stringstream ss(str);
    std::string       token;

    out->clear();
    while (std::getline(ss, token, delim)) {
        if (token.size()) {
            out->push_back(token);
        }
    }
}

i32 StrToI32(const std::string &string, i32 default_value) {
    i32 return_value = default_value;

    try {
        return_value = std::stoi(string);
    } catch (...) {
    }

    return return_value;
}

f32 StrToF32(const std::string &string, f32 default_value) {
    f32 return_value = default_value;

    try {
        return_value = std::stof(string);
    } catch (...) {
    }

    return return_value;
}

f64 GetTimeMicro() {
    return timer.GetTimeMicro();
}

f64 GetTimeMs() {
    return timer.GetTimeMs();
}

i32 RandomI32(i32 upper_bound) {
    uint32_t s = xorshift32();
    return ((s - 1) % upper_bound);
}

void RandomSeed(uint32_t seed) {
    random_state = seed;
}

f32 RandomF32() {
    uint32_t s = xorshift32();
    return f32(s - 1) / f32(0xFFFFFFFF);
}

std::string LeftPad(std::string in, i32 min_len, char padding) {
    while (in.length() < min_len) {
        in = padding + in;
    }

    return in;
}

vec3 RandomUnitInDisc() {
    vec3 p;
    do {
        p = 2.0f * vec3{RandomF32(), RandomF32(), 0.0f} - vec3{1.0f, 1.0f, 0.0f};
    } while (Vec3Dot(p, p) >= 1.0f);

    return p;
}

vec3 RandomInUnitSphere() {
    vec3 p;
    vec3 x;

    do {
        x.xyz[0] = RandomF32();
        x.xyz[1] = RandomF32();
        x.xyz[2] = RandomF32();

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

std::string StringF(const char *format, ...) {
    static char *string_buffer          = NULL;
    static i32   string_buffer_capacity = 0;
    int          new_len;

    va_list args1;
    va_start(args1, format);
    new_len = _vscprintf(format, args1) + 1;
    va_end(args1);

    if (new_len > string_buffer_capacity) {
        string_buffer_capacity = new_len;
        string_buffer          = (char *)realloc(string_buffer, string_buffer_capacity);
    }

    va_start(args1, format);
    vsprintf_s(string_buffer, new_len, format, args1);
    va_end(args1);

    return string_buffer;
}

std::wstring StringToWstring(const std::string &string) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring                                           wide = converter.from_bytes(string);

    return wide;
}

void TakeScreenshot(const char *file_name, Array_Of<u8> &color_buffer, i32 w, i32 h) {
    std::ofstream text_file;
    text_file.open(file_name);

    report("writing %s ...\n", file_name);
    std::string temp_str = StringF("P3\n%d %d\n255\n", w, h);
    for (i32 i = 0; i < color_buffer.size(); i += 3) {
        // RGB(normal humans) <-> BGR(Win32)
        temp_str += StringF("%d %d %d\n", color_buffer[i + 2], color_buffer[i + 1], color_buffer[i]);
    }

    text_file << temp_str;
    report("done!\n");
}

void Panic(string panic_string) {
    if (already_panicked) {
        return;
    }

    already_panicked = true;
    report("\n\n****** Panic! ******\n\n%s\n", panic_string.c_str());
    MessageBoxA(0, panic_string.c_str(), "System failure", MB_OK + MB_ICONERROR);
    ExitProcess(0);
}

void RunTestCode() {
    sse_f32x4 a;
    sse_f32x4 b;
    sse_f32x4 c;

    a.a4 = _mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f);
    b.a4 = _mm_set_ps(2.0f, 2.0f, 2.0f, 2.0f);

    c.a4 = _mm_mul_ps(a.a4, b.a4);

    __m128 mask = _mm_cmpge_ps(a.a4, _mm_set_ps(4.0f, 4.0f, 4.0f, 4.0f));

    report("%f %f %f %f\n", a.a[0], a.a[1], a.a[2], a.a[3]);
    report("%f %f %f %f\n", b.a[0], b.a[1], b.a[2], b.a[3]);
    report("--------------\n");
    report("%f %f %f %f\n", c.a[0], c.a[1], c.a[2], c.a[3]);

    i32 result = _mm_movemask_ps(mask);

    if (result & 1) {
        report("result & 1\n");
    }

    if (result & 2) {
        report("result & 2\n");
    }

    if (result & 4) {
        report("result & 4\n");
    }

    if (result & 8) {
        report("result & 8\n");
    }
}
