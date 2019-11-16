#ifndef __Application_H_
#define __Application_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "Maze.h"
#include "md2_loader.h"
#include "MD2Pathfinder.h"
#include "LocationPicker.h"

// Derived application class that wraps up all globals neatly
class PathfindingApp : public Application
{
public:

	PathfindingApp();
	virtual ~PathfindingApp();

	Maze* m_pMaze;
	std::vector<Position> m_path;
	MD2Pathfinder* m_pPathfindingModel;
	LocationPicker* m_pLocationRaycaster;

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();



#pragma region BOILER_PLATE_GL_CODE
	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_currTexID;
	unsigned int m_program;
	unsigned int m_vertexShader;
	unsigned int m_fragmentShader;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	unsigned int glViewMinW;
	unsigned int glViewMinH;

	//MD2Model* m_pMD2Model;

	glm::vec4 m_lightPos;
#pragma endregion

	bool m_bSpacePressedLastFrame = false;
	bool m_bLeftMousePressedLastFrame = false;
	bool m_bSkinChangeKeyPressedLastFrame = false;
	bool m_bAnimationChangeKeyPressedLastFrame = false;

	//Set if we should draw the path that the model is following
	bool m_bDrawPath = false;

private:
	void InitBoilerplateGL();
	void UpdateBoilerplateGL(float a_deltaTime);

	void SetModelTextureID(unsigned int a_TextureID);
	void SetModelDrawData(unsigned int a_numVertices, unsigned int a_vertexSize, void* a_vertexData);

	void PreDraw();
	void DrawModel(unsigned int a_numVerts);

};

#endif // __Application_H_