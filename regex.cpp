#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include "tokens.h"

using namespace std;

// Patterns for regex-based lexer
vector<pair<TokenType, regex>> tokenPatterns = {
    // Keywords
    {TokenType::T_FUNCTION, regex("^fn\\b")},
    {TokenType::T_INT, regex("^int\\b")},
    {TokenType::T_FLOAT, regex("^float\\b")},
    {TokenType::T_STRING, regex("^string\\b")},
    {TokenType::T_BOOL, regex("^bool\\b")},
    {TokenType::T_RETURN, regex("^return\\b")},
    {TokenType::T_IF, regex("^if\\b")},
    {TokenType::T_ELSE, regex("^else\\b")},
    {TokenType::T_WHILE, regex("^while\\b")},
    {TokenType::T_FOR, regex("^for\\b")},
    {TokenType::T_BOOLLIT, regex("^(true|false)\\b")},

    // Literals
    {TokenType::T_FLOATLIT, regex("^[0-9]+\\.[0-9]+")},
    {TokenType::T_INTLIT, regex("^[0-9]+")},
    {TokenType::T_STRINGLIT, regex("^\"([^\"\\\\]|\\\\.)*\"")},

    // Operators & Symbols
    {TokenType::T_EQUALSOP, regex("^==")},
    {TokenType::T_NOTEQUAL, regex("^!=")},
    {TokenType::T_LESSEQ, regex("^<=")},
    {TokenType::T_GREATEREQ, regex("^>=")},
    {TokenType::T_ASSIGN, regex("^=")},
    {TokenType::T_LESS, regex("^<")},
    {TokenType::T_GREATER, regex("^>")},
    {TokenType::T_PLUSASSIGN, regex("^\\+=")},
    {TokenType::T_MINUSASSIGN, regex("^-=")},
    {TokenType::T_PLUS, regex("^\\+")},
    {TokenType::T_MINUS, regex("^-")},
    {TokenType::T_MULT, regex("^\\*")},
    {TokenType::T_DIV, regex("^/")},
    {TokenType::T_MOD, regex("^%")},
    {TokenType::T_AND, regex("^&&")},
    {TokenType::T_OR, regex("^\\|\\|")},
    {TokenType::T_NOT, regex("^!")},
    {TokenType::T_BITAND, regex("^&")},
    {TokenType::T_BITOR, regex("^\\|")},
    {TokenType::T_BITXOR, regex("^\\^")},
    {TokenType::T_BITNOT, regex("^~")},
    {TokenType::T_LSHIFT, regex("^<<")},
    {TokenType::T_RSHIFT, regex("^>>")},

    // Punctuation
    {TokenType::T_PARENL, regex("^\\(")},
    {TokenType::T_PARENR, regex("^\\)")},
    {TokenType::T_BRACEL, regex("^\\{")},
    {TokenType::T_BRACER, regex("^\\}")},
    {TokenType::T_BRACKETL, regex("^\\[")},
    {TokenType::T_BRACKETR, regex("^\\]")},
    {TokenType::T_COMMA, regex("^,")},
    {TokenType::T_SEMICOLON, regex("^;")},
    {TokenType::T_QUOTE, regex("^\"")},

    // Identifiers (must be after keywords)
    {TokenType::T_IDENTIFIER, regex("^[a-zA-Z_][a-zA-Z0-9_]*")},
    
    // Comments Handling
    {TokenType::T_COMMENT, regex("^//[^\n]*")},
    {TokenType::T_COMMENT, regex("^/\\*[^*]*\\*+([^/*][^*]*\\*+)*/")}
    
};


vector<Token> tokenize_regex(const string &input) {
    vector<Token> tokens;
    string code = input;

    while (!code.empty()) {
        // Skip whitespace
        if (isspace(code[0])) {
            code.erase(0, 1);
            continue;
        }

        bool matched = false;

        for (auto &[type, pattern] : tokenPatterns) {
            smatch match;
            if (regex_search(code, match, pattern)) {
                string value = match.str(0);
                tokens.push_back(Token(type, value));
                code.erase(0, value.size());
                matched = true;
                break;
            }
        }

        if (!matched) {
            // Unknown token
            tokens.push_back(Token(TokenType::T_UNKNOWN, string(1, code[0])));
            code.erase(0, 1);
        }
    }

    tokens.push_back(Token(TokenType::T_EOF, ""));
    return tokens;
}


void print_all_tokens(const vector<Token>& tokens) {
    cout << "[";
    for (auto &t : tokens) {
        cout << fromTokenTypeToStringGo(t.type);
        if (!t.value.empty() && (t.type == TokenType::T_STRINGLIT || t.type == TokenType::T_IDENTIFIER))
            cout << "(\"" << t.value << "\")";
        else if (!t.value.empty() && (t.type == TokenType::T_INTLIT || 
                                      t.type == TokenType::T_FLOATLIT ||
                                      t.type == TokenType::T_BOOLLIT))
            cout << "(" << t.value << ")";
        cout << ",";
    }
    cout << "]";
    cout << endl;
}

vector<Token>  LexerAlgo(string& file) {
    ifstream infile(file);
    stringstream buffer;
    buffer << infile.rdbuf();
    string programcode = buffer.str();

    vector<Token> ts = tokenize_regex(programcode);
    print_all_tokens(ts);
    return ts;
}
