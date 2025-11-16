#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>

#include "scopeAnalysis.cpp"   

using namespace std;


enum class TypeChkError {
    ErroneousVarDecl,
    FnCallParamCount,
    FnCallParamType,
    ErroneousReturnType,
    ExpressionTypeMismatch,
    ExpectedBooleanExpression,
    ErroneousBreak,
    NonBooleanCondStmt,
    EmptyExpression,
    AttemptedBoolOpOnNonBools,
    AttemptedBitOpOnNonNumeric,
    AttemptedShiftOnNonInt,
    AttemptedAddOpOnNonNumeric,
    AttemptedExponentiationOfNonNumeric,
    ReturnStmtNotFound
};


class TypeException : public runtime_error {
public:
    TypeChkError err;
    TypeException(TypeChkError e, const string &msg)
        : runtime_error(msg), err(e) {}
};


static TokenType typeNameStringToTokenType(const string &s) {
    if (s == "int")    return TokenType::T_INT;
    if (s == "float")  return TokenType::T_FLOAT;
    if (s == "string") return TokenType::T_STRING;
    if (s == "bool")   return TokenType::T_BOOL;
    return TokenType::T_UNKNOWN;
}

static string tokenTypeToBasicName(TokenType t) {
    switch (t) {
        case TokenType::T_INT:    return "int";
        case TokenType::T_FLOAT:  return "float";
        case TokenType::T_STRING: return "string";
        case TokenType::T_BOOL:   return "bool";
        default:                  return fromTokenTypeToStringGo(t);
    }
}


static bool opEquals(const string &opStr, TokenType tt) {
    return opStr == fromTokenTypeToStringGo(tt);
}


class TypeChecker {
    map<string, FunctionDecl*> functions;

    vector< map<string, TokenType> > scopeStack;

    TokenType currentFuncReturn;

    TokenType getVarType(const string &name) {
        for (int i = (int)scopeStack.size() - 1; i >= 0; --i) {
            if (scopeStack[i].count(name)) {
                return scopeStack[i][name];
            }
        }
        throw TypeException(TypeChkError::ErroneousVarDecl,
                            "Undeclared variable accessed: " + name);
    }

   
    void defineVar(const string &name, TokenType t) {
        if (scopeStack.empty())
            scopeStack.push_back(map<string, TokenType>());
        scopeStack.back()[name] = t;
    }

   
    TokenType analyzeExpr(const ExprPtr &expr) {
        if (!expr) {
            throw TypeException(TypeChkError::EmptyExpression, "Null expression");
        }

        // Identifier
        if (auto *id = dynamic_cast<IdentifierExpr*>(expr.get()))
            return getVarType(id->name);

        // Literals
        if (dynamic_cast<IntLiteral*>(expr.get()))    return TokenType::T_INT;
        if (dynamic_cast<FloatLiteral*>(expr.get()))  return TokenType::T_FLOAT;
        if (dynamic_cast<StringLiteral*>(expr.get())) return TokenType::T_STRING;
        if (dynamic_cast<BoolLiteral*>(expr.get()))   return TokenType::T_BOOL;

        /*
           ----------------------------------
           Binary Expression Types
           ----------------------------------
        */
        if (auto *be = dynamic_cast<BinaryExpr*>(expr.get())) {
            TokenType leftT = analyzeExpr(be->left);
            TokenType rightT = analyzeExpr(be->right);
            string op = be->op;

            // Arithmetic operators
            if (opEquals(op, TokenType::T_PLUS) ||
                opEquals(op, TokenType::T_MINUS) ||
                opEquals(op, TokenType::T_MULT) ||
                opEquals(op, TokenType::T_DIV) ||
                opEquals(op, TokenType::T_MOD))
            {
                // String concatenation
                if (opEquals(op, TokenType::T_PLUS) &&
                    leftT == TokenType::T_STRING &&
                    rightT == TokenType::T_STRING)
                    return TokenType::T_STRING;

                bool leftNum  = (leftT == TokenType::T_INT || leftT == TokenType::T_FLOAT);
                bool rightNum = (rightT == TokenType::T_INT || rightT == TokenType::T_FLOAT);
                if (!leftNum || !rightNum)
                    throw TypeException(TypeChkError::AttemptedAddOpOnNonNumeric,
                        "Arithmetic operator used on non-numeric types");

                return (leftT == TokenType::T_FLOAT || rightT == TokenType::T_FLOAT)
                       ? TokenType::T_FLOAT
                       : TokenType::T_INT;
            }

            // Comparison operators
            if (opEquals(op, TokenType::T_EQUALSOP) ||
                opEquals(op, TokenType::T_NOTEQUAL) ||
                opEquals(op, TokenType::T_LESS) ||
                opEquals(op, TokenType::T_LESSEQ) ||
                opEquals(op, TokenType::T_GREATER) ||
                opEquals(op, TokenType::T_GREATEREQ)) {

                if (leftT == rightT) return TokenType::T_BOOL;

                bool leftNum  = (leftT == TokenType::T_INT || leftT == TokenType::T_FLOAT);
                bool rightNum = (rightT == TokenType::T_INT || rightT == TokenType::T_FLOAT);

                if (leftNum && rightNum) return TokenType::T_BOOL;

                throw TypeException(TypeChkError::ExpressionTypeMismatch,
                                    "Comparison between incompatible types");
            }

            // Boolean ops
            if (opEquals(op, TokenType::T_AND) || opEquals(op, TokenType::T_OR)) {
                if (leftT != TokenType::T_BOOL || rightT != TokenType::T_BOOL)
                    throw TypeException(TypeChkError::AttemptedBoolOpOnNonBools,
                        "Boolean operator used on non-boolean operands");
                return TokenType::T_BOOL;
            }

            // Bitwise ops
            if (opEquals(op, TokenType::T_BITAND) ||
                opEquals(op, TokenType::T_BITOR) ||
                opEquals(op, TokenType::T_BITXOR)) {
                if (leftT != TokenType::T_INT || rightT != TokenType::T_INT)
                    throw TypeException(TypeChkError::AttemptedBitOpOnNonNumeric,
                        "Bitwise operator used on non-integer types");
                return TokenType::T_INT;
            }

            // Shift ops
            if (opEquals(op, TokenType::T_LSHIFT) || opEquals(op, TokenType::T_RSHIFT)) {
                if (leftT != TokenType::T_INT || rightT != TokenType::T_INT)
                    throw TypeException(TypeChkError::AttemptedShiftOnNonInt,
                        "Shift operator used on non-integer types");
                return TokenType::T_INT;
            }

            throw TypeException(TypeChkError::ExpressionTypeMismatch,
                                "Unknown binary operation: " + op);
        }

        if (auto *ce = dynamic_cast<CallExpr*>(expr.get())) {

            auto *calleeId = dynamic_cast<IdentifierExpr*>(ce->callee.get());
            if (!calleeId)
                throw TypeException(TypeChkError::ErroneousVarDecl,
                                    "Function call target is not an identifier");

            string fname = calleeId->name;
            if (!functions.count(fname))
                throw TypeException(TypeChkError::ErroneousVarDecl,
                                    "Undefined function called: " + fname);

            FunctionDecl *fn = functions[fname];

            if (ce->args.size() != fn->params.size())
                throw TypeException(TypeChkError::FnCallParamCount,
                    "Function '" + fname + "' expects " +
                    to_string(fn->params.size()) + " arguments");

            for (size_t i = 0; i < ce->args.size(); ++i) {
                TokenType argT   = analyzeExpr(ce->args[i]);
                TokenType paramT = typeNameStringToTokenType(fn->params[i].typeName);

                if (paramT == TokenType::T_FLOAT && argT == TokenType::T_INT)
                    continue;

                if (argT != paramT)
                    throw TypeException(TypeChkError::FnCallParamType,
                                        "Parameter type mismatch in call to " + fname);
            }

            return typeNameStringToTokenType(fn->returnType);
        }

        throw TypeException(TypeChkError::EmptyExpression, "Unsupported expression type");
    }


    void analyzeStmt(const StmtPtr &stmt, TokenType expectedReturnType) {
        if (!stmt) return;

        // Compound block (new scope)
        if (auto *bs = dynamic_cast<BlockStmt*>(stmt.get())) {
            scopeStack.push_back({});
            for (auto &s : bs->stmts)
                analyzeStmt(s, expectedReturnType);
            scopeStack.pop_back();
            return;
        }

        // Variable Declaration
        if (auto *vd = dynamic_cast<VarDeclStmt*>(stmt.get())) {
            TokenType declared = typeNameStringToTokenType(vd->typeName);
            if (declared == TokenType::T_UNKNOWN)
                throw TypeException(TypeChkError::ErroneousVarDecl,
                                    "Unknown type for variable " + vd->ident);

            TokenType initT = analyzeExpr(vd->init);

            if (!(declared == TokenType::T_FLOAT && initT == TokenType::T_INT) &&
                initT != declared)
            {
                throw TypeException(TypeChkError::ErroneousVarDecl,
                                    "Initializer type mismatch for " + vd->ident);
            }

            defineVar(vd->ident, declared);
            return;
        }

        // Assignment
        if (auto *asg = dynamic_cast<AssignStmt*>(stmt.get())) {
            TokenType varT = getVarType(asg->ident);
            TokenType valT = analyzeExpr(asg->value);

            if (!(varT == TokenType::T_FLOAT && valT == TokenType::T_INT) &&
                varT != valT)
                throw TypeException(TypeChkError::ExpressionTypeMismatch,
                                    "Assignment type mismatch for " + asg->ident);

            return;
        }

        // Return statement
        if (auto *ret = dynamic_cast<ReturnStmt*>(stmt.get())) {
            if (ret->value) {
                TokenType rT = analyzeExpr(ret->value);

                if (!(expectedReturnType == TokenType::T_FLOAT && rT == TokenType::T_INT) &&
                    rT != expectedReturnType)
                {
                    throw TypeException(TypeChkError::ErroneousReturnType,
                                        "Return type mismatch");
                }
            }
            return;
        }

        // Standalone expression
        if (auto *es = dynamic_cast<ExprStmt*>(stmt.get())) {
            analyzeExpr(es->expr);
            return;
        }
    }

public:

   
    void analyze(Program &prog) {

        // collect functions
        for (auto &fn : prog.funcs) {
            if (functions.count(fn->name))
                throw TypeException(TypeChkError::ErroneousVarDecl,
                                    "Function redefinition: " + fn->name);
            functions[fn->name] = fn.get();
        }

        // analyze each function independently
        for (auto &fnPtr : prog.funcs) {
            FunctionDecl *fn = fnPtr.get();

            scopeStack.clear();
            scopeStack.push_back({});

            // load parameters
            for (auto &p : fn->params) {
                TokenType pt = typeNameStringToTokenType(p.typeName);
                scopeStack.back()[p.ident] = pt;
            }

            TokenType expectedRet = typeNameStringToTokenType(fn->returnType);
            currentFuncReturn = expectedRet;

            analyzeStmt(fn->body, expectedRet);

            bool foundReturn = false;
            for (auto &s : fn->body->stmts) {
                if (dynamic_cast<ReturnStmt*>(s.get())) {
                    foundReturn = true;
                    break;
                }
            }

            if (!foundReturn && expectedRet != TokenType::T_UNKNOWN) {
                throw TypeException(TypeChkError::ReturnStmtNotFound,
                                    "Function '" + fn->name + "' missing return statement");
            }
        }
    }
};


void typeChecker(Program prog) {
    try {
        TypeChecker checker;
        checker.analyze(prog);
        cout << "Type checking passed, no errors found.\n";
    } catch (const TypeException &ex) {
        cerr << "Type error: " << ex.what() << "\n";
    } catch (const exception &ex) {
        cerr << "Error: " << ex.what() << "\n";
    }
}
