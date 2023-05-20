#include "pch.h"
#include <Windows.h>
#include "nivision.h"

#include "opencv2\opencv.hpp"
#define NI_OCV_SUPPORT
#include "NIImage.h"
#include "ADVImage.h"


//based on https://stackoverflow.com/questions/68703443/unsharp-mask-implementation-with-opencv
extern "C" __declspec(dllexport) void opencv2UnsharpMask(
	const void* SrcImage, void* DstImage,
	float Radius, float Amount, float Threshold,
	LVErrorCluster * ErrorCluster)
{
	Image* ImgSrc, * ImgDst;
	unsigned short int* LVImagePtrSrcU16 = NULL, * LVImagePtrDstU16 = NULL;
	float* LVImagePtrSrcSGL = NULL, * LVImagePtrDstSGL = NULL;
	int LVWidth, LVHeight, LVLineWidthSrc, LVLineWidthDst;
	int opencv2type = 0;

	CHECK_ERROR_IN(ErrorCluster);
	LV_IS_NOT_IMAGE(SrcImage, ErrorCluster);
	LV_IS_NOT_IMAGE(DstImage, ErrorCluster);

	ImgSrc = ADV_LVDTToAddress(SrcImage);
	ImgDst = ADV_LVDTToAddress(DstImage);
	LV_IS_NOT_IMAGE(ImgDst, ErrorCluster);
	LV_IS_NOT_IMAGE(ImgSrc, ErrorCluster);
	LV_IS_NOT_IMAGE(((ImageInfo*)ImgSrc)->imageStart, ErrorCluster);


	switch (((ImageInfo*)ImgSrc)->imageType) {
	case IMAQ_IMAGE_U16:
		LVImagePtrSrcU16 = (unsigned short int*)((ImageInfo*)ImgSrc)->imageStart;
		opencv2type = CV_16UC1;
		break;
	case IMAQ_IMAGE_SGL:
		LVImagePtrSrcSGL = (float*)((ImageInfo*)ImgSrc)->imageStart;
		opencv2type = CV_32FC1;
		break;
	default:
		ADV_SetLVError(ERR_INVALID_IMAGE_TYPE, __func__, ErrorCluster);
		return;
		break;
	}

	LVWidth = ((ImageInfo*)ImgSrc)->xRes;
	LVHeight = ((ImageInfo*)ImgSrc)->yRes;
	LVLineWidthSrc = ((ImageInfo*)ImgSrc)->pixelsPerLine;

	imaqSetImageSize(ImgDst, LVWidth, LVHeight);
	LVLineWidthDst = ((ImageInfo*)ImgDst)->pixelsPerLine;
	LV_IS_NOT_IMAGE(((ImageInfo*)ImgDst)->imageStart, ErrorCluster);

	switch (((ImageInfo*)ImgDst)->imageType) {
	case IMAQ_IMAGE_U16:
		LVImagePtrDstU16 = (unsigned short int*)((ImageInfo*)ImgDst)->imageStart;
		break;
	case IMAQ_IMAGE_SGL:
		LVImagePtrDstSGL = (float*)((ImageInfo*)ImgDst)->imageStart;
		break;
	default:
		ADV_SetLVError(ERR_INVALID_IMAGE_TYPE, __func__, ErrorCluster);
		return;
		break;
	}

	Mat src(LVWidth, LVHeight, opencv2type);
	Mat dst(LVWidth, LVHeight, opencv2type);

	// Copy, line by line, the LV image image into the OpenCV image.
	switch (((ImageInfo*)ImgSrc)->imageType) {
	case IMAQ_IMAGE_U16:
		for (int y = 0; y < LVHeight; ++y) {
			memcpy((unsigned short*)src.data + y * LVWidth,
				LVImagePtrSrcU16 + y * LVLineWidthSrc,
				LVWidth * sizeof(unsigned short int)); //16 bit
		}
		break;
	case IMAQ_IMAGE_SGL:
		for (int y = 0; y < LVHeight; ++y) {
			memcpy((float*)src.data + y * LVWidth,
				LVImagePtrSrcSGL + y * LVLineWidthSrc,
				LVWidth * sizeof(float));
		}
		break;
	default:
		ADV_SetLVError(ERR_INVALID_IMAGE_TYPE, __func__, ErrorCluster);
		return;
		break;
	}

	// work using floating point images to avoid overflows
	Mat input;
	src.convertTo(input, CV_32FC1);

	// copy original for our return value
	Mat retbuf = input.clone();

	// create the blurred copy
	Mat blurred;
	GaussianBlur(input, blurred, Size(3, 3), Radius);

	// subtract blurred from original, pixel-by-pixel to make unsharp mask
	Mat unsharpMask;
	subtract(input, blurred, unsharpMask);

	// --- filter on the mask ---

	//cv::medianBlur(unsharpMask, unsharpMask, 3);
	blur(unsharpMask, unsharpMask, { 3,3 });


	// --- end filter ---

	// apply mask to image
	for (int row = 0; row < src.rows; row++) {
		for (int col = 0; col < src.cols; col++) {
			float origGray = input.at<float>(row, col);
			float difference = unsharpMask.at<float>(row, col);
			if (norm(difference) >= Threshold) {
				retbuf.at<float>(row, col) = origGray + Amount * difference;
			}
		}
	}

	// convert back to original type
	retbuf.convertTo(dst, opencv2type);

	// Copy, line by line, the OpenCV image into the LV image.
	switch (((ImageInfo*)ImgDst)->imageType) {
	case IMAQ_IMAGE_U16:
		for (int y = 0; y < LVHeight; ++y) {
			memcpy(LVImagePtrDstU16 + y * LVLineWidthDst,
				(unsigned short*)dst.data + y * LVWidth,
				LVWidth * sizeof(unsigned short int));
		}
		break;
	case IMAQ_IMAGE_SGL:
		for (int y = 0; y < LVHeight; ++y) {
			memcpy(LVImagePtrDstSGL + y * LVLineWidthDst,
				(float*)dst.data + y * LVWidth,
				LVWidth * sizeof(float));
		}
		break;
	default:
		ADV_SetLVError(ERR_INVALID_IMAGE_TYPE, __func__, ErrorCluster);
		return;
		break;
	}
}
