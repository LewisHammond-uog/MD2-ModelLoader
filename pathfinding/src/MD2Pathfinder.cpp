#include "MD2Pathfinder.h"

#include "md2_loader.h"
#include "Application.h"
#include <GLFW/glfw3.h>
#include "Application_Log.h"

/// <summary>
/// Construct just model at 0,0,0
/// </summary>
/// <param name="a_modelFilename">File name of model to load</param>
MD2Pathfinder::MD2Pathfinder(const char * a_modelFilename) : PathfindingObject(glm::vec3(0, 0, 0))
{
	LoadModel(a_modelFilename);
}

/// <summary>
/// Create a model at a given postion
/// </summary>
/// <param name="a_modelFilename">File name of model to load</param>
/// <param name="a_pos">Postion to create model at</param>
MD2Pathfinder::MD2Pathfinder(const char * a_modelFilename, glm::vec3 a_pos) : PathfindingObject(a_pos)
{
	LoadModel(a_modelFilename);
}

/// <summary>
/// Create a model at a given position and start it on a path
/// </summary>
/// <param name="a_modelFilename">File name of model to load</param>
/// <param name="a_pos">Postion to create model at</param>
/// <param name="a_path">Path to follow</param>
MD2Pathfinder::MD2Pathfinder(const char * a_modelFilename, glm::vec3 a_pos, std::vector<Position>* a_path) : PathfindingObject(a_pos, a_path)
{
	LoadModel(a_modelFilename);
}

/// <summary>
/// Update Update Postion and frame data
/// </summary>
/// <param name="a_fDeltaTime"></param>
void MD2Pathfinder::Update(float a_fDeltaTime)
{
	FollowPath(a_fDeltaTime);
	Animate(a_fDeltaTime);

	//Change in to walking animation when walking and change back when 
	//we stop walking
	if (m_bForceWalkAnimationWhenMoving) {
		if (m_bFollowingPath && !m_bAnimationLocked) {
			//Store previous animation, change animation and set locked
			m_ePreWalkingAnimationState = m_eAnimationState;
			m_eAnimationState = ANIMATION_STATE_WALK;
			m_bAnimationLocked = true;
		}
		else if (!m_bFollowingPath && m_bAnimationLocked) {
			//If we have stopped walking unlock the animation
			m_eAnimationState = m_ePreWalkingAnimationState;
			m_bAnimationLocked = false;
		}
	}
}

/// <summary>
/// Sets the object to a given position
/// </summary>
/// <param name="a_pos">Position to set to</param>
void MD2Pathfinder::SetPosition(glm::vec3 a_pos)
{
	m_currentPostion = a_pos;
}

/// <summary>
/// Stops the current path
/// </summary>
void MD2Pathfinder::StopPath()
{
	m_bFollowingPath = false;
	m_path.clear(); //Empty the path
}

/// <summary>
/// Change the skin relative to the current skin selected, (i.e increase or decrease
/// currently selecteced skin)
/// </summary>
/// <param name="a_eChangeDirection"></param>
void MD2Pathfinder::ChangeSkin(ATTRIBUTE_CHANGE_DIRECTION a_eChangeDirection)
{

	//Change the current skin ID ++ or -- based on func parameters
	if (a_eChangeDirection == ATTRIBUTE_CHANGE_DIRECTION_INCREASE) {
		m_iCurrentSkinIndex++;
	}
	else if (a_eChangeDirection == ATTRIBUTE_CHANGE_DIRECTION_DECREASE) {
		m_iCurrentSkinIndex--;
	}
}

/// <summary>
/// Change the skin to a given skin id
/// </summary>
/// <param name="a_iSkinID"></param>
void MD2Pathfinder::ChangeSkin(int a_iSkinID)
{
	if (m_pModel = nullptr) {
		return;
	}

	m_iCurrentSkinIndex = a_iSkinID;

}

/// <summary>
/// Gets the verticies data for drawing the model
/// </summary>
/// <returns></returns>
MD2Vertex * MD2Pathfinder::GetVertsData() const
{
	return m_pCurrentVertexData;
}

/// <summary>
/// Gets the number of verticiles for drawing the model
/// </summary>
/// <returns></returns>
float MD2Pathfinder::GetNumVerts() const
{
	return m_pModel->GetNumVerts();
}

/// <summary>
/// Gets the texture ID from the model for the current skin that is 
/// selected
/// </summary>
/// <returns>Texture ID from the texture manager</returns>
unsigned int MD2Pathfinder::GetTextureID() const
{
	return m_pModel->GetTextureID(m_iCurrentSkinIndex);
}

/// <summary>
/// Loads in the MD2 Model
/// </summary>
/// <returns></returns>
bool MD2Pathfinder::LoadModel(const char * a_modelFilename)
{
	//Null check model and delete if model exists
	if (m_pModel != nullptr) {
		delete m_pModel;
	}

	//Load Model
	m_pModel = new MD2Model();
	if (m_pModel->Load(a_modelFilename, m_fModelScale)) {
		return true;
	}
	else {
		return false;
	}
}

/// <summary>
/// Changes the animation relative to its current state in a given direction
/// </summary>
/// <param name="a_eChangeDirection"></param>
void MD2Pathfinder::ChangeAnimation(ATTRIBUTE_CHANGE_DIRECTION a_eChangeDirection)
{
	//Check if we are walking and animation is forced when walking,
	//If so then don't change the animation
	if (m_bAnimationLocked) {
		return;
	}

	int iNewAnimationState = m_eAnimationState;

	//Change the current skin ID ++ or -- based on func parameters
	if (a_eChangeDirection == ATTRIBUTE_CHANGE_DIRECTION_INCREASE) {
		iNewAnimationState++;
	}
	else if (a_eChangeDirection == ATTRIBUTE_CHANGE_DIRECTION_DECREASE) {
		iNewAnimationState--;
	}

	//Make sure that animation number is within bounds
	if (iNewAnimationState < 0) {
		iNewAnimationState = ANIMATION_STATE_COUNT - 1;
	}
	else if (iNewAnimationState > ANIMATION_STATE_COUNT - 1) {
		iNewAnimationState = 0;
	}

	ChangeAnimation(iNewAnimationState);
}

/// <summary>
/// Changes animation to given ID
/// </summary>
/// <param name="a_iAnimationID"></param>
void MD2Pathfinder::ChangeAnimation(int a_iAnimationID)
{
	//Check if we are walking and animation is forced when walking,
	//If so then don't change the animation
	if (m_bAnimationLocked) {
		return;
	}

	m_eAnimationState = (ANIMATION_STATE)a_iAnimationID;
}

/// <summary>
/// Animate the model based on the current animation state
/// </summary>
void MD2Pathfinder::Animate(float a_fDeltaTime)
{
	//Check if the animation state has changed
	if (m_eAnimationState != m_eAnimationStateLastFrame) {
		//Change start and end frames
		m_iStartFrame = m_aModelAnimationList[m_eAnimationState].start;
		m_iEndFrame = m_aModelAnimationList[m_eAnimationState].end;
		//Set Current Frame
		m_iCurrentFrameIndex = m_iStartFrame;
		m_iNextFrameIndex = m_iStartFrame + 1;
	}

	//Increase Interpolation
	m_fInterpolation += mc_fAnimtationSpeed * a_fDeltaTime;

	//If interpolation is over 1, then change the frame
	if (m_fInterpolation >= 1) {

		m_iCurrentFrameIndex++;

		if (m_iCurrentFrameIndex == m_iEndFrame) {
			m_iNextFrameIndex = m_iStartFrame;
		}
		else if (m_iCurrentFrameIndex > m_iEndFrame) {
			m_iCurrentFrameIndex = m_iStartFrame;
			m_iNextFrameIndex = m_iStartFrame + 1;
		}
		else {
			m_iNextFrameIndex++;
		}

		m_fInterpolation = 0.f;
	}

	//Get Data
	m_pCurrentVertexData = m_pModel->GetInterpolatedData(m_iCurrentFrameIndex, m_iNextFrameIndex, m_fInterpolation, m_currentPostion + m_modelOffset);

	//Set var for animation state checking
	m_eAnimationStateLastFrame = m_eAnimationState;
}


