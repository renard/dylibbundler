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


#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "irrXML.h"
using namespace irr;
using namespace io;

/*
 *
 * -x, --fix-executable <executable file>
 * -l, --fix-libraries
 * -c, --config <config file>
 * -d, --dest-folder <directory>
 * -of, --overwrite-files
 * -od, --overwrite-dir
 * -cd, --create-dir
 * -p, --install-path <libraries install path>
 * -h, --help
 *
 */

const string VERSION = "0.1.2";

#include "Library.h"
#include "Utils.h"
#include "LibraryFileFinder.h"

// FIXME - no memory management is done at all (anyway the program closes immediately so who cares?)

bool fixEx = false;
bool fixLibs_bool = false;
bool overwrite_files = false;
bool overwrite_dir = false;
bool create_dir = false;

bool using_searches = false;

string config = "./dylibbundler.config";
string destfolder = "./libs/";

string executable;
string search_path = "/usr/local/lib/";
string installPath = "";


void showHelp()
{
	cout << "dylibbundler " << VERSION << endl;
	cout << "dylibbundler is a utility that helps bundle dynamic libraries inside mac OS X app bundles.\n" << endl;
	
	cout << "-x, --fix-executable <executable file>" << endl;
	cout << "-l, --fix-libraries" << endl;
	cout << "-c, --config <config file>" << endl;
	cout << "-d, --dest-folder <directory>" << endl;
	cout << "-of, --overwrite-files" << endl;
	cout << "-od, --overwrite-dir" << endl;
	cout << "-cd, --create-dir" << endl;
	cout << "-p, --install-path <libraries install path>" << endl;
	cout << "-h, --help" << endl;
}

int main (int argc, char * const argv[])
{
	
	// parse arguments	
	for(int i=0; i<argc; i++)
	{
		if(strcmp(argv[i],"-x")==0 or strcmp(argv[i],"--fix-executable")==0)
		{
			fixEx = true;
			i++;
			executable = argv[i];
			continue;
		}
		if(strcmp(argv[i],"-l")==0 or strcmp(argv[i],"--fix-libraries")==0)
		{
			fixLibs_bool = true;
			continue;	
		}
		if(strcmp(argv[i],"-c")==0 or strcmp(argv[i],"--config")==0)
		{
			i++;
			config = argv[i];
			continue;
		}
		if(strcmp(argv[i],"-d")==0 or strcmp(argv[i],"--dest-folder")==0)
		{
			i++;
			destfolder = argv[i];
			continue;
		}
		if(strcmp(argv[i],"-p")==0 or strcmp(argv[i],"--install-path")==0)
		{
			i++;
			installPath = argv[i];
			continue;
		}
		if(strcmp(argv[i],"-of")==0 or strcmp(argv[i]," --overwrite-files")==0)
		{
			overwrite_files = true;
			continue;	
		}
		if(strcmp(argv[i],"-od")==0 or strcmp(argv[i],"--overwrite-dir")==0)
		{
			overwrite_dir = true;
			continue;	
		}
		if(strcmp(argv[i],"-cd")==0 or strcmp(argv[i],"--create-dir")==0)
		{
			create_dir = true;
			continue;	
		}
		if(strcmp(argv[i],"-h")==0 or strcmp(argv[i],"--help")==0)
		{
			showHelp();
			exit(0);	
		}
	}
	
	if(not fixEx and not fixLibs_bool)
	{
		showHelp();
		exit(0);
	}
	

	// ---------------------------------- reading config file -----------------------
	cout << "* reading config file..." << endl;
	IrrXMLReader* xml = createIrrXMLReader(config.c_str());
	
	if(xml == NULL)
	{
		cerr << "\n\nError : Cannot open config file" << endl;
		exit(0);
	}
	
	bool read_paths = false;
	
	// parse the file until end reached
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
			// read <x> text between opening and closing tags </x>
			case EXN_TEXT:
				
				if(read_paths)
				{
					search_path = xml->getNodeData();
					std::cout << "found search path : " << search_path << std::endl;
				}
				break;
				
			case EXN_ELEMENT_END:
			{
				if (!strcmp("search_path", xml->getNodeName()))
				{
					read_paths = false;
				}
				break;
			}
			case EXN_ELEMENT:
			{
				if (!strcmp("search_path", xml->getNodeName()))
				{
					read_paths = true;
				}
				
				if (!strcmp("library", xml->getNodeName()))
				{
					// common
					const char* name = xml->getAttributeValue("name");
					const char* dependencies = xml->getAttributeValue("dependencies");
					
					// list
					const char* file = xml->getAttributeValue("file");
					const char* symlinks = xml->getAttributeValue("symlinks");
					const char* path = xml->getAttributeValue("path");
					
					// search
					const char* starts_with = xml->getAttributeValue("starts_with");
					const char* ends_with = xml->getAttributeValue("ends_with");
					const char* contains = xml->getAttributeValue("contains");
					const char* doesnt_contain = xml->getAttributeValue("doesnt_contain");
					
					if(name == NULL)
					{
						cerr << "\n\nError : 'name' attribute is mandatory in library description." << endl;
						exit(1);
					}
					

					bool file_search = (file == NULL);
					
					Library_FileSearch* search;
					Library_FileList* list;
					
					if(file_search)
					{
						if(starts_with == NULL and ends_with == NULL and contains == NULL and doesnt_contain == NULL)
						{
							cerr << "\n\nError : not enough information provided in config file for where/how to find library " << name << endl;
							exit(1);
						}
						
						search = new Library_FileSearch(name);
						if(starts_with != NULL) search -> startsWith(starts_with);
						if(ends_with != NULL) search -> endsWith(ends_with);
						if(contains != NULL) search -> contains(contains);
						if(doesnt_contain != NULL) search -> doesntContain(doesnt_contain);
						
						if(dependencies != NULL) search -> addDependencies( dependencies );
						pushBackSearch(search);
						
						using_searches = true;
					}
					else
					{
						list = new Library_FileList(name);
						
						if(file == NULL)
						{
							cerr << "\n\nError : not enough information provided in config file for where/how to find library " << name  << endl;
							exit(1);
						}
						if(path == NULL)
						{
							cerr << "\n\nError : not enough information provided in config file for where/how to find library " << name  << endl;
							exit(1);
						}
						
						list -> addLibName(path, file);
						if(symlinks != NULL) list -> addSymlinkName(path, symlinks);
						
						list -> addDependencies( dependencies );
						pushBackList(list);
					}

					
				}
			}
				break;
		}
	}
	
	// delete the xml parser after usage
	delete xml;
	
	cout << "* reading libraries..." << endl;
	addPrefix(search_path);
	
	if(using_searches) startSearch();
	
	if(installPath.size() > 0) setInstallPath(installPath);
	
	if(fixLibs_bool) fixLibs(destfolder, create_dir, overwrite_files, overwrite_dir);
	if(fixEx) fixExecutable(executable);
	
	
	/*
	addPrefix("/usr/local/lib/");
	
	{
		Library_FileSearch* cond = new Library_FileSearch("libglib");
		cond -> startsWith("lib");
		cond -> endsWith(".dylib");
		cond -> contains("glib");
		cond -> doesntContain("glibmm");
		pushBackSearch(cond);
	}
	
	{
		Library_FileSearch* cond = new Library_FileSearch("libglibmm");
		cond -> startsWith("lib");
		cond -> endsWith(".dylib");
		cond -> contains("glibmm");
		cond -> doesntContain("extra");
		cond -> addDependencies("libglib");
		pushBackSearch(cond);
	}
	
	{
		Library_FileSearch* cond = new Library_FileSearch("libglibmm_generate_extra_defs");
		cond -> startsWith("lib");
		cond -> endsWith(".dylib");
		cond -> contains("glibmm_generate_extra_defs");
		pushBackSearch(cond);
	}

	{
		Library_FileSearch* cond = new Library_FileSearch("libsdl");
		cond -> startsWith("lib");
		cond -> endsWith(".dylib");
		cond -> contains("SDL");
		cond -> doesntContain("SDL_");
		pushBackSearch(cond);
	}
	
	{
		Library_FileList* cond = new Library_FileList("libgmodule");
		cond -> addLibName("/usr/local/lib/","libgmodule-2.0.0.1200.13.dylib");
		cond -> addSymlinkName("/usr/local/lib/","libgmodule-2.0.dylib");
		cond -> addSymlinkName("/usr/local/lib/","libgmodule-2.0.0.dylib");
		pushBackSearch(cond);
	}
	*/
	//startSearch();
	//fixLibs("./libs", true /*create*/, true /*override files*/, false /*override dir*/);
	//fixExecutable("./wormux");
	
    return 0;
}
