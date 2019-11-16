#include "pcx_loader.h"

#include <iostream>
#include <fstream>
#include <cstring>

/// <summary>
/// Create empty PCX Image
/// </summary>
PCX_Image::PCX_Image()
{
	m_vgaPallet = nullptr;
}

PCX_Image::~PCX_Image()
{

}

/// <summary>
/// Load in a pxc Image
/// </summary>
/// <param name="a_filename"></param>
/// <returns></returns>
bool PCX_Image::Load(const char* a_filename)
{
	//get an fstream for loading in a file
	std::fstream file;

	file.open(a_filename, std::ios_base::in | std::ios_base::binary);
	if (file.is_open())
	{
		file.ignore(std::numeric_limits<std::streamsize>::max()); //Ignores the maximum number that file stream can in take
		std::streamsize filelength = file.gcount(); //Get the length of the file
		file.clear(); // as ignore sets End of file flag
		file.seekg(0, std::ios_base::beg); //Set the file reader flag to the start of the file

		//If the file is empty then return false the inform of error
		if (filelength == 0)
		{
			std::cout << "File constains no data!!" << std::endl;
			return false;
		}

		
		file.read((char*)(&m_header), sizeof(PCXHeader)); //Read in the header infomation from the file and put it in to m_header

		//If the file manufacturer indentitidifer is wrong or the enconding infomation is wrong return false and inform of an error
		if (m_header.manufacturer != PCX_VALID_HEADER)
		{
			std::cout << "This is not a valid PCX Image" << std::endl;
			file.close();
			return false;
		}
		if (m_header.encoding != PCX_RLE_ENCODING)
		{
			std::cout << "This file is not encoded with RLE, not a valid PCX file format" << std::endl;
			file.close();
			return false;
		}

		//get palette information if present
		//PCX version 3.0 and on there is a possible 256 colour palette 769 bytes from end of the file
		if (m_header.version == 5)
		{
			file.seekg(PCX_VGA_PALETTE_OFFSET, std::ios_base::end); //Go to the end of the file and go back by the pallet offset
			char paletteIndicator = 0;// Initalise var for determining if there is a pallet 0

			file.read(&paletteIndicator, 1); //Read 1 byte to see if there is a pallet at the end of the file

			if (paletteIndicator == 0xc) //If there is an idicator present (12) then start to get that pallet
			{
				m_vgaPallet = new PCXPaletteColour[256]; //Initalise var to store pallet for 256 colours
				file.read((char*)(m_vgaPallet), 256 * sizeof(PCXPaletteColour)); //Read in the pallet from the file
				file.clear(); //Clear the file stream
				file.seekg(0, std::ios_base::beg); //Reset the file reader to the start of the file (0 from the begining)
				file.seekg(sizeof(PCXHeader), std::ios_base::beg); //Set the file reader to after the header (the size of the header from the begining)
			}
		}

		//find the image size
		u16 width = m_header.dimensions.right - m_header.dimensions.left + 1; //Calculate the width of the image
		u16 height = m_header.dimensions.bottom - m_header.dimensions.top + 1; //Calculate the height of the image
		
		u8 pixelComponent = 1; //Initalise the pixel component var (stores the number of channels that make up a pixel)
		

		PCX_ImageFormat imageFormat = RGB; //Default the image format to RGB

		if (m_vgaPallet != nullptr) //If m_vgaPallet is not a nullptr (i.e we have found a pallet info in this file)
		{
			pixelComponent = 3; //Set the pixel component to 3, ie 3 channels RGB
		}
		else
		{
			//Work out how many channels to use based on the bits per pixel and number of colour planes
			pixelComponent = (m_header.bitsPerPixel == 8 && m_header.numColourPlanes == 1) ? 1 :
				(m_header.numColourPlanes == 3) ? 3 : 4;
			pixelComponent = (m_header.bitsPerPixel == 4 && m_header.numColourPlanes == 1) ? 3 :
				pixelComponent;
		}

		if (pixelComponent == 1)
		{
			imageFormat = GREY; //If we had 1 pixel component then the image is in greyscale
		}
		else if (pixelComponent == 4)
		{
			imageFormat = RGBA; //If we have 4 pixel components the we can have RGBA (Red, Green, Blue, Alpha)
		}

		m_rawData.dataType = imageFormat; //Set the data type of our image
		m_rawData.dataSize = m_header.bitsPerPixel * width * height * pixelComponent; //Set the size of our data
		m_rawData.width = width; //Add width to our data
		m_rawData.height = height; //Add height to our data
		m_rawData.data = malloc(m_rawData.dataSize); //Allocate enough data for our image

		u8* imageU8Ptr = (u8*)(m_rawData.data); //Set a pointer to the data and cast it as an unsigned 8 bit number

		for (int h = 0; h < height; ++h) //For the height of the image
		{
			u8 value = 0; //Reset the value (colour read)
			u8 freq = 0; //and frequency (number of time that colour occours)


			for (int cp = 0; cp < m_header.numColourPlanes; ++cp) //For each of the colour planes (channels)
			{
				u8* imgData = imageU8Ptr + (h * m_header.bytesPerScanline * pixelComponent); //Set up a image data pointer to add on the height we are at + bytes per scanline + the number of channels in the image
				imgData += cp; //Increase the image data pointer by the number of colour channels

				for (u32 i = 0; i < m_header.bytesPerScanline; i++) //For each byte in this scan line
				{
					//This section decodes the run length encoding of the image and itterates through it until we have placed all of the pixels of the same colour

					if (freq == 0) //If we the frequency of a colour is 0 (i.e default)
					{
						freq = DecodeRLEData(&file, value); //Decode the Run Length Encoding (to update the frequency of the colour)
					}
					--freq; //As we place new pixels decrease the frequnency


					//pcx images may have padding as row can be of even length and images odd sizes
					if (i < width) //If have not reached the end
					{
						imgData[0] = value; //Set the image data index 0 to the raw data from the RLE
						if (m_header.numColourPlanes == 1 && pixelComponent == 3) //If we have 1 colour plane and 3 pixel components (RGB)
						{
							PCXPaletteColour pCol = (m_vgaPallet != nullptr) ? m_vgaPallet[value] : m_header.colourPallet[value]; //If we got the pallet from the end of the file then use that, else the colour pallet defined at the start
							imgData[0] = pCol.R; //Set the red channel
							imgData[1] = pCol.G; //Set the green channel
							imgData[2] = pCol.B; //Set the blue channel
							imgData += 3; //Advance the img data pointer along
						}
						else
						{
							imgData += pixelComponent; //else if we are not using rgb add the pixel component to the img data
						}
					}
				}
			}
		}


		//Clean up memory objects
		delete m_vgaPallet;

		file.close(); //Close the file
		return true; //We have read and decoded file successfully, return true
	}


	return false; //File read failure, return false
}
#define PCX_RLE_MASK 0xc0 // 1100 0000
#define PCX_RLE_FREQ_MASK 0x3F // 0011 1111

u8 PCX_Image::DecodeRLEData(void* a_stream, u8& a_value)
{
	std::fstream* file = (std::fstream*)(a_stream); //Open a file stream
	file->read((char*)(&a_value), 1); //Read a single byte from the file

	if ((a_value & PCX_RLE_MASK) == PCX_RLE_MASK) //Run a AND mask and see if this is equal to the RLE Mask, if this is true it means that there is a flag that run length encoding is used
	{
		//we have a run length, lower 6 bits tell us the frequency of this run
		//next byte provides the value of the palett or image channel colour
		u8 frequency = a_value & PCX_RLE_FREQ_MASK; //Run an and mask to get the frequency of the next colour byte
		file->read((char*)(&a_value), 1); //Get the colour for the frequency, this is passed by refrence so is passed back to the parameter var
		return frequency; //Return the frequency from the mask
	}
	return 1; 
}



PCX_Data* PCX_Image::GetData()
{
	return &m_rawData; //Return the raw data
}