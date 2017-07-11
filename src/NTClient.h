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
	NTLogger* log;
	bool connected;
	NTClient(int, double);
	~NTClient();
	bool login(string, string);
	bool connect();
	// Typing options
	int typeIntervalMS;
	double accuracy;
	time_t lastRaceStart;
	int wpm;
	int racesCompleted;
protected:
	// Properties
	Hub* wsh;
	string token; // Login token
	string loginCookie; // For outgoing requests that require authentication
	string pword;
	string ioCookie;
	string primusSid;
	string rawCookieStr;
	vector<SPair> cookies;
	SMap customHeaders;
	string lesson; // Ciphered via ROT47, but deciphering is not needed due to the nature of typing packets.
	int lessonLen;
	int rIdx;
	int eIdx;
	bool recievedEndPacket;
	bool finished;
	bool firstConnect;
	// Methods
	void addCookie(string, string);
	bool getPrimusSID();
	string getJoinPacket(int);
	void addListeners();
	void onConnection(WebSocket<CLIENT>*, HttpRequest);
	void onDisconnection();
	void onMessage(WebSocket<CLIENT>*, char*, size_t, OpCode);
	void handleData(WebSocket<CLIENT>*, json*);
	void handleRaceFinish(WebSocket<CLIENT>*);
	void sendTypePacket(WebSocket<CLIENT>*, int, string);
	void type(WebSocket<CLIENT>*);
};

#endif