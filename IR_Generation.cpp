#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>

#include "typeChecker.cpp"

using namespace std;

struct IrGenException : public runtime_error {
    string msg;
    IrGenException(const string &m) : runtime_error(m), msg(m) {}
};

struct Operand {
    string text;
    Operand() : text("") {}
    Operand(const string &t) : text(t) {}
};

enum class Opcode {
    ASSIGN,
    BINOP,
    LABEL,
    GOTO,
    IFGOTO,
    PARAM,
    CALL,
    RETURN,
    FUNC_BEGIN,
    FUNC_END
};

struct Instruction {
    Opcode op;
    string dst;
    string left;
    string right;
    string extra;

    Instruction(Opcode o = Opcode::ASSIGN)
        : op(o), dst(""), left(""), right(""), extra("") {}
};

static string opcodeName(Opcode o) {
    switch (o) {
        case Opcode::ASSIGN: return "ASSIGN";
        case Opcode::BINOP: return "BINOP";
        case Opcode::LABEL: return "LABEL";
        case Opcode::GOTO: return "GOTO";
        case Opcode::IFGOTO: return "IFGOTO";
        case Opcode::PARAM: return "PARAM";
        case Opcode::CALL: return "CALL";
        case Opcode::RETURN: return "RETURN";
        case Opcode::FUNC_BEGIN: return "FUNC_BEGIN";
        case Opcode::FUNC_END: return "FUNC_END";
    }
    return "OP";
}

struct IRModule {
    vector<Instruction> instrs;

    void push(const Instruction &i) { instrs.push_back(i); }

    void print(ostream &out = cout) const {
        for (int i = 0; i < (int)instrs.size(); i++) {
            const Instruction &ins = instrs[i];
            if (ins.op == Opcode::LABEL) {
                out << ins.left << ":\n";
            }
            else if (ins.op == Opcode::FUNC_BEGIN) {
                out << "func " << ins.extra << ":\n";
            }
            else if (ins.op == Opcode::FUNC_END) {
                out << "endfunc " << ins.extra << "\n";
            }
            else if (ins.op == Opcode::ASSIGN) {
                out << "  " << ins.dst << " = " << ins.left << "\n";
            }
            else if (ins.op == Opcode::BINOP) {
                out << "  " << ins.dst << " = " << ins.left
                    << " " << ins.extra << " " << ins.right << "\n";
            }
            else if (ins.op == Opcode::GOTO) {
                out << "  goto " << ins.left << "\n";
            }
            else if (ins.op == Opcode::IFGOTO) {
                out << "  if " << ins.left << " goto " << ins.right << "\n";
            }
            else if (ins.op == Opcode::PARAM) {
                out << "  param " << ins.left << "\n";
            }
            else if (ins.op == Opcode::CALL) {
                out << "  " << ins.dst << " = call "
                    << ins.extra << ", " << ins.left << "\n";
            }
            else if (ins.op == Opcode::RETURN) {
                if (ins.left == "") out << "  return\n";
                else out << "  return " << ins.left << "\n";
            }
        }
    }
};

class IRGenerator {
    IRModule module;
    int tempCounter;
    int labelCounter;
    map<string,string> currentFunctionLocals;

public:
    IRGenerator() : tempCounter(0), labelCounter(0) {}

    IRModule generate(Program &prog) {
        for (int i = 0; i < (int)prog.funcs.size(); i++) {
            shared_ptr<FunctionDecl> f = prog.funcs[i];
            generateFunction(f);
        }
        return module;
    }

private:
    string newTemp() {
        tempCounter++;
        return string("t") + to_string(tempCounter);
    }

    string newLabel(const string &prefix) {
        labelCounter++;
        return prefix + to_string(labelCounter);
    }

    void generateFunction(shared_ptr<FunctionDecl> f) {
        Instruction ib;
        ib.op = Opcode::FUNC_BEGIN;
        ib.extra = f->name;
        module.push(ib);

        currentFunctionLocals.clear();
        for (int p = 0; p < (int)f->params.size(); p++) {
            string pname = f->params[p].ident;
            currentFunctionLocals[pname] = pname;
        }

        generateStmt(f->body);

        Instruction ie;
        ie.op = Opcode::FUNC_END;
        ie.extra = f->name;
        module.push(ie);
    }

    void generateStmt(StmtPtr s) {
        if (!s) return;

        BlockStmt* bs = dynamic_cast<BlockStmt*>(s.get());
        if (bs != NULL) {
            for (int i = 0; i < (int)bs->stmts.size(); i++)
                generateStmt(bs->stmts[i]);
            return;
        }

        VarDeclStmt* vd = dynamic_cast<VarDeclStmt*>(s.get());
        if (vd != NULL) {
            string rhs = generateExpr(vd->init);
            currentFunctionLocals[vd->ident] = vd->ident;

            Instruction i;
            i.op = Opcode::ASSIGN;
            i.dst = vd->ident;
            i.left = rhs;
            module.push(i);
            return;
        }

        AssignStmt* asg = dynamic_cast<AssignStmt*>(s.get());
        if (asg != NULL) {
            string rhs = generateExpr(asg->value);
            Instruction i;
            i.op = Opcode::ASSIGN;
            i.dst = asg->ident;
            i.left = rhs;
            module.push(i);
            return;
        }

        ReturnStmt* ret = dynamic_cast<ReturnStmt*>(s.get());
        if (ret != NULL) {
            Instruction i;
            i.op = Opcode::RETURN;
            if (ret->value != NULL)
                i.left = generateExpr(ret->value);
            module.push(i);
            return;
        }

        ExprStmt* es = dynamic_cast<ExprStmt*>(s.get());
        if (es != NULL) {
            generateExpr(es->expr);
            return;
        }

        IfStmt* ifs = dynamic_cast<IfStmt*>(s.get());
        if (ifs != NULL) {
            string condTemp = generateExpr(ifs->cond);
            string labelThen = newLabel("L_then_");
            string labelElse = newLabel("L_else_");
            string labelEnd = newLabel("L_end_");

            Instruction ic;
            ic.op = Opcode::IFGOTO;
            ic.left = condTemp;
            ic.right = labelThen;
            module.push(ic);

            if (ifs->elseBody != NULL) {
                Instruction ig;
                ig.op = Opcode::GOTO;
                ig.left = labelElse;
                module.push(ig);

                Instruction il;
                il.op = Opcode::LABEL;
                il.left = labelThen;
                module.push(il);

                generateStmt(ifs->thenBody);

                Instruction ig2;
                ig2.op = Opcode::GOTO;
                ig2.left = labelEnd;
                module.push(ig2);

                Instruction il2;
                il2.op = Opcode::LABEL;
                il2.left = labelElse;
                module.push(il2);

                generateStmt(ifs->elseBody);

                Instruction il3;
                il3.op = Opcode::LABEL;
                il3.left = labelEnd;
                module.push(il3);
            } else {
                Instruction il;
                il.op = Opcode::LABEL;
                il.left = labelThen;
                module.push(il);

                generateStmt(ifs->thenBody);

                Instruction il2;
                il2.op = Opcode::LABEL;
                il2.left = labelEnd;
                module.push(il2);
            }
            return;
        }

        WhileStmt* ws = dynamic_cast<WhileStmt*>(s.get());
        if (ws != NULL) {
            string labelCond = newLabel("L_while_cond_");
            string labelBody = newLabel("L_while_body_");
            string labelEnd = newLabel("L_while_end_");

            Instruction igoto;
            igoto.op = Opcode::GOTO;
            igoto.left = labelCond;
            module.push(igoto);

            Instruction ibody;
            ibody.op = Opcode::LABEL;
            ibody.left = labelBody;
            module.push(ibody);

            generateStmt(ws->body);

            Instruction icond;
            icond.op = Opcode::LABEL;
            icond.left = labelCond;
            module.push(icond);

            string condTemp = generateExpr(ws->cond);

            Instruction ifg;
            ifg.op = Opcode::IFGOTO;
            ifg.left = condTemp;
            ifg.right = labelBody;
            module.push(ifg);

            Instruction iend;
            iend.op = Opcode::LABEL;
            iend.left = labelEnd;
            module.push(iend);

            return;
        }

        ForStmt* fs = dynamic_cast<ForStmt*>(s.get());
        if (fs != NULL) {
            string labelCond = newLabel("L_for_cond_");
            string labelBody = newLabel("L_for_body_");
            string labelEnd = newLabel("L_for_end_");

            if (fs->init != NULL) generateStmt(fs->init);

            Instruction ig;
            ig.op = Opcode::GOTO;
            ig.left = labelCond;
            module.push(ig);

            Instruction ilbody;
            ilbody.op = Opcode::LABEL;
            ilbody.left = labelBody;
            module.push(ilbody);

            generateStmt(fs->body);

            if (fs->update != NULL) generateStmt(fs->update);

            Instruction ilcond;
            ilcond.op = Opcode::LABEL;
            ilcond.left = labelCond;
            module.push(ilcond);

            if (fs->condition != NULL) {
                string condTemp = generateExpr(fs->condition);
                Instruction ifg;
                ifg.op = Opcode::IFGOTO;
                ifg.left = condTemp;
                ifg.right = labelBody;
                module.push(ifg);
            } else {
                Instruction ifg;
                ifg.op = Opcode::GOTO;
                ifg.left = labelBody;
                module.push(ifg);
            }

            Instruction iend;
            iend.op = Opcode::LABEL;
            iend.left = labelEnd;
            module.push(iend);

            return;
        }

        throw IrGenException("Unhandled statement node");
    }

    string generateExpr(ExprPtr e) {
        if (!e) throw IrGenException("Null expression");

        IdentifierExpr* id = dynamic_cast<IdentifierExpr*>(e.get());
        if (id != NULL) return id->name;

        IntLiteral* il = dynamic_cast<IntLiteral*>(e.get());
        if (il != NULL) return il->val;

        FloatLiteral* fl = dynamic_cast<FloatLiteral*>(e.get());
        if (fl != NULL) return fl->val;

        StringLiteral* sl = dynamic_cast<StringLiteral*>(e.get());
        if (sl != NULL) return string("\"") + sl->val + string("\"");

        BoolLiteral* bl = dynamic_cast<BoolLiteral*>(e.get());
        if (bl != NULL) return bl->val;

        BinaryExpr* be = dynamic_cast<BinaryExpr*>(e.get());
        if (be != NULL) {
            string leftOp = generateExpr(be->left);
            string rightOp = generateExpr(be->right);
            string temp = newTemp();

            Instruction i;
            i.op = Opcode::BINOP;
            i.dst = temp;
            i.left = leftOp;
            i.right = rightOp;
            i.extra = be->op;
            module.push(i);

            return temp;
        }

        CallExpr* ce = dynamic_cast<CallExpr*>(e.get());
        if (ce != NULL) {
            int argc = (int)ce->args.size();
            for (int i = 0; i < argc; i++) {
                string a = generateExpr(ce->args[i]);
                Instruction ip;
                ip.op = Opcode::PARAM;
                ip.left = a;
                module.push(ip);
            }

            IdentifierExpr* calleeId =
                dynamic_cast<IdentifierExpr*>(ce->callee.get());
            if (calleeId == NULL)
                throw IrGenException("Call target must be identifier");

            string fname = calleeId->name;
            string dest = newTemp();

            Instruction ic;
            ic.op = Opcode::CALL;
            ic.dst = dest;
            ic.left = to_string(argc);
            ic.extra = fname;
            module.push(ic);

            return dest;
        }

        throw IrGenException("Unhandled expression node");
    }
};

IRModule generateIR(Program &prog) {
    IRGenerator g;
    return g.generate(prog);
}
