// Program Information ////////////////////////////////////////////////////////
/**
 * @file Table.cpp
 * 
 * @brief Implementation file for Table class
 * 
 * @author Carli Decapito, Sanya Gupta, Eugene Nelson
 *
 * @details Implements all member methods of the Table class
 *
 * @Note Requires Table.h
 */
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include "Table.h"

using namespace std;

// Precompiler directives /////////////////////////////////////////////////////
#ifndef TABLE_CPP
#define TABLE_CPP

const string ALL = "*";
//struct containing attribute name and index values 
struct AttributeSubset{
	string attributeName;
	int attributeIndex;
};

int findAttrOccur( vector< Attribute > attributes, string attrName );
void getWhereCondition( WhereCondition &wCond, string whereType, vector< Attribute > attributes );
void getSetCondition( SetCondition &sCond, string setType, vector< Attribute > attributes );
bool currIndexIsSubset( vector< AttributeSubset > attrSubsets, int indexVal );
bool indexExists( int i, vector< int > indexCounter );
/**
 * @brief getCommaCount
 *
 * @details counts how many commas are in the passed in string
 *          
 * @pre assumes there's a string that has commas in 
 *
 * @post returns the number of commas in given string
 *
 * @par Algorithm for loop that parses the the given string to check for commas
 *      
 * @param [in] string str
 *
 * @return int (the number of commas)
 *
 * @note None
 */
int getCommaCount( string str )
{
	int length = str.length();
	int count = 0;
	for( int index = 0; index < length; index++ )
	{
		//look for num of commas
		if( str[ index ] == ',' )
		{
			count++;
		}
	}
	return count;
}

/**
 * @brief getNextWord
 *
 * @details Used to find the action word (command)
 *          
 * @pre assumes there is more to the string and erases
 *
 * @post action word is found and returned
 *
 * @par Algorithm parses through the string and erases till next space
 *
 * @param [in] string &input
 *      
 * @return string (the next word)
 *
 * @note None
 */
string getNextWord( string &input )
{

	string actionType;
	bool spaceExists = false;
	int inputSize = input.size();

	for( int index = 0; index < inputSize; index++ )
	{
		if( input[ index ] == ' ' )
		{
			spaceExists = true;
		}
	}
	if( !spaceExists )
	{
		actionType = input;
		input.clear();
		return actionType;
	}
	else
	{
		//take first word of input and set as action word
		actionType = input.substr( 0, input.find(" "));
		//erase word from original str to further parse
		input.erase( 0, input.find(" ") + 1 );

		return actionType;
	}
}

/**
 * @brief getUntilTabg
 *
 * @details Used to find the action word (command) if tab is present prior
 *          
 * @pre assumes there is more to the string and erases
 *
 * @post action word is found and returned
 *
 * @par Algorithm parses through the string and erases till next space
 *
 * @param [in] string &input
 *      
 * @return string (the word)
 *
 * @note None
 */
string getUntilTab( string &input )
{
	string attribute;
	bool tabExists = false;
	int inputSize = input.size();

	for ( int index = 0; index < inputSize; index++ )
	{
		if( input[ index ] == '\t' )
		{
			tabExists = true;
		}
	}
	if ( !tabExists )
	{
		attribute = input;
		input.clear();
		return attribute;
	}
	else
	{
		attribute = input.substr( 0, input.find("\t") );
		input.erase( 0, input.find( "\t" ) + 1 );
		return attribute;	
	}

}

/**
 * @brief removeLeadingWS
 *
 * @details Used to find the action word (command)
 *          
 * @pre assumes there is a string with potential whitespace in front of the word
 *
 * @post leading white space infront of a string is removed
 *
 * @par Algorithm parses through the string and moves index upto white space and deletes it through erase function
 *
 * @param [in] string &input
 *      
 * @return none
 *
 * @note None
 */
void removeLeadingWS( string &input )
{
	int index = 0;
	while( input[ index ] == ' ' || input[ index ] == '\t' )
	{ 
		index++;
	}

	input.erase( 0, index );

	index = input.size() - 1;
	while( input[ index ] == ' ' || input[ index ] == '\t' )
	{
		index--;
	}
	input.erase( index + 1, input.size() - 1 );
}


/**
 * @brief attributeNameExists
 *
 * @details checks vector of tables to see if attribute name already exists
 *          
 * @pre assumes there's an vector of attributes to search through
 *
 * @post action word is found and returned
 *
 * @par Algorithm parses through the string and erases till next space
 *
 * @param [in] vector <Attribute> attributeTable
 * 
 * @param [in] Attribute attr - the attribute stored locally
 *      
 * @return bool true if already in table, else false
 *
 * @note None
 */
bool attributeNameExists( vector< Attribute > attributeTable, Attribute attr )
{
	int size = attributeTable.size();
	for( int index = 0; index < size; index++ )
	{
		if( attributeTable[ index ].attributeName == attr.attributeName )
		{
			return true;
		}
	}
	return false;
}


/**
 * @brief table default constructor
 *
 * @details table class default constructor - needs to be implemented
 *
 * @note None
 */
Table::Table()
{

}


/**
 * @brief Table default destructor
 *
 * @detials Table class default destructor - needs to be implmented
 *
 * @notes Tables should be deleted
*/
Table::~Table()
{

}


/**
 * @brief tableCreate
 *
 * @details creates table and stores in disk otherwise handles errors too
 *          
 * @pre assumes there is more to the string and erases
 *
 * @post action word is found and returned
 *
 * @par Algorithm checks if table already exists in current directory, if not, then creates table in current database & directory
 *
 * @param [in] string currentWorkingDirectory
 *
 * @param [in] string currentDatabase
 *
 * @param [in] string tblName
 *
 * @param [in] string input
 *
 * @param [in] bool &errorCode
 *      
 * @return  none
 *
 * @note None
 */
void Table::tableCreate( string currentWorkingDirectory, string currentDatabase, string tblName, string input, bool &errorCode )
{
	vector< Attribute> tblAttributes;
	Attribute attr;
	string temp;
	int commaCount;

	//get filepath, Database name + table name
	string filePath = "/" + currentDatabase + "/" + tblName;
	//output to file using ofstream operator
	ofstream fout( ( currentWorkingDirectory + filePath ).c_str() );

	//parse input str
		//remove beginning and end ()'s
		//get first open paren
	input.erase( 0, input.find( "(" ) + 1 );
	input.erase( input.find_last_of( ")" ), input.length()-1 );

	commaCount = getCommaCount( input );

	for( int index = 0; index < commaCount; index++ )
	{
		//remove beginning parameter
		temp = input.substr( 0, input.find( "," ));
		input.erase( 0, input.find(",") + 1 );
		//remove leading white space
		removeLeadingWS( temp );
		//parse, get next two words
		attr.attributeName = getNextWord( temp );
		attr.attributeType = getNextWord( temp );

		//check that variable name does not already exist
		if( attributeNameExists( tblAttributes, attr ) )
		{
			errorCode = true;
			cout << "-- !Failed to create table " << tblName << " because there are multiple ";
			cout << attr.attributeName << " variables." << endl;
			system( ( "rm " +  filePath ).c_str() ) ;
			fout.close();
			return;
		}

		//push attribute onto file
		tblAttributes.push_back( attr );

		//output to file
		fout << attr.attributeName << " ";
		fout << attr.attributeType << "\t";
	}
	
	//remove leading WS from input
	removeLeadingWS( input );
	//parse next two words
	attr.attributeName = getNextWord( input );
	//type is remaining string
	attr.attributeType = input;
	if( attributeNameExists( tblAttributes, attr ) )
	{
		errorCode = true;
		cout << "-- !Failed to create table " << tblName << " because there are multiple ";
		cout << attr.attributeName << " variables." << endl;
		system( ( "rm " +  currentDatabase + "/" + tblName ).c_str() ) ;
		fout.close();
		return;
	}

	//push onto vecotr
	tblAttributes.push_back( attr );
	
	//output to file
	fout << attr.attributeName << " ";
	fout << attr.attributeType;
	fout.close();

	cout << "-- Table " << tblName << " created." << endl;
}



/**
 * @brief tableDrop
 *
 * @details Used to delete a table from current database
 *          
 * @pre assumes table exists in current database
 *
 * @post table no longer exists
 *
 * @par Algorithm uses sys library to run linux terminal commands to delete table
 *
 * @param [in] string dbName - the database currently in 
 *      
 * @return None
 *
 * @note None
 */
void Table::tableDrop( string currentWorkingDirectory, string dbName )
{
	system( ( "rm " + currentWorkingDirectory + "/" + dbName + "/" + tableName ).c_str() ) ;
	cout << "-- Table " << tableName << " deleted." << endl;
}


/**
 * @brief tableAlter method 
 *
 * @details used to add attributes to a specified table
 *          
 * @pre assumes table exists and attribute name and type are specified
 *
 * @post attribute(s) are added to the table
 *
 * @par Algorithm checks if table exists in current directory, and if so, adds the parsed attribute name & type
 *
 * @param [in] string currentWorkingDirectory
 *
 * @param [in] string currentDatabase
 *
 * @param [in] string input
 *
 * @param [in] bool &errorCode
 *      
 * @return none
 *
 * @note None
 */
void Table::tableAlter( string currentWorkingDirectory, string currentDatabase, string input, bool &errorCode )
{
	vector < Attribute > tableAttributes;
	vector < string > fileContents;
	string attrLine;
	Attribute attr;
	Attribute tempAttr;
	int commaCount = 0;
	string temp;
	int originalNumOfAttr = 0;
	int newNumOfAttr = 0;
	//create filepath  to read from file
	string filePath = "/" + currentDatabase + "/" + tableName;
	ifstream fin( ( currentWorkingDirectory + filePath ).c_str() );

	string action = getNextWord( input );

	if( action == "ADD" )
	{
		while( !fin.eof() )
		{
			//get attribute line
			getline( fin, attrLine );

			while( !attrLine.empty() )
			{
				string attribute = getUntilTab( attrLine );
				//parse attribute further
				tempAttr.attributeName = getNextWord( attribute );
				tempAttr.attributeType = attribute;

				tableAttributes.push_back( tempAttr );
			}
			//check again that it is not end of file
			if( !fin.eof() )
			{
				getline( fin, temp );
				fileContents.push_back( temp );
			}	
		}

		//get comma count to get num of attributes
		commaCount = getCommaCount( input );

		//get number of attributes
		originalNumOfAttr = tableAttributes.size();

		ofstream fout( ( currentWorkingDirectory + filePath ).c_str() );
		//get additional attributes
		for( int index = 0; index < commaCount; index++ )
		{
			//remove beginning parameter
			temp = input.substr( 0, input.find( "," ));
			input.erase( 0, input.find(",") + 1 );
			//remove leading white space
			removeLeadingWS( temp );
			//parse, get next two words
			attr.attributeName = getNextWord( temp );
			attr.attributeType = getNextWord( temp );

			//check that variable name does not already exist
			if( attributeNameExists( tableAttributes, attr ) )
			{
				errorCode = true;
				cout << "-- !Failed to modify table " << tableName << " because there are multiple ";
				cout << attr.attributeName << " variables." << endl;
				system( ( "rm " +  filePath ).c_str() ) ;
				return;
			}

			//push attribute onto file
			tableAttributes.push_back( attr );
		}
	
		//remove leading WS from input
		removeLeadingWS( input );
		//parse next two words
		attr.attributeName = getNextWord( input );
		//type is remaining string
		attr.attributeType = input;
		if( attributeNameExists( tableAttributes, attr ) )
		{
			errorCode = true;
			cout << "-- !Failed to modify table " << tableName << " because there are multiple ";
			cout << attr.attributeName << " variables." << endl;
			return;
		}
		//push onto vecotr
		tableAttributes.push_back( attr );

		int tableSize = tableAttributes.size();
		newNumOfAttr = tableSize - originalNumOfAttr;
		for( int index = 0; index < tableSize; index++ )
		{
			if( index != tableSize - 1 )
			{
				//if its the last table then do not add tab
				fout << tableAttributes[ index ].attributeName << " ";		
				fout << tableAttributes[ index ].attributeType << "\t";	
			}
			else
			{
				fout << tableAttributes[ index ].attributeName << " ";		
				fout << tableAttributes[ index ].attributeType;	
			}

		}

		//initalize all records so that attribtue is null
		int contentSize = fileContents.size();
		for( int index = 0; index < contentSize; index++ )
		{
			fout << endl << fileContents[ index ];
			for( int newAttr = 0; newAttr < newNumOfAttr; newAttr++ )
			{
				fout << "\tnull";
			}
		}
		fin.close();
		fout.close();
		cout << "-- Table " << tableName << " modified." << endl;
	}
	else
	{
		return;
	}
}


/**
 * @brief tableSelect method
 *
 * @details  displays the attributes from queried table
 *          
 * @pre assumes table specified is in the current directory
 *
 * @post attributes stored in the directory are displayed 
 *
 * @par Algorithm goes to the filepath, and displays info from the file
 *
 * @param [in] string currentWorkingDirectory
 *
 * @param [in] string currentDatabase
 *   
 * @param [in] string whereType
 *
 * @param [in] string queryType
 *
 * @return None
 *
 * @note None
 */
void Table::tableSelect( string currentWorkingDirectory, string currentDatabase, string whereType, string queryType )
{
	vector< Attribute > attributes;
	vector< AttributeSubset > attrSubsets;
	WhereCondition wCond;
	string filePath = "/" + currentDatabase + "/" + tableName;
	string temp;
	int commaCount;
	int contentLineCount = 0;
	double tempDouble;
	ifstream fin( ( currentWorkingDirectory + filePath ).c_str() );

	//get attributes
		//get until end of line
	//check type of query
	getline( fin, temp);

	//automatically get all the attributes
	while( !temp.empty() )
	{
		Attribute tempAttribute;
		tempAttribute.attributeName = getNextWord( temp );
		tempAttribute.attributeType = getUntilTab( temp );
		attributes.push_back( tempAttribute );
	}

	//if query all attributes
	if( queryType == ALL )
	{
		cout << "-- ";
		int size = attributes.size();

		//output all attributes
		for( int index = 0; index < size; index++ )
		{
			cout << attributes[ index ].attributeName << " ";
			cout << attributes[ index ].attributeType;
			if( index != size - 1 )
			{
				cout << "|";
			}
		}
		cout << endl;

		//if there is a where condition then output with where cond
		if( !whereType.empty() )
		{
			getWhereCondition( wCond, whereType, attributes);
		
					//get size of file content
			while( !fin.eof() )
			{
				getline( fin, temp );
				contentLineCount++;
			}
			fin.close();
			ifstream fin( ( currentWorkingDirectory + filePath ).c_str() );
			//get attribute and store into temp, not used
			getline( fin, temp );
			//create 2d array
			string twoDArr[ contentLineCount ][ attributes.size() ];
			int attributesSize = attributes.size();
			for( int iIndex = 0; iIndex < contentLineCount; iIndex++ )
			{
				getline( fin, temp );
				for( int jIndex = 0; jIndex < attributesSize; jIndex++ )
				{
					twoDArr[ iIndex ][ jIndex ] = getUntilTab( temp );
				}
			}

			//output specific data
			//for each row
			for( int iIndex = 0; iIndex < contentLineCount; iIndex++ )
			{
				bool printResult = false;
				cout << "-- ";
				//for each col
				for( int jIndex = 0; jIndex < attributesSize; jIndex++ )
				{
						if( wCond.operatorValue == "=" )
						{
							
							//if we are not comparing floats, then do not convert to double
							if( !wCond.floatValue ) 
							{
								if( twoDArr[ iIndex ][ wCond.attributeIndex ] == wCond.comparisonValue )
								{
									printResult = true;
								}
							}
							else if( wCond.floatValue )
							{
								tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
								if( tempDouble == wCond.comparisonValueFloat )
								{
									printResult = true;
								}
							}
						}
						else if( wCond.operatorValue == "!=" )
						{
							if( !wCond.floatValue ) 
							{
								if( twoDArr[ iIndex ][ wCond.attributeIndex ] != wCond.comparisonValue )
								{
									printResult = true;
								}
							}
							else if( wCond.floatValue )
							{
								tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
								if( tempDouble != wCond.comparisonValueFloat )
								{
									printResult = true;
								}
							}
						}
						if( wCond.operatorValue == "<" )
						{
							if( !wCond.floatValue ) 
							{
								if( twoDArr[ iIndex ][ wCond.attributeIndex ] < wCond.comparisonValue )
								{
									printResult = true;
								}
							}
							else if( wCond.floatValue )
							{
								tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
								if( tempDouble < wCond.comparisonValueFloat )
								{
									printResult = true;
								}
							}
						}
						else if( wCond.operatorValue == "<=" )
						{
							if( !wCond.floatValue ) 
							{
								if( twoDArr[ iIndex ][ wCond.attributeIndex ] <= wCond.comparisonValue )
								{
									printResult = true;
								}
							}
							else if( wCond.floatValue )
							{
								tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
								if( tempDouble <= wCond.comparisonValueFloat )
								{
									printResult = true;
								}
							}
						}
						else if( wCond.operatorValue == ">" )
						{
							if( !wCond.floatValue ) 
							{
								if( twoDArr[ iIndex ][ wCond.attributeIndex ] >= wCond.comparisonValue )
								{
									printResult = true;
								}
							}
							else if( wCond.floatValue )
							{
								tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
								if( tempDouble > wCond.comparisonValueFloat )
								{
									printResult = true;
								}
							}
						}
						else if( wCond.operatorValue == ">=" )
						{
							if( !wCond.floatValue ) 
							{
								if( twoDArr[ iIndex ][ wCond.attributeIndex ] >= wCond.comparisonValue )
								{
									printResult = true;
								}
							}
							else if( wCond.floatValue )
							{
								tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
								if( tempDouble >= wCond.comparisonValueFloat )
								{
									printResult = true;
								}
							}
						}
						if( printResult )
						{
							string content = twoDArr[ iIndex ][ jIndex ];
							if( content[ 0 ] == '\'' && content[ content.size() - 1 ] == '\'' )
							{
								content.erase( 0, content.find( "'" ) + 1 );
								content.erase( content.find_last_of( "'" ), content.length()-1 );
							}
							cout << content << "|";
						}	
				}
				if( printResult )
				{
					cout << "\b \b";
					cout << endl;
				}
				else
				{
					cout << "\b \b\b\b";	
				}
			}

		}
		else //output everything otherwise
		{
			while( !fin.eof() )
			{
				getline( fin, temp);
				cout << "-- ";
				while( !temp.empty() )
				{
					string content = getUntilTab( temp );
					//check that if start of string is "'" and end is "'"
					if( content[ 0 ] == '\'' && content[ content.size() - 1 ] == '\'' )
					{
						content.erase( 0, content.find( "'" ) + 1 );
						content.erase( content.find_last_of( "'" ), content.length()-1 );
					}
					cout << content << "|";
				}
				cout << "\b \b";
				cout << endl;
			}
			fin.close();
		}
	}
	else
	{
		//get attributes in query subset 
		commaCount = getCommaCount( queryType );
		
		//get subset to query
		for ( int index = 0; index < commaCount+1; index++ )
		{
			AttributeSubset tempAttr;

			//remove beginning parameter
			temp = queryType.substr( 0, queryType.find( "," ));
			queryType.erase( 0, queryType.find(",") + 1 );

			//remove leading white space
			removeLeadingWS( temp );

			tempAttr.attributeName = temp;
			tempAttr.attributeIndex = findAttrOccur( attributes, tempAttr.attributeName );
			attrSubsets.push_back( tempAttr );
		}

		//check that there is where condition
		if( !whereType.empty() )
		{
			getWhereCondition( wCond, whereType, attributes);
		}

		//get size of file content
		while( !fin.eof() )
		{
			getline( fin, temp );
			contentLineCount++;
		}

		fin.close();
		ifstream fin( ( currentWorkingDirectory + filePath ).c_str() );
		//get attribute and store into temp, not used
		getline( fin, temp );
		//create 2d array
		string twoDArr[ contentLineCount ][ attributes.size() ];
		
		//store data in 2d array
		int attributesSize = attributes.size();
		for( int iIndex = 0; iIndex < contentLineCount; iIndex++ )
		{
			getline( fin, temp );
			for( int jIndex = 0; jIndex < attributesSize; jIndex++ )
			{
				twoDArr[ iIndex ][ jIndex ] = getUntilTab( temp );
			}
		}

		//output attribute subset
		cout << "-- ";
		for( int index = 0; index < attributesSize; index++ )
		{
			if( currIndexIsSubset( attrSubsets, index ) )
			{
				cout << attributes[ index ].attributeName;
				cout << " " << attributes[ index ].attributeType << "|"; 
			}
		}
		cout << "\b \b" << endl;

		//output specific data
		//for each row
		for( int iIndex = 0; iIndex < contentLineCount; iIndex++ )
		{
			bool printResult = false;
			cout << "-- ";
			//for each col
			for( int jIndex = 0; jIndex < attributesSize; jIndex++ )
			{
				//for each col check that the subset is to be called
				if( currIndexIsSubset( attrSubsets, jIndex ) )
				{
					if( wCond.operatorValue == "=" )
					{
						
						//if we are not comparing floats, then do not convert to double
						if( !wCond.floatValue ) 
						{
							if( twoDArr[ iIndex ][ wCond.attributeIndex ] == wCond.comparisonValue )
							{
								printResult = true;
							}
						}
						else if( wCond.floatValue )
						{
							tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
							if( tempDouble == wCond.comparisonValueFloat )
							{
								printResult = true;
							}
						}
					}
					else if( wCond.operatorValue == "!=" )
					{
						if( !wCond.floatValue ) 
						{
							if( twoDArr[ iIndex ][ wCond.attributeIndex ] != wCond.comparisonValue )
							{
								printResult = true;
							}
						}
						else if( wCond.floatValue )
						{
							tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
							if( tempDouble != wCond.comparisonValueFloat )
							{
								printResult = true;
							}
						}
					}
					if( wCond.operatorValue == "<" )
					{
						if( !wCond.floatValue ) 
						{
							if( twoDArr[ iIndex ][ wCond.attributeIndex ] < wCond.comparisonValue )
							{
								printResult = true;
							}
						}
						else if( wCond.floatValue )
						{
							tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
							if( tempDouble < wCond.comparisonValueFloat )
							{
								printResult = true;
							}
						}
					}
					else if( wCond.operatorValue == "<=" )
					{
						if( !wCond.floatValue ) 
						{
							if( twoDArr[ iIndex ][ wCond.attributeIndex ] <= wCond.comparisonValue )
							{
								printResult = true;
							}
						}
						else if( wCond.floatValue )
						{
							tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
							if( tempDouble <= wCond.comparisonValueFloat )
							{
								printResult = true;
							}
						}
					}
					else if( wCond.operatorValue == ">" )
					{
						if( !wCond.floatValue ) 
						{
							if( twoDArr[ iIndex ][ wCond.attributeIndex ] >= wCond.comparisonValue )
							{
								printResult = true;
							}
						}
						else if( wCond.floatValue )
						{
							tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
							if( tempDouble > wCond.comparisonValueFloat )
							{
								printResult = true;
							}
						}
					}
					else if( wCond.operatorValue == ">=" )
					{
						if( !wCond.floatValue ) 
						{
							if( twoDArr[ iIndex ][ wCond.attributeIndex ] >= wCond.comparisonValue )
							{
								printResult = true;
							}
						}
						else if( wCond.floatValue )
						{
							tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
							if( tempDouble >= wCond.comparisonValueFloat )
							{
								printResult = true;
							}
						}
					}
					if( printResult )
					{
						string content = twoDArr[ iIndex ][ jIndex ];
						if( content[ 0 ] == '\'' && content[ content.size() - 1 ] == '\'' )
						{
							content.erase( 0, content.find( "'" ) + 1 );
							content.erase( content.find_last_of( "'" ), content.length()-1 );
						}
						cout << content << "|";
					}	
				}

				
			}
			if( printResult )
			{
				cout << "\b \b";
				cout << endl;
			}
			else
			{
				cout << "\b \b\b\b";	
			}
		}
	}
}

/**
 *@brief tableInsert
 *
 *@details inserts a new record into an existing table
 *
 *@par Algorithm goes to correct file path where table info is, parses record, 
 *            and adds new record to the file and outputs succession to terminal
 *
 *@param [in] string currentWorkingDirectory
 * 
 *@param [in] string currentDatabase
 *
 *@param [in] string tblName
 *
 *@param [in] string input
 *
 *@param [in] bool &errorCode
 *
*/
void Table::tableInsert( string currentWorkingDirectory, string currentDatabase, string tblName, string input, bool &errorCode )
{
	string contentStr = "\n";
	int commaCount;
	string filePath = "/" + currentDatabase + "/" + tableName;
	string temp;

	commaCount = getCommaCount( input );
	for( int index = 0; index < commaCount; index++ )
	{
		//remove beginning parameter
		temp = input.substr( 0, input.find( "," ));
		input.erase( 0, input.find(",") + 1 );

		//remove leading white space
		removeLeadingWS( temp );

		//concat temp to content str
		contentStr = contentStr + temp + '\t';
	}
	
	//remove leading WS from input
	removeLeadingWS( input );
	contentStr = contentStr + input;

	ofstream fout;
	fout.open( ( currentWorkingDirectory + filePath ).c_str(), ofstream::out | ofstream::app );
	fout << contentStr;

	fout.close();

	cout << "-- 1 new record inserted." << endl;
}

/**
 *@brief tableUpdate
 *
 *@details updates the table based on all records that match the given condition  
 *
 *@param [in] string currentWorkingDirectory
 *
 *@param [in] string currentDatabase
 *
 *@param [in] string whereType
 *
 *@param [in] string setType
 *
*/
void Table::tableUpdate( string currentWorkingDirectory, string currentDatabase, string whereType, string setType )
{
	vector< Attribute > attributes;
	SetCondition sCond;
	WhereCondition wCond;
	string filePath = "/" + currentDatabase + "/" + tableName;
	string temp;
	int recordsModified = 0;
	int contentLineCount = 0;
	double tempDouble;
	ifstream fin( ( currentWorkingDirectory + filePath ).c_str() );

	//get attributes
		//get until end of line
	//check type of query
	getline( fin, temp);

	//get attribute data
	while( !temp.empty() )
	{
		Attribute tempAttribute;
		tempAttribute.attributeName = getNextWord( temp );
		tempAttribute.attributeType = getUntilTab( temp );
		attributes.push_back( tempAttribute );
	}

	//get where and set conditions
	getWhereCondition( wCond, whereType, attributes);
	getSetCondition( sCond, setType, attributes );

	//get size of file content
	while( !fin.eof() )
	{
		getline( fin, temp );
		contentLineCount++;
	}

	fin.close();
	
	fin.open(( currentWorkingDirectory + filePath ).c_str() );
	
	//get attribute and store into temp 
	getline( fin, temp );
	string attributeData = temp;
	
	//create 2d array and store content data
	string twoDArr[ contentLineCount ][ attributes.size() ];
	int attributesSize = attributes.size();
	for( int iIndex = 0; iIndex < contentLineCount; iIndex++ )
	{
		getline( fin, temp );
		for( int jIndex = 0; jIndex < attributesSize; jIndex++ )
		{
			twoDArr[ iIndex ][ jIndex ] = getUntilTab( temp );
		}
	}
	fin.close();

	ofstream fout( ( currentWorkingDirectory + filePath ).c_str() );
	
	fout << attributeData << endl;
	
	//output specific data
	//for each row
	for( int iIndex = 0; iIndex < contentLineCount; iIndex++ )
	{
		bool printResult = false;
		//for each col
		for( int jIndex = 0; jIndex < attributesSize; jIndex++ )
		{
			//for each col check that the set index is to be called
			if( sCond.attributeIndex == jIndex )
			{
				if( wCond.operatorValue == "=" )
				{
					//if we are not comparing floats, then do not convert to double
					if( !wCond.floatValue ) 
					{
						if( twoDArr[ iIndex ][ wCond.attributeIndex ] == wCond.comparisonValue )
						{
							printResult = true;
						}
					}
					else if( wCond.floatValue )
					{
						tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
						if( tempDouble == wCond.comparisonValueFloat )
						{
							printResult = true;
						}
					}
				}
				else if( wCond.operatorValue == "!=" )
				{
					if( !wCond.floatValue ) 
					{
						if( twoDArr[ iIndex ][ wCond.attributeIndex ] != wCond.comparisonValue )
						{
							printResult = true;
						}
					}
					else if( wCond.floatValue )
					{
						tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
						if( tempDouble != wCond.comparisonValueFloat )
						{
							printResult = true;
						}
					}
				}
				if( wCond.operatorValue == "<" )
				{
					if( !wCond.floatValue ) 
					{
						if( twoDArr[ iIndex ][ wCond.attributeIndex ] < wCond.comparisonValue )
						{
							printResult = true;
						}
					}
					else if( wCond.floatValue )
					{
						tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
						if( tempDouble < wCond.comparisonValueFloat )
						{
							printResult = true;
						}
					}
				}
				else if( wCond.operatorValue == "<=" )
				{
					if( !wCond.floatValue ) 
					{
						if( twoDArr[ iIndex ][ wCond.attributeIndex ] <= wCond.comparisonValue )
						{
							printResult = true;
						}
					}
					else if( wCond.floatValue )
					{
						tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
						if( tempDouble <= wCond.comparisonValueFloat )
						{
							printResult = true;
						}
					}
				}
				else if( wCond.operatorValue == ">" )
				{
					if( !wCond.floatValue ) 
					{
						if( twoDArr[ iIndex ][ wCond.attributeIndex ] >= wCond.comparisonValue )
						{
							printResult = true;
						}
					}
					else if( wCond.floatValue )
					{
						tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
						if( tempDouble > wCond.comparisonValueFloat )
						{
							printResult = true;
						}
					}
				}
				else if( wCond.operatorValue == ">=" )
				{
					if( !wCond.floatValue ) 
					{
						if( twoDArr[ iIndex ][ wCond.attributeIndex ] >= wCond.comparisonValue )
						{
							printResult = true;
						}
					}
					else if( wCond.floatValue )
					{
						tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
						if( tempDouble >= wCond.comparisonValueFloat )
						{
							printResult = true;
						}
					}
				}
				if( printResult )
				{
					recordsModified++;
					twoDArr[ iIndex ][ jIndex ] = sCond.newValue;
				}	
			}
			fout << twoDArr[ iIndex ][ jIndex ];
			if( jIndex != attributesSize - 1 )
			{
				fout << "\t";
			}
		}
		if( iIndex != contentLineCount- 1 )
		{
			fout << endl;
		}
	}
	cout << "-- " << recordsModified; 
	if( recordsModified == 1 )
	{
		cout  << " record modified." << endl;
	}
	else
	{
		cout << " records modified." << endl;
	}
}


void Table::tableDelete( string currentWorkingDirectory, string currentDatabase, string whereType )
{
	vector< Attribute > attributes;
	vector< string > contentOutput;
	SetCondition sCond;
	WhereCondition wCond;
	string filePath = "/" + currentDatabase + "/" + tableName;
	string temp;
	vector< int > recordsModified;
	int contentLineCount = 0;
	double tempDouble;
	ifstream fin( ( currentWorkingDirectory + filePath ).c_str() );

	//get attributes
		//get until end of line
	//check type of query
	getline( fin, temp);

	//get attribute data
	while( !temp.empty() )
	{
		Attribute tempAttribute;
		tempAttribute.attributeName = getNextWord( temp );
		tempAttribute.attributeType = getUntilTab( temp );
		attributes.push_back( tempAttribute );
	}

	getWhereCondition( wCond, whereType, attributes);

	//get size of file content
	while( !fin.eof() )
	{
		getline( fin, temp );
		contentLineCount++;
	}

	fin.close();
	//ifstream fin( ( currentWorkingDirectory + filePath ).c_str() );
	fin.open(( currentWorkingDirectory + filePath ).c_str() );
	//get attribute and store into temp 
	getline( fin, temp );
	string attributeData = temp;
	//create 2d array
	string twoDArr[ contentLineCount ][ attributes.size() ];
	int attributesSize = attributes.size();
	for( int iIndex = 0; iIndex < contentLineCount; iIndex++ )
	{
		getline( fin, temp );
		for( int jIndex = 0; jIndex < attributesSize; jIndex++ )
		{
			twoDArr[ iIndex ][ jIndex ] = getUntilTab( temp );
		}
	}
	fin.close();

	ofstream fout( ( currentWorkingDirectory + filePath ).c_str() );
	
	fout << attributeData << endl;
	//output specific data
	//for each row
	for( int iIndex = 0; iIndex < contentLineCount; iIndex++ )
	{
		bool printResult = true;
		//check that index val does 
		if( wCond.operatorValue == "=" )
		{
			
			//if we are not comparing floats, then do not convert to double
			if( !wCond.floatValue ) 
			{
				if( twoDArr[ iIndex ][ wCond.attributeIndex ] == wCond.comparisonValue )
				{
					printResult = false;
				}
			}
			else if( wCond.floatValue )
			{
				tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
				if( tempDouble == wCond.comparisonValueFloat )
				{
					printResult = false;
				}
			}
		}
		else if( wCond.operatorValue == "!=" )
		{
			if( !wCond.floatValue ) 
			{
				if( twoDArr[ iIndex ][ wCond.attributeIndex ] != wCond.comparisonValue )
				{
					printResult = false;
				}
			}
			else if( wCond.floatValue )
			{
				tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
				if( tempDouble != wCond.comparisonValueFloat )
				{
					printResult = false;
				}
			}
		}
		if( wCond.operatorValue == "<" )
		{
			if( !wCond.floatValue ) 
			{
				if( twoDArr[ iIndex ][ wCond.attributeIndex ] < wCond.comparisonValue )
				{
					printResult = false;
				}
			}
			else if( wCond.floatValue )
			{
				tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
				if( tempDouble < wCond.comparisonValueFloat )
				{
					printResult = false;
				}
			}
		}
		else if( wCond.operatorValue == "<=" )
		{
			if( !wCond.floatValue ) 
			{
				if( twoDArr[ iIndex ][ wCond.attributeIndex ] <= wCond.comparisonValue )
				{
					printResult = false;
				}
			}
			else if( wCond.floatValue )
			{
				tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
				if( tempDouble <= wCond.comparisonValueFloat )
				{
					printResult = false;
				}
			}
		}
		else if( wCond.operatorValue == ">" )
		{
			if( !wCond.floatValue ) 
			{
				if( twoDArr[ iIndex ][ wCond.attributeIndex ] >= wCond.comparisonValue )
				{
					printResult = false;
				}
			}
			else if( wCond.floatValue )
			{
				tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
				if( tempDouble > wCond.comparisonValueFloat )
				{
					printResult = false;
				}
			}
		}
		else if( wCond.operatorValue == ">=" )
		{
			if( !wCond.floatValue ) 
			{
				if( twoDArr[ iIndex ][ wCond.attributeIndex ] >= wCond.comparisonValue )
				{
					printResult = false;
				}
			}
			else if( wCond.floatValue )
			{
				tempDouble = atof( twoDArr[ iIndex ][ wCond.attributeIndex ].c_str() );
				if( tempDouble >= wCond.comparisonValueFloat )
				{
					printResult = false;
				}
			}
		}

		if( printResult == true )
		{
			string content = "";
			for( int jIndex = 0; jIndex < attributesSize; jIndex++ )
			{
				content = content + twoDArr[ iIndex ][ jIndex ];
				if( jIndex != attributesSize - 1 )
				{
					content = content + '\t';
				}
			}
			contentOutput.push_back( content );
		}
	}

	int newContentSize = contentOutput.size();

	for( int index = 0; index < newContentSize; index++ )
	{
		fout << contentOutput[ index ];
		if( index != newContentSize - 1 )
		{
			fout << endl;
		}
	}

	int recordsDeleted = contentLineCount - newContentSize;
	cout << "-- " << recordsDeleted;
	if( recordsDeleted == 1 )
	{
		cout << " record deleted." << endl;
	}
	else
	{
		cout << " records deleted." << endl;
	}
}

int findAttrOccur( vector< Attribute > attributes, string attrName )
{
	int attrSize = attributes.size();
	int attrIndex;
	for ( int index = 0; index < attrSize; index++ )
	{
		if( attributes[ index ].attributeName == attrName )
		{
			attrIndex = index;
		}
	}
	return attrIndex;
}

bool isAttrFloat( vector< Attribute > attributes, string attrName )
{
	int attrSize = attributes.size();
	for ( int index = 0; index < attrSize; index++ )
	{
		if( attributes[ index ].attributeName == attrName )
		{
			if( attributes[ index ].attributeType == "float" || attributes[ index ].attributeType == "FLOAT" )
			{
				return true;
			}
		}
	}
	return false;	
}

/**
*@brief getWhereCondition method
*
*@details if there is a where condition in statement, parses that data
*
*@par Algorithm gets Where condition, and checks operatorValue from prev. function
*			and stores the comparison which is converted into a floata and stored
*
*@param [in] WhereCondition &wCond
*
*@param [in] string whereType
*
*@param [in] vector <Attribute> attributes
*
*@note uses functions from setCondition 
*/
void getWhereCondition( WhereCondition &wCond, string whereType, vector< Attribute > attributes )
{
	wCond.attributeName = getNextWord( whereType );
	wCond.attributeIndex = findAttrOccur( attributes, wCond.attributeName );
	wCond.operatorValue = getNextWord( whereType );
	wCond.comparisonValue = whereType;

	if( isAttrFloat( attributes, wCond.attributeName ) )
	{
		wCond.floatValue = true;
		wCond.comparisonValueFloat = atof( wCond.comparisonValue.c_str() );
	}
}

/**
*@brief getSetCondition method
*
*@details parses through to store set command information
*
*@par Algorithm uses previous functions related to parsing such as getNextWord and removeLeadingWS to parse through phrase and store info 
*
*@param [in] SetCondition &sCond
*
*@param [in] string setType
*
*@param [in] vector <Attribute> attributes
*
*@return none (void)
*
*/
void getSetCondition( SetCondition &sCond, string setType, vector< Attribute > attributes )
{
	removeLeadingWS( setType );
	sCond.attributeName = getNextWord( setType );
	sCond.attributeIndex = findAttrOccur( attributes, sCond.attributeName );
	sCond.operatorValue = getNextWord( setType );
	sCond.newValue = setType;
}

/**
*@brief bool currIndexIsSubset Method
*
*@details checks if the the current Index mathces the index passed in 
*
*@param [in] vector < AttributeSubset > attrSubsets
*
*@param [in] int indexVal
*
*@return bool true if the same value
*/
bool currIndexIsSubset( vector< AttributeSubset > attrSubsets, int indexVal )
{
	int subsetSize = attrSubsets.size();
	for( int index = 0; index < subsetSize; index++ )
	{
		if( attrSubsets[ index ].attributeIndex == indexVal )
		{
			return true;
		}
	}
	return false;
}

/**
*@brief indexExists method
*
*@details checks if the index value passed in exists 
*
*@param [in] int i (index value)
*
*@param [in] vector < int > indexCounter
*
*@return bool true if index passed in exists in the vector of indices
*/
bool indexExists( int i, vector< int > indexCounter )
{
	int size = indexCounter.size();
	for ( int index = 0; index < size; index++ )
	{
		if( indexCounter[ index ] == i )
		{
			return true;
		}
	}
	return false;
}




// Terminating precompiler directives  ////////////////////////////////////////
#endif
