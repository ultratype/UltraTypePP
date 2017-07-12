#include "NTClient.h"
#include "colors.h"
#include "json.hpp"
#include "Account.h"

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
using namespace std;
vector<Account> accounts;
mutex tlock;

void initlog(string msg) {
	cout << CLR_GRN << STYLE_BOLD << "[INIT] " << CLR_RESET << CLR_WHT << msg << CLR_RESET;
}
void errlog(string msg) {
	cout << CLR_RED << STYLE_BOLD << "[ERR!] " << CLR_RESET << CLR_WHT << msg << CLR_RESET;
}
void threadCallback(Account a) {
	NTClient cli(a.wpm, a.acc);
	bool success = cli.login(a.name, a.pass);
	if (success) {
		cli.connect();
	} else {
		errlog("Failed to log in to an account.\n");
	}
}
void initMultiBot() {
	initlog("Starting accounts...\n");
	for (int i = 0; i < accounts.size(); ++i) {
		tlock.lock();
		thread t(threadCallback, accounts.at(i));
		t.detach();
		tlock.unlock();
	}
	initlog("Press enter to terminate the bots.");
	getchar();
}
void initSingleBot(string username, string password, int wpm, double acc) {
	NTClient nclient = NTClient(wpm, acc);
	bool success = nclient.login(username, password);
	if (success) {
		nclient.connect();
	} else {
		errlog("Failed to log in to the account. Exiting the program.\n");
	}
}
int main(int argc, char** argv) {
	srand(static_cast<unsigned int>(time(0)));
	bool useCustomConfig = false;
	string customConfig;
	for (int i = 0; i < argc; ++i) {
		string arg = string(argv[i]);
		if (i != 0) {
			if (arg == "--help" || arg == "-h") {
				cout << "UltraType++ - An open-source command line NitroType bot." << endl
				<< "GitHub URL: https://github.com/ultratype/UltraTypePP" << endl
				<< "Version: 1.0" << endl
				<< "Arguments:" << endl
				<< "	--help or -h: Display this help message." << endl
				<< "	--config <filename> or -c <filename>: Load the config from the specified file." << endl;
				return 0;
			} else if (arg == "--config" || arg == "-c") {
				customConfig = string(argv[i + 1]);
				useCustomConfig = true;
			}
		}
	}
	ifstream configf;
	configf.exceptions(std::ios::failbit | std::ios::badbit);
	initlog("Attempting to read config file...\n");
	try {
		if (useCustomConfig) {
			configf.open(customConfig.c_str());
		} else {
			configf.open("config.json");
		}
	} catch(const exception& e) {
		errlog("Failed to open the JSON config. For help, read the UltraType++ repository README, or use --help.\n");
		return 1;
	}
	// Read entire stream into string
	string fdata(istreambuf_iterator<char>(configf), {});
	json jdata;
	try {
		jdata = json::parse(fdata);
	} catch (const exception& e) {
		errlog("Failed to parse the JSON config file. Maybe read over it for syntax errors?\n");
		return 1;
	}
	bool multiAcc = false;
	try {
		multiAcc = jdata["multi_account"];
	} catch (const exception& e) {
		errlog("Failed to detect multi account. Maybe read over it for errors?\n");
		return 1;
	}
	initlog("Read and parsed config file.\n");
	if (multiAcc == false) {
		string uname = jdata["username"];
		string pword = jdata["password"];
		int wpm = jdata["wpm"];
		double accuracy = jdata["accuracy"];
		initSingleBot(uname, pword, wpm, accuracy);
	} else {
		json jaccs = jdata["accounts"];
		for (json& ac : jaccs) {
			string aname;
			string apass;
			int awpm;
			double aacc;
			try {
				aname = ac["user"];
				apass = ac["pass"];
				awpm = ac["wpm"];
				aacc = ac["accuracy"];
			} catch (const exception& e) {
				errlog("Failed to parse accounts. Maybe read over it for errors?\n");
				return 1;
			}
			Account a = Account(aname, apass, awpm, aacc);
			accounts.push_back(a);
		}
		initMultiBot();
	}
	return 0;
}
