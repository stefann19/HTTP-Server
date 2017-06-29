#pragma once
#include "HTTPEquation.h"
#include "asio.hpp"
#include "json11.hpp"
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <windows.h>
#include <ctime>

typedef std::pair<std::string, std::string> stringPair;
typedef std::pair<std::string, std::string>(*ScriptFunction)(std::string,std::string); // function pointer type 
typedef std::map<stringPair, ScriptFunction> script_map;

//Json my_json = Json::object{
//	{ "key1", "value1" },
//	{ "key2", false },
//	{ "key3", Json::array { 1, 2, 3 } },
//};


FunctionsMap FM;

void FunctionsMap::GenerateJson() {
	std::vector<stringPair> vs = FM.FindAllFunctions();
	std::string jsonString,aux;
	jsonString = "{";
	for (int i = 0;i < map.size();i++) {
		if (i + 1 == map.size()) {
			jsonString += "\"" + vs[i].first + "\":\"" + vs[i].second + "\"";
		}
		else {
			jsonString += "\"" + vs[i].first + "\":\"" + vs[i].second + "\", ";
		}
	}
	jsonString += "}";
	mapJson = json11::Json::parse(jsonString,aux);
}

std::pair<std::string, std::string> FunctionsMap::call_script(const stringPair& pFunction, std::string token,std::string aux)
{
	std::pair<std::string, std::string> p;
	script_map::const_iterator iter = map.find(pFunction);
	if (iter == map.end())
	{
		p.first = "404";
		p.second = "bad url";
		return p;
	}
	else {
		p = (*iter->second)(token,aux);
		return p;
	}
}


using asio::ip::tcp;

std::string VectorStringMapToString(std::vector<stringPair> vs) {
	std::string s;
	for (int i = 0; i < vs.size(); i++) {
		s += "<br>"+vs[i].first + " : " + vs[i].second + "\n";
	}
	return s;
}



class PacketInfo {
public:
	std::string type;
	std::string httpV;
	std::string host;
	std::string function;
	std::string responseCode;
	std::vector<stringPair> packetBody;
	std::vector<stringPair> processedPacketBody;
	std::string procceseedPacketBodyString;
	std::string jsonS;
	bool keepAlive;

	void ProcessPacket() {
		std::string variables="";
		for (int i = 0;i < packetBody.size();i++) {
			processedPacketBody.push_back(FM.call_script(std::make_pair(function, type), packetBody[i].second, jsonS));
			processedPacketBody[i].first = packetBody[i].first+" ["+processedPacketBody[i].first+"] ";
		}
	}
};

namespace ExtractInfo {

	 std::vector<stringPair> ExtractInfoPost(char data_[8192], std::string &httpV, std::string &host,bool &keepAlive) {
		char *aux = "";
		std::string delimiter = "\r\n";
		std::string s(data_);
		size_t pos = 0;
		std::string token;
		//std::cout << std::endl;
		int numberOfVariables = -2;

		std::string aus=s;
		while (pos != s.npos) {
			pos = aus.find("------");
			aus.erase(0, pos+1);
			numberOfVariables++;
		}
		std::vector<stringPair> variables;

		if (s.find("keep-alive")!=s.npos)keepAlive = true;
		else keepAlive = false;

		pos = s.find("/"); s.erase(0, pos + 1); pos = s.find("/");
		host = s.substr(0, s.find(" "));
		token = s.substr(pos + 1, pos + 4);
		httpV = token.substr(0, 3);
		pos = s.find(delimiter);
		s.erase(0, pos + delimiter.length());

		pos = s.find(delimiter);
		token = s.substr(0, pos); pos = token.find(":");
		token = token.substr(pos + 2, token.size());
		host = token + "/" + host;
		pos = s.find("\"");
		s.erase(0, pos+1 );

		for (int i = 0;i < numberOfVariables;i++) {
			stringPair p;
			p.first = s.substr(0, s.find("\""));
			s.erase(0,s.find("\r\n\r\n")+4);
			p.second = s.substr(0, s.find("\r\n"));
			variables.push_back(p);

			s.erase(0, s.find("\"")+1);
		}



		pos = s.find((char)(-51));
		token = s.substr(0, pos);
		return variables;
	}
	std::vector<stringPair> ExtractInfoGet(char data_[8192], std::string &httpV, std::string &host,bool &keepAlive) {
		char *aux = ""; int nr = 0;
		std::string delimiter = "\r\n";
		std::string s(data_);
		size_t pos = 0;
		std::string token, dummy = s, auxS;//auxiliaries
		std::vector<stringPair> variables;

		if (s.find("keep-alive")!=s.npos)keepAlive = true;
		else keepAlive = false;

		pos = dummy.find("/"); dummy.erase(0, pos + 1); pos = dummy.find("/");
		host = dummy.substr(0, dummy.find(" "));
		auxS = host.substr(host.find("?") + 1, host.size());//auxS holds the variables eg : auxS = "name=20&name=30"
		int numberOfVariables = std::count(auxS.begin(), auxS.end(), '=');
		

		for (int i = 0; i < numberOfVariables; i++) {
			stringPair p;
			p.first = auxS.substr(0, auxS.find('='));
			auxS.erase(0, auxS.find('=') + 1);
			p.second = auxS.substr(0,auxS.find('&'));
			variables.push_back(p);
			auxS.erase(0, auxS.find('&') + 1);
		}



		token = dummy.substr(pos + 1, pos + 4);
		httpV = token.substr(0, 3);//httpv holds the httpVersion : httpV = "1.1"
		pos = dummy.find(delimiter);
		dummy.erase(0, pos + delimiter.length());

		pos = dummy.find(delimiter);
		token = dummy.substr(0, pos); pos = token.find(":");
		token = token.substr(pos + 2, token.size());
		host = token + "/" + host;//host holds the full addres eg :host = "127.0.0.1:8080/Test?name=20&name=30"
		return variables;
	}
	PacketInfo ExtractInfoMain(char data_[8192]) {
		PacketInfo info;
		std::string type, dataS(data_);
		type = dataS.substr(0, dataS.find(" "));
		std::vector<stringPair> token;
		std::string httpV, host;
		bool keepAlive;
		if (type == "GET") {
			token = ExtractInfoGet(data_, httpV, host, keepAlive);
		}
		else {
			token = ExtractInfoPost(data_, httpV, host, keepAlive);
		}



		info.type = type;
		info.httpV = httpV;
		info.host = host;
		info.packetBody = token;
		info.keepAlive = keepAlive;
		info.function = info.host.substr(host.find("/") + 1, host.find("?") - 1);
		info.function = info.function.substr(0, info.function.find("?"));
		return info;
	}
};

int writes=0;
int threads = 1;
int sessions = 0;

int timeToKeepOpen = 1000;

class mainSession 
	: public std::enable_shared_from_this<mainSession>
{
public:
	int keepAliveSeconds;
	mainSession(tcp::socket socket)
		: socket_(std::move(socket))
	{
		keepAliveSeconds = 10;
	}
	char buffsize[8192];
	asio::mutable_buffers_1 buff = asio::buffer(buffsize, 8192);
	time_t lastReadTime;
	void start()
	{
		jsonString = "{ \"Name\":\"Default\" }";
		std::string aux;
		variables = json11::Json::parse(jsonString, aux);

		sessions++;
		threads++;
		//std::cout << "Sessions:" << sessions<<std::endl;
		
		lastReadTime = time(0);
		do_read();




	}
	~mainSession() {
		threads--;
		std::cout << threads << "[]" << sessions << std::endl;
	}
	std::string jsonString;
private:
	
	json11::Json variables;

	void addVariable(std::string name,std::string value) {
		int pos;
		std::string aux;

		if (name.find("set(") != std::string::npos) {
			name = name.substr(name.find("(")+1, name.find(")")-name.find("(")-1);
		}

		if ( jsonString.find(name) == std::string::npos || variables[name].is_null()) {
			aux = ",\"" + name + "\":\"" + value + "\"";
			jsonString.insert(jsonString.length() - 1, aux);
		}
		else {
			
			std::string oldValue = variables[name].string_value();
			pos = jsonString.find(oldValue);
			jsonString.erase(pos, oldValue.length());
			jsonString.insert(pos, value);
		}
		std::cout << jsonString;
		variables = json11::Json::parse(jsonString, aux);

		std::cout << variables["x"].string_value()<<std::endl;
	}


	void do_read()
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		auto self(shared_from_this());
		socket_.async_read_some(asio::buffer(data_, max_length),
			[this, self](std::error_code ec, std::size_t length)
		{
				std::string resp = "";
				//std::cout << ec.message();
				if (!ec)
				{
					std::cout << data_<<"|||"<<std::endl;
					PacketInfo packet;
					packet = ExtractInfo::ExtractInfoMain(data_);

					for (int i = 0;i < packet.packetBody.size();i++) {
						if (packet.packetBody[i].first.find("set(") != std::string::npos) {
							addVariable(packet.packetBody[i].first, packet.packetBody[i].second);
						}
					}
					packet.jsonS = jsonString;
					if (packet.keepAlive) {
						//keepAliveSeconds = 10;
						lastReadTime = time(0);
					}
					else { //keepAliveSeconds = 100;
						lastReadTime = 0;
					}

					if (packet.host.substr(0, packet.host.find("/")+1) == packet.host) {
						packet.responseCode = "200";
						packet.processedPacketBody = FM.FindAllFunctions();
						packet.procceseedPacketBodyString = VectorStringMapToString(packet.processedPacketBody);
						
					}
					else if (packet.function.find("htm")!= std::string::npos) {
						std::ifstream t(packet.function);
						std::stringstream buffer;
						buffer << t.rdbuf();
						packet.procceseedPacketBodyString = buffer.str();
						packet.responseCode = "200 OK";
					}else {
						packet.ProcessPacket();
						packet.procceseedPacketBodyString = VectorStringMapToString(packet.processedPacketBody);

					}
				/*	for (int i = 0;i < packet.packetBody.size();i++) {
						if (packet.packetBody[i].first.find("set(") != std::string::npos) {
							addVariable(packet.packetBody[i].first, packet.packetBody[i].second);
						}
					}*/
					for (int i = 0;i < packet.packetBody.size();i++) {
						std::cout << packet.packetBody[i].first<<std::endl;
					}
					resp = resp +
						"HTTP/" + packet.httpV + " " + packet.responseCode /*info.first */ + "\r\n" +
						"Host: " + packet.host + "\r\n" +
						"Content-Type: text/html\r\n" +
						"Connection: close\r\n" +
						"Access-Control-Allow-Origin: null \r\n" +
						"Access-Control-Allow-Headers: Content-Type \r\n"+
						"Accept: */*\r\n" +
						"Content-Length:" + std::to_string(packet.procceseedPacketBodyString.length()) + "\r\n" +
						"\r\n" +
						packet.procceseedPacketBodyString
						+ "\r\n" + "\r\n";

				//std::cout << std::endl;
				
				strcpy(buffsize, resp.c_str());
				//std::cout << resp.length() << std::endl;
				//std::cout << time(0) - lastReadTime << std::endl;
				do_write(resp.length());

				}
				else { /*if (keepAliveSeconds > 0 && keepAliveSeconds < 80) {
					   do_write(0);
			   }*/	//std::cout << time(0) - lastReadTime << std::endl;
					time(0);
					if (time(0) - lastReadTime < timeToKeepOpen) {
						do_write(0);
					}
				}
		});
		
	}

	void do_write(int length)
	{
		auto self(shared_from_this());
		asio::async_write(socket_, asio::buffer(buffsize, length),
			[this, self](std::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				writes++;
				if (writes % 25 == 0)
					
				time(0);
				if (time(0) - lastReadTime < timeToKeepOpen){
					do_read();
				}
			}
		});
	}

	tcp::socket socket_;
	enum { max_length = 8192 };
	char data_[max_length];
};

class server
{
public:
	server(asio::io_service& io_service, short port)
		: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		socket_(io_service)
	{
		do_accept();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(socket_,
			[this](std::error_code ec)
		{
			if (!ec)
			{
				std::make_shared<mainSession>(std::move(socket_))->start();
			}
			do_accept();
		});
	}

	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

void FunctionsMap::CoutMSize() {
	//std::cout << map.size();
}



std::vector<stringPair> FunctionsMap::FindAllFunctions() {
	std::vector<stringPair> functions;
	int k = 0;
	for (script_map::iterator i = map.begin(); i != map.end(); i++) {
		functions.push_back(std::make_pair(i->first.first, i->first.second));
		k++;
	}
	return functions;
}


void StartHTTPServicee(int port,int keepAliveTime) {
	timeToKeepOpen = keepAliveTime;
	try
	{
		asio::io_service io_service;
		server s(io_service, port);
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

}