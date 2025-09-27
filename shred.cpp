#include <iostream>
#include <fstream>
#include <random>
#include <cstdio>
#include <string>
#include <vector>
#include <io.h>
#include <windows.h> 
#include <direct.h> 




std::string random_name_generate(size_t len) {
	std::string result = "";
	char letters[] = "qwertyuiopasdfghjklzxcvbnm1234567890";
	for (int i = 0; i < len; i++) {
		result += letters[rand() % 36];
	}
	return result;
}


std::string file_rename(std::string& path)
{
	std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
	if (!file) {
		std::cout << "<!> ERROR File " << path << " didn't open\n";
		return "Error";
	}

	size_t pos = path.find_last_of("/\\");
	std::string directory = (pos == std::string::npos) ? "" : path.substr(0, pos + 1);
	std::string old_name = (pos == std::string::npos) ? path : path.substr(pos + 1);

	size_t ext_pos = old_name.find_last_of('.');
	std::string name_part = (ext_pos == std::string::npos) ? old_name : old_name.substr(0, ext_pos);
	std::string extension = (ext_pos == std::string::npos) ? "" : old_name.substr(ext_pos);

	std::string new_name = directory + random_name_generate(name_part.length()) + extension;

	file.close();

	if (std::rename(path.c_str(), new_name.c_str()) == 0) {
		std::cout << "File renamed in: " << new_name << std::endl;
		path = new_name;
	}
	else {
		perror("<!> Error rename");
	}
	return new_name;
}

void shred(std::string& path, int count = 7) {
	std::setlocale(LC_ALL, "ru");
	std::string renamed_path = file_rename(path);
	if (renamed_path == "Error") {
		std::cout << "Error";
		return;
	}
	std::fstream file(renamed_path, std::ios::binary | std::ios::in | std::ios::out);
	if (!file) {
		std::cout << "<!> ERROR File " << renamed_path << " didn't open\n";
		
		return ;
	}
	file.seekg(0, std::ios::end); 
	auto length_file = file.tellg(); 
	file.seekg(0, std::ios::beg);
	for (int c = 0; c < count; c++) {
		file.seekg(0, std::ios::beg);
		std::random_device rd;                   
		std::mt19937 gen(rd());                   
		std::uniform_int_distribution<> dist(0, 255);   

		std::vector<char> buffer(length_file * 2);

		for (int i = 0; i < length_file; ++i) {
			int random_byte = dist(gen);
			sprintf_s(buffer.data() + i * 2, 3, "%02x", random_byte);  	
		}
		buffer[length_file * 2] = '\0';
		file.seekp(0);
		file.write(buffer.data(), buffer.size());
		file.flush();

		FILE* f = nullptr;
		if (fopen_s(&f, renamed_path.c_str(), "rb+") == 0) {
			_commit(_fileno(f));
			fclose(f);
		}
		else {
			std::cout << "Error open file";
		}
		
	}
	file.close();
	const char* renamed_path_char = renamed_path.c_str();
	remove(renamed_path_char);

}

void help() {
	std::fstream file("message.txt", std::ios::in);
	if (!file) {
		std::cout << "<!> ERROR File " << "message.txt" << " didn't open\n";
		return;
	}
	std::string s;
	while (std::getline(file, s)) {
		std::cout << s << std::endl;
	}
	file.close();
}



void dir(char* dirs[], int len, int c, char path[]) {
	std::cout << "<<Start remove dir(s)>>" << std::endl;
	std::string path_dir = (std::string)path;
	std::string path_to_dir;
	for (int i=2; i < len - 2; i++){
		std::cout << path_dir + '\\' + dirs[i]<< std::endl;
	}
}

void file(char* files[], int len, int c, char path[]) {
	std::cout << "<<Start remove files>>" << std::endl;
	std::string path_dir = (std::string)path;
	std::string path_to_file;
	std::string file_name;
	for (int i = 2; i < len - 2; i++) {
		file_name = (std::string)files[i];
		path_to_file = path_dir + '\\' + file_name;
		std::cout << path_to_file << std::endl;
	}
}


int main(int argc, char* argv[])
{
	char buffer[FILENAME_MAX];
	if (_getcwd(buffer, sizeof(buffer)) == nullptr) { std::wcerr << "Error when getting the current directory\n"; }
	try {
		if (argc < 2) {
			std::cout << "<!> Error. Not enough arguments." << std::endl;
			return 1;
		}
		std::string func = argv[1];
		int co = 0;
		if (std::strcmp(argv[argc - 2], "-c") == 0) { 
			co = std::stoi(argv[argc - 1]);
		}
		if (func == "-help") { help(); }
		else if (func == "-d") { dir(argv, argc, co, buffer); }
		else if (func == "-f") { file(argv, argc, co, buffer); }
		else {
			std::cout << "Unknown option." << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cout << "<!> Error: " << e.what() << std::endl;
	}
		
	
}