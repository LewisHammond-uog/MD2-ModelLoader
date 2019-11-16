#ifndef __LOCATION_PICKER_H__
#define __LOCATION_PICKER_H__

#include <glm/glm.hpp>

/// <summary>
/// Class used to pick and store a location for the player to walk to
/// </summary>
class LocationPicker
{
public:
	//Constructor/Destructor
	LocationPicker(int a_iTargetWindowWidth, int a_iTargetWindowHeight);
	~LocationPicker();

	//Update/Draw
	void Update(const glm::mat4 a_cameraMatrix, const glm::mat4 a_projectionMatrix, const double a_dMouseXPos, const double a_dMouseYPos);
	void Draw();

	glm::vec3 GetCurrentLocation();

private:

	//Store the location at which we are intersecting the axis
	glm::vec3 m_currentLocation;

	//mouse pos in world
	glm::vec4 m_mouseCameraSpacePos = glm::vec4(0.f);
	glm::vec4 m_mouseCameraSpaceRay = glm::vec4(0.f);

	//Store window info
	unsigned int m_iTargetWindowWidth;
	unsigned int m_iTargetWindowHeight;

	//Sphere draw info
	glm::vec4 const mc_sphereDrawColour = glm::vec4(0, 0, 0, 1);
	int const mc_sphereDrawVerts = 8;
	float const mc_sphereDrawRadius = 0.3f;
	

};

#endif // !__LOCATION_PICKER_H__
