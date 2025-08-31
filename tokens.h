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
string fromTokenTypeToStringGo(TokenType type)
{
    switch(type)
    {
        case TokenType::T_FUNCTION: return "T_FUNCTION";
        case TokenType::T_INT: return "T_INT";
        case TokenType::T_FLOAT: return "T_FLOAT";
        case TokenType::T_STRING: return "T_STRING";
        case TokenType::T_BOOL: return "T_BOOL";
        case TokenType::T_RETURN: return "T_RETURN";
        case TokenType::T_IF: return "T_IF";
        case TokenType::T_ELSE: return "T_ELSE";
        case TokenType::T_WHILE: return "T_WHILE";
        case TokenType::T_FOR: return "T_FOR";

        case TokenType::T_IDENTIFIER: return "T_IDENTIFIER";
        case TokenType::T_INTLIT: return "T_INTLIT";
        case TokenType::T_FLOATLIT: return "T_FLOATLIT";
        case TokenType::T_STRINGLIT: return "T_STRINGLIT";
        case TokenType::T_BOOLLIT: return "T_BOOLLIT";

        case TokenType::T_ASSIGN: return "T_ASSIGN";
        case TokenType::T_EQUALSOP: return "T_EQUALSOP";
        case TokenType::T_NOTEQUAL: return "T_NOTEQUAL";
        case TokenType::T_LESS: return "T_LESS";
        case TokenType::T_LESSEQ: return "T_LESSEQ";
        case TokenType::T_GREATER: return "T_GREATER";
        case TokenType::T_GREATEREQ: return "T_GREATEREQ";
        case TokenType::T_PLUS: return "T_PLUS";
        case TokenType::T_MINUS: return "T_MINUS";
        case TokenType::T_MULT: return "T_MULT";
        case TokenType::T_DIV: return "T_DIV";
        case TokenType::T_MOD: return "T_MOD";
        case TokenType::T_PLUSASSIGN: return "T_PLUSASSIGN";
        case TokenType::T_MINUSASSIGN: return "T_MINUSASSIGN";
        case TokenType::T_AND: return "T_AND";
        case TokenType::T_OR: return "T_OR";
        case TokenType::T_NOT: return "T_NOT";
        case TokenType::T_BITAND: return "T_BITAND";
        case TokenType::T_BITOR: return "T_BITOR";
        case TokenType::T_BITXOR: return "T_BITXOR";
        case TokenType::T_BITNOT: return "T_BITNOT";
        case TokenType::T_LSHIFT: return "T_LSHIFT";
        case TokenType::T_RSHIFT: return "T_RSHIFT";

        case TokenType::T_PARENL: return "T_PARENL";
        case TokenType::T_PARENR: return "T_PARENR";
        case TokenType::T_BRACEL: return "T_BRACEL";
        case TokenType::T_BRACER: return "T_BRACER";
        case TokenType::T_BRACKETL: return "T_BRACKETL";
        case TokenType::T_BRACKETR: return "T_BRACKETR";
        case TokenType::T_COMMA: return "T_COMMA";
        case TokenType::T_SEMICOLON: return "T_SEMICOLON";
        case TokenType::T_QUOTE: return "T_QUOTE";

        case TokenType::T_COMMENT: return "T_COMMENT";
        case TokenType::T_UNKNOWN: return "T_UNKNOWN";
        case TokenType::T_EOF: return "T_EOF";
        default: return "T_UNKNOWN";
        }
}


#endif
