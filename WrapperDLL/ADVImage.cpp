#include "pch.h"
#include "ADVImage.h"

#pragma warning(disable:4996)

Image* ADV_LVDTToAddress(const void* LVImageHdl)
{
	IMAQ_Image* LV_Image;
	Image* ret;

	LV_Image = (IMAQ_Image*)DSNewPtr(sizeof(IMAQ_Image));
	LV_Image->name = (LStrHandle)DSNewHandle(sizeof(LStrHandle));
	LV_LVDTToImage(LV_Image, LVImageHdl); //first cluster, second image
	ret = (LV_Image->address);
	DSDisposeHandle(LV_Image->name);
	DSDisposePtr(LV_Image);

	return ret;
}


int ADV_SetLVError(int ErrorCode, const char* errText, LVErrorCluster* ErrorCluster)
{
	size_t strerrsize, len;
	size_t strsize;
	uint8_t code_ok;

	LStrHandle ErrStr;

	ErrStr = (LStrHandle)DSNewHandle(sizeof(LStr));
	code_ok = GGetOneErrorCode(ErrorCode, &ErrStr);
	strerrsize = (code_ok ? (*ErrStr)->cnt : 0);

	strsize = strlen(errText);

	if (ErrorCode) {

		len = strsize + strerrsize + 1;
		//sizeof(int)
		if (noErr == NumericArrayResize(uB, 1, (UHandle*)(&(ErrorCluster->source)), len)) {
			strncpy((char*)(*(ErrorCluster->source))->str, (const char*)(errText), strsize);
			strncat((char*)(*(ErrorCluster->source))->str, "\n", 1);
			if (code_ok) strncat((char*)(*(ErrorCluster->source))->str, (const char*)(*ErrStr)->str, strerrsize);
			(*(ErrorCluster->source))->cnt = (int32)len;
		}
	}
	else {
		len = 0;
		if (noErr == NumericArrayResize(uB, 1, (UHandle*)(&(ErrorCluster->source)), len + 0)) {
			(*(ErrorCluster->source))->cnt = (int32)len;
		}
	}

	ErrorCluster->code = ErrorCode;

	if (ErrorCode) ErrorCluster->status = TRUE; else ErrorCluster->status = FALSE;

	DSDisposeHandle(ErrStr);

	return ((int)code_ok);
}


