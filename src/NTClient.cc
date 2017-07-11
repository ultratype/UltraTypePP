#include "NTClient.h"
using namespace std;

NTClient::NTClient(int _wpm, double _accuracy) {
	typeIntervalMS = 12000 / _wpm;
	wpm = _wpm;
	accuracy = _accuracy;
	hasError = false;
	firstConnect = true;
	connected = false;
	finished = false;
	lessonLen = 0;
	log = new NTLogger("(Not logged in)");
	wsh = nullptr;
	racesCompleted = 0;
	rIdx = 0;
	eIdx = 0;
}
NTClient::~NTClient() {
	if (log != nullptr) {
		delete log;
		log = nullptr;
	}
	if (wsh != nullptr) {
		delete wsh;
		wsh = nullptr;
	}
}
bool NTClient::login(string username, string password) {
	log->type(LOG_HTTP);
	log->wr("Logging into the NitroType account...\n");
	log->setUsername(username);
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
				log->type(LOG_HTTP);
				log->wr("Resolved ntuserrem login token.\n");
				// addCookie("ntuserrem", token);
			}
		}
		if (!foundLoginCookie) {
			ret = false;
			log->type(LOG_HTTP);
			log->wr("Unable to locate the login cookie. Maybe try a different account?\n");
		}
	} else {
		ret = false;
		log->type(LOG_HTTP);
		log->wr("Login request failed. This might be a network issue. Maybe try resetting your internet connection?\n");
	}
	if (ret == false) {
		log->type(LOG_HTTP);
		log->wr("Failed to log in.\n");
	} else {
		bool success = getPrimusSID();
		if (!success) return false;
	}
	return ret;
}
bool NTClient::connect() {
	wsh = new Hub();
	time_t tnow = time(0);
	rawCookieStr = Utils::stringifyCookies(&cookies);
	stringstream uristream;
	uristream  << "wss://realtime1.nitrotype.com:443/realtime/?_primuscb=" << tnow << "-0&EIO=3&transport=websocket&sid=" << primusSid << "&t=" << tnow << "&b64=1";
	string wsURI = uristream.str();
	log->type(LOG_CONN);
	log->wr("Attempting to connect to the NitroType realtime server...\n");
	if (firstConnect) {
		addListeners();
	}
	// cout << "Cookies: " << rawCookieStr << endl << endl;
	// Create override headers
	customHeaders["Cookie"] = rawCookieStr;
	customHeaders["Origin"] = "https://www.nitrotype.com";
	customHeaders["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/55.0.2883.87 Chrome/55.0.2883.87 Safari/537.36";
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
	log->type(LOG_HTTP);
	log->wr("Resolving Primus SID...\n");
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
		log->type(LOG_HTTP);
		log->wr("Resolved Primus SID successfully.\n");
		// addCookie("io", primusSid);
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
			{"avgSpeed", avgSpeed},
			{"debugging", false},
			{"music", "standard"},
			{"track", "forest"},
			{"update", 3417}
		}}
	};
	return "4" + p.dump();
}
void NTClient::addListeners() {
	assert(wsh != nullptr);
	wsh->onError([this](void* udata) {
		cout << "Failed to connect to WebSocket server." << endl;
		hasError = true;
	});
	wsh->onConnection([this](WebSocket<CLIENT>* wsocket, HttpRequest req) {
		log->type(LOG_CONN);
		log->wr("Established a WebSocket connection with the realtime server.\n");
		onConnection(wsocket, req);
	});
	wsh->onDisconnection([this](WebSocket<CLIENT>* wsocket, int code, char* msg, size_t len) {
		log->type(LOG_CONN);
		log->wr("Disconnected from the realtime server.\n");
		onDisconnection(wsocket, code, msg, len);
	});
	wsh->onMessage([this](WebSocket<CLIENT>* ws, char* msg, size_t len, OpCode opCode) {
		onMessage(ws, msg, len, opCode);
	});
}
void NTClient::onDisconnection(WebSocket<CLIENT>* wsocket, int code, char* msg, size_t len) {
	/*
	cout << "Disconn message: " << string(msg, len) << endl;
	cout << "Disconn code: " << code << endl;
	*/
	log->type(LOG_CONN);
	log->wr("Reconnecting to the realtime server...\n");
	getPrimusSID();
	rawCookieStr = Utils::stringifyCookies(&cookies);
	stringstream uristream;
	uristream  << "wss://realtime1.nitrotype.com:443/realtime/?_primuscb=" << time(0) << "-0&EIO=3&transport=websocket&sid=" << primusSid << "&t=" << time(0) << "&b64=1";
	string wsURI = uristream.str();
	wsh->connect(wsURI, (void*)this, customHeaders, 7000);
}
void NTClient::handleData(WebSocket<CLIENT>* ws, json* j) {
	// Uncomment to dump all raw JSON packets
	// cout << "Recieved json data:" << endl << j->dump(4) << endl;
	if (j->operator[]("msg") == "setup") {
		log->type(LOG_RACE);
		log->wr("I joined a new race.\n");
		recievedEndPacket = false;
		rIdx = 0;
		eIdx = 0;
		finished = false;
	} else if (j->operator[]("msg") == "joined") {
		string joinedName = j->operator[]("payload")["profile"]["username"];
		string dispName;
		try {
			dispName = j->operator[]("payload")["profile"]["displayName"];
		} catch (const exception& e) {
			dispName = "[None]";
		}
		log->type(LOG_RACE);
		if (joinedName == "bot") {
			log->wr("Bot user '");
			log->wrs(dispName);
			log->wrs("' joined the race.\n");
		} else {
			log->wr("Human user '");
			log->wrs(joinedName);
			log->wrs("' joined the race.\n");
		}
	} else if (j->operator[]("msg") == "status" && j->operator[]("payload")["status"] == "countdown") {
		lastRaceStart = time(0);
		log->type(LOG_RACE);
		log->wr("The race has started.\n");
		lesson = j->operator[]("payload")["l"];
		lessonLen = lesson.length();
		log->type(LOG_INFO);
		log->wr("Lesson length: ");
		log->operator<<(lessonLen);
		log->ln();
		this_thread::sleep_for(chrono::milliseconds(50));
		type(ws);
	} else if (j->operator[]("msg") == "update" &&
		j->operator[]("payload")["racers"][0] != nullptr &&
		j->operator[]("payload")["racers"][0]["r"] != nullptr) {
			// Race has finished for a client
			if (recievedEndPacket == false) {
				// Ensures its this client
				recievedEndPacket = true;
				handleRaceFinish(ws);
			}
	}
}
void NTClient::onMessage(WebSocket<CLIENT>* ws, char* msg, size_t len, OpCode opCode) {
	if (opCode != OpCode::TEXT) {
		cout << "The realtime server did not send a text packet for some reason, ignoring.\n";
		return;
	}
	string smsg = string(msg, len);
	if (smsg == "3probe") {
		// Response to initial connection probe
		ws->send("5", OpCode::TEXT);
		// Join packet
		this_thread::sleep_for(chrono::seconds(1));
		string joinTo = getJoinPacket(20); // 20 WPM just to test
		ws->send(joinTo.c_str(), OpCode::TEXT);
	} else if (smsg.length() > 2 && smsg[0] == '4' && smsg[1] == '{') {
		string rawJData = smsg.substr(1, smsg.length());
		json jdata;
		try {
			jdata = json::parse(rawJData);
		} catch (const exception& e) {
			// Some error parsing real race data, something must be wrong
			cout << "There was an issue parsing server data: " << e.what() << endl;
			return;
		}
		handleData(ws, &jdata);
	} else {
		cout << "Recieved unknown WebSocket message: '" << smsg << "'\n";
	}
}
void NTClient::onConnection(WebSocket<CLIENT>* wsocket, HttpRequest req) {
	// Send a probe, which is required for connection
	wsocket->send("2probe", OpCode::TEXT);
}
void NTClient::sendTypePacket(WebSocket<CLIENT>* ws, int idx, string typeType) {
	json p = {
		{"stream", "race"},
		{"msg", "update"},
		{"payload", {}}
	};
	p["payload"][typeType] = idx;
	string packet = "4" + p.dump();
	// cout << "Packet: " << packet << endl;
	ws->send(packet.c_str(), OpCode::TEXT);
}
void NTClient::type(WebSocket<CLIENT>* ws) {
	if (rIdx > lessonLen) {
		// All characters have been typed
		if (!finished) {
			handleRaceFinish(ws);
		}
		return;
	}
	int low = typeIntervalMS - 15;
	int high = typeIntervalMS + 15;
	bool isRight = Utils::randBool(accuracy);
	int sleepFor = Utils::randInt(low, high);
	if (low < 10) {
		low = Utils::randInt(9, 12);
	}
	if (rIdx % 25 == 0) { // Display info every 25 characters
		// Log race updated
		log->type(LOG_INFO);
		log->wr("I have finished ");
		log->operator<<((((double)rIdx) / ((double)lessonLen)) * 100.00);
		log->wrs("% of the race (");
		log->operator<<((int)rIdx);
		log->wrs(" / ");
		log->operator<<((int)lessonLen);
		log->wrs(" characters at ");
		log->operator<<((int)wpm);
		log->wrs(" WPM)");
		log->ln();
	}
	if (!isRight) {
		++eIdx;
		sendTypePacket(ws, eIdx, "e");
	}
	++rIdx;
	sendTypePacket(ws, rIdx, "t");
	// cout << "rIdx " << rIdx << ", eIdx: " << eIdx << ", isRight: " << isRight << ", sleepFor: " << sleepFor << endl;
	this_thread::sleep_for(chrono::milliseconds(sleepFor));
	type(ws); // Call the function until the lesson has been "typed"
}
void NTClient::handleRaceFinish(WebSocket<CLIENT>* ws) {
	finished = true;
	racesCompleted++;
	int raceCompleteTime = time(0) - lastRaceStart;
	rIdx = 0;
	eIdx = 0;
	log->type(LOG_RACE);
	log->wr("The race has finished.\n");
	log->type(LOG_INFO);
	log->wr("The race took ");
	log->operator<<(raceCompleteTime);
	log->wrs(" seconds to complete. Waiting 5 seconds before the next race.\n");
	log->type(LOG_RACE);
	log->wr("I have completed ");
	log->operator<<(racesCompleted);
	log->wrs(" race(s).\n");
	this_thread::sleep_for(chrono::seconds(5));
	log->type(LOG_CONN);
	log->wr("Closing WebSocket...\n");
	ws->close();
}