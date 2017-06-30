#include "httplib.h"
#include "json.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <uWS/uWS.h>
using namespace std;
using namespace httplib;
using namespace nlohmann;
using namespace uWS;

class NTClient {
public:
	string uname;
	string pword;
	NTClient(string username, string password) {
		uname = username;
		pword = password;
	}
protected:
	Hub* ws;
};