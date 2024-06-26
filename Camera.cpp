#include "Camera.h"

Camera::Camera() {}
Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), (float)(width / height), nearPlane, farPlane);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::Inputs(GLFWwindow* window)
{
	glm::vec3 velocity = glm::vec3(0);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		velocity += Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		velocity += -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		velocity += -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		velocity += glm::normalize(glm::cross(Orientation, Up));
	}
	
	if (glm::length(velocity) > 1) {
		velocity = glm::normalize(velocity);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		velocity += Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		velocity -= Up;
	}

	Position += velocity * speed;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		holdingDownRight = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClickRight) {
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClickRight = false;
		}

		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (height / 2)) / height;
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));
		if (!(glm::angle(newOrientation, Up) <= glm::radians(5.0f) || glm::angle(newOrientation, -Up) <= glm::radians(5.0f))) {
			Orientation = newOrientation;
		}
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		if (holdingDownRight) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstClickRight = true;
			holdingDownRight = false;
		}
	}

}
