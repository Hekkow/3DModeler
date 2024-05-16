#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shaderClass.h"

class Camera {
public:
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0, 0, -1);
	glm::vec3 Up = glm::vec3(0, 1, 0);

	int width;
	int height;
	bool firstClick = true;
	float speed = 0.005f;
	float sensitivity = 70;
	Camera(int width, int height, glm::vec3 position);
	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
	void Inputs(GLFWwindow* window);
};

#endif