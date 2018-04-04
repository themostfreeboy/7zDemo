// 7z_Demo_1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "7z_Demo_1.h"

#include "My7z.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: ���Ĵ�������Է���������Ҫ
			_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
			char* in_file_name_1="1.txt";//ԭ�ļ�����ѹ���ļ�
			char* out_file_name_1="2.txt";//LZMA�㷨ѹ�����ļ�
			char* in_file_name_2="3.txt";//ԭ�ļ�������ѹ���ļ�
			char* out_file_name_2="4.txt";//LZMA�㷨��ѹ�����ļ�
			My7z_LZMA_CodeFile(in_file_name_1, out_file_name_1);//ͨ��LZMA�㷨ѹ��
            My7z_LZMA_DecodeFile(in_file_name_2, out_file_name_2);//ͨ��LZMA�㷨��ѹ��
		}
	}
	else
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: GetModuleHandle ʧ��\n"));
		nRetCode = 1;
	}
	printf("\n");
	system("pause");
	printf("\n");
	return nRetCode;
}
