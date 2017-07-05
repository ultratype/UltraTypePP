#ifndef NTLOGGER_H
#define NTLOGGER_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "colors.h"
using namespace std;

class NTLogger {
public:
	NTLogger() {
		closed = false;
	}
	NTLogger(string _fname) {
		fname = _fname;
		stream = ofstream();
		stream.open(_fname);
	}
protected:
	string fname;
	ofstream stream;
	bool closed;
};

#endif