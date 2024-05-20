#include "lexer.cpp"
class Expretion;
class Number;
class String;
class Boolean;
class Variable;
class Lambda;
class Call;
class IF;
class Assign;
class Binary;
class Sequence;

class Expretion
{
public:
    str exprType;

    Number *number;
    String *string;
    Boolean *boolean;
    Variable *variable;
    Lambda *lambda;
    Call *call;
    IF *If;
    Assign *assign;
    Binary *binary;
    Sequence *sequence;
};
class Number
{
public:
    str type{"num"};
    str value;
};
class String
{
public:
    str type{"str"};
    str value;
};
class Boolean
{
public:
    str type{"bool"};
    str value;
};
class Variable
{
public:
    str type{"var"};
    str name;
};
class Lambda
{
public:
    str type{"lambda"};
    std::vector<Expretion *> args;
    Expretion *body;
};
class Call
{
public:
    str type{"call"};
    Expretion *func;
    std::vector<Expretion *> args;
};
class IF
{
public:
    str type{"if"};
    Expretion *cond;
    Expretion *then;
    Expretion *els{nullptr};
};
class Binary
{
public:
    str type{"bin"};
    str op;
    Expretion *left;
    Expretion *right;
};
class Sequence
{
public:
    str type{"seque"};
    std::vector<Expretion *> seque;
};

struct
{
    lexArray *lx;
    int pos{0};
    Token next()
    {
        if (lx->at(pos).type != "end")
            pos++;
        return lx->at(pos);
    }
    Token prev()
    {
        if (pos != 0)
            pos--;
        return lx->at(pos);
    }
    Token get()
    {
        return lx->at(pos);
    }
} tkman;
std::map<std::string, int> precedence;
void setPrecedence()
{
    precedence["="] = 1;

    precedence["||"] = 2;

    precedence["&&"] = 3;

    precedence["<"] = 4;
    precedence[">"] = 4;
    precedence["<="] = 4;
    precedence[">="] = 4;
    precedence["!="] = 4;
    precedence["=="] = 4;

    precedence["+"] = 5;
    precedence["-"] = 5;

    precedence["*"] = 6;
    precedence["/"] = 6;
    precedence["%"] = 6;
}
Expretion *parceAtom();
Expretion *parceExpr();
Expretion *mbBin(Expretion *left, int myPre);
Expretion *parceLambda();
Expretion *parceIf();
Expretion *valueType(Token tk);
Expretion *parceCall(Expretion *ex);
Expretion *parceIf();
Expretion *parceSeque();
Expretion *parce();

Expretion *parce()
{
    Expretion *expr = new Expretion;
    expr->exprType = "seque";
    Sequence *seque = new Sequence;
    while (1)
    {
        if (tkman.get().value == "end")
            break;
        seque->seque.push_back(parceExpr());
        tkman.next();
    }
    expr->sequence = seque;

    return expr;
}

Expretion *createBin(str op, Expretion *left, Expretion *right)
{
    Expretion *expr = new Expretion;
    expr->exprType = "bin";
    expr->binary = new Binary;
    expr->binary->op = op;
    expr->binary->left = left;
    expr->binary->right = right;
    return expr;
}

Expretion *mbBin(Expretion *left, int myPre)
{
    Token tk = tkman.get();
    if (tk.type == "op")
    {
        int hisPre = precedence[tk.value];
        if (hisPre > myPre)
        {
            tkman.next();
            Expretion *right = mbBin(parceAtom(), hisPre);
            Expretion *expr = new Expretion;
            return mbBin(createBin(tk.value, left, right), myPre);
        }
    }
    return left;
}
Expretion *parceSeque()
{
    Expretion *expr = new Expretion;
    expr->exprType = "seque";
    Sequence *seque = new Sequence;
    tkman.next();
    while (1)
    {
        if (tkman.get().value == "}")
            break;
        seque->seque.push_back(parceExpr());
        if (tkman.get().value == "}")
            break;
        tkman.next();
    }
    tkman.next();
    expr->sequence = seque;
    return expr;
}
Expretion *parceIf()
{
    tkman.next();
    Expretion *expr = new Expretion;
    expr->exprType = "if";
    IF *If = new IF;
    If->cond = parceExpr();
    if (tkman.get().value != "{")
        tkman.next();
    If->then = parceExpr();
    if (tkman.get().value == "else")
    {

        tkman.next();
        If->els = parceExpr();
    }
    expr->If = If;
    return expr;
}
Expretion *parceCall(Expretion *ex)
{
    Expretion *expr = new Expretion;
    expr->exprType = "call";
    Call *call = new Call;
    call->func = ex;
    tkman.next();
    while (1)
    {
        if (tkman.get().value == ")")
            break;
        if (tkman.get().value == ",")
        {
            tkman.next();
            continue;
        }
        call->args.push_back(parceExpr());
    }
    tkman.next();
    expr->call = call;
    return expr;
}
Expretion *parceLambda()
{
    tkman.next();
    Expretion *expr = new Expretion;
    expr->exprType = "lambda";
    Lambda *lamd = new Lambda;
    tkman.next();
    while (tkman.get().value != ")")
    {
        if (tkman.get().type != "spec")
            lamd->args.push_back(valueType(tkman.get()));
        tkman.next();
    }
    tkman.next();
    lamd->body = parceExpr();
    expr->lambda = lamd;
    return expr;
}
Expretion *parceExpr()
{
    return mbBin(parceAtom(), 0);
}

Expretion *parceAtom()
{
    Expretion *expr = new Expretion;
    if (tkman.get().value == "(")
    {
        tkman.next();
        expr = parceExpr();
        tkman.next();
        return expr;
    }
    if (tkman.get().value == "if")
        return parceIf();
    if (tkman.get().value == "{")
        return parceSeque();
    if (tkman.get().value == "lambda")
        return parceLambda();
    expr = valueType(tkman.get());
    tkman.next();
    if (expr->exprType == "var" && tkman.get().value == "(")
        return parceCall(expr);
    return expr;
}

Expretion *valueType(Token tk)
{
    Expretion *expr = new Expretion;
    if (tk.type == "val")
    {
        expr->exprType = "var";
        expr->variable = new Variable;
        expr->variable->name = tk.value;
        return expr;
    }
    if (tk.type == "num")
    {
        expr->exprType = "num";
        expr->number = new Number;
        expr->number->value = tk.value;
        return expr;
    }
    if (tk.type == "str")
    {
        expr->exprType = "str";
        expr->string = new String;
        expr->string->value = tk.value;
        return expr;
    }
    return nullptr;
}

void outVar(Variable *Var, str indent);
void outBin(Binary *Bin, str indent);
void outNum(Number *Bin, str indent);
void outStr(String *Str, str indent);
void outSeque(Sequence *seque, str indent);
void outIf(IF *If, str indent);
void outLambda(Lambda *lambd, str indent);
void outCall(Call *call, str indent);
void outTree(Expretion *Tree, str indent);

void outVar(Variable *Var, str indent)
{
    std::cout << indent + "type: var" << std::endl
              << indent + "name: " << Var->name << std::endl;
}
void outBin(Binary *Bin, str indent)
{
    std::cout << indent + "type: bin" << std::endl
              << indent + "op: " << Bin->op << std::endl;
    std::cout << indent + "left: " << std::endl;
    outTree(Bin->left, indent + "   ");
    std::cout << indent + "right: " << std::endl;
    outTree(Bin->right, indent + "   ");
    std::cout << std::endl;
}
void outNum(Number *Bin, str indent)
{
    std::cout << indent + "type: num" << std::endl
              << indent + "value: " << Bin->value << std::endl;
}
void outStr(String *Str, str indent)
{
    std::cout << indent + "type: str" << std::endl
              << indent + "value: " << Str->value << std::endl;
}
void outSeque(Sequence *seque, str indent)
{
    std::cout << indent + "type: seque" << std::endl
              << indent + "{" << std::endl;
    int i = 1;
    for (auto &expr : seque->seque)
    {
        std::cout << indent << i << "." << std::endl;
        outTree(expr, indent);
        i++;
    }
    std::cout << indent + "}" << std::endl;
}
void outIf(IF *If, str indent)
{
    std::cout << indent + "type: if" << std::endl;
    std::cout << indent + "cond: " << std::endl;
    outTree(If->cond, indent + "   ");
    std::cout << indent + "then: " << std::endl;
    outTree(If->then, indent + "   ");
    if (If->els != nullptr)
    {
        std::cout << indent + "else: " << std::endl;
        outTree(If->els, indent + "   ");
    }
}
void outLambda(Lambda *lambd, str indent)
{
    std::cout << indent + "type: lambda: " << std::endl;
    std::cout << indent + "args: " << std::endl;
    if (lambd->args.size() > 0)
        for (auto &ar : lambd->args)
        {
            outTree(ar, indent + "   ");
        }
    else
        std::cout << indent + "   NONE";
    std::cout << indent + "body: " << std::endl;
    outTree(lambd->body, indent + "   ");
    std::cout << std::endl;
}
void outCall(Call *call, str indent)
{
    std::cout << indent + "type: call" << std::endl;
    std::cout << indent + "func: " << std::endl;
    outTree(call->func, indent + "   ");
    std::cout << indent + "args: " << std::endl;
    if (call->args.size() > 0)
    {
        for (auto &ar : call->args)
        {
            outTree(ar, indent + "   ");
        }
    }
    else
        std::cout << indent + "   NONE";
    std::cout << std::endl;
}
void outTree(Expretion *Tree, str indent)
{
    if (Tree->exprType == "bin")
        outBin(Tree->binary, indent);
    if (Tree->exprType == "str")
        outStr(Tree->string, indent);
    if (Tree->exprType == "num")
        outNum(Tree->number, indent);
    if (Tree->exprType == "var")
        outVar(Tree->variable, indent);
    if (Tree->exprType == "seque")
        outSeque(Tree->sequence, indent);
    if (Tree->exprType == "if")
        outIf(Tree->If, indent);
    if (Tree->exprType == "lambda")
        outLambda(Tree->lambda, indent);
    if (Tree->exprType == "call")
        outCall(Tree->call, indent);
}