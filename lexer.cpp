#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include <cstdlib>

// start lex
using str = std::string;

str readTxt(str filename)
{
    str out{""};
    std::ifstream in(filename);
    str line;
    while (std::getline(in, line))
    {
        out += line;
    }
    in.close();
    return out;
}

enum class symbls
{
    LETTER = 'L',
    DIGIT = 'D',
    SYMBOL = 'S',
    BINNARY = 'B'
};
struct SYMBL
{
    symbls type;
    char value;
};
using chStream = std::vector<SYMBL>;
const str letters{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"};
const str digits{"0123456789."};
const str binary{"-=+!<>/*||&&%"};
const str spec("(){};,");
chStream *lexSymbls(chStream *sStream, str file)
{
    for (auto &sym : file)
    {
        if (letters.find(sym) != std::string::npos)
            sStream->push_back({symbls::LETTER, sym});
        else if (digits.find(sym) != std::string::npos)
            sStream->push_back({symbls::DIGIT, sym});
        else if (binary.find(sym) != std::string::npos)
            sStream->push_back({symbls::BINNARY, sym});
        else
            sStream->push_back({symbls::SYMBOL, sym});
    }
    return sStream;
}
struct Token
{
    str type;
    str value;
};
using lexArray = std::vector<Token>;
const str kwords[] = {"lambda", "if", "else", "then"};
str ifkw(str word)
{
    for (auto &kw : kwords)
    {
        if (word == kw)
            return "kw";
    }
    return "val";
}
lexArray *makeLexArray(lexArray *lxRes, chStream *sStream)
{
    std::string word{""};
    std::string num{""};
    std::string op{""};
    char tp;
    str vl;
    int openBracket{0};
    for (auto &sym : *sStream)
    {
        tp = static_cast<char>(sym.type);
        vl = sym.value;
        if (sym.value == '"')
        {
            if (openBracket)
            {
                lxRes->push_back({"str", word});
                word = "";
                openBracket = 0;
            }
            else
            {
                if (word != "")
                    lxRes->push_back({ifkw(word), word});
                else if (num != "")
                    lxRes->push_back({"num", num});
                else if (op != "")
                    lxRes->push_back({"op", op});
                word = "";
                num = "";
                op = "";
                openBracket = 1;
            }
            continue;
        }
        if (openBracket)
            word += vl;

        else
        {
            if (tp != 'D' && num != "")
            {
                lxRes->push_back({"num", num});
                num = "";
            }
            if (tp != 'L' && word != "")
            {
                lxRes->push_back({ifkw(word), word});
                word = "";
            }
            if (tp != 'B' && op != "")
            {
                lxRes->push_back({"op", op});
                op = "";
            }
            switch (tp)
            {
            case 'L':
                word += vl;
                break;
            case 'D':
                num += vl;
                break;
            case 'B':
                op += vl;
                break;
            case 'S':
                if (spec.find(vl) != std::string::npos)
                    lxRes->push_back({"spec", vl});
                break;
            }
        }
    }
    if (word != "")
        lxRes->push_back({ifkw(word), word});
    else if (num != "")
        lxRes->push_back({"num", num});
    else if (op != "")
        lxRes->push_back({"op", op});
    lxRes->push_back({"end", "end"});
    return lxRes;
}
lexArray *lexing(str filename)
{
    str file = readTxt(filename);
    // std::cout<<"---start out file---"<<std::endl<<file<<std::endl<<"----end out file----"<<std::endl;

    chStream *sStream = lexSymbls(new chStream, file);

    // for(auto&sym:*sStream)
    //{
    //    std::cout<<"type: "<<static_cast<char>(sym.type)<<" value: "<<sym.value<<std::endl;
    //}

    return makeLexArray(new lexArray, sStream);
}