#include <iostream>
#include <fstream>
#include <random>
#include <cstdio>
#include <string>
#include <vector>
#include <io.h>



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
	std::setlocale(LC_ALL, "ru");
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
		std::random_device rd;                    // Источник энтропии (аппаратный)
		std::mt19937 gen(rd());                   // Генератор псевдослучайных чисел, инициализируемый rd
		std::uniform_int_distribution<> dist(0, 255);   // Равномерное распределение от 0 до 255

		std::vector<char> buffer(length_file * 2);

		for (int i = 0; i < length_file; ++i) {
			int random_byte = dist(gen);
			std::sprintf(buffer.data() + i * 2, "%02x", random_byte);	
		}
		buffer[length_file * 2] = '\0';
		file.seekp(0);
		file.write(buffer.data(), buffer.size());
		file.flush();

		FILE* f = fopen(renamed_path.c_str(), "rb+");
		if (f) {
			_commit(_fileno(f));
			fclose(f);
		}
		
	}
	file.close();
	const char* renamed_path_char = renamed_path.c_str();
	remove(renamed_path_char);

}

int main()
{
	std::string file_path;
	std::cin >> file_path;
	shred(file_path);
}