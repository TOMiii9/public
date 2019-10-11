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
    Token_Type  type;
    std::string text;
    f32         fl_value;
    i32         int_value;

    bool identifier_match(const string &t) {
        return (type == Token_Identifier && text == t);
    }
};

struct Lexer {
    struct tokenizer_state {
        bool        ok;
        bool        eof;
        i8          script_pointer;
        i32         line;
        u64         cursor;
        std::string error;
    } state;

    std::string input;
    Token       next_token;
    bool        is_alpha(i8 c);
    bool        is_number(i8 c);
    void        refill();
    std::string forward(int count);

    void        error(std::string error);
    Token       peek_token();
    bool        expect_token(Token &t, Token_Type expected);
    bool        expect_identifier(Token &t, const string &text);
    Token       get_token();
    std::string consume_line();
    void        from_file(const i8 *file_name);
    void        skip_tokens(i32 n);
    bool        stopped_parsing();
};

#endif
