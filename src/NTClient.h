#include "httplib.h"
#include "json.hpp"
#include "utils.h"

#include <iostream>
#include <vector>
#include <string>
#include <uWS/uWS.h>
#include <map>
#include <algorithm>
using namespace std;
using namespace nlohmann;
using namespace uWS;

#define NITROTYPE_HOSTNAME "www.nitrotype.com"
#define NT_LOGIN_ENDPOINT "/api/login"
#define HTTP_PORT 80
#define HTTPS_PORT 443

class NTClient {
public:
	string uname;
	string pword;
	string token; // Login token
	NTClient(string username, string password) {
		uname = username;
		pword = password;
	}
	bool login() {
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
				if (cookie.find("ntuserrem=") == 0) {
					foundLoginCookie = true;
					vector<string> parts = Utils::split(cookie, '=');
					string part1 = parts.at(1);
					vector<string> parts2 = Utils::split(part1, ';');
					token = parts2.at(0);
					cout << "Retrieved login token: " << token << endl;
				}
			}
			if (!foundLoginCookie) {
				cout << "Unable to locate the login cookie. Maybe try a different account?\n";
			}
		} else {
			ret = false;
			cout << "Login request failed. This might be a network issue. Maybe try resetting your internet connection?\n";
		}
		return ret;
	}
	bool primusRequest() {
		return false;
	}
protected:
	Hub* ws;
};