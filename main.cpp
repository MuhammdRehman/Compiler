#include "regex.cpp"
#include "IR_Generation.cpp"


int main()
{
    string file = "TestFile.txt";
    vector<Token> ts = LexerAlgo(file);
    Program prog = ParserAlgo(ts);
    ScopeAnalysis(prog);
    typeChecker(prog);
    IRModule mod = generateIR(prog);
    mod.print();
    return 0;
}