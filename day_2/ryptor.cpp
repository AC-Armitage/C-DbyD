#include <iostream>
#include <fstream>
#include <filesystem>

void Encrypt(unsigned int Key, std::string line, std::string Ofile){
	
	std::ofstream outputFile(Ofile, std::ios::app);
        if (!outputFile.is_open()){ 
		std::cerr << "Failt to write output" << std::endl;
        }
	for (int i = 0; line[i] != '\0'; i++){
		char Char = line[i];
		if (std::isalpha(Char)){
			if (Char >= 'a' && Char <= 'z'){
				Char = Char + Key;
				if ( Char > 'z'){
					Char = Char - 'z' + 'a' - 1;
				}

			} else if (Char >= 'A' && Char <= 'Z'){
				Char = Char + Key;
                                if ( Char > 'Z'){
                                        Char = Char - 'Z' + 'A' - 1;
                                }
			}
		}
		outputFile << Char;
	}
	outputFile << '\n';
	outputFile.close();
}

void Decrypt(unsigned int Key, std::string line, std::string Ofile){
	std::ofstream outputFile(Ofile, std::ios::app);
	if (!outputFile.is_open()){
		std::cerr << "Faild to write output" << std::endl;
	}
	for (int i = 0; line[i] != '\0'; i++){
		char Char = line[i];
		if (std::isalpha(Char)){
			if (Char >= 'a' && Char <= 'z'){
				Char = Char - Key;
				if (Char < 'a'){
					Char = Char + 'z' - 'a' + 1; 
				}
			} else if (Char >= 'A' && Char <= 'Z'){
				Char = Char - Key;
				if (Char < 'A'){
					Char = Char + 'Z' - 'A' + 1;
				}
			}
		}
		outputFile << Char;
	
	}
	outputFile << '\n';
	outputFile.close();
}

int main(){

	unsigned int Key;
	std::string Ifile, Ofile;
	int chox;
	std::cout << "(1) Encrypt\n(2) Decrypt\nChose an operation: ";
	std::cin >> chox;
	while (chox != 1 && chox != 2){
		std::cout << "Wrong input please chose again: ";
		std::cin >> chox;
	}
	std::cout << "Enter positive shift value: ";
	std::cin >> Key;
	std::cout << "Enter Absulote path to input file: ";
	std::cin >> Ifile;
	std::cout << "Enter Absulote path to output file: ";
	std::cin >> Ofile;
	std::ifstream inputFile(Ifile);
	if (!inputFile.is_open()){
		std::cerr << "Faild to open the input file! " << std::endl;
		return 1;
	}
	std::string line;
	if (chox == 1){
		while (std::getline(inputFile, line)){
			Encrypt(Key, line, Ofile);
		}
	} else if (chox == 2){
		while (std::getline(inputFile, line)){
			Decrypt(Key, line, Ofile);
		}
	
	}
	inputFile.close();
	std::filesystem::path OutPath = std::filesystem::absolute(Ofile);
	std::cout << "Success\nhave been writing to: " << OutPath.string() << std::endl;
	return 0;
}
