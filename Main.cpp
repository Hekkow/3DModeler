
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
#include "SegmentDetection.h"
float windowWidth = 800;
float windowHeight = 800;
const char* windowName = "3D Modeler";

int verticesRowAmount = 5;

// vertices, first three are vertex coordinates then last two are texture coordinates
std::vector<GLfloat> vertices =
{
	-0.5f, -0.5f, -0.5f, 0, 0, // back left down
	-0.5f, 0.5f, -0.5f, 0, 1, // back left up
	0.5f, 0.5f, -0.5f, 1, 1, // back right up
	0.5f, -0.5f, -0.5f, 1, 0, // back right down
	-0.5f, -0.5f, 0.5f, 1, 0, // front left down
	-0.5f, 0.5f, 0.5f, 1, 1, // front left up
	0.5f, 0.5f, 0.5f, 0, 1, // front right up
	0.5f, -0.5f, 0.5f, 0, 0, // front right down
	
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

enum Mode {
	FACE = 1,
	EDGE = 2,
};
Mode currentMode = FACE;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void updateEdges();
Camera camera;
class Edge{
public:
	int index1;
	int index2;
	Edge(int i1, int i2) { 
		index1 = i1;
		index2 = i2;
	}
	bool operator==(Edge edge) {
		return (edge.index1 == index1 && edge.index2 == index2) || (edge.index1 == index2 && edge.index2 == index1);
	}
};
std::vector<int> highlightedFaces;
std::vector<Edge> highlightedEdges;
std::vector<GLuint> highlightedEdgeIndices;
std::vector<GLuint> nonhighlightedEdgeIndices;

bool moving = false;
double lastX = 0.0, lastY = 0.0;
bool firstMouse = true;

void Binds() {
	VAO1.Init();
	VAO1.Bind();
	VBO1 = VBO(&vertices[0], vertices.size() * sizeof(vertices));
	EBO1 = EBO(&indices[0], indices.size() * sizeof(indices));
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, verticesRowAmount * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, verticesRowAmount * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();
}
int find(std::vector<int> v, int finding) {
	for (int i = 0; i < v.size(); i++) {
		if (v[i] == finding) return i;
	}
	return -1;
}
int main() 
{
	glfwInit();
	
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
	Binds();
	
	Texture popCat("popcat.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	popCat.texUnit(shaderProgram, "tex0", 0);
	glEnable(GL_DEPTH_TEST);
	camera = Camera(windowWidth, windowHeight, glm::vec3(0, 0, 10));
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	updateEdges();
	GLuint highlightedLinesEBO, nonhighlightedLinesEBO;
	glGenBuffers(1, &highlightedLinesEBO);
	glGenBuffers(1, &nonhighlightedLinesEBO);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.07f, 0.13f, 0.17f, 1);
		
		shaderProgram.Activate();
		popCat.Bind();
		
		
		camera.Inputs(window);
		camera.Matrix(45, 0.1f, 100, shaderProgram, "camMatrix");

		VAO1.Bind();
		EBO1.Bind();
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "isEdge"), false);
		for (int i = 0; i < indices.size(); i++) {
			glUniform1i(glGetUniformLocation(shaderProgram.ID, "highlighted"), find(highlightedFaces, i) != -1);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(GLuint)));
		}

		// draw regular edges
		glClear(GL_DEPTH_BUFFER_BIT);
		glLineWidth(2);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "isEdge"), true);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "highlighted"), false);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nonhighlightedLinesEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nonhighlightedEdgeIndices.size() * sizeof(GLuint), nonhighlightedEdgeIndices.data(), GL_STATIC_DRAW);
		glDrawElements(GL_LINES, nonhighlightedEdgeIndices.size(), GL_UNSIGNED_INT, 0);

		// draw highlighted edges
		glClear(GL_DEPTH_BUFFER_BIT);
		glLineWidth(5);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "highlighted"), true);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, highlightedLinesEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, highlightedEdgeIndices.size() * sizeof(GLuint), highlightedEdgeIndices.data(), GL_STATIC_DRAW);
		glDrawElements(GL_LINES, highlightedEdgeIndices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	glDeleteBuffers(1, &highlightedLinesEBO);
	popCat.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
void updateEdges() {
	highlightedEdgeIndices.clear();
	nonhighlightedEdgeIndices.clear();
	for (int i = 0; i < indices.size(); i += 3) {
		int vertexIndices[3] = { indices[i], indices[i + 1], indices[i + 2] };
		for (int j = 0; j < 3; ++j) {
			Edge edge(vertexIndices[j], vertexIndices[(j + 1) % 3]);
			if (std::find(highlightedEdges.begin(), highlightedEdges.end(), edge) != highlightedEdges.end()) {
				highlightedEdgeIndices.push_back(vertexIndices[j]);
				highlightedEdgeIndices.push_back(vertexIndices[(j + 1) % 3]);
			}
			else {
				nonhighlightedEdgeIndices.push_back(vertexIndices[j]);
				nonhighlightedEdgeIndices.push_back(vertexIndices[(j + 1) % 3]);
			}
		}
	}
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		glm::vec3 rayOrigin = camera.Position;
		glm::vec3 rayWorld = SegmentDetection::getRayWorld(xpos, ypos, rayOrigin, windowWidth, windowHeight, camera);
		

		int closestFaceIndex;
		glm::vec3 intersectionPoint;

		if (SegmentDetection::getClickedTriangle(rayOrigin, rayWorld, indices, vertices, verticesRowAmount, closestFaceIndex, intersectionPoint) && currentMode == EDGE) {
			int index1, index2;
			SegmentDetection::getClosestEdge(intersectionPoint, indices, vertices, verticesRowAmount, closestFaceIndex, index1, index2);
			highlightedEdges.push_back(Edge(index1, index2));
			updateEdges();
		}
		if (currentMode == FACE) {
			int index = find(highlightedFaces, closestFaceIndex);
			if (index != -1) highlightedFaces.erase(highlightedFaces.begin() + index);
			else if (closestFaceIndex != -1) highlightedFaces.push_back(closestFaceIndex);
			else highlightedFaces.clear();
		}
		
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		moving = !moving;
	}
}
glm::vec3 mapMouseMovementToDirection(double deltaX, double deltaY) {
	glm::vec3 cameraFront = glm::normalize(camera.Orientation);
	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, camera.Up));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

	float sensitivity = 0.01f;

	return cameraRight * ((float)deltaX*sensitivity) + cameraUp * ((float)deltaY*sensitivity);
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double deltaX = xpos - lastX;
	double deltaY = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	if (moving) {
		glm::vec3 direction = mapMouseMovementToDirection(deltaX, deltaY);
		std::vector<int> scaled;
		for (int i = 0; i < highlightedFaces.size(); i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					// i for highlighted triangle, j for vertex of that triangle, k for x/y/z
					int index = indices[highlightedFaces[i] * 3 + j] * verticesRowAmount + k;
					if (find(scaled, index) == -1) {
						vertices[index] += direction[k];
						scaled.push_back(index);
					}
				}
			}
		}
		VBO1.UpdateData(&vertices[0], vertices.size() * sizeof(float));
		Binds();
	}
}


