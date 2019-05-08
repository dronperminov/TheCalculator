#include <iostream>
#include <string>

#include "Calculator.hpp"

using namespace std;

int main() {
	string mode;

	cout << "Welcome to CALCULATOR!" << endl;
	cout << "Enter trigonometry mode (1 - degrees, [2] - radians): ";
	getline(cin, mode);

	while (mode != "1" && mode != "2" && mode != "") {
		cout << "Incorrect mode. Try again: ";
		getline(cin, mode);
	}

	cout << "Type your commands after '>' and press 'Enter'" << endl;
	cout << "Use 'help' command for usage" << endl;

	Calculator calculator(mode == "1");

	do {
		string command; // строка для считывания команды
		cout << ">"; // приглашение ко вводу
		getline(cin, command); // считываем строку-команду

		// если команда вывода сообщения
		if (command == "help") {
			calculator.PrintHelp(); // выводим сообщение
			continue;
		}

		// если команда вывода состояния
		if (command == "print state") {
			calculator.PrintState(); // выводим состояние калькулятора
			continue;
		}

		// если команда сброса состояния калькулятора
		if (command == "reset") {
			calculator.Reset(); // сбрасываем состояние калькулятора
			continue;
		}

		// если команда выхода, то выходим
		if (command == "quit")
			break;

		try {
			calculator.Calculate(command);
		}
		catch (string error) {
			cout << "error: " << error << endl;
		}
	} while (1);
}