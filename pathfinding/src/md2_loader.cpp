#include "md2_loader.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include "md2_Normals.h"
#include "texture_manager.h"

/// <summary>
/// Create a model shell with no model loaded
/// </summary>
MD2Model::MD2Model() {
	m_pModel = nullptr;
}

/// <summary>
/// Destroy allocated memory
/// </summary>
MD2Model::~MD2Model() {

	if (m_pModel != nullptr) {
		delete[] m_pModel->m_textureID;

		for (int i = 0; i < m_pModel->m_header.num_frames; ++i) {
			delete[] m_pModel->m_pFrames[i].verts;
		}

		delete[] m_pModel->m_pSkins;
		delete[] m_pModel->m_pTextcoords;
		delete[] m_pModel->m_pTriangles;
		delete[] m_pModel->m_pFrames;
		delete[] m_pModel->m_glcmds;
	}

	delete m_pModel;
	m_pModel = nullptr;
}

/// <summary>
/// Load in MD2 model data
/// </summary>
/// <param name="a_filename">Name of model file to load</param>
/// <param name="a_fScale">Scale to make model at</param>
/// <returns></returns>
bool MD2Model::Load(const char* a_filename, float a_fScale) {

	std::fstream file;

	//Open the file for reading
	file.open(a_filename, std::ios::in | std::ios::binary);

	if (file.is_open()) {
		//Get size of the file to ensure that its not empty
		file.ignore(std::numeric_limits<std::streamsize>::max());
		std::streamsize fileLength = file.gcount();
		file.clear();

		file.seekg(0, std::ios_base::beg);

		if (fileLength == 0) {
			std::cout << "File Containts No Data" << std::endl;
			return false;
		}

		m_pModel = new MD2Mesh();

		//Read Header
		file.read((char*)(&m_pModel->m_header), sizeof(MD2Header));

		if ((m_pModel->m_header.ident != 844121161) || (m_pModel->m_header.version != 8)) {
			//Error Wrong File
			std::cout << "This is not a valid MD2 file" << std::endl;
			file.close();
			return false;
		}

 		MD2Header* header = &m_pModel->m_header;

		//Allocate memory
		m_pModel->m_pSkins = new MD2Skin[header->num_skins];
		m_pModel->m_pTextcoords = new MD2TextCoord[header->num_st];
		m_pModel->m_pTriangles = new MD2Triangle[header->num_tris];
		m_pModel->m_pFrames = new MD2Frame[header->num_frames];
		m_pModel->m_glcmds = new int[header->num_glcmds];

		//Read in Model
		file.seekg(header->offset_skins, std::ios_base::beg);
		file.read((char*)(m_pModel->m_pSkins), sizeof(MD2Skin)*header->num_skins);

		//Read in Texture Coordinate Data
		file.seekg(header->offset_st, std::ios_base::beg);
		file.read((char*)(m_pModel->m_pTextcoords), sizeof(MD2TextCoord) * header->num_st);

		//Raed in Triangle Data
		file.seekg(header->offset_tris, std::ios_base::beg);
		file.read((char*)(m_pModel->m_pTriangles), sizeof(MD2Triangle) * header->num_tris);

		//Read in glCmnds
		file.seekg(header->offset_glcmds, std::ios_base::beg);
		file.read((char*)(m_pModel->m_glcmds), sizeof(int) * header->num_glcmds);

		//Raed in Frames
		file.seekg(header->offset_frames, std::ios_base::beg);
		for (int i = 0; i < header->num_frames; ++i) {
			//Allocate Memory for the verticies
			m_pModel->m_pFrames[i].verts = new MD2CompressedVertex[header->num_vertices];
			//read frame data in to vertex
			MD2Frame* frame = &m_pModel->m_pFrames[i];
			file.read((char*)(&(frame->scale)), sizeof(glm::vec3));
			file.read((char*)(&(frame->translate)), sizeof(glm::vec3));
			file.read((char*)(frame->name), sizeof(char) * 16);
			file.read((char*)(frame->verts), sizeof(MD2CompressedVertex) * header->num_vertices);
		}

		file.sync();
		file.close();
		file.clear();

		m_pModel->m_textureID = new unsigned int[header->num_skins];

		//acquire texture manager
		TextureManager* pTM = TextureManager::GetInstance();
		//Load the texture for this model
		for (int i = 0; i < header->num_skins; ++i) {
			std::string textureName = m_pModel->m_pSkins[i].name;
			//Remove any quoteation marks present and strip out the texture
			textureName.erase(std::remove(textureName.begin(), textureName.end(), '\"'), textureName.end());

			//
			if (textureName.find_last_of("\\/") != std::string::npos) {
				textureName = textureName.substr(textureName.find_last_of("\\/"), textureName.length());
			}

			std::string texturePath(a_filename);
			if (texturePath.find_last_of("\\/") != std::string::npos) {
				texturePath = texturePath.substr(0, texturePath.find_last_of("\\/"));
			}
			else {
				//We have not path
				texturePath = "";
			}

			texturePath = texturePath + textureName;

			//Call TEXTURE LOAD FUNCTION HERE, have it return the texture for the model, assign it to texture ID
			if (pTM != nullptr) // if we have a texture manager then load teh texture...
			{
				m_pModel->m_textureID[i] = pTM->LoadTexture(texturePath.c_str());
			}
		}

		//Set scale
		m_fScale = a_fScale;

	}
	else {
		std::cout << "Unable to open file for reading" << std::endl;
		return false;
	}

	return true;
}

/// <summary>
/// Gets the texture ID of a given skin for this model
/// </summary>
/// <param name="a_iSkinID">Skin ID to load</param>
/// <returns>Texture ID of Skin Texture</returns>
unsigned int MD2Model::GetTextureID(int a_iSkinID) {
	
	//if we have a number that is over the number of skins then return
	//the skin with the modulus of the number so we always return a valid 
	//skin
	return m_pModel->m_textureID[abs(a_iSkinID % m_pModel->m_header.num_skins)];
}

/// <summary>
/// Gets the number of verticies in this model
/// </summary>
/// <returns>Number of verts</returns>
unsigned int MD2Model::GetNumVerts() {
	if (m_pModel != nullptr) {
		return m_pModel->m_header.num_tris * 3;
	}
	return 0;
}

/// <summary>
/// Gets the interpolated animation data for this model in its current frame of 
/// animation
/// </summary>
/// <param name="a_iCurrentFrameID">Current Frame of the animation</param>
/// <param name="a_iNextFrameID">Next Frame of the animation</param>
/// <param name="a_fInterpAmount">Amount to interpolate between frames</param>
/// <param name="a_pos">Position to render model at</param>
/// <returns>Vertex Data of model interpolated between two frames</returns>
MD2Vertex * MD2Model::GetInterpolatedData(const unsigned int a_iCurrentFrameID, const unsigned int a_iNextFrameID, const float a_fInterpAmount, const glm::vec3 a_pos)
{

	//Check that we actually have a model loaded in to memory
	if (m_pModel != nullptr) {
		//Get the data of the current frame and next frame
		MD2Vertex* currentFrameData = GetVertexBufferData(a_iCurrentFrameID, a_pos);
		MD2Vertex* nextFrameData = GetVertexBufferData(a_iNextFrameID, a_pos);

		//Create new data to store interpolated data
		MD2Vertex* interpolatedFrameData = new MD2Vertex[m_pModel->m_header.num_tris * 3];

		//Null check data
		if (currentFrameData != nullptr && nextFrameData != nullptr) {
			for (int i = 0; i < m_pModel->m_header.num_tris; ++i) {
				for (int j = 0; j < 3; ++j) {
					int iOffset = (i * 3);
					//Get the linear-ly interpolated data between the current and next frame for
					//postion, normals and colour
					interpolatedFrameData[iOffset + j].position.x = currentFrameData[iOffset + j].position.x + (a_fInterpAmount * (nextFrameData[iOffset + j].position.x - currentFrameData[iOffset + j].position.x));
					interpolatedFrameData[iOffset + j].position.y = currentFrameData[iOffset + j].position.y + (a_fInterpAmount * (nextFrameData[iOffset + j].position.y - currentFrameData[iOffset + j].position.y));
					interpolatedFrameData[iOffset + j].position.z = currentFrameData[iOffset + j].position.z + (a_fInterpAmount * (nextFrameData[iOffset + j].position.z - currentFrameData[iOffset + j].position.z));
					interpolatedFrameData[iOffset + j].position.w = currentFrameData[iOffset + j].position.w + (a_fInterpAmount * (nextFrameData[iOffset + j].position.w - currentFrameData[iOffset + j].position.w));
					interpolatedFrameData[iOffset + j].normal = currentFrameData[iOffset + j].normal + (a_fInterpAmount * (nextFrameData[iOffset + j].normal - currentFrameData[iOffset + j].normal));
					interpolatedFrameData[iOffset + j].colour = currentFrameData[iOffset + j].colour + (a_fInterpAmount * (nextFrameData[iOffset + j].colour - currentFrameData[iOffset + j].colour));

					//Get the texture cordinates
					interpolatedFrameData[iOffset + j].texCoord1 = currentFrameData[iOffset + j].texCoord1;
				}
			}
			//Free allocated memory
			delete[] currentFrameData;
			delete[] nextFrameData;

			//Return thge data
			return interpolatedFrameData;
		}

		return nullptr;
	}

	return nullptr;

}

/// <summary>
/// Gets the raw vertex buffer data for a given frame and postion
/// </summary>
/// <param name="a_frame">Key frame of aninmation to display</param>
/// <param name="a_pos">Postion to render model at</param>
/// <returns>Vertex data of model at given frame and pos</returns>
MD2Vertex* MD2Model::GetVertexBufferData(const unsigned int a_frame, const glm::vec3 a_pos) {

	if (m_pModel != nullptr) {

		//used in calculating UV coordinates
		float invSkinWidth = 1.f / m_pModel->m_header.skinwidth;
		float invSkinHeight = 1.f / m_pModel->m_header.skinheight;

		//Can Change 0 for ohter frame info
		unsigned int fID = (a_frame >= m_pModel->m_header.num_frames) ? (a_frame % m_pModel->m_header.num_frames) : a_frame;
		MD2Frame* pCurrentFrame = &m_pModel->m_pFrames[fID];
		MD2Vertex* pVertexBufferData = new MD2Vertex[m_pModel->m_header.num_tris*3];

		glm::mat4 model = glm::mat4(1.0f);

		for (int i = 0; i < m_pModel->m_header.num_tris; ++i) {
			//For each triangle in the model
			MD2Triangle currentTri = m_pModel->m_pTriangles[i];
			//For each vertex in the triangle
			for (int j = 0; j < 3; ++j) {
				MD2CompressedVertex* pV = &pCurrentFrame->verts[currentTri.vertex[j]];
				//Calculate the real postion of the vertex
				int iOffset = (i * 3);
				pVertexBufferData[iOffset + j].position.x = (pCurrentFrame->scale.x * m_fScale * pV->v[0]) + (pCurrentFrame->translate.x * m_fScale) + a_pos.x;
				pVertexBufferData[iOffset + j].position.y = (pCurrentFrame->scale.z * m_fScale * pV->v[2]) + (pCurrentFrame->translate.z * m_fScale) + a_pos.y;
				pVertexBufferData[iOffset + j].position.z = (pCurrentFrame->scale.y * m_fScale * pV->v[1]) + (pCurrentFrame->translate.y * m_fScale) + a_pos.z;
				pVertexBufferData[iOffset + j].position.w = 1.f;
				pVertexBufferData[iOffset + j].normal = glm::vec4(precalculated_normals[pV->normalIndex], 0.f);
				pVertexBufferData[iOffset + j].colour = glm::vec4(1.f, 1.f, 1.f, 1.f);

				//Calculate texture coordinates
				MD2TextCoord& textCoord = m_pModel->m_pTextcoords[currentTri.st[j]];
				pVertexBufferData[iOffset + j].texCoord1 = glm::vec2((float)(textCoord.s * invSkinWidth), (float)(textCoord.t * invSkinHeight));
			}
		}

		return pVertexBufferData;
	}

	return nullptr;

}
