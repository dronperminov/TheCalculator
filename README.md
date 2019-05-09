# TheCalculator
Calculator of expressions with creating and using functions and variables

## Main commands:
* `help` — print help message
* `print state` — print defined variables and functions
* `reset` — remove all defined variables and functions
* `def` — start to function definition
* `set` — start to variable definition
* `quit` — terminate program

## Function definition syntax:
```
def [function name] = [function definition]
  [function name] - identifier
  [function definition] - expression
```

#### Example: `def f(x) = sin(2*x)`

## Variable definition syntax:
```
set [variable name] = [variable definition]
  [variable name] - identifier
  [variable definition] - expression
 ```

#### Example: `set twopi = 2 * pi`

## Built-in functions and constants:
* `Trigonometry:` sin, cos, tg, ctg, arcsin, arccos, arctg
* `Other functions:` sqrt, log, ln, lg, exp, abs, sign, min, max, pow
* `Constants:` pi, e