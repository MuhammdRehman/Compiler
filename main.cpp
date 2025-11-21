#include "regex.cpp"
#include "IR_Generation.cpp"


int main()
{
    string file = "TestFile.txt";
    vector<Token> ts = LexerAlgo(file);
    Program prog = ParserAlgo(ts);
    if(prog.funcs.empty()){
        return -1;
    }
    if(!ScopeAnalysis(prog)){
        return -1;
    }
    if(!typeChecker(prog)){
        return -1;
    }
    IRModule mod = generateIR(prog);
    mod.print();
    return 0;
}