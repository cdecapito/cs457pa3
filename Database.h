// Program Information ////////////////////////////////////////////////////////
/**
 * @file Database.h
 *
 * @brief Definition file for Database class
 * 
 * @details Specifies all member methods of the Database class
 *
 *
 * @Note None
 */

#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "Table.cpp"

// Precompiler directives /////////////////////////////////////////////////////
#ifndef DATABASE_H
#define DATABASE_H

class Database{
	public: 
		string databaseName;
		vector <Table> databaseTable;

		Database();
		~Database();
		void databaseDrop(string currentWorkingDirectory);
		void databaseCreate();
		void databaseAlter( string input );
		void databaseUse();
		bool tableExists( string &tblName, int &tblReturn );
};

// Terminating precompiler directives  ////////////////////////////////////////
#endif
