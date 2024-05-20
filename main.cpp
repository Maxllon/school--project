#include "interpretator.cpp"
int main()
{
    std::cout << "Enter filename: ";
    str filename;
    std::cin >> filename;
    std::cout << std::endl;
    lexArray *lxRes = lexing(filename);
    for (auto &token : *lxRes)
    {
        std::cout << token.type << " ||| " << token.value << std::endl;
    }
    tkman.lx = lxRes;
    setPrecedence();
    Expretion *Tree = parce();
    outTree(Tree, "");
    Environment *global = new Environment;
    global->tree = Tree;
    global->stInterpret();
    std::cin >> filename;

    return 0;
}