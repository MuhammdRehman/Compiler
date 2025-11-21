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
    string tokName = fromTokenTypeToStringGo(tt);
    return opStr == tokName;
}

class TypeChecker {
    map<string, FunctionDecl*> functions;
    vector< map<string, TokenType> > scopeStack;
    TokenType currentFuncReturn;

    TokenType getVarType(const string &name) {
        for (int i = (int) scopeStack.size() - 1; i >= 0; --i) {
            if (scopeStack[i].count(name)) {
                return scopeStack[i][name];
            }
        }
        throw TypeException(TypeChkError::ErroneousVarDecl,
                            "Undeclared variable accessed: " + name);
    }

    void defineVar(const string &name, TokenType t) {
        if (scopeStack.empty()) {
            map<string, TokenType> newScope;
            scopeStack.push_back(newScope);
        }
        scopeStack.back()[name] = t;
    }

    bool isNumericType(TokenType t) {
        return t == TokenType::T_INT || t == TokenType::T_FLOAT;
    }

    TokenType analyzeExpr(const ExprPtr &expr) {
        if (!expr) throw TypeException(TypeChkError::EmptyExpression, "Null expression");

        IdentifierExpr* idNode = dynamic_cast<IdentifierExpr*>(expr.get());
        if (idNode != nullptr) return getVarType(idNode->name);

        IntLiteral* intLit = dynamic_cast<IntLiteral*>(expr.get());
        if (intLit != nullptr) return TokenType::T_INT;

        FloatLiteral* floatLit = dynamic_cast<FloatLiteral*>(expr.get());
        if (floatLit != nullptr) return TokenType::T_FLOAT;

        StringLiteral* stringLit = dynamic_cast<StringLiteral*>(expr.get());
        if (stringLit != nullptr) return TokenType::T_STRING;

        BoolLiteral* boolLit = dynamic_cast<BoolLiteral*>(expr.get());
        if (boolLit != nullptr) return TokenType::T_BOOL;

        BinaryExpr* bin = dynamic_cast<BinaryExpr*>(expr.get());
        if (bin != nullptr) {
            TokenType leftT = analyzeExpr(bin->left);
            TokenType rightT = analyzeExpr(bin->right);
            string op = bin->op;

            if (opEquals(op, TokenType::T_PLUS) ||
                opEquals(op, TokenType::T_MINUS) ||
                opEquals(op, TokenType::T_MULT) ||
                opEquals(op, TokenType::T_DIV) ||
                opEquals(op, TokenType::T_MOD)) {

                if (opEquals(op, TokenType::T_PLUS) &&
                    leftT == TokenType::T_STRING &&
                    rightT == TokenType::T_STRING) return TokenType::T_STRING;

                if (!isNumericType(leftT) || !isNumericType(rightT)) {
                    throw TypeException(TypeChkError::AttemptedAddOpOnNonNumeric,
                        "Arithmetic operator used on non-numeric types");
                }

                return (leftT == TokenType::T_FLOAT || rightT == TokenType::T_FLOAT) ? TokenType::T_FLOAT : TokenType::T_INT;
            }

            if (opEquals(op, TokenType::T_EQUALSOP) ||
                opEquals(op, TokenType::T_NOTEQUAL)) {

                if (leftT == rightT) return TokenType::T_BOOL;

                if (isNumericType(leftT) && isNumericType(rightT)) return TokenType::T_BOOL;

                throw TypeException(TypeChkError::ExpressionTypeMismatch,
                                    "Equality comparison between incompatible types");
            }

            if (opEquals(op, TokenType::T_LESS) ||
                opEquals(op, TokenType::T_LESSEQ) ||
                opEquals(op, TokenType::T_GREATER) ||
                opEquals(op, TokenType::T_GREATEREQ)) {

                if (!isNumericType(leftT) || !isNumericType(rightT)) {
                    throw TypeException(TypeChkError::ExpressionTypeMismatch,
                                        "Relational operator used on non-numeric types");
                }
                return TokenType::T_BOOL;
            }

            if (opEquals(op, TokenType::T_AND) || opEquals(op, TokenType::T_OR)) {
                if (leftT != TokenType::T_BOOL || rightT != TokenType::T_BOOL) {
                    throw TypeException(TypeChkError::AttemptedBoolOpOnNonBools,
                        "Boolean operator used on non-boolean operands");
                }
                return TokenType::T_BOOL;
            }

            if (opEquals(op, TokenType::T_BITAND) ||
                opEquals(op, TokenType::T_BITOR) ||
                opEquals(op, TokenType::T_BITXOR)) {
                if (leftT != TokenType::T_INT || rightT != TokenType::T_INT) {
                    throw TypeException(TypeChkError::AttemptedBitOpOnNonNumeric,
                        "Bitwise operator used on non-integer types");
                }
                return TokenType::T_INT;
            }

            if (opEquals(op, TokenType::T_LSHIFT) || opEquals(op, TokenType::T_RSHIFT)) {
                if (leftT != TokenType::T_INT || rightT != TokenType::T_INT) {
                    throw TypeException(TypeChkError::AttemptedShiftOnNonInt,
                        "Shift operator used on non-integer types");
                }
                return TokenType::T_INT;
            }

            throw TypeException(TypeChkError::ExpressionTypeMismatch,
                                string("Unknown binary operation: ") + op);
        }

        CallExpr* call = dynamic_cast<CallExpr*>(expr.get());
        if (call != nullptr) {
            IdentifierExpr* calleeId = dynamic_cast<IdentifierExpr*>(call->callee.get());
            if (calleeId == nullptr) throw TypeException(TypeChkError::ErroneousVarDecl,
                                                        "Function call callee is not an identifier");

            string fname = calleeId->name;
            if (functions.count(fname) == 0) throw TypeException(TypeChkError::ErroneousVarDecl,
                                                                "Undefined function called: " + fname);

            FunctionDecl* fdecl = functions[fname];
            if ((int)call->args.size() != (int)fdecl->params.size()) {
                throw TypeException(TypeChkError::FnCallParamCount,
                    "Function '" + fname + "' expects " + to_string(fdecl->params.size()) +
                    " arguments, got " + to_string(call->args.size()));
            }

            for (int i = 0; i < (int)call->args.size(); ++i) {
                TokenType argType = analyzeExpr(call->args[i]);
                TokenType paramType = typeNameStringToTokenType(fdecl->params[i].typeName);
                if (argType == paramType) continue;
                if (paramType == TokenType::T_FLOAT && argType == TokenType::T_INT) continue;
                throw TypeException(TypeChkError::FnCallParamType,
                    "Parameter type mismatch in call to " + fname);
            }

            return typeNameStringToTokenType(fdecl->returnType);
        }

        throw TypeException(TypeChkError::EmptyExpression, "Unsupported expression node");
    }

    void analyzeStmt(const StmtPtr &stmt, TokenType expectedReturnType) {
        if (!stmt) return;

        BlockStmt* blockNode = dynamic_cast<BlockStmt*>(stmt.get());
        if (blockNode != nullptr) {
            map<string, TokenType> newScope;
            scopeStack.push_back(newScope);
            for (int i = 0; i < (int)blockNode->stmts.size(); ++i) {
                analyzeStmt(blockNode->stmts[i], expectedReturnType);
            }
            scopeStack.pop_back();
            return;
        }

        VarDeclStmt* varDecl = dynamic_cast<VarDeclStmt*>(stmt.get());
        if (varDecl != nullptr) {
            TokenType declaredT = typeNameStringToTokenType(varDecl->typeName);
            if (declaredT == TokenType::T_UNKNOWN) {
                throw TypeException(TypeChkError::ErroneousVarDecl,
                                    "Unknown declared type for variable " + varDecl->ident);
            }
            TokenType initT = analyzeExpr(varDecl->init);
            if (initT == declaredT) {
                // ok
            } else if (declaredT == TokenType::T_FLOAT && initT == TokenType::T_INT) {
                // implicit promotion allowed
            } else {
                throw TypeException(TypeChkError::ErroneousVarDecl,
                                    "Initializer type mismatch for " + varDecl->ident);
            }
            defineVar(varDecl->ident, declaredT);
            return;
        }

        AssignStmt* assignNode = dynamic_cast<AssignStmt*>(stmt.get());
        if (assignNode != nullptr) {
            TokenType varT = getVarType(assignNode->ident);
            TokenType valT = analyzeExpr(assignNode->value);
            if (varT == valT) {
                // ok
            } else if (varT == TokenType::T_FLOAT && valT == TokenType::T_INT) {
                // implicit promotion allowed
            } else {
                throw TypeException(TypeChkError::ExpressionTypeMismatch,
                                    "Assignment type mismatch for " + assignNode->ident);
            }
            return;
        }

        ReturnStmt* returnNode = dynamic_cast<ReturnStmt*>(stmt.get());
        if (returnNode != nullptr) {
            if (returnNode->value != nullptr) {
                TokenType retT = analyzeExpr(returnNode->value);
                if (retT == expectedReturnType) {
                    // ok
                } else if (expectedReturnType == TokenType::T_FLOAT && retT == TokenType::T_INT) {
                    // implicit promotion allowed
                } else {
                    throw TypeException(TypeChkError::ErroneousReturnType, "Return type mismatch");
                }
            }
            return;
        }

        ExprStmt* exprStmt = dynamic_cast<ExprStmt*>(stmt.get());
        if (exprStmt != nullptr) {
            analyzeExpr(exprStmt->expr);
            return;
        }

        IfStmt* ifNode = dynamic_cast<IfStmt*>(stmt.get());
        if (ifNode != nullptr) {
            TokenType condT = analyzeExpr(ifNode->cond);
            if (condT != TokenType::T_BOOL) {
                throw TypeException(TypeChkError::NonBooleanCondStmt, "Non-boolean condition in if statement");
            }
            map<string, TokenType> thenScope;
            scopeStack.push_back(thenScope);
            analyzeStmt(ifNode->thenBody, expectedReturnType);
            scopeStack.pop_back();
            if (ifNode->elseBody) {
                map<string, TokenType> elseScope;
                scopeStack.push_back(elseScope);
                analyzeStmt(ifNode->elseBody, expectedReturnType);
                scopeStack.pop_back();
            }
            return;
        }

        WhileStmt* whileNode = dynamic_cast<WhileStmt*>(stmt.get());
        if (whileNode != nullptr) {
            TokenType condT = analyzeExpr(whileNode->cond);
            if (condT != TokenType::T_BOOL) {
                throw TypeException(TypeChkError::NonBooleanCondStmt, "Non-boolean condition in while statement");
            }
            map<string, TokenType> whileScope;
            scopeStack.push_back(whileScope);
            analyzeStmt(whileNode->body, expectedReturnType);
            scopeStack.pop_back();
            return;
        }

        ForStmt* forNode = dynamic_cast<ForStmt*>(stmt.get());
        if (forNode != nullptr) {
            map<string, TokenType> forScope;
            scopeStack.push_back(forScope);

            if (forNode->init) {
                VarDeclStmt* vd = dynamic_cast<VarDeclStmt*>(forNode->init.get());
                if (vd != nullptr) {
                    analyzeStmt(forNode->init, expectedReturnType);
                } else {
                    analyzeStmt(forNode->init, expectedReturnType);
                }
            }

            if (forNode->condition) {
                TokenType condT = analyzeExpr(forNode->condition);
                if (condT != TokenType::T_BOOL) {
                    throw TypeException(TypeChkError::NonBooleanCondStmt, "Non-boolean condition in for statement");
                }
            }

            if (forNode->update) {
                analyzeStmt(forNode->update, expectedReturnType);
            }

            analyzeStmt(forNode->body, expectedReturnType);

            scopeStack.pop_back();
            return;
        }

        return;
    }

public:
    void analyze(Program &prog) {
        for (int i = 0; i < (int)prog.funcs.size(); ++i) {
            FunctionDecl* f = prog.funcs[i].get();
            if (functions.count(f->name)) throw TypeException(TypeChkError::ErroneousVarDecl, "Function redefinition: " + f->name);
            functions[f->name] = f;
        }

        for (int i = 0; i < (int)prog.funcs.size(); ++i) {
            FunctionDecl* f = prog.funcs[i].get();
            scopeStack.clear();
            map<string, TokenType> topScope;
            scopeStack.push_back(topScope);

            for (int p = 0; p < (int)f->params.size(); ++p) {
                TokenType paramT = typeNameStringToTokenType(f->params[p].typeName);
                scopeStack.back()[f->params[p].ident] = paramT;
            }

            TokenType expectedReturn = typeNameStringToTokenType(f->returnType);
            currentFuncReturn = expectedReturn;

            analyzeStmt(f->body, expectedReturn);

            bool foundReturn = false;
            for (int s = 0; s < (int)f->body->stmts.size(); ++s) {
                ReturnStmt* r = dynamic_cast<ReturnStmt*>(f->body->stmts[s].get());
                if (r != nullptr) {
                    foundReturn = true;
                    break;
                }
            }
            if (!foundReturn && expectedReturn != TokenType::T_UNKNOWN) {
                throw TypeException(TypeChkError::ReturnStmtNotFound,
                                    "Function '" + f->name + "' missing return statement");
            }
        }
    }
};

bool typeChecker(Program prog) {
    try {
        TypeChecker checker;
        checker.analyze(prog);
        cout << "Type checking passed, no errors found.\n";
        return true;
    } catch (const TypeException &ex) {
        cerr << "Type error: " << ex.what() << "\n";
    } catch (const exception &ex) {
        cerr << "Error: " << ex.what() << "\n";
    }
    return false;
}
