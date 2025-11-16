#include "regex.cpp"
#include "scopeAnalysis.cpp"

int main()
{
    string file = "TestFile.txt";
    vector<Token> ts = LexerAlgo(file);
    Program prog = ParserAlgo(ts);
    ScopeAnalysis(prog);
    
    return 0;
}