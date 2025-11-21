#include <string>
#include <map>
#include <vector>
#include "parser.cpp"
using namespace std;

/* -------------------------
   ScopeError As defined in assignment
   ------------------------- */
enum class ScopeError {
    UndeclaredVariableAccessed,
    UndefinedFunctionCalled,
    VariableRedefinition,
    FunctionPrototypeRedefinition,
    InvalidBreakUsage
};

struct ScopeException : public runtime_error {
    ScopeError err;
    string msg;
    ScopeException(ScopeError e, string m) : runtime_error(m), err(e), msg(m) {}
};

/* -------------------------
   Scope Analyzer
   ------------------------- */
class ScopeAnalyzer {
    map<string, bool> functions;
    vector<map<string, bool>> scopeStack;
    int loopDepth = 0;  // To track if we're inside a loop for break statements

    void checkVarDefined(const string& name) const {
        vector<map<string,bool>>::const_reverse_iterator it;
        for (it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if (it->count(name)) return;
        }
        throw ScopeException(ScopeError::UndeclaredVariableAccessed,
                             "Variable accessed but not declared: " + name);
    }

    void analyzeExpr(const ExprPtr& expr) {
        if (shared_ptr<IdentifierExpr> id = dynamic_pointer_cast<IdentifierExpr>(expr)) {
            checkVarDefined(id->name);
        } else if (shared_ptr<BinaryExpr> bin = dynamic_pointer_cast<BinaryExpr>(expr)) {
            analyzeExpr(bin->left);
            analyzeExpr(bin->right);
        } else if (shared_ptr<CallExpr> call = dynamic_pointer_cast<CallExpr>(expr)) {
            if (shared_ptr<IdentifierExpr> id = dynamic_pointer_cast<IdentifierExpr>(call->callee)) {
                if (functions.find(id->name) == functions.end()) {
                    throw ScopeException(ScopeError::UndefinedFunctionCalled,
                                         "Undefined function called: " + id->name);
                }
            }
            vector<ExprPtr>::const_iterator ait;
            for (ait = call->args.begin(); ait != call->args.end(); ++ait) {
                analyzeExpr(*ait);
            }
        } else if (dynamic_pointer_cast<IntLiteral>(expr) ||
                   dynamic_pointer_cast<FloatLiteral>(expr) ||
                   dynamic_pointer_cast<StringLiteral>(expr) ||
                   dynamic_pointer_cast<BoolLiteral>(expr)) {
            return; // literals ok
        }
    }

    void analyzeStmt(const StmtPtr& stmt) {
        if (shared_ptr<BlockStmt> block = dynamic_pointer_cast<BlockStmt>(stmt)) {
            analyzeBlock(block);
        } else if (shared_ptr<VarDeclStmt> vdecl = dynamic_pointer_cast<VarDeclStmt>(stmt)) {
            if (scopeStack.back().count(vdecl->ident))
                throw ScopeException(ScopeError::VariableRedefinition,
                                     "Variable redefinition: " + vdecl->ident);
            scopeStack.back()[vdecl->ident] = true;
            analyzeExpr(vdecl->init);
        } else if (shared_ptr<AssignStmt> assign = dynamic_pointer_cast<AssignStmt>(stmt)) {
            checkVarDefined(assign->ident);
            analyzeExpr(assign->value);
        } else if (shared_ptr<ReturnStmt> ret = dynamic_pointer_cast<ReturnStmt>(stmt)) {
            if (ret->value) analyzeExpr(ret->value);
        } else if (shared_ptr<ExprStmt> estmt = dynamic_pointer_cast<ExprStmt>(stmt)) {
            analyzeExpr(estmt->expr);
        } else if (shared_ptr<IfStmt> ifstmt = dynamic_pointer_cast<IfStmt>(stmt)) {
            analyzeExpr(ifstmt->cond);
            analyzeStmt(ifstmt->thenBody);
            if (ifstmt->elseBody) analyzeStmt(ifstmt->elseBody);
        } else if (shared_ptr<WhileStmt> whilestmt = dynamic_pointer_cast<WhileStmt>(stmt)) {
            analyzeExpr(whilestmt->cond);
            loopDepth++;
            analyzeStmt(whilestmt->body);
            loopDepth--;
        } else if (shared_ptr<ForStmt> forstmt = dynamic_pointer_cast<ForStmt>(stmt)) {
            scopeStack.push_back({});
            loopDepth++;
            if (forstmt->init) analyzeStmt(forstmt->init);
            if (forstmt->condition) analyzeExpr(forstmt->condition);
            if (forstmt->update) analyzeStmt(forstmt->update);
            if (forstmt->body) analyzeStmt(forstmt->body);
            loopDepth--;
            scopeStack.pop_back();
        } else if (dynamic_pointer_cast<BreakStmt>(stmt)) {
            if (loopDepth == 0)
                throw ScopeException(ScopeError::InvalidBreakUsage,
                                     "Break statement used outside of loop");
        }
    }

    void analyzeBlock(const shared_ptr<BlockStmt>& block) {
        scopeStack.push_back({});
        vector<StmtPtr>::const_iterator it;
        for (it = block->stmts.begin(); it != block->stmts.end(); ++it) {
            analyzeStmt(*it);
        }
        scopeStack.pop_back();
    }

    void analyzeFunction(const shared_ptr<FunctionDecl>& func) {
        scopeStack.clear();
        scopeStack.push_back({}); // function scope
        vector<Param>::const_iterator pit;
        for (pit = func->params.begin(); pit != func->params.end(); ++pit) {
            if (scopeStack.back().count(pit->ident))
                throw ScopeException(ScopeError::VariableRedefinition,
                                     "Parameter redefinition: " + pit->ident);
            scopeStack.back()[pit->ident] = true;
        }
        analyzeBlock(func->body);
    }

public:
    void analyze(const Program& prog) {
        vector<shared_ptr<FunctionDecl>>::const_iterator fit;
        for (fit = prog.funcs.begin(); fit != prog.funcs.end(); ++fit) {
            if (functions.count((*fit)->name))
                throw ScopeException(ScopeError::FunctionPrototypeRedefinition,
                                     "Function redefinition: " + (*fit)->name);
            functions[(*fit)->name] = true;
        }
        for (fit = prog.funcs.begin(); fit != prog.funcs.end(); ++fit) {
            analyzeFunction(*fit);
        }
    }
};

void ScopeAnalysis(const Program& prog) {
    try {
        ScopeAnalyzer analyzer;
        analyzer.analyze(prog);
        cout << "Scope analysis passed: no errors detected.\n";
    } catch (const ScopeException& e) {
        cerr << "Scope error: ";
        switch (e.err) {
            case ScopeError::UndeclaredVariableAccessed: cerr << "UndeclaredVariableAccessed"; break;
            case ScopeError::UndefinedFunctionCalled: cerr << "UndefinedFunctionCalled"; break;
            case ScopeError::VariableRedefinition: cerr << "VariableRedefinition"; break;
            case ScopeError::FunctionPrototypeRedefinition: cerr << "FunctionPrototypeRedefinition"; break;
            case ScopeError::InvalidBreakUsage: cerr << "InvalidBreakUsage"; break;
        }
        cerr << ". Msg: " << e.what() << "\n";
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }
}
