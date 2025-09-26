#include "regex.cpp"
#include "parser.cpp"

int main()
{
    string file = "TestFile.txt";
    vector<Token> ts = LexerAlgo(file);
    ParserAlgo(ts);
    

    return 0;
}