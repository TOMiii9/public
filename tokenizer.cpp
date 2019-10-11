#include "tokenizer.h"
#include "util.h"

bool Lexer::is_alpha(i8 c) {
    bool b = ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
    return b;
}

bool Lexer::is_number(i8 c) {
    bool b = ((c >= '0') && (c <= '9'));
    return b;
}

void Lexer::refill() {
    u64 remaining_characters = input.length() - state.cursor;
    if (remaining_characters == 0 || state.cursor >= input.length()) {
        state.script_pointer = 0;
    } else if (remaining_characters >= 1) {
        state.script_pointer = input[state.cursor];
    }
}

std::string Lexer::forward(int count) {
    std::string s;
    for (int i = 0; i < count; i++) {
        if (state.cursor + i < input.length()) {
            s += input[state.cursor + i];
        }
    }
    state.cursor += count;
    refill();
    return s;
}

std::string Lexer::consume_line() {
    std::string s;
    while (state.script_pointer && state.script_pointer != '\n') {
        s += forward(1);
    }
    return s;
}

Token Lexer::peek_token() {
    tokenizer_state temp  = state;
    Token           token = get_token();
    state                 = temp; // reset the tokenizer to the state before 'get_token'

    return token;
}

bool Lexer::expect_token(Token &t, Token_Type expected) {
    Token tmp    = peek_token();
    bool  result = (tmp.type == expected);
    if (result) {
        t = get_token();
    }
    return result;
}

bool Lexer::expect_identifier(Token &t, const string &text) {
    bool result = false;
    if (expect_token(t, Token_Identifier)) {
        if (t.text == text) {
            result = true;
        }
    }

    return result;
}

Token Lexer::get_token() {
    while (1) {
        next_token = {};

        if (state.ok == false) {
            next_token.type = Token_Unknown;
            return next_token;
        }

        i8 current_char = state.script_pointer;
        next_token.text += current_char;
        forward(1);
        switch (current_char) {
            case '\0':
                next_token.type = Token_End;
                state.eof       = true;
                return next_token;

            case '/':
                next_token.type = Token_Forward_Slash;
                return next_token;

            case '#':           // TODO: comment tokens are hard coded
                consume_line(); // ignore comments
                continue;

            case '{':
                next_token.type = Token_Open_Curly_Brace;
                return next_token;

            case '}':
                next_token.type = Token_Close_Curly_Brace;
                return next_token;

            case '=':
                next_token.type = Token_Equal;
                return next_token;

            case '"':
                next_token.type = Token_String;
                while (state.script_pointer && state.script_pointer != '"') {
                    next_token.text += forward(1);
                }
                if (state.script_pointer && state.script_pointer == '"') {
                    next_token.text += forward(1);
                }
                if (next_token.text[0] == '"') {
                    next_token.text.erase(0, 1);
                }
                if (next_token.text[next_token.text.length() - 1] == '"') {
                    next_token.text.pop_back();
                } else {
                    report("****** Incomplete string ******\n");
                    state.ok = false;
                }
                return next_token;

            case ' ':
            case '\t':
            case '\r':
            case '\n':
                if (current_char == '\n') {
                    state.line++;
                }
                next_token.type = Token_Spacing;
                continue;

            default:
                if (is_alpha(current_char)) {
                    next_token.type = Token_Identifier;
                    while (is_alpha(state.script_pointer) || is_number(state.script_pointer) || state.script_pointer == '.' ||
                           state.script_pointer == '_') {
                        next_token.text += forward(1);
                    }
                } else if (is_number(current_char) || current_char == '-') {
                    next_token.type = Token_Number;
                    while (is_number(state.script_pointer)) {
                        next_token.text += forward(1);
                    }

                    if (state.script_pointer == '.') {
                        next_token.text += '.';
                        forward(1);

                        while (is_number(state.script_pointer)) {
                            next_token.text += forward(1);
                        }
                        // TODO: atof
                    } else {
                        // TODO: atoi
                    }

                } else {
                    next_token.type = Token_Unknown;
                    state.ok        = false;
                    report("\n\n****** Unexpected error ******\n\nUnknown token '%s' at line %d\n", next_token.text.c_str(), state.line);
                }
                return next_token;
        }
    }
}

void Lexer::error(std::string error) {
    if (!state.ok) {
        return;
    }

    state.ok    = false;
    state.error = error;
    report("%s Line: %d Current token: '%s'\n", error.c_str(), state.line, next_token.text.c_str());
}

void Lexer::from_file(const i8 *file_name) {
    input        = string_file(file_name);
    state.line   = 1;
    state.cursor = 0;
    state.ok     = true;
    state.eof    = false;
    refill();
}

void Lexer::skip_tokens(i32 n) {
    while (n > 0) {
        Token t = get_token();
        if (t.type == Token_End || !state.ok) {
            return;
        }
        n--;
    }
}

bool Lexer::stopped_parsing() {
    return !state.ok || state.eof;
}
