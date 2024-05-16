
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"
float windowWidth = 800;
float windowHeight = 800;
const char* windowName = "3D Modeler";

int verticesRowAmount = 5;

// vertices, first three are vertex coordinates then last two are texture coordinates
std::vector<GLfloat> vertices =
{
	-0.5f, -0.5f, 0, 0, 0, // back left down
	-0.5f, 0.5f, 0, 0, 1, // back left up
	0.5f, 0.5f, 0, 1, 1, // back right up
	0.5f, -0.5f, 0, 1, 0, // back right down
	-0.5f, -0.5f, 1, 1, 0, // front left down
	-0.5f, 0.5f, 1, 1, 1, // front left up
	0.5f, 0.5f, 1, 0, 1, // front right up
	0.5f, -0.5f, 1, 0, 0, // front right down
};
std::vector<GLuint> indices = {
	0, 2, 1,
	0, 3, 2,
	4, 6, 5,
	4, 6, 7,
	7, 6, 3,
	6, 2, 3,
	4, 5, 0,
	0, 1, 5
};
VBO VBO1;
EBO EBO1;
VAO VAO1;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void addToVertices(float x, float y, float z);
void addToIndices(GLint index1, GLint index2, GLint index3);
void updates();
float distance(int index, float x, float y);
int* getClosestPointsIndices(float xpos, float ypos);
void pixelsToPercent(double* x, double* y);
int main() 
{
	glfwInit();
	
	// these hints connect glfw to opengl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

	

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, windowWidth, windowHeight);

	Shader shaderProgram("default.vert", "default.frag");
	VAO1.Init();
	VAO1.Bind();
	VBO1 = VBO(&vertices[0], vertices.size() * sizeof(vertices));

	EBO1 = EBO(&indices[0], indices.size() * sizeof(indices));
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, verticesRowAmount * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, verticesRowAmount * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();
	
	Texture popCat("popcat.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	popCat.texUnit(shaderProgram, "tex0", 0);

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glEnable(GL_DEPTH_TEST);
	Camera camera(windowWidth, windowHeight, glm::vec3(0, 0, 2));
	while (!glfwWindowShouldClose(window)) {
		// every frame sets the background color, sets shaders, sets shapes and draws them to buffer, then displays them
		glClearColor(0.07f, 0.13f, 0.17f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderProgram.Activate();
		popCat.Bind();
		camera.Inputs(window);
		camera.Matrix(45, 0.1f, 100, shaderProgram, "camMatrix");
		//glUniform1f(uniID, 0.5f);
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	popCat.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		pixelsToPercent(&xpos, &ypos);
		int* indexes = getClosestPointsIndices(xpos, ypos);
		addToVertices(xpos, ypos, 0);
		addToIndices(indexes[0], indexes[1], vertices.size() / 3 - 1);
		updates();
	}
}
void pixelsToPercent(double* x, double* y) {
	*x = (*x - windowWidth / 2) / (windowWidth / 2);
	*y = -(*y - windowHeight / 2) / (windowHeight / 2);
}
int* getClosestPointsIndices(float xpos, float ypos) {
	int closestIndex;
	int secondClosestIndex;
	if (distance(0, xpos, ypos) < distance(3, xpos, ypos)) {
		closestIndex = 0;
		secondClosestIndex = 3;
	}
	else {
		closestIndex = 3;
		secondClosestIndex = 0;
	}
	for (int i = 6; i < vertices.size(); i+=3) {
		if (distance(i, xpos, ypos) < distance(closestIndex, xpos, ypos)) {
			secondClosestIndex = closestIndex;
			closestIndex = i;
		}
		else if (distance(i, xpos, ypos) < distance(secondClosestIndex, xpos, ypos)) {
			secondClosestIndex = i;
		}
	}
	int* indexes = new int[2] {closestIndex/3, secondClosestIndex/3};
	return indexes;
}


float distance(int index, float x, float y) {
	return sqrt(pow(vertices[index] - x, 2) + pow(vertices[index+1] - y, 2));
}
void updates() {
	VAO1.Bind();

	VBO1.Bind();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VBO1.Unbind();
	VAO1.Unbind();

	EBO1.Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	EBO1.Unbind();
}
void addToVertices(float x, float y, float z) {
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
}
void addToIndices(GLint index1, GLint index2, GLint index3) {
	indices.push_back(index1);
	indices.push_back(index2);
	indices.push_back(index3);
}