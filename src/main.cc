#include <iostream>
#include "NTClient.h"

int main(int argc, char** argv) {
	NTClient nclient = NTClient("gfsiosd", "123asd123"); // Throw-away account
	cout << "Logging account in...\n";
	nclient.login();
	nclient.connect();
	return 0;
}