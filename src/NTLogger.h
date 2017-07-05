#ifndef NTLOGGER_H
#define NTLOGGER_H
#define LOG_HTTP (unsigned char)1
#define LOG_RACE (unsigned char)2
#define LOG_INFO (unsigned char)3
#define LOG_CONN (unsigned char)4
#define lln 	 (unsigned char)5

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "colors.h"
using namespace std;

class NTLogger {
public:
	NTLogger(string uname) {
		closed = false;
		hasFile = false;
		username = uname;
	}
	NTLogger(string uname, string _fname) {
		fname = _fname;
		username = uname;
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
		writeTxt(msg);
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
	void ln() {
		writeLine();
	}
	void wr(string msg) {
		writeTxt(msg);
		writeLine();
	}
protected:
	string fname;
	ofstream stream;
	bool closed;
	bool hasFile;
	string username;
	void writeTxt(string msg) {
		cout << CLR_YEL << username << ": " << CLR_RESET;
		cout << msg;
		if (hasFile) {
			stream << username << ": " << msg;
		}
	}
	void writeLine() {
		cout << '\n';
		if (hasFile) {
			stream << '\n';
		}
	}
	void writeType(unsigned char type) { // Unsigned char type to allow integers to be written to the stream
		if (type == lln) {
			writeLine();
			return;
		}
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