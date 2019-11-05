#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


/*******************************************************
FILETYPES:

.obj - Wavefront OBJ & MTL
.dae - Collada DAE
.fbx - Autodesk FBX (proprietary)
.3ds - Autodesk 3DS (proprietary)

********************************************************/


void printWelcomeAscii();

bool loadObj(
	const std::string path,
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs, 
	std::vector<glm::vec3>& normals
);

bool display(
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals
);


const int DEFAULT_SCR_WIDTH = 800;
const int DEFAULT_SCR_HEIGHT = 600;

int main()
{
	// BEGIN DEBUG
	std::string modelPath = "Creeper.obj";
	// END DEBUG

	///////////////////////////////////////////////////
	// Get user input (file/folder directory)
	printWelcomeAscii();
	std::cout << "Enter the location of a model file or folder of models to continue..." << std::endl;
	std::string filePath; std::cin >> modelPath;

	///////////////////////////////////////////////////
	// Read Model
	std::regex pattern(".[a-z0-9]+$", std::regex_constants::icase);
	std::smatch fileExtension;
	std::regex_search(modelPath, fileExtension, pattern);
	
	std::vector<glm::vec3> vertices, normals;
	std::vector<glm::vec2> uvs;

	if (fileExtension[0] == ".obj") {
		loadObj(modelPath, vertices, uvs, normals); // TODO: check return value for this
	} 
	else if (fileExtension[0] == ".dae") {
		// TODO: load a .dae file
		// loadDae(modelPath);
	}
	else if (fileExtension[0] == ".fbx") {
		// TODO: load a .fbx file
		// loadFbx(modelPath);
	}
	else if (fileExtension[0] == ".3ds") {
		// TODO: load a .3ds file
		// load3ds(modelPath);
	}
	else {
		std::cout << std::endl;
		std::cout << "ERROR->" << __FUNCTION__ << ": Unsupported file type" << std::endl;
		std::cout << "Try using the supported file types:" << std::endl;
		std::cout << "  - .obj" << std::endl;
	}

	display(vertices, uvs, normals);
}


bool loadObj(
	const std::string path,					// model.obj filepath
	std::vector<glm::vec3>& out_vertices,	// ojb data vectors to be output
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
)
{
	std::vector<glm::vec3> tmpVertices;
	std::vector<glm::vec2> tmpUvs;
	std::vector<glm::vec3> tmpNormals;
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

	///////////////////////////////////////////////////
	// 1. Read file
	std::string line;
	std::ifstream modelFile(path, std::ios::binary);

	if (modelFile.is_open()) {
		while (std::getline(modelFile, line)) {
			// TODO: 2. Validate correct format
			// only read strings if file is in plaintext encoding

			///////////////////////////////////////////////////
			// 3. Create data structures
			std::string objDataType = line.substr(0, line.find(" "));
			
			if (objDataType == "v")	{				
				std::istringstream sstr(line.substr(2));
				glm::vec3 vertex;
				sstr >> vertex.x;
				sstr >> vertex.y;
				sstr >> vertex.z;
				tmpVertices.push_back(vertex);
			}
			else if (objDataType == "vt") {
				std::istringstream sstr(line.substr(3));
				glm::vec2 vertex;
				sstr >> vertex.x;
				sstr >> vertex.y;
				tmpUvs.push_back(vertex);
			}
			else if (objDataType == "vn") {
				std::istringstream sstr(line.substr(3));
				glm::vec3 vertex;
				sstr >> vertex.x;
				sstr >> vertex.y;
				sstr >> vertex.z;
				tmpNormals.push_back(vertex);
			}
			else if (objDataType == "f") {
				std::string buf;
				std::stringstream lineStream(line.substr(2));
								
				while (lineStream >> buf) {
					std::string token;
					std::stringstream faceStream(buf);
					std::vector<std::string> faceElements;					

					while (getline(faceStream, token, '/')) {
						faceElements.push_back(token);					
					}

					vertexIndices.push_back(std::stoi(faceElements[0]));
					uvIndices.push_back(std::stoi(faceElements[1]));
					normalIndices.push_back(std::stoi(faceElements[2]));

					faceElements.clear();
				}
			}
		}

		///////////////////////////////////////////////////
		// 4. Process data

		// Process Vertices
		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			unsigned int vertexIndex = vertexIndices[i];
			glm::vec3 vertex = tmpVertices[vertexIndex - 1];
			out_vertices.push_back(vertex);
		}

		// Process UVs
		for (unsigned int i = 0; i < uvIndices.size(); i++) {
			unsigned int uvIndex = uvIndices[i];
			glm::vec2 uv = tmpUvs[uvIndex- 1];
			out_uvs.push_back(uv);
		}

		// Process Normals
		for (unsigned int i = 0; i < normalIndices.size(); i++) {
			unsigned int normalIndex = normalIndices[i];
			glm::vec3 normal = tmpNormals[normalIndex - 1];
			out_normals.push_back(normal);
		}
	}
	else {
		std::cout << std::endl;
		std::cout << "ERROR->" << __FUNCTION__ << ": Unable to open file, file may not exist or be corrupt" << std::endl;
		return false;
	}

	///////////////////////////////////////////////////
	// 5. Deallocate resources

	return true;
}


bool display(
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals)
{
	return true;
}

void printWelcomeAscii() {
	std::cout << R"(
	##################################################################################################
	#                                                                                                #
	#          ,___          .-;'                                                                    #
	#         `"-.`\_...._/`.`                                                                       #
	#      ,      \        /      __  __           _      _   _                     _                #
	#   .-' ',    / ()   ()\     |  \/  |         | |    | | | |                   | |               #
	#  `'._   \  /()    .  (|    | \  / | ___   __| | ___| | | |     ___   __ _  __| | ___ _ __      #
	#      > .' ;,     -'-  /    | |\/| |/ _ \ / _` |/ _ \ | | |    / _ \ / _` |/ _` |/ _ \ '__|     #
	#     / <   |;,     __.;     | |  | | (_) | (_| |  __/ | | |___| (_) | (_| | (_| |  __/ |        #
	#     '-.'-.|  , \    , \    |_|  |_|\___/ \__,_|\___|_| |______\___/ \__,_|\__,_|\___|_|        #
	#        `>.|;, \_)    \_)                                                                       #
	#         `-;     ,    /                                                                         #
	#            \    /   <                                                                          #
	#             '. <`'-,_)                                                                         #
	#          jgs '._)                                                                              #
	#                                                                                                #
	##################################################################################################
	)" << '\n';
}