#ifndef TOKENS_H
#define TOKENS_H

#include<string>
using namespace std;

enum class TokenType {
    // Keywords
T_FUNCTION,
T_INT,
T_FLOAT,
T_STRING,
T_BOOL,
T_RETURN,
T_IF,
T_ELSE,
T_WHILE,
T_FOR,

// literals
T_IDENTIFIER,
T_INTLIT,
T_FLOATLIT,
T_STRINGLIT,
T_BOOLLIT,

// operatorrrs
T_ASSIGN,
T_EQUALSOP,
T_NOTEQUAL,
T_LESS,
T_LESSEQ,
T_GREATER,
T_GREATEREQ,
T_PLUS,
T_MINUS,
T_MULT,
T_DIV,
T_MOD,
T_PLUSASSIGN,
T_MINUSASSIGN,
T_AND,
T_OR,
T_NOT,
T_BITAND,
T_BITOR,
T_BITXOR,
T_BITNOT,
T_LSHIFT,
T_RSHIFT,

// symbolss
T_PARENL,
T_PARENR,
T_BRACEL,
T_BRACER,
T_BRACKETL,
T_BRACKETR,
T_COMMA,
T_SEMICOLON,
T_QUOTE,

// others lefty
T_COMMENT,
T_UNKNOWN,
T_EOF


};

struct Token {
    TokenType type;
    string value;

    Token(TokenType t,const string& v = "")
        : type(t),value(v){

        }
};

#endif
