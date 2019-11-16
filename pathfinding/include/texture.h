#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

//C Includes
#include <string>

class Texture
{
public:

	//Enu to describe image format
	typedef enum Format : unsigned int {
		NONE = 0,
		GREY,
		GREY_ALPHA,
		RGB,
		RBGA
	} Format;

	//Constructors
	Texture();
	Texture(const char* a_filename);

	//Destructor
	~Texture();

	//Load and Unload functions
	bool Load(const char* a_filename);
	bool Unload();

	//Getters 
	unsigned int GetHandle() const { return m_glHandle; }
	const std::string& GetFilename() const { return m_filename; }
	void GetSize(unsigned int &iWidth, unsigned int &iHeight) const;
	unsigned int GetWidth() const { return m_iWidth; }
	unsigned int GetHeight() const { return m_iHeight; }
	unsigned int GetFormat() const { return m_glHandle; }


private:

	//Private Vars
	std::string m_filename;
	unsigned int m_glHandle;
	Format m_format;
	int m_iWidth;
	int m_iHeight;

};

#endif // !__TEXTURE_H__