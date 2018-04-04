#include "My7z.h"

const char *kCantReadMessage = "Can not read input file";//复制于7z官方LZMA算法SDK的LzmaUtil.c中
const char *kCantWriteMessage = "Can not write output file";//复制于7z官方LZMA算法SDK的LzmaUtil.c中
const char *kCantAllocateMessage = "Can not allocate memory";//复制于7z官方LZMA算法SDK的LzmaUtil.c中
const char *kDataErrorMessage = "Data error";//复制于7z官方LZMA算法SDK的LzmaUtil.c中

void PrintHelp(char *buffer)//复制于7z官方LZMA算法SDK的LzmaUtil.c中
{
  strcat(buffer, "\nLZMA Utility " MY_VERSION_COPYRIGHT_DATE "\n"
      "\nUsage:  lzma <e|d> inputFile outputFile\n"
             "  e: encode file\n"
             "  d: decode file\n");
}

int PrintError(char *buffer, const char *message)//复制于7z官方LZMA算法SDK的LzmaUtil.c中
{
  strcat(buffer, "\nError: ");
  strcat(buffer, message);
  strcat(buffer, "\n");
  return 1;
}

int PrintErrorNumber(char *buffer, SRes val)//复制于7z官方LZMA算法SDK的LzmaUtil.c中
{
  sprintf(buffer + strlen(buffer), "\nError code: %x\n", (unsigned)val);
  return 1;
}

int PrintUserError(char *buffer)//复制于7z官方LZMA算法SDK的LzmaUtil.c中
{
  return PrintError(buffer, "Incorrect command");
}

static SRes Decode2(CLzmaDec *state, ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 unpackSize)//复制于7z官方LZMA算法SDK的LzmaUtil.c中
{
  int thereIsSize = (unpackSize != (UInt64)(Int64)-1);
  Byte inBuf[IN_BUF_SIZE];
  Byte outBuf[OUT_BUF_SIZE];
  size_t inPos = 0, inSize = 0, outPos = 0;
  LzmaDec_Init(state);
  for (;;)
  {
    if (inPos == inSize)
    {
      inSize = IN_BUF_SIZE;
      RINOK(inStream->Read(inStream, inBuf, &inSize));
      inPos = 0;
    }
    {
      SRes res;
      SizeT inProcessed = inSize - inPos;
      SizeT outProcessed = OUT_BUF_SIZE - outPos;
      ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
      ELzmaStatus status;
      if (thereIsSize && outProcessed > unpackSize)
      {
        outProcessed = (SizeT)unpackSize;
        finishMode = LZMA_FINISH_END;
      }
      
      res = LzmaDec_DecodeToBuf(state, outBuf + outPos, &outProcessed,
        inBuf + inPos, &inProcessed, finishMode, &status);
      inPos += inProcessed;
      outPos += outProcessed;
      unpackSize -= outProcessed;
      
      if (outStream)
        if (outStream->Write(outStream, outBuf, outPos) != outPos)
          return SZ_ERROR_WRITE;
        
      outPos = 0;
      
      if (res != SZ_OK || (thereIsSize && unpackSize == 0))
        return res;
      
      if (inProcessed == 0 && outProcessed == 0)
      {
        if (thereIsSize || status != LZMA_STATUS_FINISHED_WITH_MARK)
          return SZ_ERROR_DATA;
        return res;
      }
    }
  }
}

static SRes Decode(ISeqOutStream *outStream, ISeqInStream *inStream)//复制于7z官方LZMA算法SDK的LzmaUtil.c中
{
  UInt64 unpackSize;
  int i;
  SRes res = 0;

  CLzmaDec state;

  /* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
  unsigned char header[LZMA_PROPS_SIZE + 8];

  /* Read and parse header */

  RINOK(SeqInStream_Read(inStream, header, sizeof(header)));

  unpackSize = 0;
  for (i = 0; i < 8; i++)
    unpackSize += (UInt64)header[LZMA_PROPS_SIZE + i] << (i * 8);

  LzmaDec_Construct(&state);
  RINOK(LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc));
  res = Decode2(&state, outStream, inStream, unpackSize);
  LzmaDec_Free(&state, &g_Alloc);
  return res;
}

static SRes Encode(ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 fileSize, char *rs)//复制于7z官方LZMA算法SDK的LzmaUtil.c中
{
  CLzmaEncHandle enc;
  SRes res;
  CLzmaEncProps props;

  UNUSED_VAR(rs);

  enc = LzmaEnc_Create(&g_Alloc);
  if (enc == 0)
    return SZ_ERROR_MEM;

  LzmaEncProps_Init(&props);
  res = LzmaEnc_SetProps(enc, &props);

  if (res == SZ_OK)
  {
    Byte header[LZMA_PROPS_SIZE + 8];
    size_t headerSize = LZMA_PROPS_SIZE;
    int i;

    res = LzmaEnc_WriteProperties(enc, header, &headerSize);
    for (i = 0; i < 8; i++)
      header[headerSize++] = (Byte)(fileSize >> (8 * i));
    if (outStream->Write(outStream, header, headerSize) != headerSize)
      res = SZ_ERROR_WRITE;
    else
    {
      if (res == SZ_OK)
        res = LzmaEnc_Encode(enc, outStream, inStream, NULL, &g_Alloc, &g_Alloc);
    }
  }
  LzmaEnc_Destroy(enc, &g_Alloc, &g_Alloc);
  return res;
}

int main2(int numArgs, char *args[], char *rs)//复制于7z官方LZMA算法SDK的LzmaUtil.c中
{
  CFileSeqInStream inStream;
  CFileOutStream outStream;
  char c;
  int res;
  int encodeMode;
  Bool useOutFile = False;

  FileSeqInStream_CreateVTable(&inStream);
  File_Construct(&inStream.file);

  FileOutStream_CreateVTable(&outStream);
  File_Construct(&outStream.file);

  if (numArgs == 1)
  {
    PrintHelp(rs);
    return 0;
  }

  if (numArgs < 3 || numArgs > 4 || strlen(args[1]) != 1)
    return PrintUserError(rs);

  c = args[1][0];
  encodeMode = (c == 'e' || c == 'E');
  if (!encodeMode && c != 'd' && c != 'D')
    return PrintUserError(rs);

  {
    size_t t4 = sizeof(UInt32);
    size_t t8 = sizeof(UInt64);
    if (t4 != 4 || t8 != 8)
      return PrintError(rs, "Incorrect UInt32 or UInt64");
  }

  if (InFile_Open(&inStream.file, args[2]) != 0)
    return PrintError(rs, "Can not open input file");

  if (numArgs > 3)
  {
    useOutFile = True;
    if (OutFile_Open(&outStream.file, args[3]) != 0)
      return PrintError(rs, "Can not open output file");
  }
  else if (encodeMode)
    PrintUserError(rs);

  if (encodeMode)
  {
    UInt64 fileSize;
    File_GetLength(&inStream.file, &fileSize);
    res = Encode(&outStream.s, &inStream.s, fileSize, rs);
  }
  else
  {
    res = Decode(&outStream.s, useOutFile ? &inStream.s : NULL);
  }

  if (useOutFile)
    File_Close(&outStream.file);
  File_Close(&inStream.file);

  if (res != SZ_OK)
  {
    if (res == SZ_ERROR_MEM)
      return PrintError(rs, kCantAllocateMessage);
    else if (res == SZ_ERROR_DATA)
      return PrintError(rs, kDataErrorMessage);
    else if (res == SZ_ERROR_WRITE)
      return PrintError(rs, kCantWriteMessage);
    else if (res == SZ_ERROR_READ)
      return PrintError(rs, kCantReadMessage);
    return PrintErrorNumber(rs, res);
  }
  return 0;
}

int My7z_LZMA_CodeFile(const char *in_file_name, const char* out_file_name)//通过LZMA算法压缩
{
	//检测in_file_name文件是否存在
	errno_t err_read;
	FILE *fp_read = NULL;
	err_read = fopen_s(&fp_read, in_file_name, "rb");
	if (err_read != 0)
	{
		return -2;//in_file_name文件不存在
	}
	fclose(fp_read);

	//in_file_name文件存在
	char buf[800];
    char *argvs[4];
	int result = -1;
    argvs[0] = NULL;
    argvs[1] = "e";
	argvs[2] = new char[100];
	argvs[3] = new char[100];
	strcpy(argvs[2],in_file_name);
	strcpy(argvs[3],out_file_name);
    result = main2(4, argvs, buf);
	delete[] (argvs[2]);
	delete[] (argvs[3]);
	return result;
}

int My7z_LZMA_DecodeFile(const char *in_file_name, const char* out_file_name)//通过LZMA算法解压缩，解压一个非My7z_LZMA_CodeFile函数压缩的任意文件会导致缓冲区溢出，程序崩溃，此bug尚未解决
{
	//检测in_file_name文件是否存在
	errno_t err_read;
	FILE *fp_read = NULL;
	err_read = fopen_s(&fp_read, in_file_name, "rb");
	if (err_read != 0)
	{
		return -2;//in_file_name文件不存在
	}
	fclose(fp_read);

	//in_file_name文件存在
	char buf[800];
    char *argvs[4];
	int result = -1;
    argvs[0] = NULL;
    argvs[1] = "d";
	argvs[2] = new char[100];
	argvs[3] = new char[100];
    strcpy(argvs[2],in_file_name);
	strcpy(argvs[3],out_file_name);                
    result = main2(4, argvs, buf);
	delete[] (argvs[2]);
	delete[] (argvs[3]);
	return result;
}