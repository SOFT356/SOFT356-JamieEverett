#include "LoadDae.h"

void loadDae(Model& model) {
	///////////////////////////////////////////////////
	// 1. Read file
	std::string line;
	std::ifstream objFile(model.path, std::ios::binary);

	if (objFile.is_open()) {
		while (std::getline(objFile, line)) {
			std::string xmlTag = line.substr(0, line.find(" "));

			if (strcmp(xmlTag.c_str(), "<float_array")) {

			}
		}
	}
	else {
		std::cout << std::endl;
		std::cout << "ERROR->" << __FUNCTION__ << ": Unable to open dae file, the file may not exist or be corrupt" << std::endl;
	}


	///////////////////////////////////////////////////
	// 2. Create data structures
}