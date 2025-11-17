#include <string>
#include <map>
#include<vector>
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
};

struct ScopeException : public runtime_error {
    ScopeError err;
    string msg;
    ScopeException(ScopeError e, string m) : runtime_error(m), err(e), msg(m) {}
};
/* -------------------------

 spaghetti stack 
   ------------------------- */
class ScopeAnalyzer {
    map<string, bool> functions; 
    vector<map<string, bool>> scopeStack; 



    void checkVarDefined(const string& name, const vector<map<string, bool>>& stack) const 
    {
        for (vector<map<string, bool>>::const_reverse_iterator it = stack.rbegin(); it != stack.rend(); ++it) {
    
            if (it->count(name)) return;
        }
    
        throw ScopeException(ScopeError::UndeclaredVariableAccessed, "not ddeclared variable has accessed: " + name);
    }

    void analyzeExpr(const ExprPtr& expr, const vector<map<string, bool>>& stack)
    {
        if (shared_ptr<IdentifierExpr> id = dynamic_pointer_cast<IdentifierExpr>(expr)) {
        checkVarDefined(id->name, stack);
        } else if (shared_ptr<BinaryExpr> bin = dynamic_pointer_cast<BinaryExpr>(expr)) {
            analyzeExpr(bin->left, stack);
            analyzeExpr(bin->right, stack);
        } else if (shared_ptr<CallExpr> call = dynamic_pointer_cast<CallExpr>(expr)) {
            if (shared_ptr<IdentifierExpr> id = dynamic_pointer_cast<IdentifierExpr>(call->callee)) {
                if (functions.find(id->name) == functions.end()) {
                    throw ScopeException(ScopeError::UndefinedFunctionCalled, "undefined function called: " + id->name);
                }
            } // Assume callee is identifier; in full impl
            for (const ExprPtr& a : call->args) {
                analyzeExpr(a, stack);
            }
        } else if (dynamic_pointer_cast<IntLiteral>(expr)) {
            // OK
        } else if (dynamic_pointer_cast<FloatLiteral>(expr)) {
            // OK
        } else if (dynamic_pointer_cast<StringLiteral>(expr)) {
            // OK
        } else if (dynamic_pointer_cast<BoolLiteral>(expr)) {
            // OK
        } 
    }
    void analyzeStmt(const StmtPtr& stmt, vector<map<string, bool>>& stack)
    {
        if (shared_ptr<BlockStmt> block = dynamic_pointer_cast<BlockStmt>(stmt)) {
        analyzeBlock(block, stack);
        } else if (shared_ptr<VarDeclStmt> vdecl = dynamic_pointer_cast<VarDeclStmt>(stmt)) {
            if (stack.back().count(vdecl->ident)) {
                throw ScopeException(ScopeError::VariableRedefinition, "Variable redefinition: " + vdecl->ident);
            }
            stack.back()[vdecl->ident] = true;
            analyzeExpr(vdecl->init, stack);
        } else if (shared_ptr<AssignStmt> assign = dynamic_pointer_cast<AssignStmt>(stmt)) {
            checkVarDefined(assign->ident, stack);
            analyzeExpr(assign->value, stack);
        } else if (shared_ptr<ReturnStmt> ret = dynamic_pointer_cast<ReturnStmt>(stmt)) {
            if (ret->value) {
                analyzeExpr(ret->value, stack);
            }
        } else if (shared_ptr<ExprStmt> estmt = dynamic_pointer_cast<ExprStmt>(stmt)) {
            analyzeExpr(estmt->expr, stack);
        } 
    }

    void analyzeBlock(const shared_ptr<BlockStmt>& block, vector<map<string, bool>>& stack) {
        stack.push_back({});
        for (const StmtPtr& s : block->stmts) {
            analyzeStmt(s, stack);
        }
        stack.pop_back();
    }

    void analyzeFunction(const shared_ptr<FunctionDecl>& func) {
        scopeStack.clear();
        scopeStack.push_back({}); // Function's local scope

        for (const Param& p : func->params) {
            if (scopeStack.back().count(p.ident)) {
                throw ScopeException(ScopeError::VariableRedefinition, "params redefinition: " + p.ident);
            }
            scopeStack.back()[p.ident] = true;
        }

        // Analyze body
        analyzeBlock(func->body, scopeStack);
    }

public:
    void analyze(const Program& prog) {
        for (const shared_ptr<FunctionDecl>& f : prog.funcs) {
            if (functions.count(f->name)) {
                throw ScopeException(ScopeError::FunctionPrototypeRedefinition, "func redefinition: " + f->name);
            }
            functions[f->name] = true;
        }

        for (const shared_ptr<FunctionDecl>& f : prog.funcs) {
            analyzeFunction(f);
        }
    }
};



void ScopeAnalysis(Program prog){
    try{
        ScopeAnalyzer analyzer;
        analyzer.analyze(prog);
        cout << "Scope analysis passed done no error was there......\n";
    }
    catch (const ScopeException &e) 
    {
        cerr << "scopee error: ";
        switch (e.err) {
            case ScopeError::UndeclaredVariableAccessed: cerr << "UndeclaredVariableAccessed"; break;
            case ScopeError::UndefinedFunctionCalled: cerr << "UndefinedFunctionCalled"; break;
            case ScopeError::VariableRedefinition: cerr << "VariableRedefinition"; break;
            case ScopeError::FunctionPrototypeRedefinition: cerr << "FunctionPrototypeRedefinition"; break;
        }
        cerr << ". Msg: " << e.what() << "\n";
    } 
    catch (const exception &ex) 
    {
        cerr << "Error: " << ex.what() << "\n";
    }

}