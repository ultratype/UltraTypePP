#ifndef NTLOGGER_H
#define NTLOGGER_H
#define LOG_HTTP (unsigned char)1
#define LOG_RACE (unsigned char)2
#define LOG_INFO (unsigned char)3
#define LOG_CONN (unsigned char)4

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
		hasFile = false;
	}
	NTLogger(string _fname) {
		fname = _fname;
		stream = ofstream();
		hasFile = true;
		stream.open(_fname);
	}
	void close() {
		if (closed == false && hasFile == true) {
			closed = true;
			stream.close();
		}
	}
	void operator<<(string msg) {
		cout << msg << endl;
		if (hasFile) {
			stream << msg << endl;
		}
	}
	void operator<<(int msgi) {
		cout << msgi;
		if (hasFile) {
			stream << msgi;
		}
	}
	void operator<<(unsigned char type) { // Unsigned char type to allow integers to be written to the stream
		writeType(type);
	}
	void type(unsigned char type) {
		writeType(type);
	}
protected:
	string fname;
	ofstream stream;
	bool closed;
	bool hasFile;
	void writeType(unsigned char type) { // Unsigned char type to allow integers to be written to the stream
		string out;
		string color;
		switch (type) {
			case LOG_HTTP:
				color = CLR_MAG;
				out += "[HTTP] ";
			break;
			case LOG_RACE:
				color = CLR_RED;
				out += "[RACE] ";
			break;
			case LOG_INFO:
				color = CLR_GRN;
				out += "[INFO] ";
				break;
			case LOG_CONN:
				color = CLR_BLU;
				out += "[CONNECT] ";
				break;
			default:
				cout << "Ignoring invalid log type!\n";
				break;
		}
		cout << color << out << CLR_RESET;
		if (hasFile) {
			stream << out;
		}
	}
};

#endif