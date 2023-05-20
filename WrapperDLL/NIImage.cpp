#include "pch.h"
#include "NIImage.h"

#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable:4996)

//----- (0000000180001090) ----------------------------------------------------
int NIGetErrorHandleValue(NIErrorHandle niErrorHandle)
{
	LV_SetThreadCore(1i64);
	if (!niErrorHandle) { throw NI_ERR_NULL_POINTER; }
	return *(unsigned int*)(niErrorHandle + 4);
}

//----- (0000000180001010) ----------------------------------------------------
//NIERROR niError, NIErrorHandle niErrorHandle
int NISetErrorHandleValue(NIErrorHandle niErrorHandle, NIERROR niError) //a1 handle a2 error
{
	LV_SetThreadCore(1i64);
	if (!niErrorHandle) { throw NI_ERR_NULL_POINTER; }
	LV_ProcessError_v2(niError, niErrorHandle, 40618i64); //number is the function
	return 0i64;
}


int NISetErrorHandleValueCallChain(int ErrorCode, const char* errText, NIErrorCluster* ErrorCluster)
{ 
	size_t strsize, strerrsize, strtextsize, calldepth, len;
	uint8_t code_ok;

	NITDCallChainHdl callChain;
	LStrHandle ErrStr;

	ErrStr = (LStrHandle)DSNewHandle(sizeof(LStr));
	code_ok = GGetOneErrorCode(ErrorCode, &ErrStr);
	strerrsize = (code_ok ? (*ErrStr)->cnt : 0);

	callChain = (NITDCallChainHdl)DSNewHandle(sizeof(NITDCallChain)); (*callChain)->dimSize = 0;
	(*callChain)->elt[0] = (LStrHandle)DSNewHandle(sizeof(sizeof(LStr)));

	CallChain((UHandle)callChain);

	calldepth = (*callChain)->dimSize;

	strsize = (*((*callChain)->elt[0]))->cnt;

	strtextsize = strlen(errText);


	if (ErrorCode) {

		len = strsize + strerrsize + strtextsize + 2;

		if (NI_ERR_SUCCESS == NumericArrayResize(uB, 1, (UHandle*)(&(ErrorCluster->source)), len)) {
			strncpy((char*)(*(ErrorCluster->source))->str, (const char*)(*((*callChain)->elt[0]))->str, strsize);
			strncat((char*)(*(ErrorCluster->source))->str, ":", 1);
			strncat((char*)(*(ErrorCluster->source))->str, (const char*)(errText), strtextsize);
			strncat((char*)(*(ErrorCluster->source))->str, "\n", 1);

			if (code_ok) strncat((char*)(*(ErrorCluster->source))->str, (const char*)(*ErrStr)->str, strerrsize);
			(*(ErrorCluster->source))->cnt = (int32)len;
		}
	}
	else {
		len = 0;
		if (NI_ERR_SUCCESS == NumericArrayResize(uB, 1, (UHandle*)(&(ErrorCluster->source)), len)) {
			(*(ErrorCluster->source))->cnt = (int32)len;
		}
	}


	ErrorCluster->code = ErrorCode;

	if (ErrorCode) ErrorCluster->status = TRUE; else ErrorCluster->status = FALSE;

	DSDisposeHandle(callChain);
	DSDisposeHandle(ErrStr);

	return ((int)code_ok);
}

int NIUnlockImage(NIImageHandle Img)
{
	Image* img;
	int ret = 0;

	LV_SetThreadCore(1); //must be called prior to LV_LVDTToGRImage
	LV_LVDTToGRImage(Img, &img);
	if (!IsImage(img)) return ERR_NOT_IMAGE;
	//ret = UnlockImages(img, 1i64);
	return ret;
}

int NILockImage(NIImageHandle Img)
{
	Image* img;
	int ret = 0;

	LV_SetThreadCore(1); //must be called prior to LV_LVDTToGRImage
	LV_LVDTToGRImage(Img, &img);
	if (!IsImage(img)) return ERR_NOT_IMAGE;
	//ret = LockImages(img, 1i64);
	return ret;
}

int NIGetImageInfo(NIImageHandle imageHandle, void** _pixelPtr, int* _type, int* _width, int* _height, size_t* _stepInBytes)
{
	Image* img;
	int Bpp;

	LV_SetThreadCore(1i64);
	if (!_pixelPtr || !_type || !_width || !_height || !_stepInBytes) return ERR_NULL_POINTER;
	LV_LVDTToGRImage(imageHandle, &img);

	if (!IsImage(img)) return ERR_NOT_IMAGE;

	ImageBytesPerPixel(img, &Bpp);
	*_pixelPtr = (void*)((ImageInfo*)img)->imageStart;
	*_type = ((ImageInfo*)img)->imageType;
	*_width = ((ImageInfo*)img)->xRes;
	*_height = ((ImageInfo*)img)->yRes;
	*_stepInBytes = Bpp * ((ImageInfo*)img)->pixelsPerLine;

	return 0i64;
}

int NIResizeImage(NIImageHandle imageHandle, int width, int height)
{
	Image* img;
	int ret;

	LV_SetThreadCore(1i64);
	LV_LVDTToGRImage(imageHandle, &img);

	if (!IsImage(img)) return ERR_NOT_IMAGE;

	ret = ResizeImage(img, width, height);

	return ret;
}

int NICopy(void* Src, __int64 StepInBytesSrc, void* Dst, __int64 StepInBytesDst, __int64 Bpp, int width, int height) //a2 step
{
	char* dst;
	char* src;
	size_t lineWidth;
	int i;

	dst = (char*)Dst;
	src = (char*)Src;
	if (!Src || !Dst) return ERR_NULL_POINTER;

	lineWidth = Bpp * width;
	if (lineWidth == StepInBytesSrc && lineWidth == StepInBytesDst) memmove(Dst, Src, lineWidth * height);
	else {
		for (i = 0; i < height; ++i) {
			memmove(dst, src, lineWidth);
			src += StepInBytesSrc;
			dst += StepInBytesDst;
		}
	}
	return 0;
}

int NISetImage(NIImageHandle imageHandle, void* pixelPtrSrc, int width, int height, size_t stepInBytes)
{
	Image* imgDst;
	void* _pixelPtrDst;
	int Bpp, ret;

	LV_SetThreadCore(1i64);
	LV_LVDTToGRImage(imageHandle, &imgDst);
	if (!IsImage(imgDst)) return ERR_NOT_IMAGE;
	if ((ret = ResizeImage(imgDst, width, height))) return ret;
	_pixelPtrDst = ((ImageInfo*)imgDst)->imageStart;

	ImageBytesPerPixel(imgDst, &Bpp);
	ret = NICopy(pixelPtrSrc, stepInBytes, _pixelPtrDst, (unsigned int)Bpp * ((ImageInfo*)imgDst)->pixelsPerLine, Bpp, width, height);
	return ret;
}