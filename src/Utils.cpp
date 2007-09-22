/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include "Utils.h"
#include "Library.h"
#include <iostream>
#include <sys/stat.h>
using namespace std;

string path_to_libs_folder = string("@executable_path/../libs/");

void setInstallPath(string loc)
{
	path_to_libs_folder = loc;
}

void tokenize(const string& str, const char* delim, vector<string>* vectorarg)
{
	vector<string>& tokens = *vectorarg;
	
	string delimiters(delim);
	
	// skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of( delimiters , 0);
	
	// find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	
	while (string::npos != pos || string::npos != lastPos)
	{
		// found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		
		// skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		
		// find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
	
}


bool fileExists( const char *filename )
{
	struct stat buffer ;
	if ( stat( filename, &buffer ) == 0 ) return true;
	return false;
}

void fixLibDependency(string old_lib_path, string new_lib_name, string target_file_name)
{

	string command = string("install_name_tool -change ") + old_lib_path + string(" ") + path_to_libs_folder + new_lib_name + string(" ") + target_file_name;
	cout << command.c_str() << endl;
	if( system( command.c_str() ) != 0 )
	{
		cerr << "\n\nError : An error occured while trying to fix depency of " << old_lib_path << " in " << target_file_name << endl;
		exit(1);
	}
}

void fixLibDependency(Library& lib, string new_lib_name, string file_to_fix)
{
	string command = std::string("install_name_tool -change ") + lib.path + lib.filename + string(" ") + path_to_libs_folder + new_lib_name + string(" ") + file_to_fix;
	cout << command.c_str() << endl;
	if( system( command.c_str() ) != 0 )
	{
		cerr << "\n\nError : An error occured while trying to fix depency of " << new_lib_name << " in " << file_to_fix << endl;
		exit(1);
	}
}

void fixLibIdentity(Library& lib)
{
		
	// tell each lib its new location
	string command = string("install_name_tool -id ") + string(" ") + path_to_libs_folder + lib.new_filename + string(" ") + lib.new_path + lib.new_filename;
	cout << command.c_str() << endl;
	if( system( command.c_str() ) != 0 )
	{
		cerr << "\n\nError : An error occured while trying to change identity of library from " <<
			(lib.path + lib.filename) << " to " << (path_to_libs_folder + lib.new_filename) << endl;
		
		exit(1);
	}

}

void copyFile(string from, string to, bool override)
{
	
	if(!override)
	{
		if(fileExists( to.c_str() ))
		{
			cerr << "\n\nError : File " << to.c_str() << " already exists. Remove it or enable overriding." << endl;
		}
	}
	
	string override_permission = string(override ? "-f " : "-n ");
		
	string command = string("cp ") + override_permission + from + string(" ") + to;
	cout << command.c_str() << endl;
		
	if( system( command.c_str() ) != 0 )
	{
		cerr << "\n\nError : An error occured while trying to copy file " << from << " to " << to << endl;
		exit(1);
	}
}

void copyLibFile(Library& lib, string to_path, string to_file, bool override)
{
	if( to_file.find(".dylib", 0) == string::npos )
	{
		to_file += ".dylib";
	}
	
	copyFile( lib.path + lib.filename, to_path + to_file, override );
	lib.new_path = to_path;
	lib.new_filename = to_file;
}