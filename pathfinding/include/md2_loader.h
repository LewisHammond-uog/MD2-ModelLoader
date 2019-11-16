#pragma once

#include <glm/glm.hpp>

#ifndef __MD2_LOADER_H__
#define __MD2_LOADER_H__

class MD2Vertex
{
public:

	enum VertexAttributeFlags
	{
		POSITION = (1 << 0), //The Position of the Vertex
		NORMAL = (1 << 1), //The Normal for the Vertex
		COLOUR = (1 << 2), //The Colour for the vertex
		TEXCOORD1 = (1 << 3), //The UV Coordinates for the mesh
	};

	enum Offsets
	{
		PositionOffset = 0,
		NormalOffset = PositionOffset + sizeof(glm::vec4),
		ColourOffset = NormalOffset + sizeof(glm::vec4),
		TexCoord1Offset = ColourOffset + sizeof(glm::vec4),
	};

	MD2Vertex();
	~MD2Vertex();

	glm::vec4	position;
	glm::vec4	normal;
	glm::vec4	colour;
	glm::vec2	texCoord1;
};

inline MD2Vertex::MD2Vertex() : position(0, 0, 0, 1), normal(0, 0, 0, 0), colour(1, 1, 1, 1), texCoord1(0, 0) {}
inline MD2Vertex::~MD2Vertex() {}
typedef struct MD2Header
{
	int ident;				//Magic Number must be equal to "IDP2" 
							//(('2'<<24) + ('P'<<16) + ('D'<<8) + 'I')
	int version;			       //Must be equal to 8
	int skinwidth;			//texture width
	int skinheight;			//texture height
	int framesize;			//size in bytes of a frame
	int num_skins;			//number of skins
	int num_vertices;		//number of vertices per frame
	int num_st;				//number of texture co-ordinates
	int num_tris;			//number of triangles
	int num_glcmds;			//number of OpenGL commands
	int num_frames;			//number of frames
	
	int offset_skins;		//offset to skin data
	int offset_st;			//offset to texture data
	int offset_tris;		//offset to triangle data
	int offset_frames;		//offset to frame data
	int offset_glcmds;		//offset to OpenGL command data
	int offset_end;			//offset to end of file
}MD2Header;

/*Texture name*/
typedef struct MD2Skin {
	char name[64];
}MD2Skin;

typedef struct MD2TextCoord {
	short s;
	short t;
}MD2TextCoord;

typedef struct MD2Triangle {
	unsigned short vertex[3];
	unsigned short st[3];
}MD2Triangle;

typedef struct MD2CompressedVertex {
	unsigned char v[3];
	unsigned char normalIndex;
}MD2CompressedVertex;

typedef struct MD2Frame {
	glm::vec3 scale;
	glm::vec3 translate;
	char* name[16];
	MD2CompressedVertex* verts;
};

typedef struct MD2GLCmd {
	float s;
	float t;
	int index;
}MD2GLCmd;

typedef struct MD2Mesh{
	MD2Header		m_header;
	MD2Skin*		m_pSkins;
	MD2TextCoord*	m_pTextcoords;
	MD2Triangle*	m_pTriangles;
	MD2Frame*		m_pFrames;
	int*			m_glcmds;
	unsigned int*	m_textureID;

}MD2Mesh;

typedef struct MD2Animation {
	unsigned int start;
	unsigned int end;
}MD2Animation;

class MD2Model
{
public:
	MD2Model();
	~MD2Model();

	bool Load(const char* a_filename, float a_scale);
	MD2Vertex* GetVertexBufferData(const unsigned int a_frame,const glm::vec3 a_pos);
	MD2Vertex* GetInterpolatedData(const unsigned int a_iCurrentFrameID, const unsigned int a_iNextFrameID, const float a_fInterpAmounnt, const glm::vec3 a_pos);
	unsigned int GetNumVerts();
	unsigned int GetTextureID(int a_iSkinID);

private:
	glm::mat4 m_globalTransform;
	float m_fScale;
	char* m_pmd2Filename;
	MD2Mesh* m_pModel;
	bool m_bHasAnimation;
};


#endif //__MD2_LOADER_H__
