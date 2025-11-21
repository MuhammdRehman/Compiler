#include "regex.cpp"
#include "typeChecker.cpp"


int main()
{
    string file = "TestFile.txt";
    vector<Token> ts = LexerAlgo(file);
    Program prog = ParserAlgo(ts);
    ScopeAnalysis(prog);
    typeChecker(prog);
    
    return 0;
}