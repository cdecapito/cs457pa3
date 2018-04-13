// Program Information ////////////////////////////////////////////////////////
/**
 * @file main.cpp
 *
 * @brief Driver program to exercise the entire project
 * 
 * @details Starts the simulation for the basic database mgmt program
 *
 * 
 *          2.00 
 *          Carli Decapito
 *		  Sanya Gupta
 *		  Eugene Nelson (March 27 2018)
 *          Original code
 *
 * @Note Requires sim.cpp
 */
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include "sim.cpp"

using namespace std;

int main()
{
	//get current working directory
	char buffer[200];
	getcwd( buffer, sizeof( buffer ) );
	string currentWorkingDirectory( buffer );

	startSimulation( currentWorkingDirectory );

	return 0;
}
