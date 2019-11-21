#include "LoadDae.h"

///////////////////////////////////////////////////
// Forward Declarations
void splitIntoVector(std::vector<glm::vec3>& targetVector, std::string values);
void splitIntoVector(std::vector<glm::vec4>& targetVector, std::string values);
DaeData processDaeData(
	std::vector<glm::vec3> tmpPositions,
	std::vector<glm::vec3> tmpNormals,
	std::vector<glm::vec4> tmpColours,
	std::vector<unsigned int> positionIndices,
	std::vector<unsigned int> normalIndices, 
	std::vector<unsigned int> colourIndices);


void loadDae(Model& model) {
	///////////////////////////////////////////////////
	// 1. Read file
	std::string line;
	std::ifstream objFile(model.path, std::ios::binary);

	std::vector<glm::vec3> tmpVertices;
	std::vector<glm::vec3> tmpNormals;
	std::vector<glm::vec4> tmpColours;

	std::vector<std::string> indexNames;
	std::vector<unsigned int> vertexIndices, normalIndices, colourIndices;

	bool readIndices = false; // Flag to detect when indices will be available to read in
	int npos = std::string::npos;

	if (objFile.is_open()) {
		while (getline(objFile, line)) {			
			/*
			Only interested in detecting:
				<float_array
				<triangles
				</triangles>
				<input
				<p>
			*/

			// VERTICES ////////////////////////////////////////////////////
			if (line.find("<float_array") != npos) {
				int floatStart = line.find_first_of(">") + 1;
				int floatEnd = line.find_last_of("<");
				std::string values = std::string(line.c_str() + floatStart, line.c_str() + floatEnd);

				int asda = std::string::npos;

				if (line.find("positions-array") != std::string::npos) {
					splitIntoVector(tmpVertices, values);
				}
				else if (line.find("normals-array") != std::string::npos) {
					splitIntoVector(tmpNormals, values);
				}
				else if (line.find("colour") != std::string::npos || line.find("color") != std::string::npos) {
					splitIntoVector(tmpColours, values);
				}
			}
			/////////////////////////////////////////////////////////////////
			else if (line.find("<triangles") != npos) {
				readIndices = true;
			}
			// INDICES //////////////////////////////////////////////////////
			else if (line.find("<input") != npos && readIndices) {
				std::stringstream ss(line);
				std::string token;
				char delim = ' ';
					
				// create vector of index names
				while (std::getline(ss, token, delim)) {
					if (token.find("semantic") != npos) {
						int start = token.find_first_of("\"") + 1;
						int end = token.find_last_of("\"");
						indexNames.push_back(std::string(token.c_str() + start, token.c_str() + end));
					}
				}
			}
			else if (line.find("<p>") != npos && readIndices) {
				// process index values
				int start = line.find_first_of(">") + 1;
				int end = line.find_last_of("<");
				std::string valueString = std::string(line.c_str() + start, line.c_str() + end);

				std::stringstream ss(valueString);
				std::string token;
				char delim = ' ';

				std::vector<unsigned int> tmpVector1, tmpVector2, tmpVector3; // generic names as their value types (pos/uv/norm) are not yet known
				int counter = 0;

				while (std::getline(ss, token, delim)) {
					if (counter == 0)
						tmpVector1.push_back(std::stoi(token));
					else if (counter == 1)
						tmpVector2.push_back(std::stoi(token));
					else if (counter == 2)
						tmpVector3.push_back(std::stoi(token));

					if (counter == 2)
						counter = 0;
					else
						counter++;
				}

				// assign generic vectors to their appropriate types
				if (indexNames[0].compare("VERTEX") == 0)
					vertexIndices = tmpVector1;
				else if (indexNames[0].compare("COLOR") == 0 || indexNames[0].compare("COLOUR") == 0)
					colourIndices = tmpVector1;
				else if (indexNames[0].compare("NORMAL") == 0)
					normalIndices = tmpVector1;

				if (indexNames[1].compare("VERTEX") == 0)
					vertexIndices = tmpVector1;
				else if (indexNames[1].compare("COLOR") == 0 || indexNames[1].compare("COLOUR") == 0)
					colourIndices = tmpVector2;
				else if (indexNames[1].compare("NORMAL") == 0)
					normalIndices = tmpVector2;

				if (indexNames[2].compare("VERTEX") == 0)
					vertexIndices = tmpVector3;
				else if (indexNames[2].compare("COLOR") == 0 || indexNames[2].compare("COLOUR") == 0)
					colourIndices = tmpVector3;
				else if (indexNames[2].compare("NORMAL") == 0)
					normalIndices = tmpVector3;

				/////////////////////////////////////////////////////////////////
				else if (line.find("</triangles>") != npos) {
					readIndices = false;
				}
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


void splitIntoVector(std::vector<glm::vec3>& targetVector, std::string values) {
	std::stringstream posStream(values);
	std::string token;
	char delim = ' ';

	glm::vec3 tmpVector = glm::vec3(0.0f, 0.0f, 0.0f);
	int counter = 0;

	while (getline(posStream, token, delim)) {
		if (!tmpVector.x)
			tmpVector.x = std::stof(token);
		else if (!tmpVector.y)
			tmpVector.y = std::stof(token);
		else if (!tmpVector.z)
			tmpVector.z = std::stof(token);

		counter++;

		if (counter == 3) {
			// push to vector if stride length is reached
			targetVector.push_back(tmpVector);
			
			tmpVector = glm::vec3(0.0f, 0.0f, 0.0f);			
			counter = 0;
		}
	}
}

void splitIntoVector(std::vector<glm::vec4>& targetVector, std::string values) {
	std::stringstream posStream(values);
	std::string token;
	char delim = ' ';

	glm::vec4 tmpVector = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	int counter = 0;

	while (getline(posStream, token, delim)) {
		if (!tmpVector.x)
			tmpVector.x = std::stof(token);
		else if (!tmpVector.y)
			tmpVector.y = std::stof(token);
		else if (!tmpVector.z)
			tmpVector.z = std::stof(token);
		else if (!tmpVector.a)
			tmpVector.a = std::stof(token);

		counter++;

		if (counter == 4) {
			// push to vector if stride length is reached
			targetVector.push_back(tmpVector);

			tmpVector = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			counter = 0;
		}
	}
}

DaeData processDaeData(
	std::vector<glm::vec3> tmpVertices,
	std::vector<glm::vec3> tmpNormals,
	std::vector<glm::vec4> tmpColours,
	std::vector<unsigned int> vertexIndices,  
	std::vector<unsigned int> normalIndices,
	std::vector<unsigned int> colourIndices)
{
	DaeData daeData;

	// process position data
	for (int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		daeData.vertices.push_back(tmpVertices[vertexIndex]);
	}

	// process uv data
	for (int i = 0; i < colourIndices.size(); i++) {
		unsigned int colourIndex = colourIndices[i];
		daeData.colour.push_back(tmpColours[colourIndex]);
	}

	// process normal data
	for (int i = 0; i < normalIndices.size(); i++) {
		unsigned int normalIndex = normalIndices[i];
		daeData.normals.push_back(tmpNormals[normalIndex]);
	}
}