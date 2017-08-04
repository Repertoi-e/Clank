#pragma once

#include <cl/types.h>

#include <fstream> // for file I/O

class BMPImage 
{
public:
	BMPImage(const char* fileName)
	{
		std::ifstream file(fileName);

		file.seekg(0, std::ios::end);
		std::streampos length = file.tellg();
		file.seekg(0, std::ios::beg);

		buffer.resize(length);
		file.read(&buffer[0], length);

		file_header = (PBITMAPFILEHEADER)(&buffer[0]);
		info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));

		for (s32 i = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); i < buffer.size(); i++)
			pixels.push_back(buffer[i]);
	}
	
	std::vector<char> buffer;
	std::vector<byte> pixels;
	PBITMAPFILEHEADER file_header;
	PBITMAPINFOHEADER info_header;
};