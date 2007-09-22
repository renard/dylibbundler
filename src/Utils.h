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


#ifndef _utils_h_
#define _utils_h_

#include <string>
#include <vector>
using namespace std;

class Library;

void tokenize(const string& str, const char* delimiters, vector<string>*);
bool fileExists( const char *filename );

// target_file_name can be a library as well as an executable
// tells it where to look for that lib from now on
// you can check the dependencies of a library or executable with "otool -L"
void fixLibDependency(Library& lib, string new_lib_name, string fix_this_file);

// tell each lib its new location
// you can check the identity of a library with "otool -D"
void fixLibIdentity(Library& lib);

void copyFile(string from, string to, bool override);
void copyLibFile(Library& lib, string to_path, string to_file, bool override);

void setInstallPath(string loc);

#endif