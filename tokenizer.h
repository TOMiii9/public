#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <sstream>
#include <fstream>

#include "typedefs.h"

enum Token_Type {
    Token_Unknown = 0,
    Token_Forward_Slash,
    Token_Hash,
    Token_String,
    Token_Number,
    Token_Spacing,
    Token_End,
    Token_Identifier,
    Token_Open_Curly_Brace,
    Token_Close_Curly_Brace,
    Token_Equal
};

struct Token {
    Token_Type type;
    string     text;
    f32        fl_value;
    i32        int_value;

    bool Identifier_Match(const string &t) {
        return (type == Token_Identifier && text == t);
    }
};

struct Lexer {
    struct Tokenizer_State {
        bool   ok;
        bool   eof;
        i8     script_pointer;
        i32    line;
        u64    cursor;
        string error;
    } state;

    string input;
    Token  next_token;
    bool   IsAlpha(i8 c);
    bool   IsNumber(i8 c);
    void   Refill();
    string Forward(int count);

    void   Error(string error);
    Token  PeekToken();
    bool   ExpectToken(Token &t, Token_Type expected);
    bool   ExpectIdentifier(Token &t, const string &text);
    Token  GetToken();
    string ConsumeLine();
    void   FromFile(const i8 *file_name);
    void   SkipTokens(i32 n);
    bool   StoppedParsing();
};

#endif
