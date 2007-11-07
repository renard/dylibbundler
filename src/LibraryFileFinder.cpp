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


#include "LibraryFileFinder.h"
#include "Library.h"
#include "FileList.h"
#include "Utils.h"

#include <iostream>
#include <vector>

vector<Library_FileSearch*> lib_file_searches;
vector<Library_FileList*> lib_file_lists;
vector<string> prefixes;

void addPrefix(string prefix)
{
	//if( prefix[ prefix.size()-1 ] != '/' ) prefix += "/";
	//prefixes.push_back(prefix);
	
	tokenize( prefix, ":", &prefixes );
	
	for(int n=0; n<prefixes.size(); n++)
	{
		if( prefixes[n][ prefixes[n].size()-1 ] != '/' ) prefixes[n] += "/";
	}
}

void pushBackList(Library_FileList* search)
{
	lib_file_lists.push_back( search );
}
void pushBackSearch(Library_FileSearch* search)
{
	lib_file_searches.push_back( search );
}

void startSearch()
{
	if(lib_file_searches.size() > 0)
	{
		for(int n=0; n<prefixes.size(); n++) readFilesInPath( prefixes[n] );
		
		
		const int fileAmount = getFileAmount();
		Library_FileSearch* search;
		
		for(int s=0; s<lib_file_searches.size(); s++)
		{
			search = lib_file_searches[s];
			for(int f=0; f<fileAmount; f++)
			{
				search->checkLib( getFile(f) );
			}
		}
		
	}
}

// if you're not going to copy lib files, but only fix an executable, use this instead of 'fixLibs'
void prepareLibData()
{
    Library_FileSearch* search;
	for(int s=0; s<lib_file_searches.size(); s++)
	{
		search = lib_file_searches[s];
		search->prepareLibs();
	}
    
	Library_FileList* search2;
	for(int s=0; s<lib_file_lists.size(); s++)
	{
		search2 = lib_file_lists[s];
		search2->prepareLibs();
	}
}

void fixLibs(string dest_folder, bool create, bool override_files, bool override_dir)
{
	if(override_dir) create=true;

	// fix path if needed
	if( dest_folder[ dest_folder.size()-1 ] != '/' ) dest_folder += "/";
	
	// ----------- check dest folder stuff ----------
	bool dest_exists = fileExists(dest_folder.c_str());
	
	if(dest_exists and override_dir)
	{
		string command = string("rm -r ") + string(dest_folder);
		cout << command.c_str() << endl;
		if( system( command.c_str() ) != 0)
		{
			cerr << "\n\nError : An error occured while attempting to override dest folder." << endl;
			exit(1);
		}
		dest_exists = false;
	}
	
	if(!dest_exists)
	{
		
		if(create)
		{
			string command = string("mkdir ") + string(dest_folder);
			cout << command.c_str() << endl;
			if( system( command.c_str() ) != 0)
			{
				cerr << "\n\nError : An error occured while creating dest folder." << endl;
				exit(1);
			}
		}
		else
		{
			cerr << "\n\nError : Dest folder does not exist. Create it or pass the appropriate flag for automatic dest dir creation." << endl;
			exit(1);
		}
	}
	
	// ----------- copy libs to dest folder and correct inner identity ----------
	Library_FileSearch* search;
	for(int s=0; s<lib_file_searches.size(); s++)
	{
		search = lib_file_searches[s];
		search->prepareLibs(true, dest_folder, override_files);
	}

	Library_FileList* search2;
	for(int s=0; s<lib_file_lists.size(); s++)
	{
		search2 = lib_file_lists[s];
		search2->prepareLibs(true, dest_folder, override_files);
	}

	// ----------- correct inter-library dependencies ----------
	cout << "\n*  Fixing inter-library dependencies\n" << endl;
	Library_FileSearch* search3;
	for(int s=0; s<lib_file_searches.size(); s++)
	{
		search3 = lib_file_searches[s];
		search3->findAndFixYourInterlibDeps();
	}
	
	Library_FileList* search4;
	for(int s=0; s<lib_file_lists.size(); s++)
	{
		search4 = lib_file_lists[s];
		search4->findAndFixYourInterlibDeps();
	}
}

void fixExecutable(string executable)
{
	cout << "\n* Fixing executable " << executable << endl;
	
	Library_FileSearch* search;
    if(lib_file_searches.size() == 0 and lib_file_lists.size() == 0) cerr << "\n\nWarning : no libraries specified" << std::endl;
	for(int s=0; s<lib_file_searches.size(); s++)
	{
		search = lib_file_searches[s];
		search->fixExecutable( executable );
	}

	Library_FileList* search2;
	for(int s=0; s<lib_file_lists.size(); s++)
	{
		search2 = lib_file_lists[s];
		search2->fixExecutable( executable );
	}

}

LibraryFileFinder::LibraryFileFinder(const char* query_name)
{
	name = string(query_name);
}
void LibraryFileFinder::setPath(const char* path_arg)
{
	path = string(path_arg);
}

Library_FileList::Library_FileList(const char* query_name) : LibraryFileFinder(query_name) {}
Library_FileSearch::Library_FileSearch(const char* query_name) : LibraryFileFinder(query_name) {}

void LibraryFileFinder::findAndFixYourInterlibDeps()
{
	const int depamount = dependencies.size();
	for(int n=0; n<depamount; n++)
	{
		// find which LibraryFileFinder corresponds to this dependency
		string name = dependencies[n];
		
		bool success = false;
		const int amount1 = lib_file_searches.size();
		for(int s=0; s<amount1; s++)
		{
			if( lib_file_searches[s] -> name.compare(name) == 0 )
			{
				// found it
				lib_file_searches[s] -> fixLibThatDependsOnYou(this);
				success = true;
				break;
			};
		}
		if(success) continue;
		
		const int amount2 = lib_file_lists.size();
		for(int s=0; s<amount2; s++)
		{
			if( lib_file_lists[s] -> name.compare(name) == 0 )
			{
				// found it
				lib_file_lists[s] -> fixLibThatDependsOnYou(this);
				success = true;
				break;
			};
		}
		if(success) continue;
		else
		{
			cerr << "\n\nError : Could not find any entry matching dependency " << name << endl;
			exit(1);
		}
	}//next dependency	
}

void LibraryFileFinder::addDependencies(const char* depend)
{
	//dependencies.push_back( string(depend) );
	tokenize( string(depend), ":", &dependencies );
}

void LibraryFileFinder::checkFindingsValidity()
{
	if(libraries.size()>1)
	{
		cerr << "\n\nError while searching for libraries of rule " << name << " : multiple files were found that match criteria.\n(";
		for(int n=0; n<libraries.size(); n++) cerr << libraries[n].path << libraries[n].filename << " ";
		cerr << ")\nPlease refine the criteria to fix the ambiguity." << endl;
		exit(1);
	}
	else if(libraries.size()<1)
	{
		cerr << "\n\nError while searching for libraries of rule " << name << " : no file matched criteria.\n";
		cerr << "Please check your config file or make sure file exists and its path correctly specified." << endl;
		exit(1);
	}	
}

void LibraryFileFinder::prepareLibs(bool copy, string dest_folder, bool override_files)
{
	checkFindingsValidity();
	
	cout << "\n* Entry : " << LibraryFileFinder::name << endl;
	cout << "     " << libraries[0].filename << " : main file" << endl;
	for(int n=0; n<symlinks.size(); n++)
	{
	    cout << "     " << symlinks[n].filename << " : symlink" << endl;	
	}
	
	cout << endl;
	
        prepareLibFile( copy, libraries[0], dest_folder, LibraryFileFinder::name, override_files );
        if(copy)  fixLibIdentity( libraries[0] );
}

void LibraryFileFinder::fixExecutable(string executable)
{
	checkFindingsValidity();
	
	fixLibDependency(libraries[0], libraries[0].new_filename, executable);
	const int samount = symlinks.size();
	for(int n=0; n<samount; n++)
	{
		fixLibDependency(symlinks[n], libraries[0].new_filename, executable);
	}
}

void LibraryFileFinder::fixLibThatDependsOnYou(LibraryFileFinder* lib)
{
	cout << "*" << lib -> name << " depends on " << name << endl;
	
	string libToFix = lib->libraries[0].new_path + lib->libraries[0].new_filename;
	
	fixLibDependency(libraries[0], libraries[0].new_filename, libToFix);
	const int samount = symlinks.size();
	for(int n=0; n<samount; n++)
	{
		fixLibDependency(symlinks[n], libraries[0].new_filename, libToFix);
	}
}

// ------------ file lists -----------
void Library_FileList::addLibName(const char* file)
{
	string path_to_use = string(path);
	
	// if absolute path is not specified, use search paths instead
	if(path.size() == 0)
	{
		cout << prefixes.size() << " paths in search paths" << endl;
		for(int n=0; n<prefixes.size(); n++)
		{
			cout << "checking for existance of " << prefixes[n] + string(file) << endl;
			if( fileExists( (prefixes[n] + string(file)).c_str() ) )
			{
				path_to_use = prefixes[n];
				break;
			}
		}
		if(path_to_use.size() == 0)
		{
			cerr << "\n\nError : Found no file named " << file << " in given search paths" << endl;
			exit(1);
		}
	}
	else
	{
		path_to_use = path;
		if( path_to_use[ path_to_use.size()-1 ] != '/' ) path_to_use += "/";
	}
	
	libraries.push_back( Library( path_to_use, string(file) ) );
}
void Library_FileList::addSymlinkName(const char* names)
{
	string path_to_use = path;
	if( path_to_use[ path_to_use.size()-1 ] != '/' ) path_to_use += "/";
	
	vector<string> symlinks_tokens;
	tokenize( string(names), ":", &symlinks_tokens );

	for(int n=0; n<symlinks_tokens.size(); n++)
	symlinks.push_back( Library( "" /* the main lib has the path, not necessary for symlinks */,
							 symlinks_tokens[n] ) );
}

// ------------ library search -----------
void Library_FileSearch::startsWith(const char* s) { Library_FileSearch::startsWith_s = string(s); }
void Library_FileSearch::endsWith(const char* s) { Library_FileSearch::endsWith_s = string(s); }
void Library_FileSearch::contains(const char* s) { Library_FileSearch::contains_s = string(s); }

void Library_FileSearch::doesntContain(const char* s)
{
	tokenize( string(s), ":", &doesntContain_s );
}

void Library_FileSearch::setRegex(const char* s) { Library_FileSearch::regex = string(s); }

bool Library_FileSearch::checkLib(Library& lib)
{
	
	if(startsWith_s.size() > 0)
	{
		if( lib.filename.substr( 0 , startsWith_s.size()).compare(startsWith_s) != 0 )
		{
			//std::cout << lib.filename << " rejected because it doesn't begin with " << startsWith_s << std::endl;
			return false;
		}
	}

	if(endsWith_s.size() > 0)
	{
		if( lib.filename.substr( lib.filename.size()-endsWith_s.size(), lib.filename.size()-1).compare(endsWith_s) != 0 )
		{
			//std::cout << lib.filename << " rejected because it doesn't end with " << endsWith_s << std::endl;
			return false;
		}
	}
	
	if(contains_s.size() > 0)
	{
		if( lib.filename.find(contains_s, 0) == std::string::npos )
		{
			//std::cout << lib.filename << " rejected because it doesn't contain " << contains_s << std::endl;
			return false;
		}
	}
	
	const int dncsize = doesntContain_s.size();
	if(dncsize > 0)
	{
		for(int n=0; n<dncsize; n++)
		{
			if( lib.filename.find(doesntContain_s[n], 0) != std::string::npos )
			{
				//std::cout << lib.filename << " rejected because it contains " << doesntContain_s << std::endl;
				return false;
			}
		}
	}
	
	lib.accept();
	
	if(lib.symlink) symlinks.push_back( Library(lib) );
	else libraries.push_back( Library(lib) );
	
	return true;
}
