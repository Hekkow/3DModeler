#ifndef SEGMENT_DETECTION_H
#define SEGMENT_DETECTION_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <glad/glad.h>
#include "Camera.h"
class SegmentDetection
{
public:
	static bool isTriangleClicked(glm::vec3 &rayOrigin, glm::vec3 &rayDirection, glm::vec3 &vertex0, glm::vec3 &vertex1, glm::vec3 &vertex2, float &intersectionDistance);
	static float pointToEdgeDistance(glm::vec3 &point, glm::vec3 &edgeStart, glm::vec3 &edgeEnd);
	static bool getClickedTriangle(glm::vec3 &rayOrigin, glm::vec3 &rayWorld, std::vector<GLuint> &indices, std::vector<float> &vertices, int verticesRowAmount, int &closestFaceIndex, glm::vec3 &intersectionPoint);
	static void getClosestEdge(glm::vec3 &intersectionPoint, std::vector<GLuint> &indices, std::vector<float> &vertices, int verticesRowAmount, int closestFaceIndex, int &index1, int &index2);
	static glm::vec3 getRayWorld(double xpos, double ypos, glm::vec3 rayOrigin, float windowWidth, float windowHeight, Camera& camera);
};
#endif