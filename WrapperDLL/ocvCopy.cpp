#include "pch.h"
#define NI_OCV_SUPPORT
#include "NIImage.h"
#include "ADVImage.h"

extern "C" __declspec(dllexport) void opencvCopyNI(
    NIImageHandle SrcImage, NIImageHandle DstImage,
    NIErrorHandle errorHandle)
{
    NIERROR error = NI_ERR_SUCCESS;
    ReturnOnPreviousError(errorHandle);
    try
    {
        if (!SrcImage || !DstImage || !errorHandle) {
            ThrowNIError(NI_ERR_NULL_POINTER);
        }

        NIImage srcImage(SrcImage);
        NIImage dstImage(DstImage);

        dstImage.ResizeImage(srcImage.width, srcImage.height);
        Mat srcMat;
        Mat dstMat;
        ThrowNIError(srcImage.ImageToMat(srcMat));
        ThrowNIError(dstImage.ImageToMat(dstMat));
        srcMat.copyTo(dstMat);
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
} //opencv2Copy

extern "C" __declspec(dllexport) int opencvCopyNI2(
    NIImageHandle SrcImage, NIImageHandle DstImage,
    NIErrorHandle errorHandle)
{
    NIERROR error = NI_ERR_SUCCESS;
    //ReturnOnPreviousError(errorHandle);
    if (!SrcImage || !DstImage || !errorHandle) return NI_ERR_NULL_POINTER;
  
    NIImage srcImage(SrcImage);
    NIImage dstImage(DstImage);

    //dstImage.ResizeImage(srcImage.width, srcImage.height);
    Mat srcMat;
    Mat dstMat;
    srcImage.ImageToMat(srcMat);
    dstImage.ImageToMat(dstMat);
    srcMat.copyTo(dstMat);
  
	return NI_ERR_SUCCESS;
	//ProcessNIError(error, errorHandle);
} //opencv2Copy


extern "C" __declspec(dllexport) void opencvCopyAD(
    NIImageHandle SrcImage, NIImageHandle DstImage,
	LVErrorCluster* ErrorCluster)
{
	Image *ImgSrc, *ImgDst;
	void *LVImagePtrSrc, *LVImagePtrDst;
	unsigned short *LVImagePtrSrcU16, *LVImagePtrDstU16;
	float *LVImagePtrSrcSGL, *LVImagePtrDstSGL;
	int LVWidth, LVHeight,	LVLineWidthSrc, LVLineWidthDst;
	int opencv2type = 0; int bpp;

	CHECK_ERROR_IN(ErrorCluster);
	LV_IS_NOT_IMAGE(SrcImage, ErrorCluster);
	LV_IS_NOT_IMAGE(DstImage, ErrorCluster);
	
	LV_SetThreadCore(1i64);
	LV_LVDTToGRImage(SrcImage, &ImgSrc);
	LV_LVDTToGRImage(DstImage, &ImgDst);

	LV_IS_NOT_IMAGE(ImgDst, ErrorCluster);
	LV_IS_NOT_IMAGE(ImgSrc, ErrorCluster);

	LVWidth = ((ImageInfo *)ImgSrc)->xRes;
	LVHeight = ((ImageInfo *)ImgSrc)->yRes;
	LVLineWidthSrc = ((ImageInfo *)ImgSrc)->pixelsPerLine;
 
    imaqSetImageSize (ImgDst, LVWidth, LVHeight);
	LVLineWidthDst = ((ImageInfo *)ImgDst)->pixelsPerLine;	

	switch (((ImageInfo *)ImgSrc)->imageType){
		case IMAQ_IMAGE_U16:
			LVImagePtrSrc  = (unsigned short int *)((ImageInfo *)ImgSrc)->imageStart;
			opencv2type = CV_16UC1; bpp = 2;
			LV_IS_NOT_IMAGE(LVImagePtrSrc, ErrorCluster);
			LVImagePtrDstU16  = (unsigned short int *)((ImageInfo *)ImgDst)->imageStart;
			LVImagePtrDst = (unsigned short int *)((ImageInfo *)ImgDst)->imageStart;
			LV_IS_NOT_IMAGE(LVImagePtrDst, ErrorCluster);			
			break;
		case IMAQ_IMAGE_SGL:
			LVImagePtrSrc  = (float *)((ImageInfo *)ImgSrc)->imageStart;
			opencv2type = CV_32FC1; bpp = 4;
			LV_IS_NOT_IMAGE(LVImagePtrSrc, ErrorCluster);
			LVImagePtrDstSGL  = (float *)((ImageInfo *)ImgDst)->imageStart;
						LVImagePtrDst = (float *)((ImageInfo *)ImgDst)->imageStart;
			LV_IS_NOT_IMAGE(LVImagePtrDst, ErrorCluster);			
			break;
		default:
			ADV_SetLVError(ERR_INVALID_IMAGE_TYPE, __func__, ErrorCluster);
			return;
			break;
	}

	Mat src(LVWidth, LVHeight, opencv2type, LVImagePtrSrc, LVLineWidthSrc * bpp);	
	Mat dst(LVWidth, LVHeight, opencv2type, LVImagePtrDst, LVLineWidthDst * bpp);

	src.copyTo(dst);
}


extern "C" __declspec(dllexport) void ippCopy(const void* src, void* dst, LVErrorCluster* ErrorCluster)
{
	Ipp16u* pSrc;
	Ipp16u* pDst;
	int Width, Height;
	int srcStep;
	int dstStep;

	Image* SrcImg, * DstImg;

	SrcImg = ADV_LVDTToAddress(src);
	DstImg = ADV_LVDTToAddress(dst);

	Width = ((ImageInfo*)SrcImg)->xRes;
	Height = ((ImageInfo*)SrcImg)->yRes;
	srcStep = (((ImageInfo*)SrcImg)->pixelsPerLine);
	imaqSetImageSize(DstImg, Width, Height);

	dstStep = (((ImageInfo*)DstImg)->pixelsPerLine);

	pSrc = (unsigned short int*)((ImageInfo*)SrcImg)->imageStart;
	pDst = (unsigned short int*)((ImageInfo*)DstImg)->imageStart;

	IppStatus status = ippStsNoErr;
	IppiSize roiSizeSrc = { Width, Height };  /* Size of source/destination ROI in pixels, 1024x1024 by default */
	IppiSize roiSizeDst = { Width * 2, Height * 2 };  /* Size of source/destination ROI in pixels, 2048x2048 */
	int xStep = Width * 2 * 4, yStep = Height * 2 * 4;     /* Steps, in bytes, through the x and y maps */
	IppiRect rect = { 0, 0, Width, Height }; /* Region of interest in the source image */

	ippiCopy_16u_C1R(pSrc, srcStep * 2, pDst, dstStep * 2, roiSizeDst);

	return;
}
