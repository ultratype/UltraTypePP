#include "httplib.h"
#include "json.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <uWS/uWS.h>
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
		shared_ptr<httplib::Response> res = loginReq.post(NT_LOGIN_ENDPOINT, data, "application/x-www-form-urlencoded");
		if (res) {
			cout << "Login request complete\n";
			cout << res.get_header_value("set-cookie") << endl;
		} else {
			ret = false;
			cout << "Login request failed. This might be a network issue. Maybe try resetting your internet connection?\n";
		}
		return ret;
	}
protected:
	Hub* ws;
};