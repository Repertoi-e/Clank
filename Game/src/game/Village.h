#pragma once

#include <Clank.h>

class Village
{
public:
	s32 health = 7200;

	Village() {}
	~Village() {}

	void Update()
	{
		health--;
	}

	void Render(cl::Renderer2D* renderer)
	{
		/*..*/
	}
};