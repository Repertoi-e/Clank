#include "cl/stdafx.h"
#include "ResourceManager.h"

namespace cl {

	ResourceManager<Font>& g_Fonts = ResourceManager<Font>::Instance();
	ResourceManager<Texture>& g_Textures = ResourceManager<Texture>::Instance();

}