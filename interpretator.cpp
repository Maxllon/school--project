#include "parcer.cpp"
class Environment;
str descent(Expretion *expr, Environment *envi);
str assign(str name, Expretion *right, Environment *envi);
str oper(str op, str left, str right);
str calling();
void makeLambda(str name, Expretion *expr, Environment *envi);

class Environment
{
public:
    Environment *parent{nullptr};
    Expretion *tree{nullptr};

    std::vector<str> args;

    std::map<str, str> variables;
    std::map<str, Environment *> lambds;

    void setV(str name, str value)
    {
        variables[name] = value;
    }
    str getV(str name)
    {
        if (variables.count(name))
            return variables[name];
        if (parent != nullptr)
            return parent->getV(name);
        return "0";
    }

    void setL(str name, Environment *lambd)
    {
        lambds[name] = lambd;
    }
    Environment *getL(str name)
    {
        if (lambds.count(name))
            return lambds[name];
        if (parent != nullptr)
            return parent->getL(name);
        std::cout << "where is lambda!?" << name;
        return nullptr;
    }

    str stInterpret()
    {
        return descent(this->tree, this);
    }
};
void makeLambda(str name, Expretion *expr, Environment *envi)
{
    Environment *lambda = new Environment;
    lambda->parent = envi;
    for (int i = 0; i < expr->lambda->args.size(); i++)
    {
        lambda->args.push_back(expr->lambda->args[i]->variable->name);
    }
    lambda->tree = expr->lambda->body;
    envi->setL(name, lambda);
}

str calling(str name, std::vector<Expretion *> &args, Environment *envi)
{
    if (name == "print")
    {
        str val = descent(args[0], envi);
        std::cout << val;
        return val;
    }
    if (name == "input")
    {
        str val;
        std::cin >> val;
        return val;
    }
    Environment *opLambd = envi->getL(name);
    Environment *currLambd = new Environment;
    currLambd->parent = envi;
    currLambd->tree = opLambd->tree;
    str value;
    str nm;
    for (int i = 0; i < args.size(); i++)
    {
        nm = opLambd->args[i];
        value = descent(args[i], envi);
        currLambd->variables[nm] = value;
    }
    return currLambd->stInterpret();
}

str assign(str name, Expretion *right, Environment *envi)
{
    if (right->exprType == "lambda")
    {
        makeLambda(name, right, envi);
        return "0";
    }
    envi->setV(name, descent(right, envi));
    return envi->getV(name);
}
str oper(str op, str left, str right)
{
    if (op == "==")
        return std::to_string(static_cast<int>(left == right));
    if (op == "!=")
        return std::to_string(static_cast<int>(left != right));
    int a = stoi(left);
    int b = stoi(right);
    if (op == "+")
        return std::to_string(a + b);
    if (op == "-")
        return std::to_string(a - b);
    if (op == "*")
        return std::to_string(a * b);
    if (op == "/")
        return std::to_string(a / b);
    if (op == "%")
        return std::to_string(a % b);

    if (op == ">")
        return std::to_string(static_cast<int>(a > b));
    if (op == "<")
        return std::to_string(static_cast<int>(a < b));
    if (op == ">=")
        return std::to_string(static_cast<int>(a >= b));
    if (op == "<=")
        return std::to_string(static_cast<int>(a <= b));
    if (op == "&&")
        return std::to_string(static_cast<int>(a && b));
    if (op == "||")
        return std::to_string(static_cast<int>(a || b));
}

str descent(Expretion *expr, Environment *envi)
{
    str type = expr->exprType;
    if (type == "num")
        return expr->number->value;
    if (type == "str")
        return expr->string->value;
    if (type == "var")
        return envi->getV(expr->variable->name);
    if (type == "bin")
    {
        if (expr->binary->op == "=")
            return assign(expr->binary->left->variable->name, expr->binary->right, envi);
        return oper(expr->binary->op, descent(expr->binary->left, envi), descent(expr->binary->right, envi));
    }
    if (type == "if")
    {
        str cond = descent(expr->If->cond, envi);
        if (cond != "0")
            return descent(expr->If->then, envi);
        if (expr->If->els != nullptr)
        {
            return descent(expr->If->els, envi);
        }
        return "0";
    }
    if (type == "call")
        return calling(expr->call->func->variable->name, expr->call->args, envi);
    if (type == "seque")
    {
        str val;
        for (auto &ex : expr->sequence->seque)
        {
            val = descent(ex, envi);
        }
        return val;
    }
    return "0";
}