//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDebug.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Debug Helper Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KFile.h"
#include "KMemBase.h"
#ifndef _STANDALONE
#include "KWin32Wnd.h"
#endif
#include "KDebug.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
//---------------------------------------------------------------------------
#ifndef __linux
static HWND m_hWndDebug = NULL;
#endif
//---------------------------------------------------------------------------
// ����:	find debug window
// ����:	find debug window
// ����:	char*
//			char*
// ����:	void
//---------------------------------------------------------------------------
HWND g_FindDebugWindow(char *lpClassName, char *lpWindowName)
{
#ifndef __linux
	m_hWndDebug = FindWindow(lpClassName, lpWindowName);
	return m_hWndDebug;
#else
	return 0;
#endif
}
//---------------------------------------------------------------------------
// ����:	g_DebugLog
// ����:	����Դ������������Ϣ
// ����:	Fmt		��ʽ���ַ���
//			...		������ַ���
// ����:	void
//---------------------------------------------------------------------------
void g_DebugLog(LPSTR Fmt, ...)
{
#ifndef __linux
	if (m_hWndDebug)
	{
		char buffer[256];
		va_list va;

		COPYDATASTRUCT data;
		va_start(va, Fmt);
		int n = vsprintf(buffer, Fmt, va);
		va_end(va);
		data.dwData = 1;
		data.cbData = n + 1;
		data.lpData = buffer;
		SendMessage(m_hWndDebug, WM_COPYDATA,
					(WPARAM)m_hWndDebug, (LPARAM)&data);
	}
#else
/*	char buffer[256];
	va_list va;
	va_start(va, Fmt);
	vsprintf(buffer, Fmt, va);
	strcat(buffer, "\n");
	printf(buffer);
	va_end(va);*/
#endif
}
//---------------------------------------------------------------------------
// ����:	g_MessageBox
// ����:	Display a System Message Box
// ����:	char* lpMsg, ...
// ����:	void
//---------------------------------------------------------------------------
void g_MessageBox(LPSTR lpMsg, ...)
{
#ifndef __linux
	char szMsg[256];
	va_list va;
	va_start(va, lpMsg);
	vsprintf(szMsg, lpMsg, va);
	va_end(va);
	g_DebugLog(szMsg);
	//	MessageBox(g_GetMainHWnd(), szMsg, 0, MB_OK);
	MessageBox(NULL, szMsg, 0, MB_OK);
#endif
}
//---------------------------------------------------------------------------
// ����:	g_AssertFailed
// ����:	����ʧ��
// ����:	FileName	���ĸ��ļ���ʧ��
//			LineNum		���ļ��еĵڼ���
// ����:	void
//---------------------------------------------------------------------------
void g_AssertFailed(LPSTR pFileName, int nLineNum)
{
	char szMsg[256];
	sprintf(szMsg, "Assert failed in %s, line = %i", pFileName, nLineNum);
	g_DebugLog(szMsg);
	g_MessageBox(szMsg);
	exit(1);
}
//---------------------------------------------------------------------------

void g_DebugLogTxt(const char *msg)
{
	FILE *file;

	// M? file debug trong ch? ?? ghi th��m (append)
	file = fopen("debug_log_txt.txt", "a");
	if (file == NULL)
	{
		return; // N?u kh?ng m? ???c file, tho��t h��m
	}

	// Ghi th?i gian hi?n t?i v��o file
	time_t now;
	time(&now);
	struct tm *local = localtime(&now);
	fprintf(file, "[%02d-%02d-%04d %02d:%02d:%02d] ",
			local->tm_mday, local->tm_mon + 1, local->tm_year + 1900,
			local->tm_hour, local->tm_min, local->tm_sec);

	// Ghi th?ng ?i?p v��o file
	fprintf(file, "%s\n", msg);

	// ?��ng file
	fclose(file);
}