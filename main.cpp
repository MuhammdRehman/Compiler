#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include <vector>
#include "tokens.h"
#include <cctype>

TokenType fromKeyToTokenGo(const string& word)
{
    if (word == "fn")
    {
        return TokenType::T_FUNCTION;
    }
    if (word == "int")
    {
        return TokenType::T_INT;
    }
    if (word == "float")
    {
        return TokenType::T_FLOAT;
    }
    if (word == "string")
    {
        return TokenType::T_STRING;
    }
    if (word == "bool")
    {
        return TokenType::T_BOOL;
    }
    if (word == "return")
    {
        return TokenType::T_RETURN;
    }
    if (word == "if")
    {
        return TokenType::T_IF;
    }
    if (word == "else")
    {
        return TokenType::T_ELSE;
    }
    if (word == "while")
    {
        return TokenType::T_WHILE;
    }
    if (word == "for")
    {
        return TokenType::T_FOR;
    }
    if (word == "true")
    {
        return TokenType::T_BOOLLIT;
    }
    return TokenType::T_IDENTIFIER;
}

vector<Token> tokenize_function(const string& programcode)
{
    vector<Token> tokens;
    size_t i = 0;

    while (i < programcode.size()) 
    {
        char c = programcode[i];

        if (isspace(c)) 
        {
             i++;
            continue; 
        }

        if (isalpha(c) || c == '_')
        {
            string word_make;
            while (i< programcode.size() && (isalnum(programcode[i]) || programcode[i] == '_'))
            {
                word_make.push_back(programcode[i]);
                i++;
                
            }
            TokenType type_get = fromKeyToTokenGo(word_make);
            tokens.push_back(Token(type_get,word_make));
            
            continue;
        }
        if (isdigit(c)) 
        {
            string num;
            bool floaty = false;
            while (i < programcode.size() && (isdigit(programcode[i]) || programcode[i] == '.')) 
            {
                if (programcode[i] == '.')
                {
                    if (floaty)
                    {
                         break;
                    }
                    floaty = true;
                }
                num.push_back(programcode[i]);
                i++;
            }
            tokens.push_back(Token(floaty ? TokenType::T_FLOATLIT : TokenType::T_INTLIT, num));
            continue;
        }
        
        // Comments
        if (c == '/' && i + 1 < programcode.size())
        {
            if (programcode[i + 1] == '/') 
            {
                string comment;
                i += 2;
                while (i < programcode.size() && programcode[i] != '\n') 
                {
                    comment.push_back(programcode[i++]);
                }
                tokens.push_back(Token(TokenType::T_COMMENT, comment));
                continue;
            }
            if (programcode[i + 1] == '*') 
            {
                string comment;
                i += 2;
                while (i + 1 < programcode.size() && !(programcode[i] == '*' && programcode[i + 1] == '/')) {
                    comment.push_back(programcode[i++]);
                }
                if (i + 1 < programcode.size()) i += 2; 
                tokens.push_back(Token(TokenType::T_COMMENT, comment));
                continue;
            }
        }

         // String
        if (c == '"')
        {
            i++;
            string str;
            bool terminated = false;
            while (i < programcode.size())
            {
                if (programcode[i] == '\\' && i + 1 < programcode.size()) 
                {
                    str.push_back(programcode[i]);
                    i++;
                    str.push_back(programcode[i]);
                }
                else if (programcode[i] == '"') 
                {
                     terminated = true; break; 
                }

                else 
                {
                    str.push_back(programcode[i]);
                }

                i++;
            }
            if (terminated)
             {
                 i++; 
                 tokens.push_back(Token(TokenType::T_STRINGLIT, str));
             }


            else 
            {
                tokens.push_back(Token(TokenType::T_UNKNOWN, str));
            }

            continue;
        }

        switch (c) {
        case '=':
            if (i + 1 < programcode.size() && programcode[i + 1] == '=') 
            {
                 tokens.push_back(Token(TokenType::T_EQUALSOP, "==")); 
                 i += 2; 
            }
            else 
            {
                 tokens.push_back(Token(TokenType::T_ASSIGN, "=")); 
                 i++; 
            }
            break;
        case '!':
            if (i + 1 < programcode.size() && programcode[i + 1] == '=') 
            {
                 tokens.push_back(Token(TokenType::T_NOTEQUAL, "!=")); i += 2; 
            }
            else 
            {
                 tokens.push_back(Token(TokenType::T_NOT, "!")); i++;
            }
            break;
        case '<':
            if (i + 1 < programcode.size() && programcode[i + 1] == '=') 
            {
                 tokens.push_back(Token(TokenType::T_LESSEQ, "<=")); i += 2;
            }
            else if (i + 1 < programcode.size() && programcode[i + 1] == '<') 
            {
                 tokens.push_back(Token(TokenType::T_LSHIFT, "<<")); i += 2;
            }
            else 
            {
                 tokens.push_back(Token(TokenType::T_LESS, "<")); i++;
            }
            break;
        case '>':
            if (i + 1 < programcode.size() && programcode[i + 1] == '=')
             {
                 tokens.push_back(Token(TokenType::T_GREATEREQ, ">="));
                  i += 2; 
            }
            else if (i + 1 < programcode.size() && programcode[i + 1] == '>') 
            {
                 tokens.push_back(Token(TokenType::T_RSHIFT, ">>")); i += 2; 
            }
            else 
            { 
                tokens.push_back(Token(TokenType::T_GREATER, ">"));
                 i++;
             }
            break;
        case '&':
            if (i + 1 < programcode.size() && programcode[i + 1] == '&') 
            {
                 tokens.push_back(Token(TokenType::T_AND, "&&")); i += 2;
                 }
            else 
            {
                 tokens.push_back(Token(TokenType::T_BITAND, "&")); i++;
                 }
            break;
        case '|':
            if (i + 1 < programcode.size() && programcode[i + 1] == '|')
             {
                 tokens.push_back(Token(TokenType::T_OR, "||")); i += 2; 
                }
            else 
            {
                 tokens.push_back(Token(TokenType::T_BITOR, "|")); i++; 
                }
            break;
        case '+':
            if (i + 1 < programcode.size() && programcode[i + 1] == '=') 
            {
                 tokens.push_back(Token(TokenType::T_PLUSASSIGN, "+="));
                  i += 2; 
                }
            else 
            {
                 tokens.push_back(Token(TokenType::T_PLUS, "+")); i++; 
                }
            break;
        case '-':
            if (i + 1 < programcode.size() && programcode[i + 1] == '=') 
            {
                 tokens.push_back(Token(TokenType::T_MINUSASSIGN, "-=")); 
                 i += 2;
                 }
            else 
            {
                 tokens.push_back(Token(TokenType::T_MINUS, "-")); 
                 i++;
                 }
            break;
        case '*':
            if (i + 1 < programcode.size() && programcode[i + 1] == '=')
             {
                 tokens.push_back(Token(TokenType::T_MULT, "*=")); 
                 i += 2; 
                }
            else 
            {
                 tokens.push_back(Token(TokenType::T_MULT, "*"));
                  i++;
                 }
            break;
        case '/':
            if (i + 1 < programcode.size() && programcode[i + 1] == '=')
             {
                 tokens.push_back(Token(TokenType::T_DIV, "/=")); i += 2;
                 }
            else 
            {
                 tokens.push_back(Token(TokenType::T_DIV, "/"));
                  i++;
                 }
            break;
        case '%':
            if (i + 1 < programcode.size() && programcode[i + 1] == '=')
             {
                 tokens.push_back(Token(TokenType::T_MOD, "%="));
                  i += 2;
                 }
            else 
            { 
                tokens.push_back(Token(TokenType::T_MOD, "%"));
                 i++; 
                }
            break;
        case '^': tokens.push_back(Token(TokenType::T_BITXOR, "^")); i++; break;
        case '~': tokens.push_back(Token(TokenType::T_BITNOT, "~")); i++; break;
        case '(': tokens.push_back(Token(TokenType::T_PARENL, "(")); i++; break;
        case ')': tokens.push_back(Token(TokenType::T_PARENR, ")")); i++; break;
        case '{': tokens.push_back(Token(TokenType::T_BRACEL, "{")); i++; break;
        case '}': tokens.push_back(Token(TokenType::T_BRACER, "}")); i++; break;
        case '[': tokens.push_back(Token(TokenType::T_BRACKETL, "[")); i++; break;
        case ']': tokens.push_back(Token(TokenType::T_BRACKETR, "]")); i++; break;
        case ';': tokens.push_back(Token(TokenType::T_SEMICOLON, ";")); i++; break;
        case ',': tokens.push_back(Token(TokenType::T_COMMA, ",")); i++; break;
        default: tokens.push_back(Token(TokenType::T_UNKNOWN, string(1, c))); i++; break;
        }
    }

    tokens.push_back(Token(TokenType::T_EOF, ""));
    return tokens;
}

void print_all_tokens(const vector<Token>& tokens) 
{
    cout << "[";
    for (auto& t : tokens) {
        cout << fromTokenTypeToStringGo(t.type);
        if (!t.value.empty() && (t.type == TokenType::T_STRINGLIT || t.type == TokenType::T_IDENTIFIER))
            cout << "(\"" << t.value<<"\")";
        else if (!t.value.empty() && (t.type == TokenType::T_INTLIT ||
            t.type == TokenType::T_FLOATLIT ||
            t.type == TokenType::T_BOOLLIT))
            cout << "(" << t.value << ")";
        
        cout << ",";
    }
    cout << "]";
    cout << endl;
}




int main() 
{
    ifstream infile("TestFile.txt");   
    stringstream buffer;
    buffer << infile.rdbuf();     // Read whole file into buffer

    string programcode = buffer.str(); 
    vector <Token> ts = tokenize_function(programcode);

    print_all_tokens(ts);

    return 0;
}