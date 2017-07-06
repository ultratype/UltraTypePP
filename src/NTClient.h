#ifndef NTCLIENT_H
#define NTCLIENT_H

#include "httplib.h"
#include "json.hpp"
#include "NTLogger.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <string>
#include <uWS/uWS.h>
#include <map>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <thread>
using namespace std;
using namespace nlohmann;
using namespace uWS;

#define NITROTYPE_HOSTNAME "www.nitrotype.com"
#define NT_REALTIME_HOST "realtime1.nitrotype.com"
#define NT_REALTIME_WS_ENDPOINT "wss://realtime1.nitrotype.com/realtime/"
#define NT_PRIMUS_ENDPOINT "/realtime/"
#define NT_LOGIN_ENDPOINT "/api/login"
#define HTTPS_PORT 443
typedef std::map<std::string, std::string> SMap;
typedef std::pair<std::string, std::string> SPair;

class NTClient {
public:
	string uname;
	bool connected;
	NTClient();
	bool login(string, string);
	bool connect();
protected:
	Hub* wsh;
	string token; // Login token
	string loginCookie; // For outgoing requests that require authentication
	string pword;
	string ioCookie;
	string primusSid;
	string rawCookieStr;
	vector<SPair> cookies;
	bool hasError;
	bool firstConnect;
	void addCookie(string, string);
	bool getPrimusSID();
	string getJoinPacket(int);
	void addListeners();
	void onConnection(WebSocket<CLIENT>*, HttpRequest);
	void onDisconnection(WebSocket<CLIENT>*, int, char*, size_t);
	void onMessage(WebSocket<CLIENT>*, char*, size_t, OpCode);
	void handleData(WebSocket<CLIENT>*, json*);
};

#endif