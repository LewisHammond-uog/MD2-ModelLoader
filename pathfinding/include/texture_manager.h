#pragma once

#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

//Project Includes
#include "Manager.h"

//C Includes
#include <map>
#include <string>

//Forward Declerations
class Texture;

class TextureManager : public Manager<TextureManager> {
	friend class Manager<TextureManager>;
public:

	bool TextureExists(const char* a_filename, Texture* a_texture) const;
	unsigned int LoadTexture(const char* a_pTextureName);
	Texture* CreateTexture(const char* a_pName, unsigned int a_width, unsigned int a_height,
		unsigned char* a_pixelData = nullptr, unsigned int a_format = 1);
	void RemoveTexture(Texture* a_texture);

protected:
	//Constructor and Destructor
	TextureManager();
	~TextureManager();

private:

	//Texture Ref counts the number of times the texture is attempted to be
	//loaded and then this count falls to zero then the texture is being 
	//dereferenced and only then will it be unloaded from memory
	struct TextureRef {
		Texture* pTexture;
		unsigned int refCount;
	};

	std::map<std::string, TextureRef> m_pTextureDictonary;
};


#endif // ! __TEXTURE_MANAGER_H__

