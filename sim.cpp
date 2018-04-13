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
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include "Database.cpp"

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

void startSimulation( string currentWorkingDirectory );
bool exitCheck( string str );
bool stringValid( string str );
bool removeSemiColon( string &input );
bool startEvent( string input, vector< Database> &dbms, string currentWorkingDirectory, string &currentDatabase );
string getNextWord( string &input );
bool databaseExists( vector<Database> dbms, Database dbInput, int &dbReturn );
void removeDatabase( vector< Database > &dbms, int index );
void removeTable( vector< Database > &dbms, int dbReturn, int tblReturn );
void handleError( int errorType, string commandError, string errorContainerName );
void convertToLC( string &input );
void convertToUC( string &input );
string getQueryType( string &input );
string getWhereCondition( string &input );
string getSetCondition( string &input );
void removeNewLine( string &input );
string returnNextWord( string input );

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
		//cout << input << endl;

		//check that program is not to be ended
		simulationEnd = exitCheck( input );
		//check that the line is valid
		if( !simulationEnd && stringValid( input )  && !removeSemiColon( input ) )
		{
			getline( cin, temp, ';' );
			input = input + temp;
			removeNewLine( input );
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
	if( ( str[ 0 ] == '-' && str[ 1 ] == '-' ) || str == "" )
	{
		return false;
	}
	return true;
}


bool exitCheck( string str )
{
	convertToUC( str );
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
	int semiIndex = 0;
	int strLen = input.length();
	bool semiExists = false;
	//find where ; exists if it does exist
	for ( int index = 0; index < strLen; index++ )
	{
		if( input[ index ] == ';' )
		{
			semiExists = true;
			semiIndex = index;
		}
	}
	//if semi colon does not exist or is not at the end
	if( semiExists == false || semiIndex != ( strLen - 1 ) )
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


		cout << "INPUT PRE IF is |"  << input << "| " << endl; 

		if( returnNextWord( input ) != "where" )
		{
			cout << "INPUT IS " << returnNextWord( input ) << endl;
		}

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
		else if( containerType == TABLE_TYPE )
		{
			
			//call create tbl function
			Database dbTemp;
			dbTemp.databaseName = currentDatabase;
			//get dbReturn of database
			databaseExists( dbms, dbTemp, dbReturn );
			
			// make sure the input does not specify multiple tables 
			size_t pos = input.find(" (");
			string temp = input.substr(0, pos);
			if( temp.find(" ") != string::npos )
			{
				errorExists = true;
				errorType = ERROR_INCORRECT_COMMAND;
				errorContainerName = originalInput;
			}
			else
			{
				//get table name 
				Table tblTemp;
				tblTemp.tableName = getNextWord( input );

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
			( input[ index + 3 ] == 'r' || input[ index + 3] == 'R' ) && 
			( input[ index + 4 ] == 'e' || input[ index + 3] == 'E' ))
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



string returnNextWord( string input )
{
	//doesnt actually remove WS, just modifies local input string
	removeLeadingWS( input );

	//takes first word and returns it
	string nextWord = input.substr( 0, input.find(" ") );
	return nextWord;
}
