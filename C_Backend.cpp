#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <vector>

using namespace std;

// forward declarations
struct IRModule;
struct Instruction;
enum class Opcode;

class CCodeGenerator {
private:
    ostringstream cCode;
    map<string, set<string>> functionVars;
    map<string, set<string>> stringVars;


    string currentFunction;
    bool inFunction;
    vector<string> paramQueue;

    bool isConstant(const string& str) 
    {
        if (str.empty()) return false;
    
        if (str[0] == '"') return true;
    
        if (isdigit(str[0])) return true;
    
        if (str[0] == '-' && str.length() > 1 && isdigit(str[1])) return true;
    
        if (str == "true" || str == "false") return true;
    
        return false;
    }
    
    bool isStringLiteral(const string& str) 
    {
        return !str.empty() && str[0] == '"';
    }

    string sanitizeLabelName(const string& label) {
        string result = label;
 
        for (char& c : result) {
 
            if (!isalnum(c)) c = '_';
        }
 
        return result;
    }
    
    void collectVariable(const string& varName) {
 
        if (varName.empty() || isConstant(varName)) return;
 
        if (currentFunction.empty()) return;


        // doing this so that we dont add labels as variables
        if (varName.find("L_") == 0) return;
 
        functionVars[currentFunction].insert(varName);
    }

public:
    CCodeGenerator() : inFunction(false) {}

    string generate(const IRModule& module) {
        cCode << "#include <stdio.h>\n";
        cCode << "#include <stdlib.h>\n";
        cCode << "#include <string.h>\n\n";

        // pass-01: collecting all variables per function
        currentFunction = "";
        for (size_t i = 0; i < module.instrs.size(); i++) {
            const Instruction& ins = module.instrs[i];
            
            if (ins.op == Opcode::FUNC_BEGIN) {
                currentFunction = ins.extra;
            } else if (ins.op == Opcode::FUNC_END) {
                currentFunction = "";
            } else if (!currentFunction.empty()) {
                collectVariable(ins.dst);
                collectVariable(ins.left);
                collectVariable(ins.right);
                
                if (ins.op == Opcode::ASSIGN && isStringLiteral(ins.left)) {
                    stringVars[currentFunction].insert(ins.dst);
                }
            }
        }

        cCode << "// Forward declarations\n";
        set<string> forwardDeclared;
        for (const Instruction& ins : module.instrs) {
            if (ins.op == Opcode::FUNC_BEGIN && ins.extra != "main") {
                if (forwardDeclared.find(ins.extra) == forwardDeclared.end()) {
                    cCode << "int " << ins.extra << "(";
                    for (size_t p = 0; p < ins.params.size(); p++) {
                        if (p > 0) cCode << ", ";
                        cCode << "int " << ins.params[p];
                    }
                    cCode << ");\n";
                    forwardDeclared.insert(ins.extra);
                }
            }
        }
        cCode << "\n";

        for (size_t i = 0; i < module.instrs.size(); i++) {
            generateInstruction(module.instrs[i], module, i);
        }

        return cCode.str();
    }

    void generateInstruction(const Instruction& ins, const IRModule& module, int idx) {
        switch (ins.op) {
            case Opcode::FUNC_BEGIN: {
                currentFunction = ins.extra;
                paramQueue.clear();
                inFunction = true;

                cCode << "int " << ins.extra << "(";
                // func parameters
                for (size_t p = 0; p < ins.params.size(); p++) {
                    if (p > 0) cCode << ", ";
                    cCode << "int " << ins.params[p];
                }
                cCode << ") {\n";

                if (functionVars.find(currentFunction) != functionVars.end()) {
                    for (const string& var : functionVars[currentFunction]) {
                        bool isParam = false;
                        for (const string& p : ins.params) {
                            if (var == p) {
                                isParam = true;
                                break;
                            }
                        }
                        if (!isParam) {

                            bool isString = false;
                            if (stringVars.find(currentFunction) != stringVars.end()) {
                                if (stringVars[currentFunction].find(var) != stringVars[currentFunction].end()) {
                                    isString = true;
                                }
                            }
                            
                            if (isString) {
                                cCode << "char* " << var << ";\n";
                            } else {
                                cCode << "int " << var << ";\n";
                            }
                        }
                    }
                    if (!functionVars[currentFunction].empty()) {
                        cCode << "\n";
                    }
                }
                break;
            }

            case Opcode::FUNC_END: {
                if (currentFunction == "main") {
                    cCode << "    return 0;\n";
                }
                cCode << "}\n\n";
                inFunction = false;
                currentFunction = "";
                break;
            }

            case Opcode::LABEL: {
                string label = sanitizeLabelName(ins.left);
                cCode << label << ":\n";
                break;
            }

            case Opcode::ASSIGN: {
                string value = ins.left;

                if (value.length() > 4 && value[0] == '"' && value[1] == '"') {
                    value = value.substr(1, value.length() - 2);
                }
                cCode << "    " << ins.dst << " = " << value << ";\n";
                break;
            }

            case Opcode::BINOP: {
                cCode << "    " << ins.dst << " = " << ins.left << " " 
                      << ins.extra << " " << ins.right << ";\n";
                break;
            }

            case Opcode::GOTO: {
                string label = sanitizeLabelName(ins.left);
                cCode << "    goto " << label << ";\n";
                break;
            }

            case Opcode::IFGOTO: {
                string label = sanitizeLabelName(ins.right);
                cCode << "    if (" << ins.left << ") goto " << label << ";\n";
                break;
            }

            case Opcode::RETURN: {
                if (ins.left.empty()) {
                    cCode << "    return 0;\n";
                } else {
                    cCode << "    return " << ins.left << ";\n";
                }
                break;
            }

            case Opcode::PARAM: {
                paramQueue.push_back(ins.left);
                break;
            }

            case Opcode::CALL: {
                cCode << "    " << ins.dst << " = " << ins.extra << "(";
                
                for (size_t i = 0; i < paramQueue.size(); i++) {
                    if (i > 0) cCode << ", ";
                    cCode << paramQueue[i];
                }
                paramQueue.clear();
                
                cCode << ");\n";
                break;
            }

            default:
                cCode << "// Unknown instruction\n";
        }
    }
};

void saveCCode(const string& cCode, const string& filename) {
    ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << cCode;
        outFile.close();
        cout << "C code saved to: " << filename << endl;
    } else {
        cerr << "not save C code to file" << endl;
    }
}

string generateCCode(const IRModule& module) {
    CCodeGenerator generator;
    return generator.generate(module);
}
