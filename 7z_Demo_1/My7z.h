#pragma once

#ifndef __MY7Z_H
#define __MY7Z_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "7zFile.h"
//#include "7zTypes.h"//已由其他h文件包含
#include "7zVersion.h"
#include "Alloc.h"
//#include "Compiler.h"//已由其他h文件包含
//#include "LzFind.h"//已由其他h文件包含
//#include "LzFindMt.h"//已由其他h文件包含
//#include "LzHash.h"//已由其他h文件包含
#include "LzmaDec.h"
#include "LzmaEnc.h"
#include "Precomp.h"
//#include "Threads.h"//已由其他h文件包含
//还需有7zStream.cpp

#define IN_BUF_SIZE (1 << 16)//复制于7z官方LZMA算法SDK的LzmaUtil.c中
#define OUT_BUF_SIZE (1 << 16)//复制于7z官方LZMA算法SDK的LzmaUtil.c中

void PrintHelp(char *buffer);//复制于7z官方LZMA算法SDK的LzmaUtil.c中
int PrintError(char *buffer, const char *message);//复制于7z官方LZMA算法SDK的LzmaUtil.c中
int PrintErrorNumber(char *buffer, SRes val);//复制于7z官方LZMA算法SDK的LzmaUtil.c中
int PrintUserError(char *buffer);//复制于7z官方LZMA算法SDK的LzmaUtil.c中
static SRes Decode2(CLzmaDec *state, ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 unpackSize);//复制于7z官方LZMA算法SDK的LzmaUtil.c中
static SRes Decode(ISeqOutStream *outStream, ISeqInStream *inStream);//复制于7z官方LZMA算法SDK的LzmaUtil.c中
static SRes Encode(ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 fileSize, char *rs);//复制于7z官方LZMA算法SDK的LzmaUtil.c中
int main2(int numArgs, const char *args[], char *rs);//复制于7z官方LZMA算法SDK的LzmaUtil.c中

int My7z_LZMA_CodeFile(const char *in_file_name, const char* out_file_name);//通过LZMA算法压缩
int My7z_LZMA_DecodeFile(const char *in_file_name, const char* out_file_name);//通过LZMA算法解压缩，解压一个非My7z_LZMA_CodeFile函数压缩的任意文件会导致缓冲区溢出，程序崩溃，此bug尚未解决

#endif