#pragma once

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

#define PCX_VALID_HEADER 10
#define PCX_RLE_ENCODING 1
#define PCX_VGA_PALETTE_OFFSET -769

//\-----------------------------------------------------------------------------------------------------------------------------------
//\ PCX File header format
//\-----------------------------------------------------------------------------------------------------------------------------------
//byte		item			size		desc 
//0		manufacturer		1		this value should be 10
//1		version number		1		the version number
//2		encoding			1		always 1 nothing else was used.
//3		bits per pixel		1		num bpp
//4		window				8		dimensions in pixels top left, to bottom right (x,y)
//12	HRes				2		Horizontal resolution for printing
//14	VRes				2		Vertical resolution for printing
//16	palette				48		image RBG palette 16 colours
//64	reserved byte		1		must be zero
//65	num colour planes	1		number of colour planes 1, 3, 4 (greyscale, RGB, RGBA)
//66	bytes per row		2		number of bytes per row (pcx files always even)
//68	palette info		2		how to interpret palette 1 = colour/BW, 2 = greyscale
//70	HScreen Res			2		Horizonal resolution of screen image was created on
//72	VScreen Res			2		Vertical resolution of screen image was created on
//74	Padding				54		padding up to 128 bytes of data

struct PCXDimensions
{
	u16 left; u16 top; u16 right; u16 bottom;
};

struct PCXPaletteColour
{
	u8 R; u8 G; u8 B;
};

struct PCXHeader
{
	u8					manufacturer;
	u8					version;
	u8					encoding;
	u8					bitsPerPixel;
	PCXDimensions		dimensions;
	u16					hRes;
	u16					vRes;
	PCXPaletteColour	colourPallet[16];
	u8					reservedByte; // This is always 0...
	u8					numColourPlanes;
	u16					bytesPerScanline;
	u16					paletteInfo;
	u16					hScreenRes;
	u16					vScreenRes;
	u16					padding[27];
};

enum PCX_ImageFormat {
	GREY = 0,
	RGB,
	RGBA
};

struct PCX_Data
{
	u32 dataSize;
	PCX_ImageFormat dataType;
	void* data;
	u16 width;
	u16 height;
};

class PCX_Image
{
public:
	PCX_Image();
	~PCX_Image();

	bool Load(const char* a_filename);
	PCX_Data* GetData();

private:
	PCXHeader m_header;
	PCXPaletteColour* m_vgaPallet;
	PCX_Data m_rawData;

	u8 DecodeRLEData(void* a_stream, u8& a_value);
};
