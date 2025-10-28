#include <iostream>
#include <stack>
#include <string>
#include <cmath>
#include <cctype>
#include <vector>

using namespace std;

typedef enum { ADD, SUB, MUL, DIV, POW, FAC, L_P, R_P, EOE } Operator;
#define N_OPTR 9

const char pri[N_OPTR][N_OPTR] = {
    {'>', '>', '<', '<', '<', '<', '<', '>', '>'},
    {'>', '>', '<', '<', '<', '<', '<', '>', '>'},
    {'>', '>', '>', '>', '<', '<', '<', '>', '>'},
    {'>', '>', '>', '>', '<', '<', '<', '>', '>'},
    {'>', '>', '>', '>', '>', '<', '<', '>', '>'},
    {'>', '>', '>', '>', '>', '>', '<', '>', '>'},
    {'<', '<', '<', '<', '<', '<', '<', '=', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {'<', '<', '<', '<', '<', '<', '<', ' ', '='}
};

Operator charToOp(char c) {
    switch (c) {
        case '+': return ADD;
        case '-': return SUB;
        case '*': return MUL;
        case '/': return DIV;
        case '^': return POW;
        case '!': return FAC;
        case '(': return L_P;
        case ')': return R_P;
        case '\0': return EOE;
        default: return EOE;
    }
}

double factorial(double n) {
    if (n < 0 || n != (int)n) return NAN;
    int num = (int)n;
    double res = 1;
    for (int i = 1; i <= num; ++i) res *= i;
    return res;
}

double calculate(double a, double b, Operator op) {
    switch (op) {
        case ADD: return a + b;
        case SUB: return a - b;
        case MUL: return a * b;
        case DIV: return b == 0 ? NAN : a / b;
        case POW: return pow(a, b);
        default: return NAN;
    }
}

double stringCalculator(const string& expr) {
    stack<double> numStack;
    stack<Operator> opStack;
    opStack.push(EOE);
    int i = 0;
    int n = expr.size();

    while (!opStack.empty()) {
        if (isdigit(expr[i]) || expr[i] == '.') {
            double num = 0.0;
            while (i < n && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            if (i < n && expr[i] == '.') {
                i++;
                double dec = 0.1;
                while (i < n && isdigit(expr[i])) {
                    num += (expr[i] - '0') * dec;
                    dec *= 0.1;
                    i++;
                }
            }
            numStack.push(num);
        } else {
            Operator currOp = charToOp(expr[i]);
            Operator topOp = opStack.top();
            switch (pri[topOp][currOp]) {
                case '<':
                    opStack.push(currOp);
                    i++;
                    break;
                case '=':
                    opStack.pop();
                    i++;
                    break;
                case '>':
                    opStack.pop();
                    if (topOp == FAC) {
                        if (numStack.empty()) return NAN;
                        double num = numStack.top();
                        numStack.pop();
                        double res = factorial(num);
                        if (isnan(res)) return NAN;
                        numStack.push(res);
                    } else {
                        if (numStack.size() < 2) return NAN;
                        double b = numStack.top(); numStack.pop();
                        double a = numStack.top(); numStack.pop();
                        double res = calculate(a, b, topOp);
                        if (isnan(res)) return NAN;
                        numStack.push(res);
                    }
                    break;
                default:
                    return NAN;
            }
        }
    }

    if (numStack.size() != 1) return NAN;
    return numStack.top();
}

void testCalculator() {
    cout << "==================== 任务2：字符串计算器 ====================" << endl;
    vector<string> testCases;
    testCases.push_back("3+5*2");
    testCases.push_back("10-(2+3)*2");
    testCases.push_back("2^3+4!");
    testCases.push_back("(3+5)*(7-2)");
    testCases.push_back("10/2-3");
    testCases.push_back("3.5+2.5*2");
    testCases.push_back("5!");
    testCases.push_back("2^(3+1)");
    testCases.push_back("10/0");
    testCases.push_back("3+*5");
    testCases.push_back("(3+5");
    testCases.push_back("7!-2^5");

    cout << "=== 基础字符串计算器测试 ===" << endl;
    for (size_t i = 0; i < testCases.size(); ++i) {
        double res = stringCalculator(testCases[i]);
        if (isnan(res)) {
            cout << "表达式 \"" << testCases[i] << "\"：无效" << endl;
        } else {
            cout << "表达式 \"" << testCases[i] << "\"：" << res << endl;
        }
    }

    cout << "\n=== 扩展计算器测试（示例） ===" << endl;
    string advancedExpr = "sin(3.1416/2)+log(10)";
    double advRes = stringCalculator(advancedExpr);
    if (isnan(advRes)) {
        cout << "表达式 \"" << advancedExpr << "\"：无效" << endl;
    } else {
        cout << "表达式 \"" << advancedExpr << "\"：" << advRes << endl;
    }
}

int main() {
    testCalculator();
    return 0;
}
