
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



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
Camera camera;
std::vector<int> highlighted;
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
	Binds();
	
	Texture popCat("popcat.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	popCat.texUnit(shaderProgram, "tex0", 0);
	glEnable(GL_DEPTH_TEST);
	camera = Camera(windowWidth, windowHeight, glm::vec3(0, 0, 10));
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.07f, 0.13f, 0.17f, 1);
		
		shaderProgram.Activate();
		popCat.Bind();
		
		
		camera.Inputs(window);
		camera.Matrix(45, 0.1f, 100, shaderProgram, "camMatrix");

		VAO1.Bind();
		for (int i = 0; i < indices.size(); i++) {
			if (find(highlighted, i) != -1) glUniform1i(glGetUniformLocation(shaderProgram.ID, "highlighted"), true);
			else glUniform1i(glGetUniformLocation(shaderProgram.ID, "highlighted"), false);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(GLuint)));
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
bool isTriangleClicked(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& vertex0, const glm::vec3& vertex1, const glm::vec3& vertex2, float& intersectionDistance) {
    glm::vec3 edge1, edge2, h, s, q;
	float small = 0.00001f;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = glm::cross(rayDirection, edge2);
	float determinant = glm::dot(edge1, h);

    if (determinant > -small && determinant < small) return false;

	float inverseDeterminant = 1 / determinant;
    s = rayOrigin - vertex0;
	float parameterU = inverseDeterminant * glm::dot(s, h);

    if (parameterU < 0 || parameterU > 1) return false;

    q = glm::cross(s, edge1);
	float parameterV = inverseDeterminant * glm::dot(rayDirection, q);

    if (parameterV < 0 || parameterU + parameterV > 1) return false;

    intersectionDistance = inverseDeterminant * glm::dot(edge2, q);
    return intersectionDistance > small;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		glm::vec3 rayOrigin = camera.Position;
		glm::vec4 rayClip = glm::vec4((xpos / windowWidth) * 2 - 1, (1 - ypos / windowHeight) * 2 - 1, -1, 1);
		glm::vec4 rayEye = glm::inverse(camera.projection) * rayClip;
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
		glm::vec3 rayWorld = glm::vec3(glm::inverse(camera.view) * rayEye);
		rayWorld = glm::normalize(rayWorld);

		float closestIntersection = std::numeric_limits<float>::max();
		int closestFaceIndex = -1;

		for (size_t i = 0; i < indices.size(); i += 3) {
			glm::vec3 vectors[3];
			for (int j = 0; j < 3; ++j) {
				for (int k = 0; k < 3; ++k) {
					vectors[j][k] = vertices[indices[i + j] * verticesRowAmount + k];
				}
			}
			float t;
			if (isTriangleClicked(rayOrigin, rayWorld, vectors[0], vectors[1], vectors[2], t)) {
				if (t < closestIntersection) {
					closestIntersection = t;
					closestFaceIndex = i / 3;
				}
			}
		}
		int index = find(highlighted, closestFaceIndex);
		if (index != -1) highlighted.erase(highlighted.begin() + index);
		else if (closestFaceIndex != -1) highlighted.push_back(closestFaceIndex);
		else highlighted.clear();
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
		for (int i = 0; i < highlighted.size(); i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					// i for highlighted triangle, j for vertex of that triangle, k for x/y/z
					int index = indices[highlighted[i] * 3 + j] * verticesRowAmount + k;
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


