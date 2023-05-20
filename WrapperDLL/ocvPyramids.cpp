#include "pch.h"
#define NI_OCV_SUPPORT
#include "NIImage.h"
#include "ADVImage.h"

extern "C" __declspec(dllexport) void opencv2PyrDown(
	NIImageHandle SrcImage, NIImageHandle DstImage,
	NIErrorHandle errorHandle)
{
	Image* ImgSrc, * ImgDst;

	NIERROR error = NI_ERR_SUCCESS;
	ReturnOnPreviousError(errorHandle);
	try{

		NIImage srcImage(SrcImage);
		NIImage dstImage(DstImage);
		LV_SetThreadCore(1); //must be called prior to LV_LVDTToGRImage
		LV_LVDTToGRImage(DstImage, &ImgDst);
		//dstImage.ResizeImage(srcImage.width / 2, srcImage.height / 2);
		imaqSetImageSize(ImgDst, srcImage.width / 2, srcImage.height / 2);

		Mat srcMat;
		Mat dstMat;
		ThrowNIError(srcImage.ImageToMat(srcMat));
		ThrowNIError(dstImage.ImageToMat(dstMat));

		pyrDown(srcMat, dstMat, Size(srcImage.width / 2, srcImage.height / 2));
	}
	catch (NIERROR _err) {
		error = _err;
		OutputDebugStringA("catch(NIERROR _err)");
	}
	catch (...) {
		OutputDebugStringA("catch(...)");
		error = NI_ERR_OCV_USER;
	}
	ProcessNIError(error, errorHandle);
} //opencv2PyrDownNI

extern "C" __declspec(dllexport) void opencv2PyrUp(
	const NIImageHandle SrcImage, NIImageHandle DstImage, //Dst is double size as Src
	NIErrorHandle errorHandle)
{
	Image* ImgSrc, * ImgDst;

	NIERROR error = NI_ERR_SUCCESS;
	ReturnOnPreviousError(errorHandle);
	try {

		NIImage srcImage(SrcImage);
		NIImage dstImage(DstImage);

		//dstImage.ResizeImage(srcImage.width * 2, srcImage.height * 2);
		LV_SetThreadCore(1); //must be called prior to LV_LVDTToGRImage
		LV_LVDTToGRImage(DstImage, &ImgDst);
		imaqSetImageSize(ImgDst, srcImage.width * 2, srcImage.height * 2);

		Mat srcMat;
		Mat dstMat;
		ThrowNIError(srcImage.ImageToMat(srcMat));
		ThrowNIError(dstImage.ImageToMat(dstMat));

		// apply the algorithm
		pyrUp(srcMat, dstMat, Size(srcImage.width * 2, srcImage.height * 2));
	}
	catch (NIERROR _err) {
		error = _err;
		OutputDebugStringA("catch(NIERROR _err)");
	}
	catch (...) {
		OutputDebugStringA("catch(...)");
		error = NI_ERR_OCV_USER;
	}
	ProcessNIError(error, errorHandle);
} //opencv2PyrUpNI


//----------------------------------------------------------------------
extern "C" __declspec(dllexport) void opencv2PyrDownAD(
	NIImageHandle SrcImage, NIImageHandle DstImage,
	LVErrorCluster * ErrorCluster)
{
	Image* ImgSrc, * ImgDst;
	void* LVImagePtrSrc, * LVImagePtrDst;
	int LVWidth, LVHeight, LVLineWidthSrc, LVLineWidthDst;
	int opencv2type = 0; int bpp = 0;

	CHECK_ERROR_IN(ErrorCluster);
	LV_IS_NOT_IMAGE(SrcImage, ErrorCluster);
	LV_IS_NOT_IMAGE(DstImage, ErrorCluster);

	LV_SetThreadCore(1); //must be called prior to LV_LVDTToGRImage
	LV_LVDTToGRImage(SrcImage, &ImgSrc);
	LV_LVDTToGRImage(DstImage, &ImgDst);

	LV_IS_NOT_IMAGE(ImgDst, ErrorCluster);
	LV_IS_NOT_IMAGE(ImgSrc, ErrorCluster);

	switch (((ImageInfo*)ImgSrc)->imageType) {
	case IMAQ_IMAGE_U16:
		opencv2type = CV_16UC1; bpp = 2;
		break;
	case IMAQ_IMAGE_SGL:
		opencv2type = CV_32FC1; bpp = 4;
		break;
	default:
		ADV_SetLVError(ERR_INVALID_IMAGE_TYPE, __func__, ErrorCluster);
		return;
		break;
	}

	LVWidth = ((ImageInfo*)ImgSrc)->xRes;
	LVHeight = ((ImageInfo*)ImgSrc)->yRes;
	LVLineWidthSrc = ((ImageInfo*)ImgSrc)->pixelsPerLine;

	imaqSetImageSize(ImgDst, LVWidth / 2, LVHeight / 2);
	LVLineWidthDst = ((ImageInfo*)ImgDst)->pixelsPerLine;

	LV_IS_SAME_TYPE(ImgSrc, ImgDst, ErrorCluster);
	LVImagePtrSrc = ((ImageInfo*)ImgSrc)->imageStart;
	LVImagePtrDst = ((ImageInfo*)ImgDst)->imageStart;
	LV_IS_NOT_IMAGE(LVImagePtrSrc, ErrorCluster);
	LV_IS_NOT_IMAGE(LVImagePtrDst, ErrorCluster);

	Mat src(LVHeight, LVWidth, opencv2type, LVImagePtrSrc, LVLineWidthSrc * bpp);
	Mat dst(LVHeight / 2, LVWidth / 2, opencv2type, LVImagePtrDst, LVLineWidthDst * bpp);

	// apply the algorithm
	pyrDown(src, dst, Size(LVWidth / 2, LVHeight / 2));
} //opencv2PyrDown

extern "C" __declspec(dllexport) void opencv2PyrUpAD(
	const NIImageHandle SrcImage, NIImageHandle DstImage, //Dst is double size as Src
	LVErrorCluster * ErrorCluster)
{
	Image* ImgSrc, * ImgDst;
	void* LVImagePtrSrc, * LVImagePtrDst;
	int LVWidth, LVHeight, LVLineWidthSrc, LVLineWidthDst;
	int opencv2type = 0; int bpp = 0;

	CHECK_ERROR_IN(ErrorCluster);
	LV_IS_NOT_IMAGE(SrcImage, ErrorCluster);
	LV_IS_NOT_IMAGE(DstImage, ErrorCluster);

	LV_SetThreadCore(1);
	LV_LVDTToGRImage(SrcImage, &ImgSrc);
	LV_LVDTToGRImage(DstImage, &ImgDst);

	LV_IS_NOT_IMAGE(ImgDst, ErrorCluster);
	LV_IS_NOT_IMAGE(ImgSrc, ErrorCluster);

	switch (((ImageInfo*)ImgSrc)->imageType) {
	case IMAQ_IMAGE_U16:
		opencv2type = CV_16UC1; bpp = 2;
		break;
	case IMAQ_IMAGE_SGL:
		opencv2type = CV_32FC1; bpp = 4;
		break;
	default:
		ADV_SetLVError(ERR_INVALID_IMAGE_TYPE, __func__, ErrorCluster);
		return;
		break;
	}

	LVWidth = ((ImageInfo*)ImgSrc)->xRes;
	LVHeight = ((ImageInfo*)ImgSrc)->yRes;
	LVLineWidthSrc = ((ImageInfo*)ImgSrc)->pixelsPerLine;

	imaqSetImageSize(ImgDst, LVWidth * 2, LVHeight * 2);
	LVLineWidthDst = ((ImageInfo*)ImgDst)->pixelsPerLine;

	LV_IS_SAME_TYPE(ImgSrc, ImgDst, ErrorCluster);
	LVImagePtrSrc = ((ImageInfo*)ImgSrc)->imageStart;
	LVImagePtrDst = ((ImageInfo*)ImgDst)->imageStart;
	LV_IS_NOT_IMAGE(LVImagePtrSrc, ErrorCluster);
	LV_IS_NOT_IMAGE(LVImagePtrDst, ErrorCluster);

	Mat src(LVHeight, LVWidth, opencv2type, LVImagePtrSrc, LVLineWidthSrc * bpp);	//rows, cols
	Mat dst(LVHeight * 2, LVWidth * 2, opencv2type, LVImagePtrDst, LVLineWidthDst * bpp);

	// apply the algorithm
	pyrUp(src, dst, Size(LVWidth * 2, LVHeight * 2));
} //opencv2PyrUp
