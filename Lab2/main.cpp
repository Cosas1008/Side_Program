#include "parser.h"


int main(int argc, char** argv)
{
    if(argc < 2){
        printf("Usage: ./Lab2 <.in>\n");
        return 0;
    }

    parser_S* pParser;
    if (argc == 2) {
        pParser = new parser_S(argv[1]);
        pParser->parse();
        pParser->dump();
    } else if (argc == 3) { //verifier
        pParser = new parser_S(argv[1], argv[2]);
        if (!pParser->parse()) {
            return 0;
        }
        pParser->dump();
    }

    /* Start your code */
    // 1. find minimum mean cycle
    // 2. dump minimum mean weight and minimum mean cycle in "solution.txt"

    delete pParser;
    return 0;
}
