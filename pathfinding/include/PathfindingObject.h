#ifndef __PATHFINDING_OBJECT_H__
#define __PATHFINDING_OBJECT_H__

//C Includes
#include <vector>
#include <glm/glm.hpp>
#include "Maze.h"

class PathfindingObject {
public:

	void StartPath(std::vector<Position>* a_path, glm::vec3 a_pathOffset);
	glm::vec3 GetCurrentPosition();

protected:
	//Constructors / Desctructors
	PathfindingObject(glm::vec3 a_pos);
	PathfindingObject(glm::vec3 a_pos, std::vector<Position>* a_path);
	~PathfindingObject();

	void FollowPath(float a_fDeltaTime);
	void DrawDebugBox();

	glm::vec3 m_currentPostion;
	bool m_bFollowingPath = false;
	std::vector<Position> m_path;

private:

	unsigned int m_iCurrentIndexInPath = 0;
	glm::vec3 m_currentTargetPostion;

	//Speed
	const float m_fMoveSpeed = 5.f;

	glm::vec3 m_pathOffset;

	//Changing next path position
	bool IncrementNextPathPostion();

};

#endif // !__PATHFINDING_OBJECT_H__

