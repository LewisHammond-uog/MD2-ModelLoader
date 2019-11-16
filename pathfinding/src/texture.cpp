#include "texture.h"

//Project Includes
#include "pcx_loader.h"

//Include OpenGL Methods
#include <glad/glad.h>
#include <GLFW/glfw3.h>


/// <summary>
/// Default Constructor, should NOT be used
/// </summary>
Texture::Texture()

	//Initalise Properties
	: m_filename("none"), m_glHandle(0), m_iWidth(0), m_iHeight(0), m_format(NONE)
	
{
}

/// <summary>
/// Constructor with file name
/// </summary>
/// <param name="a_filename">File Name</param>
Texture::Texture(const char* a_filename)
	: m_filename(a_filename), m_glHandle(0), m_iWidth(0), m_iHeight(0), m_format(NONE)
{

	//Check that we have a file name, if we do then load in the file
	if (a_filename != NULL) {
		Load(a_filename);
	}

}

/// <summary>
/// Destructor, delete textures
/// </summary>
Texture::~Texture()
{
	//If GL Handle is not 0 (uninitalised, delete the texture)
	if (m_glHandle != 0) {
		glDeleteTextures(1, &m_glHandle);
	}
}


/// <summary>
/// Load Texture from file
/// </summary>
/// <param name="a_filename">File to read from</param>
/// <returns>Success of texture loading</returns>
bool Texture::Load(const char * a_filename)
{
	//If we have a glHandle already then we have an image loaded already, 
	//we need to unload that image
	if (m_glHandle != 0) {
		glDeleteTextures(1, &m_glHandle); //Delete Texture
		m_glHandle = 0; //Reset GL Handle
		m_filename = "none"; //Reset File Name
	}

	//Load PCX Image Data
	PCX_Image pcx;
	if (pcx.Load(a_filename)) {

		//Get the PCX Data
		PCX_Data* imageData = pcx.GetData();

		//Format for Open GL Bytes
		unsigned int glFormat = 0;

		//Set GL Format based on the PCX Image Format
		switch (imageData->dataType) {
		case PCX_ImageFormat::GREY:
			glFormat = GL_LUMINANCE;
			break;
		case PCX_ImageFormat::RGB:
			glFormat = GL_RGB;
			break;
		case PCX_ImageFormat::RGBA:
			glFormat = GL_RGBA;
			break;
		}

		//Pixel data has now been loaded as bytes from the CPU/memeory
		//this is now ready to be pass to the GPU (OpenGL)
		//dump this data in to an Open GL Texture
		glGenTextures(1, &m_glHandle);

		glBindTexture(GL_TEXTURE_2D, m_glHandle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, glFormat, imageData->width, imageData->height, 0, glFormat, GL_UNSIGNED_BYTE, imageData->data);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_filename = a_filename;

		return true;
	}

	return false;
}

/// <summary>
/// Unloads the current texture
/// </summary>
/// <returns>Success of unloading</returns>
bool Texture::Unload()
{
	//If we have a texture defined
	if (m_glHandle != 0) {
		//Delete textures
		glDeleteTextures(1, &m_glHandle);
		return true;
	}

	return false;
}

/// <summary>
/// Gets the size of the texture
/// </summary>
/// <param name="iWidth"></param>
/// <param name="iHeight"></param>
void Texture::GetSize(unsigned int & iWidth, unsigned int & iHeight) const
{
	iWidth = m_iWidth;
	iHeight = m_iHeight;
}