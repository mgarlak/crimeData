// Michael Garlak
// Parsing HTML from file obtained from webpage
// 02-24-2017

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "cDat.h"
#include <sqlite3.h>

void ckAndInput(std::vector<cDat>&, cDat&, std::string, std::vector<std::string>::iterator&);
std::string charCorrect(std::string&);
std::vector<std::string> removeTags(std::string);
std::string getHTML(std::ifstream&);

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(){

	// ---------------------------- FILE INPUT --------------------------
	std::ifstream input;
	input.open("page.html");
	if (!input.is_open()){ 
		std::cout << "Could Not Open File\n";
		return 1;
	}

	// ------------------------- MAKING DATABASE ------------------------
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc, rc2;
	char *sqlcreate;

	rc = sqlite3_open("crime.db", &db);
	if (rc){
		fprintf(stderr, "Cant Open Database: %s\n", sqlite3_errmsg(db));
		return 0;
	}

	sqlcreate = "CREATE TABLE CRIME (" \
		    "REPORT TEXT PRIMARY KEY NOT NULL," \
		    "DATE 	TEXT 	NOT NULL," \
		    "LOCATION 	TEXT 	NOT NULL," \
		    "INCIDENT 	TEXT 	NOT NULL," \ 
		    "DISPOSITION TEXT 	NOT NULL," \ 
		    "COMMENTS 	TEXT 	NOT NULL);";

	rc = sqlite3_exec(db, sqlcreate, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
   	}
	
	// ------------------------- SET CHECK STRINGS ----------------------
	std::string LOC = "Location:";
	LOC += '\0';
	std::string INC = "Incident:";
	INC += '\0';
	std::string DATE = "Date of Incident: ";
	DATE += '\0';
	std::string REP = "Report #: ";
	REP += '\0';
	std::string DIS = "Disposition:";
	DIS += '\0';
	std::string COM = "Comments: ";
	COM += '\0';

	// -------------- GET STRING INPUT & REMOVE HTML TAGS ------------------
	std::string buffer = getHTML(input);
	input.close();
	std::vector<std::string> holder = removeTags(buffer);

	std::vector<cDat> activityStruct;
	cDat crimeData;

	
	// --------- PUTTING STRING INFO INTO CDAT DATA STRUCTURE --------------
	// ---------- AND MAKING DATA ACCESSIBLE BY USING VECTOR ---------------
	std::vector<std::string>::iterator i = holder.begin();
	while (i != holder.end()-1){
		*i = charCorrect(*i);
		if (*i == REP) {
			crimeData.repNum = charCorrect(*(i + 1));
			ckAndInput(activityStruct, crimeData, crimeData.repNum, i);
			continue;
		}
		else if (*i == DATE) {
			crimeData.date = charCorrect(*(i + 1));
			ckAndInput(activityStruct, crimeData, crimeData.date, i);
			continue;
		}
		else if (*i == LOC) {
			crimeData.loc = charCorrect(*(i + 1));
			ckAndInput(activityStruct, crimeData, crimeData.loc, i);
			continue;
		}
		else if (*i == INC) {
			crimeData.incident = charCorrect(*(i + 1));
			ckAndInput(activityStruct, crimeData, crimeData.incident, i);
			continue;
		}
		else if (*i == DIS) {
			crimeData.dis = charCorrect(*(i + 1));
			ckAndInput(activityStruct, crimeData, crimeData.dis, i);
			continue;
		}
		else if (*i == COM) {
			crimeData.comment = charCorrect(*(i + 1));
			ckAndInput(activityStruct, crimeData, crimeData.comment, i);
			continue;
		}
		else {
			while (*i != REP && i != holder.end() - 1){
				++i;
			}
			crimeData.reset();
			continue;
		}
	}

	// --------------------- PUT DATA INTO DATABASE ------------------------
	for (std::vector<cDat>::iterator j = activityStruct.begin(); j != activityStruct.end(); ++j){
		// REMOVING "\0" from each string to help convert strings to char*
		(*j).repNum.pop_back();
		(*j).date.pop_back();
		(*j).loc.pop_back();
		(*j).incident.pop_back();
		(*j).dis.pop_back();
		(*j).comment.pop_back();
		// ---------------------------------------------------------------
		// SQL STATEMENT
		std::string sqlinsert = "INSERT INTO CRIME (REPORT, DATE, LOCATION, INCIDENT, DISPOSITION, COMMENTS) " \ 
				        "VALUES ('" + (*j).repNum + "', '" + (*j).date + "', '" + (*j).loc + "', '" \
					+ (*j).incident + "', '" + (*j).dis + "', '" + (*j).comment + "');";
		// CONVERT TO CHAR ARRAY
		const char* sqlin1 = sqlinsert.c_str();
		// EXECUTE SQL STATEMENT
		rc2 = sqlite3_exec(db, sqlin1, callback, 0, &zErrMsg);
		if( rc2 != SQLITE_OK ){
			fprintf(stderr, "SQL COULD NOT INSERT\n");
		}
	}
	sqlite3_close(db);
	return 0;
}

void ckAndInput(std::vector<cDat>& a, cDat& e, std::string c, std::vector<std::string>::iterator& d){
	if (e.isFull() && c != "\0"){
		a.push_back(e);
		e.reset();
	}
	d += 2;
}

std::string getHTML(std::ifstream& a){
	char buffer;
	std::string statement;
	while (!a.eof()){
		a.get(buffer);
		statement += buffer;
	}
	statement.pop_back();
	return statement;
}

std::vector<std::string> removeTags(std::string a){
	int i = 0;
	std::string info;
	std::vector<std::string> store;
	int found;
	while (true){
		if (a[i] == '>'){
			info.clear();
			int j = i;
			while (a[j] != '<' || (a[j]=='<' && a[j+1]=='s' && a[j+2]=='u' && a[j+3]== 'p') || 
				(a[j] == '<' && a[j + 1] == '/' && a[j + 2] == 's' && a[j + 3] == 'u' && a[j+4]=='p')){
				info += a[j];
				++j;
				if (a[j] == '\0') break;
			}
			i = j;
			info.erase(info.begin());
			if (!info.empty()){
				found = info.find("_________");
				if (found != std::string::npos) continue;
				if (*info.begin() == ':') info.erase(info.begin());
				if (*info.begin() == ' ') info.erase(info.begin());
				info += '\0';
				store.push_back(info);
			}
		}
	if (a[i] == '\0') break;
	++i;
	}
	return store;
}

std::string charCorrect(std::string& a){
	std::string check = "Location";
	check += '\0';
	if (a == check) { a.insert(8, ":"); }
	else {
		for (int i = 0; a[i] != '\0'; ++i){
			if (a[i] == '<' && a[i + 1] == 's'){
				a.replace(i, 5, "");
			}
			else if (a[i] == '<' && a[i + 1] == '/'){
				a.replace(i, 6, "");
			}
			if (a[i] == '–') a[i] = '-';
			if (a[i] == '&'){
				if (a[i + 1] == 'a' && a[i + 2] == 'm' && a[i + 3] == 'p'){
					a.replace(i + 1, 4, "");
				}
			}
		}
	}
	return a;
}
