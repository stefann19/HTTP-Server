#pragma once
#include <string>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <map>
#include <ctype.h>
#include "json11.hpp"

typedef std::pair<std::string, std::string> stringPair;
typedef std::pair<std::string, std::string>(*ScriptFunction)(std::string,std::string); // function pointer type 
typedef std::map<stringPair, ScriptFunction> script_map;



class FunctionsMap {
public:
	script_map map;
	json11::Json mapJson = json11::Json::object{};

	std::pair<std::string, std::string> call_script(const stringPair& pFunction, std::string token,std::string aux);
	
	void CoutMSize();
	void GenerateJson();

	std::vector<std::pair<std::string,std::string>> FindAllFunctions();
};
extern FunctionsMap FM;

void StartHTTPServicee(int port,int keepAliveTime);