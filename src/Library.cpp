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


#include "Library.h"
#include "LibraryFileFinder.h"
#include <iostream>

Library::Library(string path, string filename)
{
	Library::path = path;
	Library::filename = filename;
	
	symlink = false;
}

void Library::accept()
{
	int return_value = system( (string("readlink ") + path + filename + string(" > /dev/null")).c_str() );
	symlink = (return_value == 0);
}

void Library::setSymlink(bool symlink)
{
	Library::symlink = symlink;
}
