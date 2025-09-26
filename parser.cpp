// parser.cpp
#include <bits/stdc++.h>
#include "tokens.h"   
using namespace std;

/* -------------------------
   ParseError As defined/told in instructions to Handle
   ------------------------- */
enum class ParseError {
    UnexpectedEOF,
    FailedToFindToken,
    ExpectedTypeToken,
    ExpectedIdentifier,
    UnexpectedToken,
    ExpectedFloatLit,
    ExpectedIntLit,
    ExpectedStringLit,
    ExpectedBoolLit,
    ExpectedExpr
};

// ParseException carries an error 
struct ParseException : public runtime_error {
    ParseError err;
    Token token;
    ParseException(ParseError e, const Token& t, const string &msg = "")
        : runtime_error(msg), err(e), token(t) {}
};

/* -------------------------
   AST node definitions
   ------------------------- */
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
protected:
    static void pad(int n) { for (int i = 0; i < n; ++i) cout << ' '; }
};

using ASTPtr = shared_ptr<ASTNode>;

/* Expressions */
struct Expr : public ASTNode {};
using ExprPtr = shared_ptr<Expr>;

struct IdentifierExpr : public Expr {
    string name;
    IdentifierExpr(string n): name(move(n)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "Identifier: " << name << "\n";
    }
};

struct IntLiteral : public Expr {
    string val;
    IntLiteral(string v): val(move(v)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "IntLiteral: " << val << "\n";
    }
};

struct FloatLiteral : public Expr {
    string val;
    FloatLiteral(string v): val(move(v)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "FloatLiteral: " << val << "\n";
    }
};
struct StringLiteral : public Expr {
    string val;
    StringLiteral(string v): val(move(v)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "StringLiteral: \"" << val << "\"\n";
    }
};

struct BoolLiteral : public Expr {
    string val;
    BoolLiteral(string v): val(move(v)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "BoolLiteral: " << val << "\n";
    }
};





int ParserAlgo(vector<Token> ts){
    cout<<"Parser is under Construction. Once completed then it's code will be written here.";
    return 0;
}