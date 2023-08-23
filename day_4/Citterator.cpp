#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <cstdlib>
#include <cstring>
void Help(char* Arg){
	using namespace std;
	cout << "Usage:\n\t- " << Arg << "\t\t<word> <directory>\n\t" << "- help\t\t\tshow this manual" << endl;
}
int ReadFile(std::string TextFile, std::string Target, int Hits){

	std::ifstream File(TextFile);
	if (!File.is_open()){
		std::cerr << "Could not open file: " << TextFile << std::endl;
	}
	std::string Line;
	while (std::getline(File, Line)){
		std::istringstream Iss(Line);
		std::string Word;
		while (Iss >> Word){
			if (Word == Target){
				Hits++;
			}
		}
	}
	File.close();
	return Hits;
}
int IterateDirectory(std::string Directory, std::string Target, int Hits){
	for (const auto& Entry : std::filesystem::directory_iterator(Directory)){
		if (Entry.is_regular_file() && Entry.path().extension() == ".txt"){
			std::cout << "-Scanning " << Entry.path() << std::endl;
			Hits = ReadFile(Entry.path(), Target, Hits);
		}
	}
	return Hits;
}
int main(int argc, char* argv[]){
	int Hits = 0;
	if (argc > 3, argv[1] == "help"){
		Help(argv[0]);
		exit(0);
	}
	Hits = IterateDirectory(argv[2], argv[1], Hits);
	std::cout << "Scan finished\n" << argv[1] << " was found "<< Hits << " times" << std::endl;
	return 0;
}

