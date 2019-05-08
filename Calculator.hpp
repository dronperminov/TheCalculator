#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stack>
#include <cmath>

using namespace std;

class Calculator {
	const string DEF = "def"; // строка для определения функции
	const string SET = "set"; // строка для введения переменной
	
	// вектор функций одного аргумента
	const vector<string> functions = {
		"sin", "cos", "tan", "tg", "cot", "ctg", 
		"asin", "arcsin", "acos", "arccos", "atan", "arctan", "arctg", 
		"sqrt", "ln", "lg", "exp", "abs", "sign"
	};

	// вектор функций двух аргументов
	const vector<string> functionsBinary = {
		"pow", "log", "min", "max"
	};

	// вектор констант
	const vector<string> constants = {
		"pi", "e"
	};

	// структура для переменной
	struct Variable {
		string name; // имя переменной
		double value;
	};

	// структура для функции
	struct Function {
		string name; // имя функции
		string arg; // имя аргумента
		vector<string> rpn; // полиз функции
	};

	bool degrees; // в градусах ли вычисление тригонометрии

	vector<string> lexemes; // вектор лексем
	vector<string> rpn; // обратная польская запись выражения
	
	vector<Variable> userVariables; // вектор пользовательских переменных
	vector<Function> userFunctions; // вектор пользовательских функций

	void SplitToLexemes(const string& s); // разбивка на лексемы

	string CurrLexeme(); // получение текущей дексемы
	string NextLexeme(); // получение следующей лексемы
	void CheckLexeme(const string& value); // проверка на совпадение с ожидаемой лексемой

	bool IsNumber(const string& s) const; // проверка на число
	bool IsConstant(const string& s) const; // проверка на константу
	bool IsIdentifier(const string& s) const; // проверка на идентификатор (переменную)
	bool IsOperator(const string& s) const; // проверка на операцию
	bool IsUserVariable(const string& s) const; // проверка на пользовательскую переменную
	bool IsUserFunction(const string& s) const; // проверка на пользовательскую функцию
	bool IsFunction(const string& s) const; // проверка на функцию одного аргумента
	bool IsBinaryFunction(const string& s) const; // проверка на функцию двух аргументов

	void Addition(); // обработка аддитивных операций
	void Multiplying(bool isUnary = true); // обработка мультипликативных операций
	void Exponenting(bool isUnary = true); // обработка возведения в степень
	bool Entity(bool isUnary = true, bool insertUnary = true); // обработка операндов

	void ParseSet(); // обработка введения переменной
	void ParseDef(); // обработка введения функции

	const Variable* GetVariable(const string& name) const; // получение указателя на переменную по её имени
	const Function* GetFunction(const string& name) const; // получение указателя на функцию по её имени

	double EvaluateConstant(const string& constant) const; // получение значения константы
	double EvaluateOperator(const string& op, double arg1, double arg2) const; // вычисление значения операции
	double EvaluateFunction(const string& name, double arg) const; // вычисление значения функции
	double EvaluateBinaryfunction(const string& name, double arg1, double arg2) const; // вычисление значения бинарной функции
	double Evaluate(const vector<string>& rpn) const; // вычисление выражения, записанного в ПОЛИЗе

public:
	Calculator(bool degrees); // конструткор из режима тригонометрии

	void Calculate(const string& command);
	void Reset(); // сброс информации о переменных и функциях
	
	void PrintState() const; // вывод состояния калькулятора
	void PrintHelp() const; // вывод сообщений о работе калькулятора
};

Calculator::Calculator(bool degrees) {
	this->degrees = degrees; // запоминаем режим
}

// разбивка строки с командой на лексемы
void Calculator::SplitToLexemes(const string& s) {
	size_t i = 0;

	while (i < s.length()) {
		// если знак операции, скобки, запятая или знак равно
		if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/' || s[i] == '^' || s[i] == '(' || s[i] == ')' || s[i] == ',' || s[i] == '=') {
			lexemes.push_back(string(1, s[i])); // добавляем лексему операции или скобки
			i++;
		}
		else if (s[i] >= '0' && s[i] <= '9') { // если цифра
			string number = ""; // будущее число
			int points = 0; // количество точек

			// пока цифры или точка
			while (i < s.length() && ((s[i] >= '0' && s[i] <= '9') || (s[i] == '.'))) {
				number += s[i]; // добавляем цифру к строке с числом

				// если встретили точку
				if (s[i] == '.')
					points++; // увеличиваем число точек в числе

				if (points > 1) // если их стало слишком много
					throw string("incorrect real number '") + number + "'"; // бросаем исключение

				i++;
			}

			lexemes.push_back(number); // добавляем в вектор число
		}
		else if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z')) { // если буква
			string word = ""; // будущее слово

			// пока буквы или цифры
			while (i < s.length() && ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= '0' && s[i] <= '9'))) {
				word += s[i]; // добавляем букву к строке со словом
				i++;
			}

			lexemes.push_back(word); // добавляем в вектор слово
		}
		else if (s[i] == ' ') { // если пробел
			i++; // пропускаем
		}
		else // неизвестный символ
			throw string("unknown character '") + s[i] + "' in command"; // бросаем исключение
	}
}

// получение текущей дексемы
string Calculator::CurrLexeme() { 
	return lexemes.size() > 0 ? lexemes[0] : ""; // если есть лексемы, берём, иначе пустая строка
}

// получение следующей лексемы
string Calculator::NextLexeme() {
	lexemes.erase(lexemes.begin()); // удаляем первую лексему
	
	return CurrLexeme(); // получаем текущую
}

// проверка на совпадение с ожидаемой лексемой
void Calculator::CheckLexeme(const string& value) {
	if (CurrLexeme() != value)
		throw string("exprected '") + value + "', but got '" + CurrLexeme() + "'"; // если значения не совпали, бросаем исключение
}

// проверка на число
bool Calculator::IsNumber(const string& s) const {
	for (size_t i = 0; i < s.length(); i++)
		if ((s[i] < '0' || s[i] > '9') && s[i] != '.') // если какой-то символ не цифра и не точка
			return false; // значит не число

	return true; // иначе число
}

// проверка на константу
bool Calculator::IsConstant(const string& s) const {
	for (size_t i = 0; i < constants.size(); i++)
		if (constants[i] == s)
			return true;

	return false;
}

// проверка на идентификатор (переменную)
bool Calculator::IsIdentifier(const string& s) const {
	// если это ключевое слово
	if (s == DEF || s == SET)
		return false; // то это не переменная

	// если первый символ не буква
	if ((s[0] < 'a' || s[0] > 'z') && (s[0] < 'A' || s[0] > 'Z'))
		return false;

	// есе остальные символы должны быть буквами или цифрами
	for (size_t i = 1; i < s.length(); i++)
		if ((s[i] < 'a' || s[i] > 'z') && (s[i] < 'A' || s[i] > 'Z') && (s[i] < '0' || s[i] > '9'))
			return false; // если это не так, то это не переменная
	
	return true; // иначе переменная
}

// проверка на операцию
bool Calculator::IsOperator(const string& s) const {
	return s == "+" || s == "-" || s == "*" || s == "/" || s == "mod" || s == "^";
}

// проверка на пользовательскую переменную
bool Calculator::IsUserVariable(const string& s) const {
	for (size_t i = 0; i < userVariables.size(); i++)
		if (userVariables[i].name == s)
			return true;

	return false;
}

// проверка на пользовательскую функцию
bool Calculator::IsUserFunction(const string& s) const {
	for (size_t i = 0; i < userFunctions.size(); i++)
		if (userFunctions[i].name == s)
			return true;

	return false;
}

// проверка на функцию одного аргумента
bool Calculator::IsFunction(const string& s) const {
	for (size_t i = 0; i < functions.size(); i++)
		if (functions[i] == s)
			return true;

	return false;
}

// проверка на функцию двух аргументов
bool Calculator::IsBinaryFunction(const string& s) const {
	for (size_t i = 0; i < functionsBinary.size(); i++)
		if (functionsBinary[i] == s)
			return true;

	return false;
}

// обработка аддитивных операций
void Calculator::Addition() {
    Multiplying();

    while (CurrLexeme() == "+" || CurrLexeme() == "-") {
        string operation = CurrLexeme();
        NextLexeme();

        Multiplying(false);

        rpn.push_back(operation);
    }
}

// обработка мультипликативных операций
void Calculator::Multiplying(bool isUnary) {
    Exponenting(isUnary);

    while (CurrLexeme() == "*" || CurrLexeme() == "/" || CurrLexeme() == "mod") {
        string operation = CurrLexeme();
        NextLexeme();

        Exponenting(false);

        rpn.push_back(operation);
    }
}

// обработка операции возведения в степень
void Calculator::Exponenting(bool isUnary) {
    bool wasUnary = Entity(isUnary, false);

    while (CurrLexeme() == "^") {
        string operation = CurrLexeme();
        NextLexeme();

        Entity(false, false);

        rpn.push_back(operation);
    }

    if (wasUnary)
    	rpn.push_back("!");
}

bool Calculator::Entity(bool isUnary, bool insertUnary) {
    if (CurrLexeme() == "(") { // если скобка
        NextLexeme();

        Addition(); // парсим заного выражение в скобках

        CheckLexeme(")"); // проверяем закрывающую скобку
        NextLexeme();
    }
    else if (IsNumber(CurrLexeme()) || IsConstant(CurrLexeme())) { // если число или константа
        rpn.push_back(CurrLexeme()); // заносим его в полиз
        NextLexeme();
    }
    else if (IsUserVariable(CurrLexeme())) { // если пользовательская переменная
    	rpn.push_back(CurrLexeme()); // заносим её в полиз
    	NextLexeme();
    }
    else if (IsFunction(CurrLexeme()) || IsUserFunction(CurrLexeme())) { // если функция
		string func = CurrLexeme();
		NextLexeme();
		CheckLexeme("(");
		NextLexeme();
		Addition();

		CheckLexeme(")");
		NextLexeme();

		rpn.push_back(func); // добавляем функцию в полиз
    }
    else if (IsBinaryFunction(CurrLexeme())) {
    	string func = CurrLexeme();

    	NextLexeme();
		CheckLexeme("(");
		NextLexeme();
		Addition(); // парсим первый аргумент

		CheckLexeme(","); // проверяем на разделитель
		NextLexeme();

		Addition(); // парсим второй аргумент

		CheckLexeme(")");
		NextLexeme();

		rpn.push_back(func); // добавляем функцию в полиз
    }
    else if (isUnary && CurrLexeme() == "-") { // если унарный минус и минус
        NextLexeme();
        Entity(false); // парсим аргумент
        
        if (insertUnary)
        	rpn.push_back("!"); // заносим символ унарного минуса

        return true;
    }
    else {
        throw string("symbol '") + CurrLexeme() + "' is not correct"; // иначе некорректный символ
    }

    return false;
}

// обработка введения переменной
void Calculator::ParseSet() {
	NextLexeme();

	string name = CurrLexeme(); // получаем имя переменной

	// если имя не является переменной, бросаем исключение
	if (!IsIdentifier(name))
		throw string("'") + name + "' is not a variable identifier";

	// если пытаемся добавить математическую функцию
	if (IsFunction(name) || IsBinaryFunction(name))
		throw string("function '") + name + "' is math function";

	// если имя является константой, то бросаем исключение
	if (IsConstant(name))
		throw string("'") + name + "' is constant";

	// если такая переменная уже есть
	if (GetVariable(name) != nullptr)
		throw string("variable '") + name + "' already exists!"; // бросаем исключение

	NextLexeme();

	CheckLexeme("=");
	NextLexeme();

	if (lexemes.size() == 0)
		throw string("expression after variable is empty");

	Addition(); // парсим выражение за знаком равенства

	if (lexemes.size())
		throw string("incorrect variable definition");

	Variable variable;
	variable.name = name;
	variable.value = Evaluate(rpn);

	userVariables.push_back(variable);
}

// обработка введения функции
void Calculator::ParseDef() {
	NextLexeme();

	string name = CurrLexeme(); // получаем имя функции

	// если имя не является идентификатором, бросаем исключение
	if (!IsIdentifier(name))
		throw string("'") + name + "' is not a function identifier";

	// если пытаемся добавить математическую функцию
	if (IsFunction(name) || IsBinaryFunction(name))
		throw string("function '") + name + "' is math function";

	// если такая функция уже есть
	if (GetFunction(name) != nullptr)
		throw string("function '") + name + "' already exists"; // бросаем исключение

	NextLexeme();

	CheckLexeme("(");
	NextLexeme();

	string arg = CurrLexeme(); // получае имя аргумента

	// если имя аргумента не является идентификатором
	if (!IsIdentifier(arg))
		throw string("'") + arg + "' is not argument identifier"; // бросаем исключение

	NextLexeme();

	CheckLexeme(")");
	NextLexeme();

	CheckLexeme("=");
	NextLexeme();

	vector<Variable> tmpVars = userVariables; // запоминаем текущие переменные
	userVariables.clear(); // очищаем переменные
	userVariables.push_back({arg, 0}); // добавляем переменную из аргумента
	
	Addition(); // парсим функцию

	userVariables = tmpVars; // восстанавливаем переменные
	
	if (lexemes.size())
		throw string("incorrect function definition");

	Function function;

	function.name = name;
	function.arg = arg;
	function.rpn = rpn;

	userFunctions.push_back(function); // добавляем функцию в вектор
}

// получение указателя на переменную по её имени
const Calculator::Variable* Calculator::GetVariable(const string& name) const {
	for (size_t i = 0; i < userVariables.size(); i++)
		if (userVariables[i].name == name) // если имена совпали
			return &userVariables[i]; // возвращаем указатель

	return nullptr; // иначе возвращаем nullptr
}

// получение указателя на функцию по её имени
const Calculator::Function* Calculator::GetFunction(const string& name) const {
	for (size_t i = 0; i < userFunctions.size(); i++)
		if (userFunctions[i].name == name) // если имена совпали
			return &userFunctions[i]; // возвращаем указатель

	return nullptr; // иначе возвращаем nullptr
}

// получение значения константы
double Calculator::EvaluateConstant(const string& constant) const {
	if (constant == "pi")
		return M_PI;

	if (constant == "e")
		return exp(1);

	throw string("unhandled constant '") + constant + "'";
}

// вычисление значения операции
double Calculator::EvaluateOperator(const string& op, double arg1, double arg2) const {
	if (op == "+")
		return arg1 + arg2;

	if (op == "-")
		return arg1 - arg2;

	if (op == "*")
		return arg1 * arg2;

	if (op == "/") {
		if (arg2 == 0)
			throw string("division by zero");

		return arg1 / arg2;
	}

	if (op == "^")
		return pow(arg1, arg2);

	if (op == "mod")
		return fmod(arg1, arg2);

	throw string("unhandled operator '") + op + "'";
}

// вычисление значения функции
double Calculator::EvaluateFunction(const string& name, double arg) const {
	if (name == "sin")
		return sin(degrees ? M_PI / 180 * arg : arg);
	
	if (name == "cos")
		return cos(degrees ? M_PI / 180 * arg : arg);
	
	if (name == "tan" || name == "tg")
		return tan(degrees ? M_PI / 180 * arg : arg);
	
	if (name == "cot" || name == "ctg")
		return 1.0  / tan(degrees ? M_PI / 180 * arg : arg);

	if (name == "asin" || name == "arcsin")
		return degrees ? asin(arg) * 180 / M_PI : asin(arg);
	
	if (name == "acos" || name == "arccos")
		return degrees ? acos(arg) * 180 / M_PI : acos(arg);

	if (name == "atan" || name == "arctan" || name == "arctg")
		return degrees ? atan(arg) * 180 / M_PI : atan(arg);

	if (name == "sqrt")
		return sqrt(arg);
	
	if (name == "lg")
		return log10(arg);
	
	if (name == "ln")
		return log(arg);
	
	if (name == "exp")
		return exp(arg);
	
	if (name == "abs")
		return fabs(arg);

	if (name == "sign")
		return arg > 0 ? 1 : arg < 0 ? -1 : 0;

	throw string("unhandled function '") + name + "'";
}

// вычисление значения бинарной функции
double Calculator::EvaluateBinaryfunction(const string& name, double arg1, double arg2) const {
	if (name == "pow")
		return pow(arg1, arg2);
	
	if (name == "min")
		return arg1 < arg2 ? arg1 : arg2;
	
	if (name == "max")
		return arg1 > arg2 ? arg1 : arg2;
	
	if (name == "log")
		return log(arg2) / log(arg1); // log_a(b) = ln(b) / ln(a)
	
	throw string("unhandled function '") + name + "'";
}

// вычисление выражения, записанного в ПОЛИЗе
double Calculator::Evaluate(const vector<string>& rpn) const {
	stack<double> stack;

	for (size_t i = 0; i < rpn.size(); i++) {
		if (IsOperator(rpn[i])) {
			if (stack.size() < 2)
				throw string("unable to take arguments for operator '") + rpn[i] + "': stack size is too small";

			// получаем аргументы из стека
			double arg2 = stack.top();
			stack.pop();
			double arg1 = stack.top();
			stack.pop();

			stack.push(EvaluateOperator(rpn[i], arg1, arg2));
		}
		else if (rpn[i] == "!") { // если унарный минус
			stack.top() *= -1; // меняем знак у числа на верхушке стека
		}
		else if (IsFunction(rpn[i])) {
			if (stack.size() == 0)
				throw string("unable to take arguments for function '") + rpn[i] + "': stack size is too small";

			double arg = stack.top();
			stack.pop();

			stack.push(EvaluateFunction(rpn[i], arg));
		}
		else if (IsBinaryFunction(rpn[i])) {
			if (stack.size() < 2)
				throw string("unable to take arguments for function '") + rpn[i] + "': stack size is too small";

			// получаем аргументы из стека
			double arg2 = stack.top();
			stack.pop();
			double arg1 = stack.top();
			stack.pop();

			stack.push(EvaluateBinaryfunction(rpn[i], arg1, arg2));
		}
		else if (IsUserVariable(rpn[i])) { // если переменная
			const Variable *variable = GetVariable(rpn[i]); // получаем значение переменной по её имени

			if (variable == nullptr)
				throw string("unknown variable '") + rpn[i] + "'";

			stack.push(variable->value); // и добавляем его в стек
		}
		else if (IsUserFunction(rpn[i])) {
			const Function *function = GetFunction(rpn[i]);

			if (function == nullptr)
				throw string("unknown function '") + rpn[i] + "'";

			if (stack.size() == 0)
				throw string("unable to take arguments for function '") + rpn[i] + "': stack size is too small";

			double arg = stack.top(); // получаем аргумент функции

			stack.pop();

			vector<string> tmp = function->rpn;

			// заменяем аргумент функции на число
			for (size_t j = 0; j < tmp.size(); j++)
				if (tmp[j] == function->arg)
					tmp[j] = to_string(arg);

			stack.push(Evaluate(tmp)); // закидываем результат вычисления функции
		}
		else if (IsConstant(rpn[i])) { // если константа
			stack.push(EvaluateConstant(rpn[i])); // кладём в стек её значение
		}
		else { // число, кидаем его в стек
			stack.push(stod(rpn[i])); // то кидаем его в стек
		}
	}

	if (stack.size() != 1)
		throw string("error during computation expression");

	return stack.top();
}

// выполнение команды
void Calculator::Calculate(const string& command) {
	lexemes.clear();
	rpn.clear();

	SplitToLexemes(command);

	if (lexemes.size() == 0)
		throw string("Command is invalid");

	// если определение функции
	if (lexemes[0] == DEF) {
		ParseDef();
	}
	else if (lexemes[0] == SET) { // если введение переменной
		ParseSet();
	}
	else {
		Addition(); // иначе парсим выражение

		if (lexemes.size() > 0)
			throw string("incorrect expression");

		double result = Evaluate(rpn); // вычисляем его
		cout << setprecision(15) << result << endl; // и выводим результат
	}
}

// сброс информации о переменных и функциях
void Calculator::Reset() {
	userFunctions.clear();
	userVariables.clear();
}

// вывод состояния калькулятора
void Calculator::PrintState() const {
	if (userVariables.size() == 0 && userFunctions.size() == 0) {
		cout << "No variables or functions" << endl;
		return;
	}

	if (userVariables.size()) {
		cout << "Variables: " << endl;

		for (size_t i = 0; i < userVariables.size(); i++)
			cout << (i + 1) << ". " << userVariables[i].name << " = " << userVariables[i].value << endl;
	}

	if (userFunctions.size()) {
		cout << "User functions: " << endl;
		for (size_t i = 0; i < userFunctions.size(); i++) {
			cout << (i + 1) << ". " << userFunctions[i].name << "(" << userFunctions[i].arg << ") = ";

			for (size_t j = 0; j < userFunctions[i].rpn.size(); j++)
				cout << userFunctions[i].rpn[j] << " ";

			cout << endl;
		}
	}
}

void Calculator::PrintHelp() const {
	cout << "Main commands:" << endl;
	cout << "  help           print this message" << endl;
	cout << "  print state    print defined variables and functions" << endl;
	cout << "  reset          remove all defined variables and functions" << endl;
	cout << "  def            start to function definition" << endl;
	cout << "  set            start to variable definition" << endl;
	cout << "  quit           terminate program" << endl;
	cout << endl;

	cout << "Function definition syntax:" << endl;
	cout << "  def [function name] = [function definition]" << endl;
	cout << "    function name - word" << endl;
	cout << "    function definition - expression" << endl;
	cout << endl;
	cout << "Example: def f(x) = sin(2*x)" << endl;
	cout << endl;

	cout << "Variable definition syntax:" << endl;
	cout << "  set [variable name] = [variable definition]" << endl;
	cout << "    variable name - word" << endl;
	cout << "    variable definition - expression" << endl;
	cout << endl;
	cout << "Example: set twopi = 2 * pi" << endl;
	cout << endl;

	cout << "Built-in functions and constants:" << endl;
	cout << "  Trigonometry: sin, cos, tg, ctg, arcsin, arccos, arctg" << endl;
	cout << "  Other functions: sqrt, log, ln, lg, exp, abs, sign, min, max, pow" << endl;
	cout << "  Constants: pi, e" << endl;
}