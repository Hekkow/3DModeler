#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int windowWidth = 800;
int windowHeight = 800;
const char* windowName = "3D Modeler";

// vertex shaders i believe are for setting the positions
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
// fragment shaders i believe are for setting the colors
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1, 1, 1, 1.0f);\n"
"}\n\0";

int main() 
{
	glfwInit();
	
	// these hints connect glfw to opengl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

	// vertices, each group of 3 is an x, y, and z
	GLfloat vertices[] = {
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0,
		0.5f, -0.5f * float(sqrt(3)) / 3, 0,
		0, 0.5f * float(sqrt(3)) * 2 / 3, 0
	};

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, windowWidth, windowHeight);

	// shader creation and connection
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// vbo is 3d objects, vao is list of vbos
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// dynamic is for multiple vertex modifications, change to that later
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glClearColor(0.07f, 0.13f, 0.17f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	// two buffers, one for writing pixels and one for reading pixels. after the writing buffer is done writing what the next frame will look like,
	// we switch the two buffers to display the written one and start writing the next frame
	glfwSwapBuffers(window);
	while (!glfwWindowShouldClose(window)) {
		// every frame sets the background color, sets shaders, sets shapes and draws them to buffer, then displays them
		glClearColor(0.07f, 0.13f, 0.17f, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		// accepts inputs such as resizing window
		glfwPollEvents();
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}