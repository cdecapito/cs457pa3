// Program Information /////////////////////////////////////////////////////////
/**
  * @file sim.cpp
  *
  * @brief implements simulation functions    
  * 
  * @details Performs command line instructions to create and drop db/tbls
  *
  * @version 1.01 Carli DeCapito, Sanya Gupta, Eugene Nelson
  *			 February 10, 2018 - DB create/drop impementation, create directories
  *
  *			 1.00 Carli DeCapito
  *			 February 8, 2018 -- Initial Setup, Create/Drop DB Implementation
  *
  * @note None
  */

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <string.h>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include "Database.cpp"

#include <stdio.h>

using namespace std;

const string DATABASE_TYPE = "DATABASE";
const string TABLE_TYPE = "TABLE";

const string DROP = "DROP";
const string CREATE = "CREATE";
const string SELECT = "SELECT";
const string USE = "USE";
const string ALTER = "ALTER";
const string INSERT = "INSERT";
const string UPDATE = "UPDATE";
const string DELETE = "DELETE";
const string EXIT = ".EXIT";

const int ERROR_DB_EXISTS = -1;
const int ERROR_DB_NOT_EXISTS = -2;
const int ERROR_TBL_EXISTS = -3;
const int ERROR_TBL_NOT_EXISTS = -4;
const int ERROR_INCORRECT_COMMAND = -5;

//main implementation
void startSimulation( string currentWorkingDirectory );
//checks if exit command has been called
bool exitCheck( string str );
//checks that input string is command not garbage
bool stringValid( string str );
//removes semicolon for easier parsing
bool removeSemiColon( string &input );
//starts specific action (aka create)
bool startEvent( string input, vector< Database> &dbms, string currentWorkingDirectory, string &currentDatabase );
//helper function to get next word for parsing
string getNextWord( string &input );
//helper function to check that db exists
bool databaseExists( vector<Database> dbms, Database dbInput, int &dbReturn );
//removes database from vector and deletes from disk
void removeDatabase( vector< Database > &dbms, int index );
//removes table from disk and vector
void removeTable( vector< Database > &dbms, int dbReturn, int tblReturn );
//outputs errors to user
void handleError( int errorType, string commandError, string errorContainerName );
//helper function, converts string to LC
void convertToLC( string &input );
//helper function, converts string to UC
void convertToUC( string &input );
//gets type of query, * or attribute type
string getQueryType( string &input );
//returns where condition
string getWhereCondition( string &input );
//returns set condition for update table
string getSetCondition( string &input );
//removes new line chars from strings for easier parsing
void removeNewLine( string &input );
//returns next word without deleting word from input string
string returnNextWord( string input );
//checks for inner join (a comma b/w table names)
bool checkJoin( string & input, string &LHS );
//checks for inner join
bool checkInnerJoin( string & input, string &LHS  );
//checks for left outer join
bool checkOuterJoin( string & input, string &LHS  );
//helper function, gets the table variable
string getTableVariable( string &input );
//helper function, gets LHS and stores RHS of = join into input
string returnLHSJoinComparison( string &input );
//helper parse function, gets LHS 
string getOnCondition( string &input );

void removeCarriageReturn( string &input );

/**
 * @brief read_Directory method
 *
 * @details reads contents of a directory into a vector
 *
 * @param [in] string &name
 *             
 * @param [in] vector <string &v>
 *
 * @return bool
 *
 * @note Code from stack overflow
 */
bool read_directory(const std::string& name, vector< string >& v)
{
	struct stat buffer;
	if( !( stat( name.c_str(), &buffer ) == 0 ) )
		return false;

    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);

    return true;
}

/**
 * @brief startSimulation 
 *
 * @details takes string input and begins to parse
 *          
 * @pre None
 *
 * @post Program ends when .EXIT is inputted
 *
 * @par Algorithm 
 *      Loop until .EXIT is inputted from terminal
 *		Otherwise parse string to find out what action to take
 *      
 * @exception None
 *
 * @param [in] None
 *
 * @param [out] None
 *
 * @return None
 *
 * @note None
 */
void startSimulation( string currentWorkingDirectory )
{
	currentWorkingDirectory += "/DatabaseSystem";

	// Check if the database system directory exists
	struct stat buffer;
	if( !( stat( currentWorkingDirectory.c_str(), &buffer ) == 0 ) )
	{
		// if not, create it.
		system( ( "mkdir " + currentWorkingDirectory ).c_str() );
	}

	string input;
	string temp;
	string currentDatabase;
	vector< Database > dbms;

	// Retrieve all of the information about existing directories
	vector< string > directoryItems;
	if( read_directory( currentWorkingDirectory, directoryItems ) )
	{
		for( unsigned int i = 0; i < directoryItems.size(); i++ )
		{
			if(directoryItems[i] == "." || directoryItems[i] == "..")
			{
				directoryItems.erase(directoryItems.begin() + i);
				i--;
			}
			else
			{
				Database tempDatabase;
				tempDatabase.databaseName = directoryItems[i];

				vector< string > tableItems;
				Table tempTable;

				if( read_directory( currentWorkingDirectory + "/" + tempDatabase.databaseName, tableItems ) )
				{
					for( unsigned int j = 0; j < tableItems.size(); j++ )
					{
						if(tableItems[j] == "." || tableItems[j] == "..")
						{
							tableItems.erase(tableItems.begin() + j);
							j--;
						}
						else
						{
							tempTable.tableName = tableItems[j];

							tempDatabase.databaseTable.push_back(tempTable);
						}
					}
				}

				dbms.push_back(tempDatabase);
			}
		}
	}

	bool simulationEnd = false;
	do{
		
		getline( cin, input );

		//converts dos to unix file by removing file \r
		removeCarriageReturn( input );
		
		//check that program is not to be ended
		simulationEnd = exitCheck( input );
	
		//cout << "VALID " << stringValid( input ) << endl;
		//check that the line is valid
		
		if( !simulationEnd && stringValid( input )  && !removeSemiColon( input ) )
		{
			getline( cin, temp, ';' );
			input = input + temp;
			removeCarriageReturn( input );
			removeNewLine( input );
			removeLeadingWS( input );
		}
		
		//helper function to remove semi colon
		//first checks that data is valid, if not valid will not check for semi colon
		if(  !simulationEnd && stringValid( input ) ) 
		{ 
			//call helper function to check if modifying db or tbl
			simulationEnd = startEvent( input, dbms, currentWorkingDirectory, currentDatabase );
		}
	}while( simulationEnd == false );

	cout << "-- All done. " << endl; 
}

/**
*@brief bool stringValid method
*
*@details checks sthat the string does not start with a dash or space
*
*@param [in] string str
*
*@return bool true if the string is valid
*
*/
bool stringValid( string str )
{
	//check that it is not a comment or empty space
	int strLen = str.length();
	bool valid = false;
	for( int index = 0; index < strLen; index++ )
	{
		if( isspace( str[ index ] ) == 0 )
		{
			valid = true;
		}
	}

	if( ( str[ 0 ] == '-' && str[ 1 ] == '-' ) || !valid )
	{
		return false;
	}
	return true;
}


bool exitCheck( string str )
{
	convertToUC( str );
	removeLeadingWS( str );
	if( str == EXIT )
	{
		return true;
	}
	return false;
}
/**
 * @brief removeSemiColon
 *
 * @details removes semi colons from input string
 *          
 * @pre string exists
 *
 * @post string ; is removed
 *
 * @par Algorithm 
 *      using find and erase string functions
 *      
 * @exception None
 *
 * @param [in] none
 *
 * @param [out] input provides a string of the input command
 *
 * @return None
 *
 * @note None
 */
bool removeSemiColon( string &input )
{
	//CHECK IF SEMI EXISTS
	int strLen = input.length();
	bool semiExists = false;
	//find where ; exists if it does exist
	for ( int index = 0; index < strLen; index++ )
	{
		if( input[ index ] == ';' )
		{
			semiExists = true;
		}
	}
	//if semi colon does not exist or is not at the end
	if( semiExists == false )
	{
		//if input is exit then we are fine
		string temp = input;
		convertToUC( temp );
		if( temp == EXIT )
		{
			return true;
		}
		return false;
	}
	else
	{
		//if semi colon is at end then erase it
		input.erase( input.find( ';' ) );
		//cout << "found and now looks like: " << input << endl;
		return true;
	}
}


/**
 * @brief startEvent
 *
 * @details Initiates action to take: create, use, drop, select, alter
 *          
 * @pre input and dbms exists
 *
 * @post action is done
 *
 * @par Algorithm 
 *      stores data into DB class and into tables
 *      
 * @exception None
 *
 * @param [in] input provides string of input command
 *
 * @param [out] dbms provides system of database to add databases and tables
 *
 * @return None
 *
 * @note None
 */
bool startEvent( string input, vector< Database> &dbms, string currentWorkingDirectory, string &currentDatabase )
{
	bool exitProgram = false;
	bool errorExists = false;
	bool attrError = false;

	//bool tblExists;
	int dbReturn;
	int tblReturn;
	int errorType;
	string originalInput = input;
	string errorContainerName;

	//get first action word & convert to uppercase
	string temp = getNextWord( input );
	convertToUC( temp );
	string actionType = temp;

	string containerType;

	if( caseInsCompare( actionType, SELECT ) )
	{

		Database dbTemp;
		dbTemp.databaseName = currentDatabase;
		databaseExists( dbms, dbTemp, dbReturn );

		//get all words before from
		string qType = getQueryType( input );

		//get table name
		string tName = getNextWord( input );

		Table tblTemp;
		tblTemp.tableName = tName;

		//join parsing
		if( returnNextWord( input ) != "where" && !returnNextWord( input ).empty() )
		{
			string table1Var; 
			string table2Var;
			string table1Attr;
			string table2Attr;
			string joinCondition;
			Table tblTemp2;

			if( checkJoin( input, table1Var ) )
			{
				tblTemp2.tableName = getNextWord( input );
				table2Var = getNextWord( input );

				joinCondition = getWhereCondition( input );

				string LHS = returnLHSJoinComparison( joinCondition );

				string tempVar1 = getTableVariable( LHS );
				string tempVar2 = getTableVariable( joinCondition );

				if( tempVar1 == table1Var && tempVar2 == table2Var )
				{
					table1Attr = LHS;
					table2Attr = joinCondition;
				}
				else
				{
					table1Attr = joinCondition;
					table2Attr = LHS;
				}
				if( !dbms[ dbReturn ].tableExists( tblTemp.tableName, tblReturn ) || 
					!dbms[ dbReturn ].tableExists( tblTemp2.tableName, tblReturn ) )
				{
					errorExists = true;
					errorType = ERROR_TBL_NOT_EXISTS;
					errorContainerName = tblTemp.tableName + " and " + tblTemp2.tableName;	
				}
				else
				{
					tblTemp.innerJoin( currentWorkingDirectory, currentDatabase, tblTemp.tableName, table1Attr, tblTemp2.tableName , table2Attr );
				}
			}
			else if( checkInnerJoin( input, table1Var ) )
			{
				tblTemp2.tableName = getNextWord( input );
				table2Var = getNextWord( input );

				joinCondition = getOnCondition( input );

				string LHS = returnLHSJoinComparison( joinCondition );

				string tempVar1 = getTableVariable( LHS );
				string tempVar2 = getTableVariable( joinCondition );

				if( tempVar1 == table1Var && tempVar2 == table2Var )
				{
					table1Attr = LHS;
					table2Attr = joinCondition;
				}
				else
				{
					table1Attr = joinCondition;
					table2Attr = LHS;
				}
				if( !dbms[ dbReturn ].tableExists( tblTemp.tableName, tblReturn ) || 
					!dbms[ dbReturn ].tableExists( tblTemp2.tableName, tblReturn ) )
				{
					errorExists = true;
					errorType = ERROR_TBL_NOT_EXISTS;
					errorContainerName = tblTemp.tableName + " and " + tblTemp2.tableName;	
				}
				else
				{
					tblTemp.innerJoin( currentWorkingDirectory, currentDatabase, tblTemp.tableName, table1Attr, tblTemp2.tableName , table2Attr );
				}
			}
			else if( checkOuterJoin( input, table1Var ) )
			{
				tblTemp2.tableName = getNextWord( input );
				table2Var = getNextWord( input );

				joinCondition = getOnCondition( input );

				string LHS = returnLHSJoinComparison( joinCondition );

				string tempVar1 = getTableVariable( LHS );
				string tempVar2 = getTableVariable( joinCondition );

				if( tempVar1 == table1Var && tempVar2 == table2Var )
				{
					table1Attr = LHS;
					table2Attr = joinCondition;
				}
				else
				{
					table1Attr = joinCondition;
					table2Attr = LHS;
				}
				if( !dbms[ dbReturn ].tableExists( tblTemp.tableName, tblReturn ) || 
					!dbms[ dbReturn ].tableExists( tblTemp2.tableName, tblReturn ) )
				{
					errorExists = true;
					errorType = ERROR_TBL_NOT_EXISTS;
					errorContainerName = tblTemp.tableName + " and " + tblTemp2.tableName;	
				}
				else
				{
					tblTemp.outerJoin( currentWorkingDirectory, currentDatabase, tblTemp.tableName, table1Attr, tblTemp2.tableName , table2Attr );
				}
			}

		}
		//normal query parsing and output
		else
		{
			string cType = getWhereCondition( input );

			if( !(dbms[ dbReturn ].tableExists( tblTemp.tableName, tblReturn )) )
			{
				errorExists = true;
				errorType = ERROR_TBL_NOT_EXISTS;
				errorContainerName = tblTemp.tableName;		
			}
			else
			{
				tblTemp.tableSelect( currentWorkingDirectory, currentDatabase, cType, qType );
			}
		}

	}
	else if( caseInsCompare( actionType, USE) )
	{
		
		Database dbTemp;
		dbTemp.databaseName = input;
		bool dbExists = databaseExists( dbms, dbTemp, dbReturn );
		
		//check if database exists
		if( dbExists )
		{
			//if it does then set current database as string
			currentDatabase = dbTemp.databaseName;
			dbTemp.databaseUse();
		}
		else
		{
			//if it does not then return error message
			errorExists = true;
			errorContainerName = dbTemp.databaseName;
			errorType = ERROR_DB_NOT_EXISTS; 
		}
	}
	else if( caseInsCompare( actionType, CREATE ) ) 
	{
		//get string if we are modifying table or db
		temp = getNextWord( input );
		convertToUC( temp );
		containerType = temp;

		//databse create
		if( containerType == DATABASE_TYPE )
		{
			Database dbTemp;
			//call Create db function
			dbTemp.databaseName = input;
			//check that db does not exist already
			bool dbExists = databaseExists( dbms, dbTemp, dbReturn );

			if( dbExists )
			{
				//if it does then return error message
				errorExists = true;
				errorContainerName = dbTemp.databaseName;
				errorType = ERROR_DB_EXISTS; 
			}
			else
			{
				//if it does not, return success message and push onto vector
				dbms.push_back( dbTemp );

				//create directory
				dbTemp.databaseCreate();
			}
		}
		//table create
		else if( caseInsCompare( containerType, TABLE_TYPE ) )
		{
			//call create tbl function
			Database dbTemp;
			dbTemp.databaseName = currentDatabase;
			//get dbReturn of database
			databaseExists( dbms, dbTemp, dbReturn );
			
			// make sure the input does not specify multiple tables 
			size_t pos = input.find("(");
			string temp = input.substr(0, pos);
			removeLeadingWS( temp );

			//get table name 
			Table tblTemp;
			tblTemp.tableName = getNextWord( temp );

			//check that table exists
			if( !(dbms[ dbReturn ].tableExists( tblTemp.tableName, tblReturn )) )
			{
				//check that table attributes are not the same
				tblTemp.tableCreate( currentWorkingDirectory, currentDatabase, tblTemp.tableName, input, attrError );
				if( !attrError  )
				{
					//if it doesnt then push table onto database	
					dbms[ dbReturn ].databaseTable.push_back( tblTemp );
				}
			}
			else
			{
				//if it does than handle error
			 	errorExists = true;
			 	errorType = ERROR_TBL_EXISTS;
			 	errorContainerName = tblTemp.tableName;	
			}
		}
		else
		{
			errorExists = true;
			errorType = ERROR_INCORRECT_COMMAND;
			errorContainerName = originalInput;	
		}

	}
	else if( actionType.compare( DROP ) == 0 )
	{
		temp = getNextWord( input );
		convertToUC( temp );
		containerType = temp;

		if( containerType == DATABASE_TYPE )
		{
			//create temp db to be dropped
			Database dbTemp;
			dbTemp.databaseName = input;

			//check if database exists
			if( databaseExists( dbms, dbTemp, dbReturn ) != true )
			{
				//if it does not then return error message
				errorExists = true;
				errorContainerName = dbTemp.databaseName;
				errorType = ERROR_DB_NOT_EXISTS; 
			}
			else
			{
				//if it does, return success message and remove from dbReturn element
				removeDatabase( dbms, dbReturn );

				//remove directory
				dbTemp.databaseDrop(currentWorkingDirectory);
			}


		}
		else if( containerType == TABLE_TYPE )
		{
			//call drop tbl function
			Database dbTemp;
			dbTemp.databaseName = currentDatabase;
			databaseExists( dbms, dbTemp, dbReturn );

			Table tblTemp;
			tblTemp.tableName = getNextWord( input );

			//check if table exists
			if( !(dbms[ dbReturn ].tableExists( tblTemp.tableName, tblReturn )) )
			{
				//if it doesnt exist then return error
				errorExists = true;
				errorType = ERROR_TBL_NOT_EXISTS;
				errorContainerName = tblTemp.tableName;
			}
			else
			{
				//table exists and remove from database
				removeTable( dbms, dbReturn, tblReturn );

				//remove table/file
				tblTemp.tableDrop(currentWorkingDirectory, currentDatabase );
			}
		}
		else
		{
			errorExists = true;
			errorType = ERROR_INCORRECT_COMMAND;
			errorContainerName = originalInput;	
		}
	}
	else if( actionType.compare( ALTER ) == 0 ) 
	{
		containerType = getNextWord( input );
		if( containerType == TABLE_TYPE )
		{
			//call alter tbl function
			Database dbTemp;
			dbTemp.databaseName = currentDatabase;
			databaseExists( dbms, dbTemp, dbReturn );

			Table tblTemp;
			tblTemp.tableName = getNextWord( input );

			//check if table exists
			if( !(dbms[ dbReturn ].tableExists( tblTemp.tableName, tblReturn )) )
			{
				//if it doesnt exist then return error
				errorExists = true;
				errorType = ERROR_TBL_NOT_EXISTS;
				errorContainerName = tblTemp.tableName;
			}
			else
			{
				//remove table/file
				tblTemp.tableAlter( currentWorkingDirectory, currentDatabase, input, attrError );	
			}
		}
	}
	else if( actionType.compare( INSERT ) == 0 )
	{
		temp = getNextWord( input );
		//check that temp is into
		if( temp != "into" )
		{
			errorExists = true;
			errorType = ERROR_INCORRECT_COMMAND;
			errorContainerName = originalInput;	
		}

		//check that current db exists
		Database dbTemp;
		dbTemp.databaseName = currentDatabase;
		databaseExists( dbms, dbTemp, dbReturn );

		//get table 
		Table tblTemp;
		tblTemp.tableName = getNextWord( input );


		//check if table exists
		if( !(dbms[ dbReturn ].tableExists( tblTemp.tableName, tblReturn )) )
		{
			//if it doesnt exist then return error
			errorExists = true;
			errorType = ERROR_TBL_NOT_EXISTS;
			errorContainerName = tblTemp.tableName;
		}
		else if( !errorExists )
		{
			//table exists and we can modify it
			//return only stuff between parentheses
			input.erase( 0, input.find( "(" ) + 1 );
			input.erase( input.find_last_of( ")" ), input.length()-1 );

			tblTemp.tableInsert( currentWorkingDirectory, currentDatabase, tblTemp.tableName, input, attrError );
		}	
	}
	else if( actionType.compare( UPDATE ) == 0 )
	{
		//get index of curr DB
		Database dbTemp;
		dbTemp.databaseName = currentDatabase;
		databaseExists( dbms, dbTemp, dbReturn );

		//get table name
		Table tblTemp;
		tblTemp.tableName = getNextWord( input );
		
		//get where condition
		string wCond = getWhereCondition( input );

		//get set condition
		string sCond = getSetCondition( input );
	
		//check if table exists
		if( !(dbms[ dbReturn ].tableExists( tblTemp.tableName, tblReturn )) )
		{
			//if it doesnt exist then return error
			errorExists = true;
			errorType = ERROR_TBL_NOT_EXISTS;
			errorContainerName = tblTemp.tableName;
		}
		else
		{
			//update values
			tblTemp.tableUpdate( currentWorkingDirectory, currentDatabase, wCond, sCond );
		}
	}
	else if( actionType.compare( DELETE ) == 0 )
	{
		//get index of curr DB
		Database dbTemp;
		dbTemp.databaseName = currentDatabase;
		databaseExists( dbms, dbTemp, dbReturn );

		string temp = getQueryType( input );
		//get table name
		Table tblTemp;
		tblTemp.tableName = getNextWord( input );

		//get where condition
		string wCond = getWhereCondition( input );
	
		//check if table exists
		if( !(dbms[ dbReturn ].tableExists( tblTemp.tableName, tblReturn )) )
		{
			//if it doesnt exist then return error
			errorExists = true;
			errorType = ERROR_TBL_NOT_EXISTS;
			errorContainerName = tblTemp.tableName;
		}
		else
		{
			//update values
			tblTemp.tableDelete( currentWorkingDirectory, currentDatabase, wCond );
		}
	}
	else if( actionType.compare( EXIT ) == 0 )
	{
		exitProgram = true;
	}
	else
	{
		errorExists = true;
		errorType = ERROR_INCORRECT_COMMAND;
		errorContainerName = originalInput;
	}

	if( errorExists )
	{
		handleError( errorType, actionType, errorContainerName );
	}

	return exitProgram;
}

/**
 * @brief databaseExists
 *
 * @details checks whether dbInput exists in dbms
 *          
 * @pre dbms exists and dbInput exists
 *
 * @post returns true if dbExists, false otherwise
 *
 * @par Algorithm 
 *      loop through dbms return true if match is found
 *      
 * @exception None
 *
 * @param [in] dbms provides vector of dbs
 *
 * @param [in] dbInput provides db to be created
 *
 * @return bool
 *
 * @note None
 */
bool databaseExists( vector<Database> dbms, Database dbInput, int &dbReturn )
{
	int size = dbms.size();
	for( dbReturn = 0; dbReturn < size; dbReturn++ )
	{
		if( caseInsCompare( dbInput.databaseName, dbms[ dbReturn ].databaseName ) )
		{
			return true;
		}
	}
	return false;
}



void removeDatabase( vector< Database > &dbms, int index )
{
	dbms.erase( dbms.begin() + index );
}


void removeTable( vector< Database > &dbms, int dbReturn, int tblReturn )
{
	dbms[ dbReturn ].databaseTable.erase( dbms[ dbReturn ].databaseTable.begin() + tblReturn );
}

/**
 * @brief handleError
 *
 * @details takes an errorType and outputs an error based off of it
 *          
 * @pre errorType and errorContainerName exist
 *
 * @post outputs error to terminal
 *
 * @par Algorithm 
 *      checks errorTYpe and outputs accordingly
 *      
 * @exception None
 *
 * @param [in] errorType provides an int value of the appropriate error
 *
 * @param [in] errorContainerName provides error source
 *
 * @return None
 *
 * @note None
 */
void handleError( int errorType, string commandError, string errorContainerName )
{

	if( commandError == "SELECT" )
	{
		commandError = "query";
	}
	//convert to lowercase for use
	convertToLC( commandError );

	// if problem is that databse does exist (used for create db )
	if( errorType == ERROR_DB_EXISTS )
	{
		cout << "-- !Failed to " << commandError << " database " << errorContainerName;
		cout << " because it already exists." << endl;
	}
	//if problem is that database does not exist ( used for use, drop)
	else if( errorType == ERROR_DB_NOT_EXISTS )
	{
		cout << "-- !Failed to " << commandError << " database " << errorContainerName;
		cout << " because it does not exist." << endl;
	}
	//if problem is that table exists ( used for create table)
	else if( errorType == ERROR_TBL_EXISTS )
	{
		cout << "-- !Failed to " << commandError << " table " << errorContainerName;
		cout << " because it already exists." << endl;
	}
	//if problem is that table does not exist( used for alter, select, drop )
	else if( errorType == ERROR_TBL_NOT_EXISTS )
	{
		cout << "-- !Failed to " << commandError << " table " << errorContainerName;
		cout << " because it does not exist." << endl;
	}
	//if problem is that an unrecognized error occurs
	else if( errorType == ERROR_INCORRECT_COMMAND )
	{
		cout << "-- !Failed to complete command. "<< endl;
		cout << "-- !Incorrect instruction: " << errorContainerName << endl;
	}
}


/**
 * @brief convertToLC
 *
 * @details converts a string into lowercase
 *          
 * @pre Assumes stirng has uppercase values
 *
 * @post String is now in lowercase
 *
 * @par Algorithm 
 *      parse through the string and converts toLower using string lib function
 * 
 * @exception None
 *
 * @param [in] string &input
 *
 * @return None
 *
 * @note None
 */
void convertToLC( string &input )
{
	int size = input.size();
	for( int index = 0; index < size; index++ )
	{
		input[ index ] = tolower( input[ index ] );
	}
}

/**
*@brief void convertToUC method
*
*@details converts given string to uppercase toUpper method
*
*@param [in] string &input
*
*@return none (void)
*/
void convertToUC( string &input )
{
	int size = input.size();
	for( int index = 0; index < size; index++ )
	{
		input[ index ] = toupper( input[ index ] );
	}
}

/**
*@brief string getQueryType method
*
*@details basically chesks for "from" statment and stores the table
*
*@param [in] string &input
*
*@return string (aka the record name) or space
*/
string getQueryType( string &input )
{
	bool fromOccurs = false;
	int fromOccurance = 0;
	int inputSize = input.size();

	for( int index = 0; index < inputSize; index++ )
	{
		//check that i is f or F
		if( ( input[ index ] == 'f' || input[ index ] == 'F' ) &&
			( input[ index + 1 ] == 'r' || input[ index + 1 ] == 'R' ) && 
			( input[ index + 2 ] == 'o' || input[ index + 2 ] == 'O' ) &&
			( input[ index + 3 ] == 'm' || input[ index + 3] == 'M' ))
		{
			fromOccurs = true;
			fromOccurance = index;
		}
	}

	if( fromOccurs )
	{
		string queryType;
		//take first word of input and set as action word
		queryType = input.substr( 0, fromOccurance - 1);
		//erase word from original str to further parse
		input.erase( 0, fromOccurance + 5 );

		return queryType;
	}
	else
	{
		return "";
	}
	
}

/**
*@brief string getWhereCondition method
*
*@details checks for where in string, and if true stores the condition
*
*@param [in] string &input
*
*@return string
*/
string getWhereCondition( string &input )
{
	bool whereOccurs = false;
	int whereOccurance = 0;
	int inputSize = input.size();

	for( int index = 0; index < inputSize; index++ )
	{
		//check that i is f or F
		if( ( input[ index ] == 'w' || input[ index ] == 'W' ) &&
			( input[ index + 1 ] == 'h' || input[ index + 1 ] == 'H' ) && 
			( input[ index + 2 ] == 'e' || input[ index + 2 ] == 'E' ) &&
			( input[ index + 3 ] == 'r' || input[ index + 3 ] == 'R' ) && 
			( input[ index + 4 ] == 'e' || input[ index + 4 ] == 'E' ))
		{
			whereOccurs = true;
			whereOccurance = index;
		}
	}

	if( whereOccurs )
	{
		string condType;
		//take first word of input and set as action word
		condType = input.substr( whereOccurance + 6, input.size() - 1 );
		//erase word from original str to further parse
		input.erase( whereOccurance, input.size() - 1 );
		return condType;
	}
	else
	{
		return "";
	}
}

/**
*@brief string getSetCondition method
*
*@details returns the attribute that is being reset
*
*@param [in] string &input
*
*@return string (the set value)
*/
string getSetCondition( string &input )
{
	bool setOccurs = false;
	int setOccurance = 0;
	int inputSize = input.size();

	for( int index = 0; index < inputSize; index++ )
	{
		//check that i is f or F
		if( ( input[ index ] == 's' || input[ index ] == 'S' ) &&
			( input[ index + 1 ] == 'e' || input[ index + 1 ] == 'E' ) && 
			( input[ index + 2 ] == 't' || input[ index + 2 ] == 'T' ) )
		{
			setOccurs = true;
			setOccurance = index;
		}
	}

	if( setOccurs )
	{
		string setType;
		//take first word of input and set as action word
		setType = input.substr( setOccurance + 4, input.find("\n") - 1);
		//erase word from original str to further parse
		input.erase( setOccurance, input.size() - 1);
		return setType;
	}
	else
	{
		return "";
	}
}

/**
*@brief void removeNewLine method
*
*@details takes care of newlines by replacing them with a space
*
*@param [in] string &input
*
*@return none (void)
*/
void removeNewLine( string &input )
{	
	int inputSize = input.size();
	for( int index = 0; index < inputSize; index++ )
	{
		if( input[ index ] == '\n' )
		{
			input[ index ] = ' ';
		}
	}
}


/**
 * @brief returnNextWord
 *
 * @details returns next word without deleting anything from input string
 *          
 * @pre input is not empty
 *
 * @post first word is returned
 *
 * @par Algorithm 
 *      get string until first space appears
 * 
 * @exception None
 *
 * @param [in] string input provides input string to parse
 *
 * @return string
 *
 * @note None
 */
string returnNextWord( string input )
{
	//doesnt actually remove WS, just modifies local input string
	removeLeadingWS( input );

	//takes first word and returns it
	string nextWord = input.substr( 0, input.find(" ") );
	return nextWord;
}

/**
 * @brief checkJoin
 *
 * @details checks for inner join without using inner join keyword
 *          
 * @pre nothing
 *
 * @post returns true if default join
 *
 * @par Algorithm 
 *      parse through the string and find , and return LHS and RHS
 * 
 * @exception None
 *
 * @param [in] string &input
 *
 * @return bool
 *
 * @note None
 */
bool checkJoin( string & input, string &LHS )
{
	//check input for comma
	size_t found = input.find( ',' );
	if( found != input.npos )
	{
		LHS = input.substr(0, found );
		removeLeadingWS( LHS );

		input.erase( 0, found + 1 );
		removeLeadingWS( input );
		return true;
	}
	return false;
}


/**
 * @brief checkInnerJoin
 *
 * @details checks that query is inner join
 *          
 * @pre 
 *
 * @post 
 *
 * @par Algorithm 
 *      parse through the string and look for occurance of inner join and return lhs and rhs
 * 
 * @exception None
 *
 * @param [in] string &input
 *
 * @return bool, true if inner join, false otherwise
 *
 * @note None
 */
bool checkInnerJoin( string & input, string &LHS  )
{
	bool innerOccurs = false;
	int found = 0;
	int inputSize = input.size();

	for( int index = 0; index < inputSize; index++ )
	{
		//check that i is f or F  
		if( ( input[ index ] == 'i' || input[ index ] == 'I' ) &&
			( input[ index + 1 ] == 'n' || input[ index + 1 ] == 'N' ) && 
			( input[ index + 2 ] == 'n' || input[ index + 2 ] == 'N' ) &&
			( input[ index + 3 ] == 'e' || input[ index + 3] == 'E' ) && 
			( input[ index + 4 ] == 'r' || input[ index + 4] == 'R' ) &&
			  input[ index + 5 ] == ' ' &&
			( input[ index + 6 ] == 'j' || input[ index + 6 ] == 'J' ) && 
			( input[ index + 7 ] == 'o' || input[ index + 7 ] == 'O' ) &&
			( input[ index + 8 ] == 'i' || input[ index + 8 ] == 'I' ) && 
			( input[ index + 9 ] == 'n' || input[ index + 9 ] == 'N' ))
		{
			innerOccurs = true;
			found = index;
		}
	}

	if( innerOccurs )
	{
		LHS = input.substr( 0, found );
		removeLeadingWS( LHS );

		input.erase( 0, found + 10 );
		removeLeadingWS( input );
		return true;
	}
	return false;
}

/**
 * @brief 
 *
 * @details 
 *          
 * @pre 
 *
 * @post 
 *
 * @par Algorithm 
 *     
 * 
 * @exception None
 *
 * @param [in] 
 *
 * @return 
 *
 * @note None
 */
bool checkOuterJoin( string & input, string &LHS  )
{
	bool outerOccurs = false;
	int found = 0;
	int inputSize = input.size();

	for( int index = 0; index < inputSize; index++ )
	{
		//check that i is f or F  
		if( ( input[ index ] == 'l' || input[ index ] == 'L' ) &&
			( input[ index + 1 ] == 'e' || input[ index + 1 ] == 'E' ) && 
			( input[ index + 2 ] == 'f' || input[ index + 2 ] == 'F' ) &&
			( input[ index + 3 ] == 't' || input[ index + 3 ] == 'T' ) && 

			  input[ index + 4 ] == ' ' &&

			( input[ index + 5 ] == 'o' || input[ index + 5 ] == 'O' ) &&
			( input[ index + 6 ] == 'u' || input[ index + 6 ] == 'U' ) && 
			( input[ index + 7 ] == 't' || input[ index + 7 ] == 'T' ) &&
			( input[ index + 8 ] == 'e' || input[ index + 8 ] == 'E' ) && 
			( input[ index + 9 ] == 'r' || input[ index + 9 ] == 'R' ) &&

			  input[ index + 10 ] == ' ' && 

			( input[ index + 11 ] == 'j' || input[ index + 11 ] == 'J' ) &&
			( input[ index + 12 ] == 'o' || input[ index + 12 ] == 'O' ) && 
			( input[ index + 13 ] == 'i' || input[ index + 13 ] == 'I' ) &&
			( input[ index + 14 ] == 'n' || input[ index + 14 ] == 'N' ))
		{
			outerOccurs = true;
			found = index;
		}
	}

	if( outerOccurs )
	{
		LHS = input.substr( 0, found );
		removeLeadingWS( LHS );

		input.erase( 0, found + 15 );
		removeLeadingWS( input );
		return true;
	}
	return false;
}


/**
 * @brief 
 *
 * @details 
 *          
 * @pre 
 *
 * @post 
 *
 * @par Algorithm 
 *     
 * 
 * @exception None
 *
 * @param [in] 
 *
 * @return 
 *
 * @note None
 */
string getTableVariable( string &input )
{
	string tblVar;
	size_t found = input.find( '.' );
	if( found != input.npos )
	{
		tblVar = input.substr( 0, found );
		removeLeadingWS( tblVar );

		input.erase( 0, found + 1 );
		removeLeadingWS( input );
	}

	return tblVar;
}


/**
 * @brief 
 *
 * @details 
 *          
 * @pre 
 *
 * @post 
 *
 * @par Algorithm 
 *     
 * 
 * @exception None
 *
 * @param [in] 
 *
 * @return 
 *
 * @note None
 */
string returnLHSJoinComparison( string &input )
{
	string LHS;
	size_t found = input.find( '=' );
	if ( found != input.npos )
	{
		LHS = input.substr( 0, found );
		removeLeadingWS( LHS );

		input.erase( 0, found + 1 );
		removeLeadingWS( input );
	}
	return LHS;
}


/**
 * @brief 
 *
 * @details 
 *          
 * @pre 
 *
 * @post 
 *
 * @par Algorithm 
 *     
 * 
 * @exception None
 *
 * @param [in] 
 *
 * @return 
 *
 * @note None
 */
string getOnCondition( string &input )
{
	bool onOccurs = false;
	int found = 0;
	int inputSize = input.size();
	string LHS;

	for( int index = 0; index < inputSize; index++ )
	{
		//check that i is f or F  
		if( ( input[ index ] == 'o' || input[ index ] == 'O' ) &&
			( input[ index + 1 ] == 'n' || input[ index + 1 ] == 'N' ) )
		{
			onOccurs = true;
			found = index;
		}
	}

	if( onOccurs )
	{
		LHS = input.substr( found + 2, input.size() - 1 );
		removeLeadingWS( LHS );

		input.erase( found, input.size() - 1 );
		removeLeadingWS( input );
	}
	return LHS;
}


void removeCarriageReturn( string &input )
{
	int strLen = input.length();
	for( int index = 0; index < strLen; index++ )
	{
		if( input[ index ] == '\r' )
		{
			input[ index ] = ' ';
		}
	}	
}