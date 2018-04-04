// 7z_Demo_1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "7z_Demo_1.h"

#include "My7z.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 更改错误代码以符合您的需要
			_tprintf(_T("错误: MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: 在此处为应用程序的行为编写代码。
			char* in_file_name_1="1.txt";//原文件，待压缩文件
			char* out_file_name_1="2.txt";//LZMA算法压缩后文件
			char* in_file_name_2="3.txt";//原文件，待解压缩文件
			char* out_file_name_2="4.txt";//LZMA算法解压缩后文件
			My7z_LZMA_CodeFile(in_file_name_1, out_file_name_1);//通过LZMA算法压缩
            My7z_LZMA_DecodeFile(in_file_name_2, out_file_name_2);//通过LZMA算法解压缩
		}
	}
	else
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: GetModuleHandle 失败\n"));
		nRetCode = 1;
	}
	printf("\n");
	system("pause");
	printf("\n");
	return nRetCode;
}
