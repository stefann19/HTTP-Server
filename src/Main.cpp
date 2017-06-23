//#include <iostream>
#include "HTTPEquation.h"
#include <string>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <map>
#include <ctype.h>
#include "asio.hpp"
#include "exprtk.hpp"
//class Process {
//	struct nod {
//		char info;
//		int prioritate;
//		nod *leg;
//	};
//	struct stiva {
//		nod *varf;
//	};
//	bool Push(stiva *s, char info, int prioritate) {
//		if (nod *x = new nod) {
//			x->info = info;
//			x->prioritate = prioritate;
//
//			x->leg = s->varf;
//			s->varf = x;
//			return true;
//		}
//		else return false;
//	}
//	char Pop(stiva *s) {
//		if (s->varf == NULL) {
//			return ' ';
//		}
//		else {
//			nod *y;
//			y = s->varf;
//			s->varf = s->varf->leg;
//			return y->info;
//		}
//	}
//	int Pop2(stiva *s) {
//		if (s->varf == NULL) {
//			return 0;
//		}
//		else {
//			nod *y;
//			y = s->varf;
//			s->varf = s->varf->leg;
//			return y->prioritate;
//		}
//	}
//	int xOpY(int x, int y, char op) {
//		if (op == '+')return x + y;
//		else if (op == '-')return x - y;
//		else if (op == '*')return x*y;
//		else if (op == '/') {
//			if (y != 0)
//				return x / y;
//		}
//		else if (op == '^')return (int)pow(x, y);
//		else return 0;
//	}
//public:
//	std::string Equation(std::string ec) {
//		if (ec.length() == 0)return "NO Expression";
//		stiva aux;
//		char **f;
//		aux.varf = NULL;
//		int numberOfParanthesis = 0, NumbersMinusSigns = 0;
//		f = new char*[ec.length()];
//		for (int i = 0; i < ec.length(); i++) {
//			f[i] = new char[10];
//		}
//		int k = 0;
//		int j = 0;
//		int prioritateSi;
//		std::string s;
//		for (int i = 0; i < ec.length(); i++) {
//			if (ec[i] == ' ')return "WHITE SPACE";
//			if (isalpha(ec[i]))return "Letters";
//			s = "";
//			j = 0;
//			int nr;
//			
//			std::stringstream ss(ec.substr(i, ec.length()));
//			ss >> nr;
//
//			s = std::to_string(nr);
//			j = s.length();
//			
//			
//			/*while (ec[i + j] <= '9'&& ec[i + j] >= '0') {
//				if (ec[i + j] == ' ')return "WHITE SPACE";
//				if (isalpha(ec[i]))return "Letters";
//				s += ec[i + j];
//				j++;
//			}*/
//			std::cout << ec.substr(i, ec.length()).find(s)<<" ()() "<<s<<" ()() " <<ec.substr(i, ec.length())<<" ()() " << j<<std::endl;
//			if (strchr("+-*/^", ec[i]) != NULL &&ec.substr(i, ec.length()).find(s) != 0) {
//				NumbersMinusSigns--;
//				prioritateSi = 0;
//				if (ec[i] == '+' || ec[i] == '-')prioritateSi = 2;
//				if (ec[i] == '*' || ec[i] == '/')prioritateSi = 3;
//				if (ec[i] == '^')prioritateSi = 4;
//
//				while (aux.varf != NULL && aux.varf->prioritate >= prioritateSi) {
//					f[k][0] = (Pop(&aux));
//					k++;
//				}
//				Push(&aux, ec[i], prioritateSi);
//			}
//			else if (ec[i] == '(') {
//				numberOfParanthesis++;
//				Push(&aux, ec[i], 1);
//			}
//			else if (ec[i] == ')') {
//				numberOfParanthesis--;
//				while (aux.varf != NULL && aux.varf->info != '(') {
//					f[k][0] = Pop(&aux);
//					k++;
//				}
//				if (aux.varf != NULL) {
//					if (aux.varf->info == '(') {
//						Pop(&aux);
//					}
//				}
//				else {
//					return "MISING (";
//				}
//			}
//			else {
//				NumbersMinusSigns++;
//				for (int l = 0; l < s.length(); l++) {
//					f[k][l] = s[l];
//				}
//				k++;
//				i += j - 1;
//
//			}
//		}
//		if (numberOfParanthesis != 0)return "MISSING )";
//		//if (NumbersMinusSigns != 1)return "OP miss";
//
//		char aux2 = Pop(&aux);
//		while (aux2 != ' ') {
//			f[k][0] = aux2;
//			aux2 = Pop(&aux);
//			k++;
//		}
//
//
//		for (int i = 0;i < k;i++) {
//			std::cout << f[i]<<std::endl;
//		}
//
//		int x, y;
//		for (int i = 0; i < k; i++) {
//			if (f[i][0] <= '9' && f[i][0] >= '0') {
//				Push(&aux, ' ', atoi(f[i]));
//			}
//			else {
//				y = Pop2(&aux);
//				x = Pop2(&aux);
//				if (f[i][0] == '/' && y == 0)return "Division by 0";
//				Push(&aux, ' ', xOpY(x, y, f[i][0]));
//			}
//		}
//		return std::to_string(Pop2(&aux));
//	}
//};


std::pair<std::string, std::string> EvaluateExpression(std::string expressionString,std::string jsonString) {
	typedef exprtk::symbol_table<double> symbol_table_t;
	typedef exprtk::expression<double>     expression_t;
	typedef exprtk::parser<double>             parser_t;

	std::string expression_str = expressionString;

	std::string err;
	json11::Json aux ;
	aux = json11::Json::parse(jsonString, err);

	// Register x with the symbol_table
	symbol_table_t symbol_table;

	// Instantiate expression and register symbol_table
	expression_t expression;
	std::vector<double> vars;
	for (int i = 0;i < expressionString.length();i++) {
		if (expressionString[i] >= 'a' && expressionString[i]<='z' || expressionString[i]>='A' && expressionString[i] <= 'Z'){
			if (expressionString[i + 1] >= 'a' && expressionString[i + 1]<='z' || expressionString[i + 1]>='A' && expressionString[i + 1] <= 'Z')break;
			std::string sAux="";
			sAux.push_back(expressionString[i]);

			vars.push_back(std::stod(EvaluateExpression(aux[sAux].string_value(),jsonString).second));
			//vars.push_back(std::stod(aux[sAux].string_value()));

			symbol_table.add_constant(sAux,(vars[vars.size()-1]));
		}
	}
	//std::cout<< symbol_table.get_variable("x")<<std::endl;

	//symbol_table.add_variable("x", x);

	//json11::Json symbols;

	expression.register_symbol_table(symbol_table);
	// Instantiate parser and compile the expression
	parser_t parser;
	parser.compile(expression_str, expression);

	double result = 0.0;

	// Evaluate and print result for when x = 1.1
	result = expression.value();

	return std::make_pair("200",std::to_string(result));
}

//Process ProcessInstance;

//std::pair<std::string, std::string> Equation(std::string token)
//{
//	std::pair<std::string, std::string> p;
//	if (token == "No data sent , or not sent properly") {
//		p.first = "404";
//		p.second = "No data sent , or not sent properly";
//		return p;
//	}
//	else {
//		std::string processResp = ProcessInstance.Equation(token);
//		std::string Code;
//		if (processResp[0] == 'M' || processResp[0] == 'D' || processResp[0] == 'O' || processResp[0] == 'N'
//			|| processResp[0] == 'W' || processResp[0] == 'L') {
//			Code = "400";
//		}
//		else {
//			Code = "200 OK";
//		}
//		p.first = Code; p.second = processResp;
//		return p;
//	}
//}

std::pair<std::string, std::string> Test(std::string token,std::string null = "") {
	std::pair<std::string, std::string> pair;
	pair.second = token + " was Tested";
	pair.first = "200 OK";
	return pair;
}

int main()
{

	FM.map.insert(std::make_pair(std::make_pair("Equation", "POST"), &EvaluateExpression));
	FM.map.insert(std::make_pair(std::make_pair("Test", "GET"), &Test));
	FM.map.insert(std::make_pair(std::make_pair("Test", "PUT"), &Test));
	FM.map.insert(std::make_pair(std::make_pair("Test", "DELETE"), &Test));
	FM.GenerateJson();
	//std::cout << FM.mapJson["Equation"].is_null();

	StartHTTPServicee(8080,1000);

	return 0;
}