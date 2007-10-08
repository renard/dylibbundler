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


#include "FileList.h"
#include "Library.h"
#include <dirent.h>
#include <iostream>
#include <string>
#include <vector>

vector<Library> files;

int getFileAmount()
{
	return files.size();
}
Library& getFile(int id)
{
	return files[id];
}

void readFilesInPath(string path)
{
	
	// read all files in directory 'path'.
	DIR* libs_dir = opendir( path.c_str() );
	
	if(libs_dir == false)
	{
		cerr << ( string("\n\nError : cannot open directory ") + path ).c_str() << endl;
		exit(1);
	}
	
	struct dirent* entry;
	
	string prefix = string("readlink ") + path;
	
	while(true)
	{
		entry = readdir(libs_dir);
		if(entry == NULL) break;
		
		string libname = string( entry->d_name );
		
		//if( libname.find(".dylib", 0) != string::npos )
		//{
			files.push_back( Library(path, libname) );
		//}
	}
}
