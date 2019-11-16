#include "Maze.h"
#include <random>
#include "Gizmos.h"
#include <map>
#include <vector>

/// <summary>
/// Generate a maze with randomised walls
/// </summary>
/// <param name="width">Width of maze</param>
/// <param name="height">Height of maze</param>
/// <param name="tileSize">Size of each tile in the maze</param>
Maze::Maze(unsigned int width, unsigned int height, float tileSize)
{
	m_iWidth = width;
	m_iHeight = height;
	m_fTileSize = tileSize;
	m_Tiles = new bool*[width];


	for (unsigned int x = 0; x < width; ++x)
	{
		m_Tiles[x] = new bool[height];
	}

	RandomiseWalls();
}

/// <summary>
/// Maze Destructor
/// </summary>
Maze::~Maze()
{
	for (unsigned int x = 0; x < m_iWidth; ++x)
	{
		delete[] m_Tiles[x];
	}

	delete[] m_Tiles;
}



/// <summary>
/// Randomise the walls within the maze
/// </summary>
void Maze::RandomiseWalls()
{
	for (unsigned int x = 0; x < m_iWidth; ++x)
	{
		for (unsigned int y = 0; y < m_iHeight; ++y)
		{
			m_Tiles[x][y] = rand() % 5 == 0 ? true : false;
		}
	}
}

/// <summary>
/// Gets the offset of the maze
/// </summary>
/// <returns></returns>
glm::vec3 Maze::GetOffset()
{
	return glm::vec3(-m_fTileSize * (m_iWidth - 1) * 0.5f, 0, -m_fTileSize * (m_iHeight - 1) * 0.5f);
}


/// <summary>
/// Gets the size of the tiles within the maze
/// </summary>
/// <returns></returns>
float Maze::GetTileSize()
{
	return m_fTileSize;
}

/// <summary>
/// Gets the total width of the maze
/// </summary>
/// <returns></returns>
float Maze::GetWidth()
{
	return m_fTileSize * m_iWidth;
}

/// <summary>
/// Gets the total height of the maze
/// </summary>
/// <returns></returns>
float Maze::GetHeight()
{
	return m_fTileSize * m_iHeight;
}

/// <summary>
/// Gets the number of tiles wide of the maze
/// </summary>
/// <returns></returns>
unsigned int Maze::GetNumTilesWidth()
{
	return m_iWidth;
}

/// <summary>
/// Geths the number of tiles high of the maze
/// </summary>
/// <returns></returns>
unsigned int Maze::GetNumTilesHeight()
{
	return m_iHeight;
}

/// <summary>
/// Gets if a particular postion is a wall 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
bool Maze::IsWall(int x, int y)
{
	if (x >= m_iWidth)
		return true;
	if (y >= m_iHeight)
		return true;
	if (x < 0)
		return true;
	if (y < 0)
		return true;
	
	return m_Tiles[x][y];
}

/// <summary>
/// Gets if a particular postion is a wall 
/// </summary>
bool Maze::IsWall(Position pos)
{
	return IsWall(pos.x, pos.y);
}

/// <summary>
/// Draws the cubes of the maze
/// </summary>
void Maze::DrawMaze()
{
	for (unsigned int x = 0; x < m_iWidth; ++x)
	{
		for (unsigned int y = 0; y < m_iHeight; ++y)
		{
			if (m_Tiles[x][y])
			{
				Gizmos::addBox(GetVec3(x, y), glm::vec3(m_fTileSize), true);
			}
		}
	}
	
}

/// <summary>
/// Draws a given path around the maze
/// </summary>
/// <param name="path"></param>
void Maze::DrawPath(std::vector<Position>& path)
{
	int size = (int)path.size();
	if (size < 2)
		return;
	
	for (int i = 1; i < size; ++i)
	{
		Position from = path[i - 1];
		Position to = path[i];

		Gizmos::addLine(GetVec3(from), GetVec3(to), glm::vec4(1.0f, 0.41f, 0.7f, 1.0f));
	}
}


/// <summary>
/// Converts x y values in to a position as a vector 3 within the maze
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
glm::vec3 Maze::GetVec3(int x, int y)
{
	glm::vec3 tileOffset = glm::vec3(0.0f, m_fTileSize * 0.5f, 0.0f);
	glm::vec3 centerAllign = glm::vec3(-m_fTileSize * (m_iWidth - 1) * 0.5f, 
		0, 
		-m_fTileSize * (m_iHeight - 1) * 0.5f);
	return glm::vec3(x*m_fTileSize, 0, y*m_fTileSize) + tileOffset + centerAllign;
}

/// <summary>
/// Converts a postion to a vector 3 within the maze
/// </summary>
/// <param name="pos"></param>
/// <returns></returns>
glm::vec3 Maze::GetVec3(Position pos)
{
	return GetVec3(pos.x, pos.y);
}


/// <summary>
/// Finds a path between two postions using the Dijkstra pathfinding algorithm
/// </summary>
/// <param name="start"></param>
/// <param name="end"></param>
/// <param name="finalPath"></param>
/// <returns></returns>
bool Maze::PathfindingDijkstra(Position start, Position end, std::vector<Position>& finalPath)
{
	//Clear the final path
	finalPath.clear();

	//Make sure that the path doesn't end or 
	//start in a wall
	if (IsWall(start) || IsWall(end))
	{
		return false;
	}

	std::map<Position, int> unvisited;
	std::map<Position, int> visited;

	// Initial Setup
	unvisited[start] = 0;
	Position currentTile = start;
	int currentDistance = 0;

	//Go forever until we do or don't find a path
	while (true)
	{
		// Search for a tile with the lowest score
		currentDistance = std::numeric_limits<int>::max();
		for (auto it = unvisited.begin(); it != unvisited.end(); it++)
		{
			if (it->second < currentDistance)
			{
				currentDistance = it->second;
				currentTile = it->first;
			}
		}

		// Mark this tile as visited
		visited[currentTile] = currentDistance;
		unvisited.erase(currentTile);

		// Is this the end?
		if (currentTile == end)
		{
			// Found the finish
			break;
		}

		// Check unvisited neighbours and update distances
		Position* adjacent = GetAdjacentPositions(currentTile);
		for (int i = 0; i < 4; ++i)
		{
			if (IsWall(adjacent[i]))
				continue;

			if (visited.find(adjacent[i]) != visited.end())
				continue;

			int adjacentCost = currentDistance + 1;
			auto it = unvisited.find(adjacent[i]);
			if (it != unvisited.end())
			{
				// It's already in the list
				if (it->second > adjacentCost)
				{
					it->second = adjacentCost;
				}
			}
			else
			{
				// Otherwise make a new entry
				unvisited[adjacent[i]] = adjacentCost;
			}
		}
		delete[] adjacent;

		// Failsafe
		if (unvisited.size() == 0)
		{
			// No more open nodes, must be no solution
			return false;
		}
	}
	// Now to put the path together
	currentTile = end;
	while (true)
	{
		visited.erase(currentTile);
		finalPath.push_back(currentTile);

		if (currentTile == start)
		{
			break;
		}

		int bestDistance = std::numeric_limits<int>::max();
		int bestNeighbour;
		Position* adjacent = GetAdjacentPositions(currentTile);
		for (int i = 0; i < 4; i++)
		{
			Position neighbourPosition = adjacent[i];
			if (visited.find(neighbourPosition) != visited.end())
			{
				if (visited[neighbourPosition] < bestDistance)
				{
					bestDistance = visited[neighbourPosition];
					bestNeighbour = i;
				}
			}
		}
		currentTile = adjacent[bestNeighbour];
		delete[] adjacent;
	}

	return true;
}


/*

	ATTEMPTED IMPLEMENTATION OF A*, however freezes when called

bool Maze::PathfindingAStar(Position start, Position end, std::vector<Position>& finalPath)
{

	//Clear final path
	finalPath.clear();

	//Create a start and end node
	AStarNode startNode = AStarNode(nullptr, start);
	startNode.g = startNode.h = startNode.f = 0;
	AStarNode endNode = AStarNode(nullptr, end);
	endNode.g = endNode.h = endNode.f = 0;

	//Create a visited and unvisited list
	std::vector<AStarNode> openList;
	std::vector<AStarNode> closedList;

	openList.push_back(startNode);

	//Loop until we find the end or we have nothing left in the open list
	while (openList.size() > 0) {

		//Get the current node
		AStarNode currentNode = openList[0];
		int iCurrentIndex = 0;

		for (int i = 0; i < openList.size(); i++) {
			if (openList[i].f < currentNode.f) {
				currentNode = openList[i];
				iCurrentIndex = i;
			}
		}

		//Remove current off the open list and add to the closed list
		openList.erase(openList.begin() + iCurrentIndex);
		closedList.push_back(currentNode);

		//Check if we have found the goal, construct the path
		if (currentNode == endNode) {
			AStarNode* current = &currentNode;
			//Loop through the parents of each node to construct the path
			while (current != nullptr) {
				finalPath.push_back(current->position);
				current = current->parent;
			}
			//Return that we have found a path
			return true;
		}

		//Construct Children Nodes
		std::vector<AStarNode> children;
		Position* childPostions = GetAdjacentPositions(currentNode.position);
		for (int i = 0; i < 4; i++) {

			//Check that children arent walls
			if (IsWall(childPostions[i])) {
				continue;
			}

			//Create a new node
			AStarNode* pointer = &currentNode;
			AStarNode newNode = AStarNode(pointer, childPostions[0]);
			children.push_back(newNode);
		}

		//Loop through children
		for each(AStarNode child in children) {

			//Check if child is closed
			for each(AStarNode closedNode in closedList) {
				if (child == closedNode) {
					continue;
				}
			}

			//Create G H F values
			child.g = currentNode.g + 1;
			child.h = pow(child.position.x - endNode.position.x, 2) + pow(child.position.y - endNode.position.y, 2);
			child.f = child.g + child.f;

			//Check if child is already in the open node list
			for each(AStarNode openNode in openList) {
				if (child == openNode && child.g > openNode.g) {
					continue;
				}
			}

			//Add the child to the open list
			openList.push_back(child);

		}

	}

	return false;
}
*/

/// <summary>
/// Gets the postions of tiles adjacent (up, left, down, right)
/// to the given tile
/// </summary>
/// <param name="currentTile">Tile to get adjacent tiles of</param>
/// <returns>Array (pointer) of tiles</returns>
Position * Maze::GetAdjacentPositions(Position currentTile)
{
	Position* positions = new Position[4];
	positions[0] = Position(currentTile.x + 1, currentTile.y);
	positions[1] = Position(currentTile.x - 1, currentTile.y);
	positions[2] = Position(currentTile.x , currentTile.y + 1);
	positions[3] = Position(currentTile.x , currentTile.y - 1);
	return positions;
}