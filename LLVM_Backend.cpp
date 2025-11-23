#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

struct IRModule;
struct Instruction;
enum class Opcode;

class LLVMCodeGenerator {
private:
    ostringstream llvmCode;
    map<string, string> varToLLVMReg;
    int regCounter;
    string currentFunction;
    
    string getLLVMReg(const string& varName) {
        if (varName.empty()) return "";
        
        if (isdigit(varName[0]) || varName[0] == '-' || varName[0] == '"') {
            return varName;
        }
        
        if (varToLLVMReg.find(varName) == varToLLVMReg.end()) {
            string reg = "%" + varName;
            varToLLVMReg[varName] = reg;
            return reg;
        }
        return varToLLVMReg[varName];
    }
    
    string newLLVMReg() {
        return "%" + to_string(++regCounter);
    }
    
    string getOperator(const string& op) {
        if (op == "+") return "add";
        if (op == "-") return "sub";
        if (op == "*") return "mul";
        if (op == "/") return "sdiv";
        if (op == "%") return "srem";
        if (op == "==") return "icmp eq";
        if (op == "!=") return "icmp ne";
        if (op == "<") return "icmp slt";
        if (op == "<=") return "icmp sle";
        if (op == ">") return "icmp sgt";
        if (op == ">=") return "icmp sge";
        if (op == "&&") return "and";
        if (op == "||") return "or";
        return "add"; // default
    }
    
    bool isComparisonOp(const string& op) {
        return op == "==" || op == "!=" || op == "<" || 
               op == "<=" || op == ">" || op == ">=";
    }

public:
    LLVMCodeGenerator() : regCounter(0) {}
    
    string generate(const IRModule& module);
    
    void generateHeader() {
        llvmCode << "; LLVM IR Generated from Custom Compiler\n";
        llvmCode << "; Target: x86_64-pc-windows-msvc\n\n";
        
        llvmCode << "declare i32 @printf(i8*, ...)\n";
        llvmCode << "@.str = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\", align 1\n\n";
    }
    
    void generateInstruction(const Instruction& ins);
};

string LLVMCodeGenerator::generate(const IRModule& module) {
    generateHeader();
    
    for (const Instruction& ins : module.instrs) {
        generateInstruction(ins);
    }
    
    return llvmCode.str();
}

void LLVMCodeGenerator::generateInstruction(const Instruction& ins) {
    switch (ins.op) {
        case Opcode::FUNC_BEGIN: {
            currentFunction = ins.extra;
            varToLLVMReg.clear();
            regCounter = 0;
            
            if (ins.extra == "main") {
                llvmCode << "define i32 @main() {\n";
            } else {
                llvmCode << "define i32 @" << ins.extra << "(";
                llvmCode << ") {\n";
            }
            llvmCode << "entry:\n";
            break;
        }
        
        case Opcode::FUNC_END: {
            if (currentFunction == "main") {
                llvmCode << "  ret i32 0\n";
            }
            llvmCode << "}\n\n";
            break;
        }
        
        case Opcode::LABEL: {
            llvmCode << ins.left << ":\n";
            break;
        }
        
        case Opcode::ASSIGN: {
            string dest = getLLVMReg(ins.dst);
            string src = getLLVMReg(ins.left);
            
            llvmCode << "  " << dest << " = alloca i32, align 4\n";
            llvmCode << "  store i32 " << src << ", i32* " << dest << ", align 4\n";
            break;
        }
        
        case Opcode::BINOP: {
            string dest = getLLVMReg(ins.dst);
            string left = getLLVMReg(ins.left);
            string right = getLLVMReg(ins.right);
            string op = getOperator(ins.extra);
            
            if (isComparisonOp(ins.extra)) {
                llvmCode << "  " << dest << " = " << op << " i32 " 
                         << left << ", " << right << "\n";
                string extReg = newLLVMReg();
                llvmCode << "  " << extReg << " = zext i1 " << dest << " to i32\n";
                varToLLVMReg[ins.dst] = extReg;
            } else {
                llvmCode << "  " << dest << " = " << op << " i32 " 
                         << left << ", " << right << "\n";
            }
            break;
        }
        
        case Opcode::GOTO: {
            llvmCode << "  br label %" << ins.left << "\n";
            break;
        }
        
        case Opcode::IFGOTO: {
            string cond = getLLVMReg(ins.left);
            string trueLabel = ins.right;
            
            string condBool = newLLVMReg();
            llvmCode << "  " << condBool << " = icmp ne i32 " << cond << ", 0\n";
            llvmCode << "  br i1 " << condBool << ", label %" << trueLabel 
                     << ", label %next_" << regCounter << "\n";
            llvmCode << "next_" << regCounter << ":\n";
            break;
        }
        
        case Opcode::RETURN: {
            if (ins.left.empty()) {
                llvmCode << "  ret i32 0\n";
            } else {
                string val = getLLVMReg(ins.left);
                llvmCode << "  ret i32 " << val << "\n";
            }
            break;
        }
        
        case Opcode::CALL: {
            string dest = getLLVMReg(ins.dst);
            string fname = ins.extra;
            
            llvmCode << "  " << dest << " = call i32 @" << fname << "(";
            llvmCode << ")\n";
            break;
        }
        
        case Opcode::PARAM: {
            break;
        }
        
        default:
            llvmCode << "  ; Unknown instruction\n";
    }
}

// Function to save LLVM IR to file
void saveLLVMIR(const string& llvmCode, const string& filename) {
    ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << llvmCode;
        outFile.close();
        cout << "LLVM IR saved to: " << filename << endl;
    } else {
        cerr << "Error: Could not save LLVM IR to file" << endl;
    }
}

string generateLLVMIR(const IRModule& module) {
    LLVMCodeGenerator generator;
    return generator.generate(module);
}
