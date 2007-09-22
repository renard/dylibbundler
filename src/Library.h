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


#ifndef _library_list_
#define _library_list_

#include <string>
using namespace std;

class LibraryFileFinder;

class Library
{
public:
string path, filename; // original location. e.g. "/usr/local/lib" and "libglib.dylib"
	
// new location e.g. "./libs/libglib.dylib".
// Only defined after file has been copied with copy lib function form Utils.cpp!
// new_path is the path where the file is left by dylibbundler, not the path relative to executable
string new_path, new_filename;


bool symlink;

Library(string path, string filename);

// call when you know we'll use this lib. it's simply to not
// waste CPU calculating stuff for libs we won't even use
void accept(); 

void setSymlink(bool symlink);

};

#endif