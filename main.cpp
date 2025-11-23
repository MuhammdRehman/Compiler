#include "regex.cpp"
#include "IR_Generation.cpp"
#include "C_Backend.cpp"
#include "LLVM_Backend.cpp"
#include <cstdlib>

int main()
{
    string file = "TestFile.txt";
    
    cout << "[1] Running Lexer..." << endl;
    vector<Token> ts = LexerAlgo(file);
    cout << "Tokens generated: " << ts.size() << endl << endl;
    
    cout << "[2] Running Parser..." << endl;
    Program prog = ParserAlgo(ts);
    if(prog.funcs.empty()){
        cerr << "Error: No functions parsed!" << endl;
        return -1;
    }
    cout << "Functions parsed: " << prog.funcs.size() << endl << endl;
    
    cout << "[3] Running Scope Analysis..." << endl;
    if(!ScopeAnalysis(prog)){
        cerr << "    Error: Scope analysis failed!" << endl;
        return -1;
    }
    cout << "    Scope analysis passed!" << endl << endl;
    
    cout << "[4] Running Type Checker..." << endl;
    if(!typeChecker(prog)){
        cerr << "    Error: Type checking failed!" << endl;
        return -1;
    }
    cout << "    Type checking passed!" << endl << endl;
    
    cout << "[5] Generating Three-Address Code (TAC)..." << endl;
    IRModule mod = generateIR(prog);
    cout << "\n--- TAC Output ---" << endl;
    mod.print();
    cout << "--- End TAC ---\n" << endl;
    
    cout << "[6] Generating C Code Backend..." << endl;
    string cCode = generateCCode(mod);
    saveCCode(cCode, "output.c");
    cout << endl;
    
    cout << "[7] Generating LLVM IR Backend..." << endl;
    string llvmIR = generateLLVMIR(mod);
    saveLLVMIR(llvmIR, "output.ll");
    cout << endl;
    
    cout << "[8] Compiling C code to executable..." << endl;
    string compileCmd = "gcc output.c -o output.exe 2>&1";
    int compileResult = system(compileCmd.c_str());
    
    if(compileResult == 0) {
        cout << "    C code compiled successfully!" << endl;
        cout << "    Executable: output.exe" << endl << endl;
        
        cout << "[9] Running the compiled program..." << endl;
        cout << "--- Program Output ---" << endl;
        int runResult = system("output.exe");
        cout << "--- End Output ---" << endl;
        cout << "    Program exited with code: " << runResult << endl << endl;
    } else {
        cout << "    Warning: C compilation failed. Check output.c for issues." << endl;
        cout << "    You can manually compile with: gcc output.c -o output.exe" << endl << endl;
    }
    
    cout << "=== Compilation Complete ===" << endl;
    cout << "  - output.c    (C backend)" << endl;
    cout << "  - output.ll   (LLVM IR backend)" << endl;
    cout << "  - output.exe  (Compiled executable)" << endl;
    
    return 0;
}