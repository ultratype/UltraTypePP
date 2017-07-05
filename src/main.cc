#include <iostream>
#include "NTClient.h"
#include "NTLogger.h"
using namespace std;

NTClient cli;
void testLogger() {
	NTLogger out = NTLogger("Username");
	out << LOG_INFO;
	out.wr("Test message");
	out << LOG_INFO;
	out << "Here is an integer: ";
	out << (int)3;
	out.ln();
}
void initBot() {
	NTClient nclient = NTClient();
	cout << "Logging account in...\n";
	nclient.login("gfsiosd", "123asd123"); // Throw-away account
	nclient.connect();
}
int main(int argc, char** argv) {
	// testLogger();
	initBot();
	return 0;
}