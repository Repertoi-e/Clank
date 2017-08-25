#include "cl/stdafx.h"
#include "FileUtils.h"

#include <FreeImage.h>
#include <FreeImage/Utilities.h>

#include "Log.h"

bool SwapRedBlue(FIBITMAP* dib)
{
	if (FreeImage_GetImageType(dib) != FIT_BITMAP)
		return false;

	const u32 bpp = FreeImage_GetBPP(dib) / 8;
	if (bpp > 4 || bpp < 3)
		return false;

	const u32 height = FreeImage_GetHeight(dib);
	const u32 pitch = FreeImage_GetPitch(dib);
	const u32 lineSize = FreeImage_GetLine(dib);

	byte* line = FreeImage_GetBits(dib);
	for (u32 y = 0; y < height; ++y, line += pitch)
		for (byte* pixel = line; pixel < line + lineSize; pixel += bpp)
			INPLACESWAP(pixel[0], pixel[2]);

	return true;
}

namespace cl {

	String GetFilenameFromPath(const wchar* path)
	{
		return GetFilenameFromPath(String(path));
	}

	String GetFilenameFromPath(const String& path)
	{
		String result;

		const u32 lastSlash = path.find_last_of(L'\\');
		const u32 length = path.length();

		if (lastSlash < length)
			result = path.substr(lastSlash + 1, length);

		return result;
	}

	byte* LoadImage(const wchar* filename, u32* width, u32* height, u32* bits, bool flipX, bool flipY)
	{
		FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeU(filename, 0);
		if (format == FIF_UNKNOWN)
			format = FreeImage_GetFIFFromFilenameU(filename);
		if (format == FIF_UNKNOWN)
			return NULLPTR;

		FIBITMAP* dib = NULLPTR;
		if (FreeImage_FIFSupportsReading(format))
			dib = FreeImage_LoadU(format, filename);

		ASSERT(dib, "Could not load image '", filename, "'!");

		FIBITMAP* bitmap = FreeImage_ConvertTo32Bits(dib);
		FreeImage_Unload(dib);

		byte* pixels = FreeImage_GetBits(bitmap);
		const u32 w = FreeImage_GetWidth(bitmap);
		const u32 h = FreeImage_GetHeight(bitmap);
		const u32 b = FreeImage_GetBPP(bitmap);

		if (flipX)
			FreeImage_FlipHorizontal(bitmap);
		if (flipY)
			FreeImage_FlipVertical(bitmap);

		if (FreeImage_GetRedMask(bitmap) == 0xff0000)
			SwapRedBlue(bitmap);

		if (width)
			*width = w;
		if (height)
			*height = h;
		if (bits)
			*bits = b;

		const u32 size = w * h * (b / 8);
		byte* result = anew byte[size];
		memcpy(result, pixels, size);
		FreeImage_Unload(bitmap);
		return result;
	}
	
	byte* LoadImage(const String& filename, u32* width, u32* height, u32* bits, bool flipX, bool flipY)
	{
		return LoadImage(filename.c_str(), width, height, bits, flipX, flipY);
	}
}