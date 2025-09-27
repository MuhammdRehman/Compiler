// parser.cpp
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <regex>
#include <cstring>
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



struct BinaryExpr : public Expr {
    string op;
    ExprPtr left, right;
    BinaryExpr(ExprPtr l, string o, ExprPtr r) : left(move(l)), op(move(o)), right(move(r)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "BinaryExpr: " << op << "\n";
        left->print(indent+2);
        right->print(indent+2);
    }
};

struct CallExpr : public Expr {
    ExprPtr callee;
    vector<ExprPtr> args;
    CallExpr(ExprPtr c, vector<ExprPtr> a) : callee(move(c)), args(move(a)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "CallExpr:\n";
        pad(indent+2); cout << "Callee:\n"; callee->print(indent+4);
        pad(indent+2); cout << "Args:\n";
        if (args.empty()) { pad(indent+4); cout << "(none)\n"; }
        for (auto &a : args) a->print(indent+4);
    }
};

/* Statements */
struct Stmt : public ASTNode {};
using StmtPtr = shared_ptr<Stmt>;

struct ExprStmt : public Stmt {
    ExprPtr expr;
    ExprStmt(ExprPtr e): expr(move(e)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "ExprStmt:\n";
        expr->print(indent+2);
    }
};

struct ReturnStmt : public Stmt {
    ExprPtr value; // may be null for 'return;'
    ReturnStmt(ExprPtr v): value(move(v)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "ReturnStmt:\n";
        if (value) value->print(indent+2);
        else { pad(indent+2); cout << "(void)\n"; }
    }
};

struct VarDeclStmt : public Stmt {
    string typeName;
    string ident;
    ExprPtr init;
    VarDeclStmt(string t, string id, ExprPtr i) : typeName(move(t)), ident(move(id)), init(move(i)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "VarDecl: " << typeName << " " << ident << "\n";
        pad(indent+2); cout << "Init:\n";
        init->print(indent+4);
    }
};

struct AssignStmt : public Stmt {
    string ident;
    ExprPtr value;
    AssignStmt(string id, ExprPtr v) : ident(move(id)), value(move(v)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "Assign: " << ident << "\n";
        value->print(indent+2);
    }
};

struct BlockStmt : public Stmt {
    vector<StmtPtr> stmts;
    BlockStmt(vector<StmtPtr> s = {}) : stmts(move(s)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "BlockStmt:\n";
        for (auto &st : stmts) st->print(indent+2);
    }
};

/* Function / Program */
struct Param {
    string typeName;
    string ident;
};

struct FunctionDecl : public ASTNode {
    string returnType;
    string name;
    vector<Param> params;
    shared_ptr<BlockStmt> body;
    FunctionDecl(string r, string n, vector<Param> p, shared_ptr<BlockStmt> b)
        : returnType(move(r)), name(move(n)), params(move(p)), body(move(b)) {}
    void print(int indent=0) const override {
        pad(indent); cout << "FunctionDecl: " << name << "\n";
        pad(indent+2); cout << "ReturnType: " << returnType << "\n";
        pad(indent+2); cout << "Params:\n";
        if (params.empty()) pad(indent+4), cout << "(none)\n";
        for (auto &pr : params) {
            pad(indent+4); cout << pr.typeName << " " << pr.ident << "\n";
        }
        pad(indent+2); cout << "Body:\n";
        body->print(indent+4);
    }
};

struct Program : public ASTNode {
    vector<shared_ptr<FunctionDecl>> funcs;
    void print(int indent=0) const override {
        pad(indent); cout << "Program:\n";
        for (auto &f : funcs) f->print(indent+2);
    }
};

/* -------------------------
   parser here
   ------------------------- */
class Parser {
public:
    Parser(const vector<Token>& toks) : tokens(toks), pos(0) {}

    Program parse_full_program() 
    {
        Program prog;

        while (!reach_end() && getter_now().type != TokenType::T_EOF)
        {
            if (isTypeToken(getter_now()))
            {
            
                // prog.funcs.push_back(parseFunctionDecl());
            } 
            else if (getter_now().type == TokenType::T_COMMENT) 
            {
            
                next_get(); // leaving comments
            }
            else 
            {
             
                throw ParseException(ParseError::UnexpectedToken, getter_now(), "top: expected the function(type)");
            }
        }
        //leave the eof there
        if (!reach_end() && getter_now().type == TokenType::T_EOF) 
        {
            next_get();
        }

        return prog;
    }

    const vector<Token>& tokens;
    size_t pos;

    bool reach_end() const 
    {

        return pos >= tokens.size();
    }
    const Token& getter_now() const 
    {
        if (pos < tokens.size()) 
        {
            return tokens[pos];
        }

        static Token eofTok(TokenType::T_EOF, "");
        
        return eofTok;
    }
    const Token& prev_get() const 
    {
        if (pos == 0) 
        {
            return tokens[0];
        }

        return tokens[pos-1];
    }
    const Token& next_get() 
    {
        if (!reach_end())
        {
            ++pos;
        }

        return prev_get();
    }

    bool check(TokenType t) const 
    {
        if (reach_end()) 
        {
            return false;
        }

        return getter_now().type == t;
    }

    bool match(initializer_list<TokenType> types) 
    {
        for (auto t : types) 
        {
            if (check(t)) 
            { 
                next_get(); return true; 
            }
        }
        return false;
    }

    const Token& take_func(TokenType expected, ParseError err, const string &msg = "")
    {
        if (check(expected)) 
        {
            return next_get();
        }

        throw ParseException(err, getter_now(), msg);
    }

}


int ParserAlgo(vector<Token> ts) 
{
    
    return 0;
}