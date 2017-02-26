// Michael Garlak
// Crime Data Structure

#include <string>
#include <iostream>

#ifndef CDAT_H
#define CDAT_H

class cDat {
public:
	bool isFull();
	void reset();
	friend std::ostream& operator<< (std::ostream&, const cDat&);
	std::string repNum;
	std::string date;
	std::string loc;
	std::string incident;
	std::string dis;
	std::string comment;
};

bool cDat::isFull(){
	if (repNum.empty() || date.empty() || loc.empty() || incident.empty() || dis.empty() || comment.empty())
		return false;
	return true;
}

void cDat::reset(){
	repNum.clear();
	date.clear();
	loc.clear();
	incident.clear();
	dis.clear();
	comment.clear();
}

std::ostream& operator<< (std::ostream& out, const cDat& rhs){
	out << rhs.repNum << '\n';
	out << rhs.date << '\n';
	out << rhs.loc << '\n';
	out << rhs.incident << '\n';
	out << rhs.dis << '\n';
	out << rhs.comment << '\n';
	out << '\n';
	return out;
}

#endif