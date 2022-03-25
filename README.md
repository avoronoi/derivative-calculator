# Derivative Calculator
## Description
This is a simple console application for calculating derivative of an expression.
## Compile and run
Compile cpp files with:
```
g++ -std=c++17 *.cpp -o main
```
Run main with:\
```./main``` on Linux\
```.\main.exe``` on Windows
## Usage
Available commands:
```
EXPR <expression>      // <expression> can contain braces, real numbers, variable x, binary operators +, -, *, /, ^, unary minus, functions sin, cos, tan, cot, ln
SAVE <var_name>        // assigns last expression to a variable <var_name>
PRINT                  // prints last expression
PRINT <var_name>       // prints expression <var_name>
DER                    // prints a derivative of last expression
DER <var_name>         // same, but for expression <var_name>
EVAL <x>               // evaluates last expression with x equal to <x>, where <x> is a real number
EVAL <var_name> <x>    // same, but for expression <var_name>
```
