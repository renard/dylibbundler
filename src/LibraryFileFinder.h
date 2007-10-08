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


#ifndef _library_search_
#define _library_search_

#include <string>
#include <vector>
using namespace std;

class Library;


class LibraryFileFinder
{
protected:
	vector<Library> libraries;
	vector<Library> symlinks;
	vector<string> dependencies;
	string path;
public:

	void addDependencies(const char* depend); // accepts multiple entries separated by ':'. Does not trim spaces.
		
	void setPath(const char* path);
	
	// used internally
	string name;
	LibraryFileFinder(const char* query_name);	
	void fixLibs(string dest_folder, bool override_files);
	void fixExecutable(string executable);
	void checkFindingsValidity();
	void findAndFixYourInterlibDeps();
	void fixLibThatDependsOnYou(LibraryFileFinder* lib);
};

class Library_FileList : public LibraryFileFinder
{
public:
		
	Library_FileList(const char* query_name);
	void addLibName(const char* file);
	void addSymlinkName(const char* file); // accepts multiple entries separated by ':'. Does not trim spaces.
};

class Library_FileSearch : public LibraryFileFinder
{
	string startsWith_s;
	string endsWith_s;
	string contains_s;
	vector<string> doesntContain_s;
	
	string regex;
public:

	Library_FileSearch(const char* query_name);
	
	void startsWith(const char* s);
	void endsWith(const char* s);
	void contains(const char* s);
	void doesntContain(const char* s); // accepts multiple entries separated by ':'. Does not trim spaces.

	void setRegex(const char* s); // not implemented

	// used internally
	// checks if given library matches condition. If so, the object keeps it and returns true.
	virtual bool checkLib(Library& lib);
};

void addPrefix(string prefix); // accepts multiple entries separated by ':'. Does not trim spaces.

void pushBackList(Library_FileList* search);
void pushBackSearch(Library_FileSearch* search);

void startSearch();

void fixLibs(string dest_folder, bool create, bool override_files, bool override_dir);
void fixExecutable(string executable);

#endif