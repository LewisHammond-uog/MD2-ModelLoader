#ifndef  __MD2Pathfinder_H__
#define __MD2Pathfinder_H__

#include <glm/ext.hpp>

//Project includes
#include "md2_loader.h"
#include "PathfindingObject.h"

//Predefines
struct GLFWwindow;

class MD2Pathfinder : public PathfindingObject {

public:

	//Enum to increase or decrease skin number
	typedef enum{
		ATTRIBUTE_CHANGE_DIRECTION_INCREASE,
		ATTRIBUTE_CHANGE_DIRECTION_DECREASE,

		ATTRIBUTE_CHANGE_DIRECTION_COUNT /*Total number of states*/
	} ATTRIBUTE_CHANGE_DIRECTION;

	//Constructors / Destructors
	MD2Pathfinder(const char* a_modelFilename);
	MD2Pathfinder(const char* a_modelFilename, glm::vec3 a_pos);
	MD2Pathfinder(const char* a_modelFilename, glm::vec3 a_pos, std::vector<Position>* a_path);

	void Update(float a_fDeltaTime);

	//Reset Function
	void SetPosition(glm::vec3 a_pos);
	void StopPath();

	void ChangeSkin(ATTRIBUTE_CHANGE_DIRECTION a_eChangeDirection);
	void ChangeSkin(int a_iSkinID);

	void ChangeAnimation(ATTRIBUTE_CHANGE_DIRECTION a_eChangeDirection);
	void ChangeAnimation(int a_iSkinID);
	
	MD2Vertex* GetVertsData() const;
	float GetNumVerts() const;
	unsigned int GetTextureID() const;

private:

	bool LoadModel(const char * a_modelFilename); //Function to load model
	void Animate(float a_fDeltaTime);


	//Model to move around the world
	MD2Model* m_pModel;
	int m_iCurrentSkinIndex = 0;

	//Scale of the model and offset so that it stits at 0,0,0
	float m_fModelScale = 0.02f;
	const glm::vec3 m_modelOffset = glm::vec3(-9.5f, 0.5f, -9.5f);

	//Animation Vars

	//ENUM FOR ANIMATION STATE
	enum ANIMATION_STATE {
		ANIMATION_STATE_IDLE = 0,
		ANIMATION_STATE_IDLE_LOOK_AT_GUN,
		ANIMATION_STATE_WALK,
		ANIMATION_STATE_RUN,
		ANIMATION_STATE_ATTACK_A,
		ANIMATION_STATE_ATTACK_B,
		ANIMATION_STATE_HURT_CROUCH,
		ANIMATION_STATE_HURT_SHOT,
		ANIMATION_STATE_DEAD,
		ANIMATION_STATE_DUCK,

		ANIMATION_STATE_COUNT
	};

	//Animation List
	// {start frame , end frame}
	MD2Animation m_aModelAnimationList[10]{
		{0  , 29}, //IDLE
		{30 , 68}, //IDLE, LOOK AT GUN
		{69 , 92}, //WALK
		{94 , 108}, //RUN
		{110, 128}, //ATTACK A
		{129, 158}, //ATTACK B
		{159, 176}, //HURT CROUCH
		{177, 189}, //HURT SHOT
		{188, 200}, //DEAD
		{201, 209}, //DUCK
	};

	int m_iCurrentFrameIndex = 0;
	int m_iNextFrameIndex = 1;
	float m_fInterpolation = 0;
	int m_iStartFrame = 0;
	int m_iEndFrame = 29;
	const float mc_fAnimtationSpeed = 10.f;
	const bool m_bForceWalkAnimationWhenMoving = true;
	bool m_bAnimationLocked = false;
	ANIMATION_STATE m_eAnimationState = ANIMATION_STATE_IDLE;
	ANIMATION_STATE m_eAnimationStateLastFrame = ANIMATION_STATE_IDLE;
	ANIMATION_STATE m_ePreWalkingAnimationState = ANIMATION_STATE_IDLE;

	MD2Vertex* m_pCurrentVertexData;

};

#endif // ! __MD2Pathfinder_H__