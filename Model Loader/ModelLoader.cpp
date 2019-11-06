#include "ModelLoader.h"
#include "LoadObj.h"
#include "Shader.h"


/*******************************************************
FILETYPES:

.obj - Wavefront OBJ & MTL
.dae - Collada DAE
.fbx - Autodesk FBX (proprietary)
.3ds - Autodesk 3DS (proprietary)

********************************************************/


///////////////////////////////////////////////////
// Forward Declarations
bool loadObj(
	const std::string path,
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs, 
	std::vector<glm::vec3>& normals);

void display(
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals);

int shadersInit();
int displayInit(std::vector<glm::vec3> vertices);
void onWindowResize(GLFWwindow* window, int width, int height);
void printWelcomeAscii();


///////////////////////////////////////////////////
// Global Vars (Default Program Behaviour)
const char* DEFAULT_SCR_TITLE = "JE - ModelLoader";
const int DEFAULT_SCR_WIDTH = 800;
const int DEFAULT_SCR_HEIGHT = 600;
bool firstLaunch = true;


int main()
{
	/*
	if (firstLaunch)
		printWelcomeAscii();
	 firstLaunch = false;

	///////////////////////////////////////////////////
	// Get user input (file/folder directory)
	std::cout << "Enter the location of a model file or folder of models to continue..." << std::endl;
	//std::cout << "(press 'q' to quit at any time)" << std::endl;  // TODO: this
	std::string modelPath; std::cin >> modelPath;
	*/
	std::string modelPath = "creeper.obj";

	///////////////////////////////////////////////////
	// Read Model
	std::regex pattern(".[a-z0-9]+$", std::regex_constants::icase);
	std::smatch fileExtension;
	std::regex_search(modelPath, fileExtension, pattern);
	
	std::vector<glm::vec3> vertices, normals;
	std::vector<glm::vec2> uvs;

	if (fileExtension[0] == ".obj") {
		if (!loadObj(modelPath, vertices, uvs, normals))
			exit(EXIT_FAILURE);
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

		// TODO: Need to ask for user input again here...
		exit(EXIT_FAILURE);
	}

	display(vertices, uvs, normals);

	// BEGIN DEBUG
	system("pause");
	// END DEBUG
}


void display(
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals)
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(DEFAULT_SCR_WIDTH, DEFAULT_SCR_HEIGHT, DEFAULT_SCR_TITLE, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, onWindowResize);

	glewInit();

	glEnable(GL_DEPTH_TEST);

	// Build and compile Shader program
	Shader shaders("shaders/shader.vs", "shaders/shader.fs");

	int VAO = displayInit(vertices);
	
	while (!glfwWindowShouldClose(window))
	{
		// TODO: input handling

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaders.use();

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, (GLint)vertices.size());
		//glDrawElements(GL_TRIANGLES, (GLint)vertices.size(), GL_FLOAT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}


int displayInit(std::vector<glm::vec3> vertices) {
	unsigned int VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// bind VAO
	glBindVertexArray(VAO);
	// bind + set VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	// configure vertex attribute(s)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// TODO: texture attr.

	return VAO;
}


void onWindowResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
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