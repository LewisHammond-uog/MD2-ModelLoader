#include "LocationPicker.h"

#include "Gizmos.h"
#include <glm/ext.hpp>
#include <math.h>

/// <summary>
/// Create a location picker with a window to extract mouse info from
/// </summary>
/// <param name="a_targetWindow"></param>
LocationPicker::LocationPicker(int a_iTargetWindowWidth, int a_iTargetWindowHeight)
{
	m_iTargetWindowWidth = a_iTargetWindowWidth;
	m_iTargetWindowHeight = a_iTargetWindowHeight;
}

/// <summary>
/// Destroy Location Picker
/// </summary>
LocationPicker::~LocationPicker()
{
}

/// <summary>
/// Update the position of the location picker so that it matches the on screen mouse postion
/// </summary>
/// <param name="a_cameraMatrix">Matrix Postion of the Camera</param>
/// <param name="a_projectionMatrix">Projection Materix</param>
/// <param name="a_dMouseXPos">Mouse X Postion on the screen</param>
/// <param name="a_dMouseYPos">Mouse Y Postion on the screen</param>
void LocationPicker::Update(const glm::mat4 a_cameraMatrix, const glm::mat4 a_projectionMatrix, const double a_dMouseXPos, const double a_dMouseYPos)
{
	//Get mouse position on the window
	double xpos = a_dMouseXPos;
	double ypos = a_dMouseYPos;

	//Get x pos and y pos relative to center of the screen 
	xpos = (xpos * 2.0) / m_iTargetWindowWidth - 1.0;
	ypos = 1.0 - ((2.0 * ypos) / m_iTargetWindowHeight);

	//Store mouse screen pos as vec 4
	glm::vec4 mouseScreenPos = glm::vec4((float)xpos, (float)ypos, 1.0, 1.0);

	//Convert ray to projection space
	glm::vec4 mouseProjectionSpace = glm::inverse(a_projectionMatrix) * mouseScreenPos;
	mouseProjectionSpace.w = 1.0f;
	m_mouseCameraSpacePos = a_cameraMatrix * mouseProjectionSpace;
	//this effecitley the point on our near plane now.

	m_mouseCameraSpaceRay = m_mouseCameraSpacePos - a_cameraMatrix[3];
	m_mouseCameraSpaceRay = glm::normalize(m_mouseCameraSpaceRay);

	//If we have a mouse camera screen postion
	if (glm::length(m_mouseCameraSpacePos) > 0) {

		//Create ray start and end point
		glm::vec3 rayOrigin = glm::vec3(m_mouseCameraSpacePos.x, m_mouseCameraSpacePos.y, m_mouseCameraSpacePos.z);
		glm::vec3 rayEnd = rayOrigin + glm::vec3(m_mouseCameraSpaceRay.x, m_mouseCameraSpaceRay.y, m_mouseCameraSpaceRay.z) * 100;

		//Create vec4s to store normal to plane and point that 
		//we are casting at on the plane
		glm::vec4 planeNormal = glm::vec4(0.f, 1.f, 0.f, 0.f);
		glm::vec4 pointOnPlane = glm::vec4(0.f, 0.f, 0.f, 1.f);

		//Initalise Distance to plane and plane direction
		float fDistanceToPlane = 0;
		float fPndDrd = glm::dot(planeNormal, m_mouseCameraSpaceRay);
			
		//If direction to to plane is not 0 degrees then get the distance to the plane
		if (fabsf(fPndDrd) > FLT_EPSILON) {
			fDistanceToPlane = -(glm::dot(planeNormal, m_mouseCameraSpacePos) + 0.f) / fPndDrd;
		}

		//Get the location where the ray intersects the plane
		m_currentLocation = rayOrigin + glm::vec3(m_mouseCameraSpaceRay.x, m_mouseCameraSpaceRay.y, m_mouseCameraSpaceRay.z) * fDistanceToPlane;

	}

}

/// <summary>
/// Draws an object at the point where the ray intersects the plane
/// </summary>
void LocationPicker::Draw()
{
	Gizmos::addSphere(m_currentLocation, mc_sphereDrawVerts, mc_sphereDrawVerts, mc_sphereDrawRadius, mc_sphereDrawColour);
}

/// <summary>
/// Gets the current location of the location pickuer
/// </summary>
/// <returns></returns>
glm::vec3 LocationPicker::GetCurrentLocation()
{
	return glm::vec3(m_currentLocation.x, m_currentLocation.y, m_currentLocation.z);
}
