#include "PathfindingObject.h"

//C Includes
#include "Gizmos.h"
#include "Maze.h"
#include <glm/glm.hpp>


/// <summary>
/// Constructs a pathfinding object with a postion
/// </summary>
/// <param name="a_pos"></param>
PathfindingObject::PathfindingObject(glm::vec3 a_pos)
{
	m_currentPostion = a_pos;

	//Set 
	m_pathOffset = m_currentPostion;
}

/// <summary>
/// Constructs a pathfinding object with a postion and starts it on a path
/// </summary>
/// <param name="a_pos">Postion to create </param>
/// <param name="a_path">Path to follow</param>
PathfindingObject::PathfindingObject(glm::vec3 a_pos, std::vector<Position>* a_path)
{
	//Set postion
	m_currentPostion = a_pos;

	//Start the path for our object with our current postion as the offset so that we 
	//start the path at our current postion
	StartPath(a_path, a_pos);

}

PathfindingObject::~PathfindingObject()
{
}

/// <summary>
/// Starts the pathfinding object on a path
/// </summary>
/// <param name="a_pPath">Path to follow</param>
/// <param name="a_pathOffset">Postion to start path from</param>
void PathfindingObject::StartPath(std::vector<Position>* a_pPath, glm::vec3 a_pathStartPos = glm::vec3(0))
{

	//If the path we are starting has no length then
	//abort
	if (a_pPath->size() == 0) {
		return;
	}

	m_path = *a_pPath;
	m_pathOffset = a_pathStartPos;

	//Reverse the path becuase when it is spit out from the 
	//maze it starts at the end
	std::reverse(m_path.begin(), m_path.end());

	//Set our target postion to the start of the path and index the start of the path
	m_iCurrentIndexInPath = 0;
	Position pathStartPos = m_path[0];
	m_currentTargetPostion = glm::vec3(pathStartPos.x, 0, pathStartPos.y) + m_pathOffset;
	m_bFollowingPath = true;
}

/// <summary>
/// Gets the current postion of the player in the grid
/// </summary>
/// <returns></returns>
glm::vec3 PathfindingObject::GetCurrentPosition()
{
	return m_currentPostion;
}


/// <summary>
/// Updates the relevent factors of the pathfinding object
/// </summary>
void PathfindingObject::FollowPath(float a_fDeltaTime)
{
	//Check if we should be following a path
	if (m_bFollowingPath) {
		//Check that we have a valid path (i.e > 0)
		if (m_path.size() > 0) {

			//Work out the difference between my target postion and my current position
			glm::vec3 diff = m_currentTargetPostion - m_currentPostion;

			//Check if we could move more than the distance to the next postion,
			//if we can then just move the distance to our next postion
			if (glm::distance(m_currentTargetPostion, m_currentPostion) > (m_fMoveSpeed * a_fDeltaTime)) {
				m_currentPostion += glm::normalize(diff) * m_fMoveSpeed * a_fDeltaTime;
			}
			else {
				m_currentPostion += diff;
			}

			//Check if we have reached our target position, if we are then
			//move on to the next point
			if (m_currentTargetPostion == m_currentPostion) {
				//If we can't increment the and get the next postion then
				//we must be at the end of the path. Stop trying to follow 
				//a path
				if (!IncrementNextPathPostion()) {
					m_bFollowingPath = false;
				}
			}
		}
		else {
			//If path is 0 length then don't keep attempting to follow the path
			m_bFollowingPath = false;
		}
	}
}

/// <summary>
/// Updates the what the next postion is in the path
/// Returns if there is a next postion in path
/// </summary>
/// <returns>Whether or not there is a next postion in the path
/// (i.e we are not at the end of the path)</returns>
bool PathfindingObject::IncrementNextPathPostion()
{
	//Increment the next postion in the path index, retreive the next
	//position in the path from the list and convert to vector 3
	if (++m_iCurrentIndexInPath < m_path.size()) {
		Position nextPostion = m_path[m_iCurrentIndexInPath];
		m_currentTargetPostion = glm::vec3(nextPostion.x, 0, nextPostion.y) + m_pathOffset;
		return true;
	}
	else {
		return false;
	}
}



/// <summary>
/// Draws a box at the pathfinding objects position, should be used for debug only
/// </summary>
void PathfindingObject::DrawDebugBox()
{
	Gizmos::addBox(m_currentPostion, glm::vec3(1, 1, 1), true, glm::vec4(0.2f, 0.5f, 0.7f, 1.f));
}
