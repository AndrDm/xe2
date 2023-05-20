#ifndef __NIIMAGE__
#define __NIIMAGE__

#include <windows.h>
#include <stdio.h>
#include "extcode.h"
#include "nivision.h"

#include "opencv2\opencv.hpp"
#include "ipp\ipp.h"

using namespace cv;
using namespace std;

typedef uintptr_t NIImageHandle;
typedef uintptr_t NIArrayHandle;
typedef uintptr_t NIErrorHandle;
typedef int NIERROR;

#include "lv_prolog.h"
typedef struct {
	LVBoolean status;
	int32 code;
	LStrHandle source;
} NIErrorCluster;

typedef struct {
	int32 dimSize;
	LStrHandle elt[512];
} NITDCallChain;
typedef NITDCallChain** NITDCallChainHdl;

typedef struct {
	int left;
	int top;
	int right;
	int bottom;
	double angle;
}LV_Rect;
#include "lv_epilog.h"


#define NI_ERR_SUCCESS				          0
#define NI_ERR_OCV_USER				  999999999
#define NI_ERR_NULL_POINTER			-1074395269 //ERR_NULL_POINTER

#define NI_EXPORT __declspec(dllexport)
#define NI_IMPORT __declspec(dllimport)

//EXTERN_C NIERROR NI_IMPORT 
extern "C" int LV_LVDTToGRImage(NIImageHandle niImageHandle, void *image);
extern "C" int LV_SetThreadCore(int NumThreads);
extern "C" int IsImage(Image *image);
extern "C" int ImageBytesPerPixel(Image *image, int *Bpp);
extern "C" int UnlockImages(void *images, int status);
extern "C" int LockImages(void *images, int status);
extern "C" int ResizeImage(void *image, int width, int height);
extern "C" int LV_ProcessError_v2(__int64, __int64, __int64);
extern "C" void  LV_LVDTToImage(void* image, const void* in);
extern "C" void  LV_ImageToLVDT(const void* image, void* out);
extern "C" uint8_t GGetOneErrorCode(int ErrorCode, LStrHandle * ErrStr);

int NIGetErrorHandleValue(NIErrorHandle niErrorHandle);
int NISetErrorHandleValue(NIErrorHandle niErrorHandle, NIERROR niError);
int NISetErrorHandleValueCallChain(int ErrorCode, const char* errText, NIErrorCluster* ErrorCluster);
int NIUnlockImage(NIImageHandle Img);
int NILockImage(NIImageHandle Img);
int NIGetImageInfo(NIImageHandle imageHandle, void** _pixelPtr, int* _type, int* _width, int* _height, size_t* _stepInBytes);
int NIResizeImage(NIImageHandle imageHandle, int width, int height);
int NICopy(void* Src, __int64 StepInBytesSrc, void* Dst, __int64 StepInBytesDst, __int64 Bpp, int width, int height); //a2 step
int NISetImage(NIImageHandle imageHandle, void* pixelPtrSrc, int width, int height, size_t stepInBytes);

#define ReturnOnError(niError)                                  \
{                                                               \
	if (niError != NI_ERR_SUCCESS) return niError;              \
}                                                               \

#define ReturnOnPreviousError(niErrorHandle)                    \
{                                                               \
	NIERROR status = NIGetErrorHandleValue(niErrorHandle);      \
	if (status != NI_ERR_SUCCESS) return;                       \
}                                                               \

#define ProcessNIError(niError, niErrorHandle)					\
{																\
	/*NIERROR status = NIGetErrorHandleValue(niErrorHandle);*/	\
	NIERROR status = NISetErrorHandleValueCallChain(niError,	\
					__func__, (NIErrorCluster *)niErrorHandle); \
	if (status != NI_ERR_SUCCESS) return;						\
}																\


inline NIERROR processNIError_(NIERROR niError, NIErrorHandle niErrorHandle)
{
	NIERROR error = NISetErrorHandleValue(niErrorHandle, niError);
	return error;
}


inline void ThrowNIError(NIERROR nierror)
{
	if (nierror != NI_ERR_SUCCESS) throw nierror;
}


class NIImage
{
	NIImageHandle niImageHandle;
	NIERROR lockErrorStatus;

public:
	void *pixelPtr;
	int type;
	int width;
	int height;
	size_t stepInBytes;

	NIImage()
	{
		pixelPtr = NULL;
		type = -1;
		width = -1;
		height = -1;
		stepInBytes = 0;
		lockErrorStatus = -1;
		niImageHandle = 0;
	}

	NIImage(NIImageHandle _niImageHandle)
	{
		lockErrorStatus = -1;
		ThrowNIError(SetImageHandle(_niImageHandle));
		ThrowNIError(UpdateMembers());
	}

	NIImage(NIImageHandle _niImageHandle, int _width, int _height)
	{
		lockErrorStatus = -1;
		ThrowNIError(SetImageHandle(_niImageHandle));
		ThrowNIError(ResizeImage(_width, _height));
	}

	NIERROR SetImageHandle(NIImageHandle _niImageHandle)
	{
		NIERROR err = NI_ERR_SUCCESS;
		if (lockErrorStatus == NI_ERR_SUCCESS){
			NIUnlockImage(niImageHandle);
			lockErrorStatus = -1;
		}

		try{
			lockErrorStatus = NILockImage(_niImageHandle);
			if (lockErrorStatus == NI_ERR_SUCCESS){
				niImageHandle = _niImageHandle;
				ThrowNIError(UpdateMembers());
			}
		}
		catch (NIERROR _err){err = _err;}
		return err;
	}

	bool IsValid()
	{
		if (lockErrorStatus == NI_ERR_SUCCESS) return true;
		else return false;
	}

	NIERROR UpdateMembers()
	{
		ReturnOnError(lockErrorStatus);
		NIERROR err = NI_ERR_SUCCESS;
		try{
			ThrowNIError(NIGetImageInfo(niImageHandle, &pixelPtr, &type, &width, &height, &stepInBytes));
		}
		catch (NIERROR _err){ err = _err; }
		return err;
	}

	NIERROR ResizeImage(int _width, int _height)
	{
		ReturnOnError(lockErrorStatus);
		NIERROR err = NI_ERR_SUCCESS;

		try{
			ThrowNIError(NIResizeImage(niImageHandle, _width, _height));
			ThrowNIError(UpdateMembers());
		}
		catch (NIERROR _err){err = _err;}
		return err;
	}

	NIERROR SetImage(unsigned char *_data, int _width, int _height, int _stepInBytes)
	{
		ReturnOnError(lockErrorStatus);
		NIERROR err = NI_ERR_SUCCESS;

		try{
			ThrowNIError(NISetImage(niImageHandle, _data, _width, _height, _stepInBytes));
			ThrowNIError(UpdateMembers());
		}
		catch (NIERROR _err){err = _err;}
		return err;
	}

#ifdef NI_OCV_SUPPORT

	//  _____________________________
	// |  VDM Type   |  Opencv Type  |
	// |_____________|_______________|
	// |     U8      |	  CV_8UC1    |
	// |_____________|_______________|
	// |_____________________________|
	// |     I16     |	  CV_16SC1   |
	// |_____________|_______________|
	// |_____________________________|
	// |     SGL     |    CV_32FC1   |
	// |_____________|_______________|
	// |_____________________________|
	// |   COMPLEX   |    CV_32FC2   |
	// |_____________|_______________|
	// |_____________________________|
	// |   RGB32     |    CV_8UC4    |
	// |_____________|_______________|
	// |_____________________________|
	// |   HSL32     |	  CV_8UC4    |
	// |_____________|_______________|
	// |_____________________________|
	// |   RGB_U64	 |	  CV_16UC4	 |
	// |_____________|_______________|
	// |_____________________________|
	// |    U16      |    CV_16UC1   |
	// |_____________|_______________|

	int NIImgToMatType(int niImgType)
	{
		int typeOfImage[] = { CV_8UC1, CV_16SC1, CV_32FC1, CV_32FC2, CV_8UC4, CV_8UC4, CV_16UC4, CV_16UC1 };
		return typeOfImage[niImgType];
	}

	int GetMatImageType()
	{
		int matType = NIImgToMatType(type);
		return matType;
	}

	int GetNIImageType()
	{
		return type;
	}

	NIERROR MatToImage(Mat &imgMat)
	{
		ReturnOnError(lockErrorStatus);
		NIERROR err = NI_ERR_SUCCESS;

		unsigned char *imgMatData = (unsigned char*)(imgMat.data);
		int rows = imgMat.rows;
		int cols = imgMat.cols;
		size_t matStepInBytes = imgMat.step;

		int niImgMatType = NIImgToMatType(type);
		try{
			if (imgMat.type() != niImgMatType) ThrowNIError(ERR_INVALID_IMAGE_TYPE);
			ThrowNIError(SetImage(imgMatData, cols, rows, static_cast<int>(matStepInBytes)));
			ThrowNIError(UpdateMembers());
		}
		catch (NIERROR _err){err = _err;}
		return err;
	}

	NIERROR ImageToMat(Mat &imgMat)
	{
		ReturnOnError(lockErrorStatus);
		NIERROR err = NI_ERR_SUCCESS;
		int niImgMatType = NIImgToMatType(type);
		try{
			Mat localMat(height, width, niImgMatType, pixelPtr, stepInBytes);
			imgMat = localMat;
		}
		catch (NIERROR _err) { err = _err; }
		catch (...) { throw; }
		return err;
	}

	NIERROR CloneToMat(Mat &imgMat)
	{
		ReturnOnError(lockErrorStatus);
		NIERROR err = NI_ERR_SUCCESS;
		int niImgMatType = NIImgToMatType(type);

		try{
			Mat localMat(height, width, niImgMatType, pixelPtr, stepInBytes);
			imgMat = localMat.clone();
		}
		catch (NIERROR _err){err = _err;}
		catch (...){throw;}
		return err;
	}
#endif

	~NIImage()
	{
		if (lockErrorStatus == NI_ERR_SUCCESS) NIUnlockImage(niImageHandle);
	}
};

#endif
