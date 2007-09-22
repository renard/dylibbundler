all:
	g++ -c -I./src ./src/FileList.cpp -o ./FileList.o
	g++ -c -I./src ./src/irrXML.cpp -o ./irrXML.o
	g++ -c -I./src ./src/Library.cpp -o ./Library.o
	g++ -c -I./src ./src/LibraryFileFinder.cpp -o ./LibraryFileFinder.o
	g++ -c -I./src ./src/main.cpp -o ./main.o
	g++ -c -I./src ./src/Utils.cpp -o ./Utils.o
	g++ -o ./dylibbundler ./FileList.o ./irrXML.o ./Library.o ./LibraryFileFinder.o ./main.o ./Utils.o
	rm *.o
	
install:
	cp ./dylibbundler /usr/local/bin/dylibbundler