#include "NTClient.h"
using namespace std;

NTClient::NTClient() {
	hasError = false;
	firstConnect = true;
	connected = false;
}
bool NTClient::login(string username, string password) {
	uname = username;
	pword = password;
	bool ret = true;
	string data = string("username=");
	data += uname;
	data += "&password=";
	data += pword;
	data += "&adb=1&tz=America%2FChicago"; // No need to have anything other than Chicago timezone
	httplib::SSLClient loginReq(NITROTYPE_HOSTNAME, HTTPS_PORT);
	shared_ptr<httplib::Response> res = loginReq.post(NT_LOGIN_ENDPOINT, data, "application/x-www-form-urlencoded; charset=UTF-8");
	if (res) {
		bool foundLoginCookie = false;
		for (int i = 0; i < res->cookies.size(); ++i) {
			string cookie = res->cookies.at(i);
			addCookie(Utils::extractCKey(cookie), Utils::extractCValue(cookie));
			if (cookie.find("ntuserrem=") == 0) {
				foundLoginCookie = true;
				token = Utils::extractCValue(cookie);
				cout << "Retrieved login token: " << token << endl;
				addCookie("ntuserrem", token);
			}
		}
		if (!foundLoginCookie) {
			ret = false;
			cout << "Unable to locate the login cookie. Maybe try a different account?\n";
		}
	} else {
		ret = false;
		cout << "Login request failed. This might be a network issue. Maybe try resetting your internet connection?\n";
	}
	bool success = getPrimusSID();
	if (!success) return false;
	return ret;
}
bool NTClient::connect() {
	wsh = new Hub();
	time_t tnow = time(0);
	stringstream uristream;
	uristream  << NT_REALTIME_WS_ENDPOINT << "?_primuscb=" << tnow << "-0&EIO=3&transport=websocket&sid=" << primusSid << "&t=" << tnow << "&b64=1";
	string wsURI = uristream.str();
	cout << "Connecting to endpoint: " << wsURI << endl;
	if (firstConnect) {
		addListeners();
	}
	// cout << "Cookies: " << rawCookieStr << endl << endl;
	// Create override headers
	SMap customHeaders;
	customHeaders["Cookie"] = rawCookieStr;
	customHeaders["Origin"] = "https://www.nitrotype.com";
	// customHeaders["Host"] = "realtime1.nitrotype.com";
	wsh->connect(wsURI, (void*)this, customHeaders, 7000);
	// wsh->connect(wsURI);
	if (firstConnect) {
		wsh->run();
		firstConnect = false;
	}
	return true;
}
void NTClient::addCookie(string key, string val) {
	SPair sp = SPair(key, val);
	cookies.push_back(sp);
}
bool NTClient::getPrimusSID() {
	time_t tnow = time(0);
	rawCookieStr = Utils::stringifyCookies(&cookies);
	stringstream squery;
	squery << "?_primuscb=" << tnow << "-0&EIO=3&transport=polling&t=" << tnow << "-0&b64=1";
	string queryStr = squery.str();

	httplib::SSLClient loginReq(NT_REALTIME_HOST, HTTPS_PORT);
	string path = NT_PRIMUS_ENDPOINT + queryStr;
	shared_ptr<httplib::Response> res = loginReq.get(path.c_str(), rawCookieStr.c_str());
	if (res) {
		json jres = json::parse(res->body.substr(4, res->body.length()));
		primusSid = jres["sid"];
		cout << "Resolved primus SID: " << primusSid << endl;
		addCookie("io", primusSid);
		rawCookieStr = Utils::stringifyCookies(&cookies);
	} else {
		cout << "Error retrieving primus handshake data.\n";
		return false;
	}
	return true;
}
string NTClient::getJoinPacket(int avgSpeed) {
	json p = {
		{"stream", "race"},
		{"msg", "join"},
		{"payload", {
			{"debugging", false},
			{"avgSpeed", avgSpeed},
			{"track", "desert"},
			{"music", "dirty_bit"},
			{"update", 3417}
		}}
	};
	string ret = "4";
	ret += p.dump();
	return ret;
}
void NTClient::addListeners() {
	assert(wsh != nullptr);
	wsh->onError([this](void* udata) {
		cout << "Failed to connect to WebSocket server." << endl;
		hasError = true;
	});
	wsh->onConnection([this](WebSocket<CLIENT>* wsocket, HttpRequest req) {
		cout << "Connected to the realtime server." << endl;
		onConnection(wsocket, req);
	});
	wsh->onDisconnection([this](WebSocket<CLIENT>* wsocket, int code, char* msg, size_t len) {
		cout << "Disconnected from the realtime server." << endl;
		onDisconnection(wsocket, code, msg, len);
	});
	wsh->onMessage([this](WebSocket<SERVER>* ws, char* msg, size_t len, OpCode opCode) {
		onMessage(ws, msg, len, opCode);
	});
}
void NTClient::onDisconnection(WebSocket<CLIENT>* wsocket, int code, char* msg, size_t len) {
	cout << "Disconn message: " << string(msg, len) << endl;
	cout << "Disconn code: " << code << endl;
}
void NTClient::onMessage(WebSocket<SERVER>* ws, char* msg, size_t len, OpCode opCode) {
	cout << "ws message" << endl; // TODO: parse incoming messages
}
void NTClient::onConnection(WebSocket<CLIENT>* wsocket, HttpRequest req) {
	// Send a probe, which is required for connection
	wsocket->send("2probe", OpCode::TEXT);
	/*
		string joinTo = getJoinPacket(100); // 100 WPM just to test
		cout << joinTo << endl;
		wsocket->send(joinTo.c_str());
	*/
}