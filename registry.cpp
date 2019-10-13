#include "registry.h"

#include <Windows.h>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <vector>
#include "tokenizer.h"
#include "util.h"

namespace {
std::map<std::string, std::string> kv_pairs;
std::shared_mutex                  registry_mutex;
} // namespace

static void parse_block(nano_lexer &lexer) {
    std::string current_build;
    std::string block_type;

#ifdef _DEBUG
    current_build = "debug";
#else
    current_build = "release";
#endif

    nano_lexer::token token;
    if (lexer.expect_token(token, nano_lexer::Token_String)) {
        block_type = token.text;
        if (lexer.expect_token(token, nano_lexer::Token_Open_Curly_Brace)) {
            while (lexer.state.ok) {
                token = lexer.get_token();
                switch (token.type) {
                    case nano_lexer::Token_Identifier: {
                        std::string var_name = token.text;
                        if (lexer.expect_token(token, nano_lexer::Token_Equal)) {
                            if (lexer.expect_token(token, nano_lexer::Token_String)) {
                                std::string var_value = token.text;
                                // report("cfg_token: %s -> %s\n", var_name.c_str(), var_value.c_str());
                                kv_pairs[var_name] = var_value;
                            } else {
                                lexer.error("parse_block: missing value after '='");
                            }
                        } else {
                            lexer.error("parse_block: missing '='");
                        }
                    } break;

                    case nano_lexer::Token_Close_Curly_Brace:
                        // finished with this block
                        return;

                    default:
                        lexer.error("parse_block: unexpected token");
                }
            }
        } else {
            lexer.error("opening curly brace is missing");
        }
    } else {
        lexer.error("config 'platform' expected");
    }
}

bool reg_load(const char *file_name) {
    std::unique_lock registry_lock(registry_mutex);
    nano_lexer       lexer;

    lexer.from_file(file_name);
    while (lexer.state.ok) {
        nano_lexer::token token = lexer.get_token();

        if (token.type == nano_lexer::Token_End) {
            return true;
        }

        if (token.type == nano_lexer::Token_Identifier && token.text == "config") {
            parse_block(lexer);
        }
    }

    return false;
}

void reg_save(const char *file_name) {
	*(int*)1 = 0; // not implemented
}

void reg_set_string(const char *name, const char *value) {
    std::unique_lock registry_lock(registry_mutex);

    kv_pairs[name] = value;
}

void reg_set_i32(const char *name, int value) {
    std::unique_lock registry_lock(registry_mutex);

    kv_pairs[name] = std::to_string(value);
}

void reg_set_f32(const char *name, float value) {
    std::unique_lock registry_lock(registry_mutex);

    kv_pairs[name] = std::to_string(value);
}

void reg_set_f64(const char *name, double value) {
    std::unique_lock registry_lock(registry_mutex);

    kv_pairs[name] = std::to_string(value);
}

std::string reg_get_string(const char *name, const char *default_value) {
    std::shared_lock registry_lock(registry_mutex);

    auto value = kv_pairs.find(name);
    if (value != kv_pairs.end()) {
        return value->second;
    }
    return default_value;
}

int reg_get_i32(const char *name, int default_value) {
    std::shared_lock registry_lock(registry_mutex);

    auto value = kv_pairs.find(name);
    if (value != kv_pairs.end()) {
        int ret = atoi(value->second.c_str());
        return ret;
    }
    return default_value;
}

float reg_get_f32(const char *name, float default_value) {
    std::shared_lock registry_lock(registry_mutex);

    auto value = kv_pairs.find(name);
    if (value != kv_pairs.end()) {
        float ret = std::stof(value->second.c_str());
        return ret;
    }
    return default_value;
}

std::array<float, 3> reg_get_3f32(const char *name) {
    std::shared_lock registry_lock(registry_mutex);

    std::array<float, 3> ret{0.0f, 0.0f, 0.0f};
    auto                 value = kv_pairs.find(name);

    if (value != kv_pairs.end()) {
        string_array out;
        split_string(value->second, &out, ' ');

        if (out.size() == 3) {
            ret[0] = float(atof(out[0].c_str()));
            ret[1] = float(atof(out[1].c_str()));
            ret[2] = float(atof(out[2].c_str()));
        }

        return ret;
    }
    return ret;
}

double reg_get_f64(const char *name, double default_value) {
    std::shared_lock registry_lock(registry_mutex);

    auto value = kv_pairs.find(name);
    if (value != kv_pairs.end()) {
        double ret = std::stod(value->second.c_str());
        return ret;
    }
    return default_value;
}
