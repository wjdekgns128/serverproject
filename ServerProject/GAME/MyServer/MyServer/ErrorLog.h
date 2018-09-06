#pragma once
#include <windows.h>   // include important windows stuff
#include <iostream>
namespace ERRORLOG
{
	static void  error_display(const char *msg, int err_no)
	{
		WCHAR *lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, err_no,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		std::cout << msg;
		std::wcout << L"  ¿¡·¯" << lpMsgBuf << std::endl;
		LocalFree(lpMsgBuf);
		while (true);
	}

	static void ErrorDisplay(const char * location)
	{
		error_display(location, WSAGetLastError());
	}
};