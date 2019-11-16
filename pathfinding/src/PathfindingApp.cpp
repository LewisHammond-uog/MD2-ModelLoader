#include "PathfindingApp.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

#include "Application_Log.h"
#include "MD2Pathfinder.h"
#include "texture_manager.h"
#include "md2_loader.h"

PathfindingApp::PathfindingApp()
{

}

PathfindingApp::~PathfindingApp()
{

}

//Create Pathfinding App
bool PathfindingApp::onCreate()
{
	// initialise the Gizmos helper class
	Gizmos::create();

	TextureManager* texManager = TextureManager::CreateInstance();

	m_pMaze = new Maze(20, 20, 1.0f);

	InitBoilerplateGL();

	m_pPathfindingModel = new MD2Pathfinder("./models/monsters/gunner/tris.md2");
	m_pLocationRaycaster = new LocationPicker(m_windowWidth, m_windowHeight);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

//Update App
void PathfindingApp::Update(float a_deltaTime)
{

	UpdateBoilerplateGL(a_deltaTime);

	

	#pragma region Regenerate Maze / Reset Character

	//If the space bar is pressed then regen the maze
	if (m_pMaze) {
		if (!m_bSpacePressedLastFrame && glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			m_pMaze->RandomiseWalls();
			m_pPathfindingModel->StopPath();
			m_pPathfindingModel->SetPosition(glm::vec3(0));
		}
	}

	m_bSpacePressedLastFrame = glfwGetKey(m_window, GLFW_KEY_SPACE);

	#pragma endregion

	#pragma region Click to Move

	//If mouse button is pressed then go to the location of the mouse pointer projection
	if (m_pMaze && m_pLocationRaycaster) {
		if (!m_bLeftMousePressedLastFrame && glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1)) {

			Position targetPathfindPos = Position::ConvertLocationPickerToMazeCords(m_pLocationRaycaster->GetCurrentLocation(),
				m_pMaze->GetNumTilesWidth(),
				m_pMaze->GetNumTilesHeight());
			Position currentPlayerPos = Position(m_pPathfindingModel->GetCurrentPosition());

			m_pMaze->PathfindingDijkstra(currentPlayerPos,
				targetPathfindPos,
				m_path);

			m_pPathfindingModel->StartPath(&m_path, glm::vec3(0));
		}
	}

	m_bLeftMousePressedLastFrame = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1);

	#pragma endregion

	#pragma region Change Skin of Model

	//Check left/right key press to change skin
	if (!m_bSkinChangeKeyPressedLastFrame) {

		if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			m_pPathfindingModel->ChangeSkin(MD2Pathfinder::ATTRIBUTE_CHANGE_DIRECTION_DECREASE);
		}
		else if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			m_pPathfindingModel->ChangeSkin(MD2Pathfinder::ATTRIBUTE_CHANGE_DIRECTION_INCREASE);
		}

	}
		

	//Evaluate if we changed the skin last frame.
	m_bSkinChangeKeyPressedLastFrame = glfwGetKey(m_window, GLFW_KEY_LEFT) || glfwGetKey(m_window, GLFW_KEY_RIGHT);

	#pragma endregion

	#pragma region Change Animation

	//Check left/right key press to change skin
		
	if (!m_bAnimationChangeKeyPressedLastFrame) {

		if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			m_pPathfindingModel->ChangeAnimation(MD2Pathfinder::ATTRIBUTE_CHANGE_DIRECTION_DECREASE);
		}
		else if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS) {
			m_pPathfindingModel->ChangeAnimation(MD2Pathfinder::ATTRIBUTE_CHANGE_DIRECTION_INCREASE);
		}

	}
		

	//Evaluate if we changed the skin last frame.
	m_bAnimationChangeKeyPressedLastFrame = glfwGetKey(m_window, GLFW_KEY_UP) || glfwGetKey(m_window, GLFW_KEY_DOWN);

	#pragma endregion

	//Set Texture ID

	SetModelTextureID(m_pPathfindingModel->GetTextureID());

	//Update and set draw data
	m_pPathfindingModel->Update(a_deltaTime);
	MD2Vertex* currentVertexData = m_pPathfindingModel->GetVertsData();
	SetModelDrawData(m_pPathfindingModel->GetNumVerts(), sizeof(MD2Vertex), currentVertexData);
	delete[] currentVertexData;
		

	

	//Get Window
	//Get Mouse Pos
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);
	m_pLocationRaycaster->Update(m_cameraMatrix,m_projectionMatrix,xpos,ypos);

	#pragma region Log and Demo Window Showing

	static bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);
	Application_Log* log = Application_Log::Get();
	if (log != nullptr && show_demo_window)
	{
		log->showLog(&show_demo_window);
	}
	//show application log window
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS) {
		show_demo_window = !show_demo_window;
	}
	// quit our application when escape is pressed
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();

	#pragma endregion

}

//Draw App
void PathfindingApp::Draw()
{
	PreDraw();
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);

	m_pLocationRaycaster->Draw();
	m_pMaze->DrawMaze();
	DrawModel(m_pPathfindingModel->GetNumVerts());

	if (m_bDrawPath) {
		m_pMaze->DrawPath(m_path);
	}
		
}

//Destroy Allocated memory from app
void PathfindingApp::Destroy()
{
	if (m_pPathfindingModel) {
		delete m_pPathfindingModel;
	}
	if (m_pMaze) {
		delete m_pMaze;
	}
	if (m_pLocationRaycaster) {
		delete m_pLocationRaycaster;
	}
	Gizmos::destroy();
}

