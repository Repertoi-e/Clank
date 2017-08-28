#pragma once

#include "cl/stdafx.h"

#ifdef LoadImage
#undef LoadImage
#endif

namespace cl {

	API String GetFilenameFromPath(const wchar* path);
	API String GetFilenameFromPath(const String& path);

	API byte* ReadFile(const wchar* filename, u32* readBytes = NULLPTR);
	API byte* ReadFile(const String& filename, u32* readBytes = NULLPTR);

	API byte* LoadImage(const wchar* filename, u32* width = NULLPTR, u32* height = NULLPTR, u32* bits = NULLPTR, bool flipX = false, bool flipY = false);
	API byte* LoadImage(const String& filename, u32* width = NULLPTR, u32* height = NULLPTR, u32* bits = NULLPTR, bool flipX = false, bool flipY = false);
}