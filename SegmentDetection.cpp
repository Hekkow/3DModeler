#include "SegmentDetection.h"

bool SegmentDetection::isTriangleClicked(glm::vec3 &rayOrigin, glm::vec3 &rayDirection, glm::vec3 &vertex0, glm::vec3 &vertex1, glm::vec3 &vertex2, float &intersectionDistance) {
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
float SegmentDetection::pointToEdgeDistance(glm::vec3& point, glm::vec3& edgeStart, glm::vec3 &edgeEnd) {
	glm::vec3 edge = edgeEnd - edgeStart;
	glm::vec3 pointToStart = point - edgeStart;
	float edgeLength = glm::length(edge);
	glm::vec3 edgeDirection = glm::normalize(edge);
	float projectionLength = glm::dot(pointToStart, edgeDirection);

	if (projectionLength < 0) return glm::length(pointToStart);
	else if (projectionLength > edgeLength) return glm::length(point - edgeEnd);
	else return glm::length(pointToStart - edgeDirection * projectionLength);
}
bool SegmentDetection::getClickedTriangle(glm::vec3 &rayOrigin, glm::vec3 &rayWorld, std::vector<GLuint> &indices, std::vector<float> &vertices, int verticesRowAmount, int &closestFaceIndex, glm::vec3 &intersectionPoint) {
	float closestIntersection = std::numeric_limits<float>::max();
	closestFaceIndex = -1;

	for (int i = 0; i < indices.size(); i += 3) {
		glm::vec3 vectors[3];
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				vectors[j][k] = vertices[indices[i + j] * verticesRowAmount + k];
			}
		}
		float t;
		if (SegmentDetection::isTriangleClicked(rayOrigin, rayWorld, vectors[0], vectors[1], vectors[2], t)) {
			if (t < closestIntersection) {
				closestIntersection = t;
				closestFaceIndex = i / 3;
				intersectionPoint = rayOrigin + rayWorld * t;
			}
		}
	}
	return closestFaceIndex != -1;
}
void SegmentDetection::getClosestEdge(glm::vec3 &intersectionPoint, std::vector<GLuint> &indices, std::vector<float> &vertices, int verticesRowAmount, int closestFaceIndex, int &index1, int &index2) {
	glm::vec3 vectors[3];
	for (int j = 0; j < 3; ++j) {
		for (int k = 0; k < 3; ++k) {
			vectors[j][k] = vertices[indices[closestFaceIndex * 3 + j] * verticesRowAmount + k];
		}
	}
	float distances[3];
	distances[0] = SegmentDetection::pointToEdgeDistance(intersectionPoint, vectors[0], vectors[1]);
	distances[1] = SegmentDetection::pointToEdgeDistance(intersectionPoint, vectors[1], vectors[2]);
	distances[2] = SegmentDetection::pointToEdgeDistance(intersectionPoint, vectors[2], vectors[0]);
	float minDistance = std::numeric_limits<float>::max();
	int minIndex = -1;
	for (int i = 0; i < 3; ++i) {
		if (distances[i] < minDistance) {
			minDistance = distances[i];
			minIndex = i;
		}
	}
	index1 = indices[closestFaceIndex * 3 + minIndex];
	index2 = indices[closestFaceIndex * 3 + (minIndex + 1) % 3];
}
glm::vec3 SegmentDetection::getRayWorld(double xpos, double ypos, glm::vec3 rayOrigin, float windowWidth, float windowHeight, Camera &camera) {
	glm::vec4 rayClip = glm::vec4((xpos / windowWidth) * 2 - 1, (1 - ypos / windowHeight) * 2 - 1, -1, 1);
	glm::vec4 rayEye = glm::inverse(camera.projection) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
	return glm::normalize(glm::vec3(glm::inverse(camera.view) * rayEye));
}