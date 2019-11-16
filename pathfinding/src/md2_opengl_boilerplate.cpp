#include "PathfindingApp.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>


void PathfindingApp::InitBoilerplateGL()
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);
	glViewMinW = m_windowWidth;
	glViewMinH = m_windowHeight;

	//Set up the fill screen render and the shaders to use with this view
	//Set up the Vertex Array Object and Shaders for our full screen quad
	//Load the shaders for this program
	m_vertexShader = Utility::loadShader("./shaders/vertex.glsl", GL_VERTEX_SHADER);
	m_fragmentShader = Utility::loadShader("./shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	//Define the input and output varialbes in the shaders
	//Note: these names are taken from the glsl files
	const char* szInputs[] = { "Position", "Normal", "Colour","Tex1" };
	const char* szOutputs[] = { "FragColor" };
	//bind the shaders to create our shader program
	m_program = Utility::createProgram(
		m_vertexShader,
		0,
		0,
		0,
		m_fragmentShader,
		4, szInputs, 1, szOutputs);

	//Generate our OpenGL Vertex and Index Buffers for rendering our FBX Model Data
	// OPENGL: generate the VBO, IBO and VAO
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);

	// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	//There is no need to populate the vbo & ibo buffers with any data at this stage
	//this can be done when rendering each mesh component of the FBX model

	// enable the attribute locations that will be used on our shaders
	glEnableVertexAttribArray(0); //Pos
	glEnableVertexAttribArray(1); //Norm
	glEnableVertexAttribArray(2); //Colour
	glEnableVertexAttribArray(3); //Tex1


								  // tell our shaders where the information within our buffers lie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MD2Vertex), ((char *)0) + MD2Vertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(MD2Vertex), ((char *)0) + MD2Vertex::NormalOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(MD2Vertex), ((char *)0) + MD2Vertex::ColourOffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(MD2Vertex), ((char *)0) + MD2Vertex::TexCoord1Offset);

	// finally, where done describing our mesh to the shader
	// we can describe the next mesh
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void PathfindingApp::UpdateBoilerplateGL(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);

	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);

	// clear all gizmos from last frame
	Gizmos::clear();

	// add an identity matrix gizmo
	Gizmos::addTransform(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
	// add a 20x20 grid on the XZ-plane
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),
			i == 10 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));

		Gizmos::addLine(glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),
			i == 10 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void PathfindingApp::PreDraw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

	//bing our shader program
	glUseProgram(m_program);
	//bind our vertex array object
	glBindVertexArray(m_vao);

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	unsigned int projectionViewUniform = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionMatrix * viewMatrix));

	//pass our camera position to our fragment shader uniform
	unsigned int cameraPosUniform = glGetUniformLocation(m_program, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(m_cameraMatrix[3]));


	//pass the directional light direction to our fragment shader
	m_lightPos = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	glm::vec4 lightDir = -m_lightPos;
	lightDir.w = 0.f;
	lightDir = glm::normalize(lightDir);
	unsigned int lightDirUniform = glGetUniformLocation(m_program, "lightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(lightDir));



	//Draw our MD2 Model

	glm::mat4 m4Model = glm::mat4();
	unsigned int modelUniform = glGetUniformLocation(m_program, "Model");
	glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));

}

void PathfindingApp::DrawModel(unsigned int a_numVerts)
{
	//bind our shader program
	glUseProgram(m_program);
	//bind our vertex array object
	glBindVertexArray(m_vao);

	// Bind the texture to one of the ActiveTextures
	// if your shader supported multiple textures, you would bind each texture to a new Active Texture ID here
	//bind our textureLocation variable from the shaders and set it's value to 0 as the active texture is texture 0
	unsigned int texUniformID = glGetUniformLocation(m_program, "DiffuseTexture");
	glUniform1i(texUniformID, 0);
	//set our active texture, and bind our loaded texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_currTexID);


	glDrawArrays(GL_TRIANGLES, 0, a_numVerts);


	glBindVertexArray(0);
	glUseProgram(0);

	// draw the gizmos from this frame
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void PathfindingApp::SetModelTextureID(unsigned int a_TextureID)
{
	m_currTexID = a_TextureID;
}

void PathfindingApp::SetModelDrawData(unsigned int a_numVertices, unsigned int a_vertexSize, void* a_vertexData)
{
	// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	// Send the vertex data to the VBO
	glBufferData(GL_ARRAY_BUFFER, a_numVertices * a_vertexSize, a_vertexData, GL_STATIC_DRAW);

	// finally, where done describing our mesh to the shader
	// we can describe the next mesh
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}