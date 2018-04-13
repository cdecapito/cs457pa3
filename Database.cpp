// Program Information ////////////////////////////////////////////////////////
/**
 * @file Database.cpp
 * 
 * @brief Implementation file for Database class
 * 
 * @author Carli Decapito, Sanya Gupta, Eugene Nelson
 *
 * @details Implements all member methods of the Database class 
 *
 * @Note Requires Database.h
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
 #include <sys/types.h>
#include <dirent.h>
#include "Database.h"

using namespace std;

// Precompiler directives /////////////////////////////////////////////////////
#ifndef DATABASE_CPP
#define DATABASE_CPP

//declaration of the function
bool caseInsCompare( string s1, string s2 );

/**
 *@brief caseInsCharCompareN method
 *
 *@details converts char to uppercase, used to make program case insensitive
 *
 *@param [in] char a
 *
 *@param [in] char b 
*/
inline bool caseInsCharCompareN( char a, char b )
{
	return( toupper( a ) == toupper( b ) );
}

/**
 *@brief caseInsCompare
 *
 *@details checks size of strings and if the values of each char a are equal
 *
 *@param [in] string s1
 *
 *@param [in] string s2
*/
bool caseInsCompare( string s1, string s2 )
{
	return ( ( s1.size() == s2.size() ) && 
			equal( s1.begin(), s1.end(), s2.begin(), caseInsCharCompareN ) );
}

/**
 * @brief database Default constructor
 *
 * @details creates and initalizes new database objects to null - 
 *
 * @note Needs to be implemented
 */
Database::Database()
{

}

/**
 * @brief database default destructor
 *
 * @details sets objects to null or deletes them
 *
 * @note Needs to be implemented
 */
Database::~Database()
{

}

/**
 * @brief databaseDrop method
 *
 * @details deletes database directory from disk 
 *
 * @par Algorithm uses system commands to remove database and all files associated with it
 *
 * @param [in] string currentWorkingDirectory
 *
 * @return None
 * 
 * @note None
 */
void Database::databaseDrop(string currentWorkingDirectory)
{
	cout << "-- Database " << databaseName << " deleted." << endl;
	
	//FIND FILES
	DIR* dirp = opendir( ( currentWorkingDirectory + "/" + databaseName ).c_str() );
	struct dirent * dp;
	while( ( dp = readdir( dirp ) ) != NULL )
	{
		if( ( strcmp( dp->d_name, "." ) != 0 ) && ( strcmp( dp->d_name, ".." ) != 0 ) )
		{
			system( ( "rm " + currentWorkingDirectory + "/" + databaseName + "/" + dp->d_name ).c_str() );
		}
	}
	closedir(dirp);

	system( ( "rmdir " + currentWorkingDirectory + "/" + databaseName).c_str() );
}

/**
 * @brief databaseCreate
 *
 * @details creates new database directory
 *          
 * @pre input exists
 *
 * @post database DIRECTORY is created and initalized
 *
 * @par Algorithm Uses system lib to run linux commands through program
 *      
 * @return None
 *
 * @note None
 */
void Database::databaseCreate()
{
	cout << "-- Database " << databaseName << " created." << endl;
	system( ( "mkdir DatabaseSystem/" + databaseName ).c_str() );
}

/**
 * @brief databaseAlter
 *
 * @details To be implemented when time comes
 *          
 * @return None
 *
 * @note None
 */
void Database::databaseAlter( string input )
{

}

/**
 * @brief databaseUse
 *
 * @details checks what database is being used
 *          
 * @pre assumes there's a databaseName
 *
 * @post database name is outputted 
 *
 * @par Algorithm 
 *      

 * @return None
 *
 * @note None
 */
void Database::databaseUse()
{
	cout << "-- Using Database " << databaseName;
	cout << "." << endl;
}

/**
 * @brief tableExists
 *
 * @details goes through tables to see if passed in table exists in current database	
 *          
 * @pre number of tables is found for current database
 *
 * @post if table exists, a boolean value of true is returned
 *
 * @par Algorithm Go through the tables and check if the tableName matches a table
 * 		stored in the database
 *      
 * @exception None
 *
 * @param [in] string tblName - name of the table to see if it exists
 *
 * @param [in] int &tblReturn, the index at which the table was found in the vector of database tables is stored
 * @param [out] 
 *
 * @return bool true if found, else false
 *
 * @note None
 */
bool Database::tableExists( string &tblName, int &tblReturn )
{
	int tblSize = databaseTable.size();
	for( int index = 0; index < tblSize; index++ )
	{
		if( caseInsCompare( databaseTable[ index ].tableName, tblName ) )
		{
			tblReturn = index;
			tblName = databaseTable[ index ].tableName;
			return true;
		}
	}
	return false;
}

// Terminating precompiler directives  ////////////////////////////////////////
#endif
