#include "texture_manager.h"

//Project Includes
#include "texture.h"
#include "Manager.h"


/// <summary>
/// Default Empty Constructor
/// </summary>
TextureManager::TextureManager() : Manager<TextureManager>()
{
	
}


/// <summary>
/// Destructor, clear dictonary
/// </summary>
TextureManager::~TextureManager()
{
	m_pTextureDictonary.clear();
}

/// <summary>
/// Checks if a texture already exists in memory, if it does then returns
/// that texture, and increases it
/// </summary>
/// <param name="a_pTextureName">Name of texture to load</param>
/// <returns>Texture ID</returns>
unsigned int TextureManager::LoadTexture(const char * a_pTextureName)
{
	Texture* pTexture = nullptr; //Create a pointer to store the texture

	//If we haven't passed a null pointer
	if (a_pTextureName != nullptr) {

		//Go thorugh the dictonary looking for our texture refrence from our texture name
		std::map<std::string, TextureRef>::iterator dictonaryIter = m_pTextureDictonary.find(a_pTextureName);

		//If we are not at the end of the dictonary, then the texture
		//exists in the dictonary
		if (dictonaryIter != m_pTextureDictonary.end()) {
			//We have found the texture in the dictonary return a ref to it
			//and increase how many times we have refrenced the texture
			TextureRef& texture = (TextureRef&)(dictonaryIter->second);
			pTexture = texture.pTexture;
			++texture.refCount;
		}
		else {
			//This texture is not in memory so we need to load it in
			pTexture = new Texture(a_pTextureName);
			TextureRef texRef = { pTexture,1 };
			m_pTextureDictonary[a_pTextureName] = texRef;
		}

	}

	return pTexture->GetHandle(); //Returns the texture, might be a nullptr

}

/// <summary>
/// Removes a specific the texture from memory, once a texture refcount
/// is less than zero we completly remove that texture
/// </summary>
/// <param name="a_ptexture">Pointer to the texture</param>
void TextureManager::RemoveTexture(Texture* a_ptexture)
{
	//Itterate through the dictonary
	std::map< std::string, TextureRef>::iterator dictonaryIter = m_pTextureDictonary.begin();

	//For each time this texture occours in the dictonary
	for (; dictonaryIter != m_pTextureDictonary.end(); ++dictonaryIter) {
		
		//Get the current texture refrence
		TextureRef& texRef = (TextureRef&)(dictonaryIter->second);

		//If this texture is the texture we are looking to remove
		if (a_ptexture == texRef.pTexture) {

			//Reduce the Ref Count for this texture
			--texRef.refCount;

			//If the refrence count is less than or equal to zero,
			//completly delete the texture from memory
			if (texRef.refCount <= 0) {

				delete texRef.pTexture; //Delete the texture
				texRef.pTexture = nullptr; //Set the pointer to a null pointer
				m_pTextureDictonary.erase(dictonaryIter); //Erase it from the dictonary
				break;

			}
		}
	}
}

/// <summary>
/// Checks if a texture exists in already
/// </summary>
/// <param name="a_pTextureName">Name of texture to search for</param>
/// <param name="a_texture">Texture pointer to pass texture back to</param>
/// <returns>If texture exists</returns>
bool TextureManager::TextureExists(const char* a_pTextureName, Texture * a_texture) const
{
	//Try and fine the texture in the list
	auto dictonaryIter = m_pTextureDictonary.find(a_pTextureName);

	//If we didn't reach the end of the dictonary
	if (dictonaryIter != m_pTextureDictonary.end()) {
		//We have found the texture in the dictonary return true
		TextureRef& texRef = (TextureRef&)(dictonaryIter->second);
		a_texture = texRef.pTexture;
		return true;
	}

	return false;
}
