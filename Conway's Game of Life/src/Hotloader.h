#pragma once

#include "stdafx.h"

struct HotloaderThreadArgs
{
	String Path;
	std::function<void(String)> Method;
};

class Hotloader
{
public:
	typedef std::function<void(String)> Method;
private:
	HotloaderThreadArgs m_Args;

	HANDLE m_Thread;
public:
	void Join();
public:
	static void Create(Hotloader* hotloader, const String& pathToMonitor, std::function<void(String)> method);
};