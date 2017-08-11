#include "Hotloader.h"


DWORD WINAPI ThreadProc(void* arg)
{
	HotloaderThreadArgs args = *cast(HotloaderThreadArgs*)arg;

	HANDLE hDir = CreateFile(args.Path.c_str(),
		FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	HANDLE hEvent;
	byte buffer[1024];
	DWORD bytesTransferred = 0, error = 0;
	BOOL hack = FALSE;
	OVERLAPPED o;
	ZeroMemory(&o, sizeof(OVERLAPPED));
	o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	do
	{
		if (ReadDirectoryChangesW(hDir, buffer, sizeof(buffer), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &o, NULL))
		{
			if (GetOverlappedResult(hDir, &o, &bytesTransferred, TRUE))
			{
				DWORD offset = 0;
				FILE_NOTIFY_INFORMATION *fni = NULL;

				do
				{
					fni = cast(FILE_NOTIFY_INFORMATION*) (&buffer[offset]);

					if (fni->Action == FILE_ACTION_MODIFIED)
					{
						String file = String(fni->FileName);
						// I don't even know
						file = file.substr(0, file.length() - 1);

						hack = !hack;
						if (!hack)
							args.Method(args.Path + file);
					}

					offset += fni->NextEntryOffset;
				} while (fni->NextEntryOffset != 0);
			}
			else
				bytesTransferred = 0;

			if (0 == ::ResetEvent(o.hEvent))
			{
				std::cout << "ResetEvent error = " << ::GetLastError() << std::endl;
				::CloseHandle(o.hEvent);
				return ::GetLastError();
			}
		}
		else
			std::cout << "ReadDirectoryChangesW error =  " << ::GetLastError() << std::endl;

		error = ::WaitForSingleObject(hEvent, 1);
	} while (error == WAIT_TIMEOUT);

	CloseHandle(o.hEvent);
}

void Hotloader::Create(Hotloader* hotloader, const String& pathToMonitor, std::function<void(String)> method)
{
	{
		ZeroMemory(&hotloader->m_Args, sizeof(HotloaderThreadArgs));

		hotloader->m_Args.Path = pathToMonitor;
		hotloader->m_Args.Method = method;
	}
	hotloader->m_Thread = CreateThread(NULL, 0, ThreadProc, cast(void*) &hotloader->m_Args, 0, NULL);
}

void Hotloader::Join()
{
	WaitForSingleObject(m_Thread, INFINITE);
}
