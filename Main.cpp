
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
class Edge {
public:
	int index1;
	int index2;
	glm::vec3 vert1;
	glm::vec3 vert2;
	Edge(int index1, int index2) {
		Edge::index1 = index1;
		Edge::index2 = index2;
	}
};
std::vector<Edge> edges;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
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
	glEnable(GL_DEPTH_TEST);
	Camera camera(windowWidth, windowHeight, glm::vec3(0, 0, 10));
	//edges.push_back(Edge(6, 7));
	//edges.push_back(Edge(4, 5));
	//edges.push_back(Edge(5, 6));
	//edges.push_back(Edge(7, 4));
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	for (int i = 0; i < indices.size() - 1; i+=3) {
		std::cout << i << std::endl;
		edges.push_back(Edge(indices[i], indices[i + 1]));
		edges.push_back(Edge(indices[i], indices[i + 2]));
		edges.push_back(Edge(indices[i + 2], indices[i + 1]));
	}
	std::cout << "DONE " << std::endl;
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.07f, 0.13f, 0.17f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderProgram.Activate();
		popCat.Bind();
		
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "isEdge"), false);
		camera.Inputs(window);
		camera.Matrix(45, 0.1f, 100, shaderProgram, "camMatrix");
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glLineWidth(5.0f);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "isEdge"), true);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "edgeColor"), 1, 0, 0, 1);

		for (const Edge& edge : edges) {
			GLfloat lineVertices[] = {
				vertices[edge.index1 * verticesRowAmount], vertices[edge.index1 * verticesRowAmount + 1], vertices[edge.index1 * verticesRowAmount + 2],
				vertices[edge.index2 * verticesRowAmount], vertices[edge.index2 * verticesRowAmount + 1], vertices[edge.index2 * verticesRowAmount + 2],
			};

			GLuint lineVBO, lineVAO;
			glGenVertexArrays(1, &lineVAO);
			glGenBuffers(1, &lineVBO);
			glBindVertexArray(lineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glDrawArrays(GL_LINES, 0, 2);

			glDeleteVertexArrays(1, &lineVAO);
			glDeleteBuffers(1, &lineVBO);
		}
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
		unsigned char data[3];
		glReadPixels(xpos, windowHeight - ypos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
		int r = data[0] - 0;
		int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;
		std::cout << r << std::endl;
		//std::cout << +data[0] << " " << +data[1] << " " << +data[2] << std::endl;
	}
}