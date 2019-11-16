#ifndef __MAZE_H__
#define __MAZE_H__

#include <glm/glm.hpp>
#include <vector>

struct Position
{

	Position() {};
	Position(glm::vec3 a_vec3) : x(a_vec3.x), y(a_vec3.z) {};
	Position(int a_x, int a_y) : x(a_x), y(a_y) {};
	int x;
	int y;

	/// <summary>
	/// Converts a vector 3 from the location picker in to a postion
	/// within the maze, can return maze coord outside the maze
	/// </summary>
	/// <returns>Postion in maze space</returns>
	static Position inline ConvertLocationPickerToMazeCords(glm::vec3 a_posVector, int a_fMazeWidth, int a_fMazeHeight) {
		//Get pos to axis centered maze rather than center centered
		a_posVector.x += a_fMazeWidth / 2;
		a_posVector.z += a_fMazeHeight / 2;
		return Position(a_posVector.x, a_posVector.z);
	}

	inline bool operator==(const Position& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	inline bool operator<(const Position& rhs) const
	{
		return y == rhs.y ? x < rhs.x : y < rhs.y;
	}

	inline bool operator>(const Position& rhs) const
	{
		return y == rhs.y ? x > rhs.x : y > rhs.y;
	}

	inline bool operator<=(const Position& rhs) const
	{
		return y == rhs.y ? x <= rhs.x : y <= rhs.y;
	}

	inline bool operator>=(const Position& rhs) const
	{
		return y == rhs.y ? x >= rhs.x : y >= rhs.y;
	}
};

/*
	NODE FOR ATTEMPTED IMPLEMENTATION OF A*

/// <summary>
/// Node for a start pathfinding, stores parent, postion,
/// g, h and f values
/// </summary>
struct AStarNode {
	AStarNode(AStarNode* a_pParent, Position a_pos) {
		parent = a_pParent;
		position = a_pos;
	}
	AStarNode* parent;
	Position position;

	int g = 0; //Distance from start node
	int h = 0; //Distance from end node
	int f = 0; //G + F

	//Comparision of if we are a certian node
	bool operator==(AStarNode a_other) {
		return position == a_other.position;
	}
};
*/

class Maze
{
public:
	Maze(unsigned int width, unsigned int height, float tileSize);
	~Maze();

	bool PathfindingDijkstra(Position start, Position end, std::vector<Position> & finalPath);
	//bool PathfindingAStar(Position start, Position end, std::vector<Position>& finalPath);

	void DrawMaze();
	void DrawPath(std::vector<Position> &path);

	float GetTileSize();
	float GetWidth();
	float GetHeight();
	unsigned int GetNumTilesWidth();
	unsigned int GetNumTilesHeight();

	void RandomiseWalls();

	glm::vec3 GetOffset();

private:



	float m_fTileSize;
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	bool** m_Tiles;
	glm::vec3 GetVec3(int x, int y);
	glm::vec3 GetVec3(Position pos);
	Position* GetAdjacentPositions(Position currentTile);

	
	bool IsWall(int x, int y);
	bool IsWall(Position pos);
};

#endif // !__MAZE_H__
