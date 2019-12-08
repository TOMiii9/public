#include "tokenizer.h"
#include "util.h"

bool Lexer::IsAlpha(i8 c) {
    bool b = ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
    return b;
}

bool Lexer::IsNumber(i8 c) {
    bool b = ((c >= '0') && (c <= '9'));
    return b;
}

void Lexer::Refill() {
    u64 remaining_characters = input.length() - state.cursor;
    if (remaining_characters == 0 || state.cursor >= input.length()) {
        state.script_pointer = 0;
    } else if (remaining_characters >= 1) {
        state.script_pointer = input[state.cursor];
    }
}

string Lexer::Forward(int count) {
    string s;
    for (int i = 0; i < count; i++) {
        if (state.cursor + i < input.length()) {
            s += input[state.cursor + i];
        }
    }
    state.cursor += count;
    Refill();
    return s;
}

string Lexer::ConsumeLine() {
    string s;
    while (state.script_pointer && state.script_pointer != '\n') {
        s += Forward(1);
    }
    return s;
}

Token Lexer::PeekToken() {
    Tokenizer_State temp  = state;
    Token           token = GetToken();
    state                 = temp; // reset the tokenizer to the state before 'get_token'

    return token;
}

bool Lexer::ExpectToken(Token &t, Token_Type expected) {
    Token tmp    = PeekToken();
    bool  result = (tmp.type == expected);
    if (result) {
        t = GetToken();
    }
    return result;
}

bool Lexer::ExpectIdentifier(Token &t, const string &text) {
    bool result = false;
    if (ExpectToken(t, Token_Identifier)) {
        if (t.text == text) {
            result = true;
        }
    }

    return result;
}

Token Lexer::GetToken() {
    while (1) {
        next_token = {};

        if (state.ok == false) {
            next_token.type = Token_Unknown;
            return next_token;
        }

        i8 current_char = state.script_pointer;
        next_token.text += current_char;
        Forward(1);
        switch (current_char) {
            case '\0':
                next_token.type = Token_End;
                state.eof       = true;
                return next_token;

            case '/':
                next_token.type = Token_Forward_Slash;
                return next_token;

            case '#':          // TODO: comment tokens are hard coded
                ConsumeLine(); // ignore comments
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
                    next_token.text += Forward(1);
                }
                if (state.script_pointer && state.script_pointer == '"') {
                    next_token.text += Forward(1);
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
                if (IsAlpha(current_char)) {
                    next_token.type = Token_Identifier;
                    while (IsAlpha(state.script_pointer) || IsNumber(state.script_pointer) || state.script_pointer == '.' ||
                           state.script_pointer == '_') {
                        next_token.text += Forward(1);
                    }
                } else if (IsNumber(current_char) || current_char == '-') {
                    next_token.type = Token_Number;
                    while (IsNumber(state.script_pointer)) {
                        next_token.text += Forward(1);
                    }

                    if (state.script_pointer == '.') {
                        next_token.text += '.';
                        Forward(1);

                        while (IsNumber(state.script_pointer)) {
                            next_token.text += Forward(1);
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

void Lexer::Error(string error) {
    if (!state.ok) {
        return;
    }

    state.ok    = false;
    state.error = error;
    report("%s Line: %d Current token: '%s'\n", error.c_str(), state.line, next_token.text.c_str());
}

void Lexer::FromFile(const i8 *file_name) {
    input        = StringFile(file_name);
    state.line   = 1;
    state.cursor = 0;
    state.ok     = true;
    state.eof    = false;
    Refill();
}

void Lexer::SkipTokens(i32 n) {
    while (n > 0) {
        Token t = GetToken();
        if (t.type == Token_End || !state.ok) {
            return;
        }
        n--;
    }
}

bool Lexer::StoppedParsing() {
    return !state.ok || state.eof;
}
