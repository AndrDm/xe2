#ifndef OCV_H
#define OCV_H
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

//==============================================================================
//
// Title:       Header for Wrapper
// Purpose:     A short description of the interface.
//
// Created on:  21.04.2011 at 14:45:38 by AD.
// Copyright:   AVD. All Rights Reserved.
//
//==============================================================================

#include <windows.h>
#include <stdio.h>
#include "extcode.h"
#include "nivision.h"
#include "NIImage.h"

#define U8		0x1
#define U16		0x2
#define I16		0x4
#define SGL		0x8
#define COMPLEX 0x10
#define RGB_U8	0x20
#define HSL		0x40
#define RGB_U64 0x80
		
typedef struct {
	LVBoolean status;
	int32 code;
	LStrHandle source;
} LVErrorCluster;

typedef struct {
	int32 dimSize;
	LStrHandle elt[512];
	} TDCallChain;
typedef TDCallChain **TDCallChainHdl;

typedef struct IMAQ_Image{
		LStrHandle name;
		Image *address;
} IMAQ_Image, *IMAQ_ImagePtr, **IMAQ_ImageHandle;

extern "C" void  LV_LVDTToImage(void *image, const void *in);
extern "C" void  LV_ImageToLVDT(const void *image, void *out);
extern "C" uint8_t GGetOneErrorCode(int ErrorCode, LStrHandle* ErrStr);

#define CHECK_ERROR_IN(ErrorCluster) if((ErrorCluster)->status)return;

#define RETURN_ERROR(Error,ErrorCluster) \
	{if(Error){ADV_SetLVError(Error, __func__, ErrorCluster);};return;}

//Check images
#define IS_NOT_IMAGE(img) \
	if (!(img)) return ERR_NOT_IMAGE

#define LV_IS_NOT_IMAGE(img,ErrorCluster) \
	if (!(img)) {ADV_SetLVError(ERR_NOT_IMAGE, __func__, ErrorCluster); return;}

#define IS_NOT_IMAGE2(img1,img2) \
	if ( (!(img1)) || (!(img2)) ) return;

#define LV_IS_NOT_IMAGE2(img1,img2,ErrorCluster) \
	if ( (!(img1)) || (!(img2)) ) {ADV_SetLVError(ERR_NOT_IMAGE, __func__, ErrorCluster); return;}

#define IS_NOT_IMAGE3(img1,img2,img3) \
	if ( (!(img1)) || (!(img2)) || (!(img3)) ) return;

#define LV_IS_NOT_IMAGE3(img1,img2,img3,ErrorCluster) \
	if ( (!(img1)) || (!(img2)) || (!(img3)) ) {ADV_SetLVError(ERR_NOT_IMAGE, __func__, ErrorCluster); return;}

#define IS_NOT_IMAGE4(img1,img2,img3,img4) \
	if ( (!(img1)) || (!(img2)) || (!(img3)) || (!(img4)) ) return;

#define LV_IS_NOT_IMAGE4(img1,img2,img3,img4,ErrorCluster) \
	if ( (!(img1)) || (!(img2)) || (!(img3)) || (!(img4)) )  {ADV_SetLVError(ERR_NOT_IMAGE, __func__, ErrorCluster); return;}

#define IS_TOO_SMALL(img) \
	if ( ( ((ImageInfo*)(img))->xRes == 0) || (((ImageInfo*)(img))->yRes == 0) ) return;

#define LV_IS_TOO_SMALL(img,ErrorCluster) \
	if ( ( ((ImageInfo*)(img))->xRes == 0) || (((ImageInfo*)(img))->yRes == 0) ) {ADV_SetLVError(ERR_IMAGE_TOO_SMALL, __func__, ErrorCluster); return;} 

#define IS_TOO_SMALL2(img1, img2) \
	if ( ( ((ImageInfo*)(img1))->xRes == 0) || (((ImageInfo*)(img1))->yRes == 0) ) return; \
	if ( ( ((ImageInfo*)(img2))->xRes == 0) || (((ImageInfo*)(img2))->yRes == 0) ) return;

#define LV_IS_TOO_SMALL2(img1, img2,ErrorCluster) \
	if ( ( ((ImageInfo*)(img1))->xRes == 0) || (((ImageInfo*)(img1))->yRes == 0) ) {ADV_SetLVError(ERR_IMAGE_TOO_SMALL, __func__, ErrorCluster); return;} ; \
	if ( ( ((ImageInfo*)(img2))->xRes == 0) || (((ImageInfo*)(img2))->yRes == 0) ) {ADV_SetLVError(ERR_IMAGE_TOO_SMALL, __func__, ErrorCluster); return;}

#define IS_SAME_SIZE(img1,img2) \
	if ((((ImageInfo*)(img1))->xRes != ((ImageInfo*)(img2))->xRes) || (((ImageInfo*)(img1))->yRes != ((ImageInfo*)(img2))->yRes) )return ERR_INCOMP_SIZE

#define LV_IS_SAME_SIZE(img1,img2,ErrorCluster) \
	if ((((ImageInfo*)(img1))->xRes != ((ImageInfo*)(img2))->xRes) || (((ImageInfo*)(img1))->yRes != ((ImageInfo*)(img2))->yRes) ){ADV_SetLVError(ERR_INCOMP_SIZE, __func__, ErrorCluster); return;} 

#define IS_SAME_TYPE(img1,img2) \
	if ((((ImageInfo*)(img1))->imageType != ((ImageInfo*)(img2))->imageType) )return ERR_INCOMP_TYPE

#define LV_IS_SAME_TYPE(img1,img2,ErrorCluster) \
	if ((((ImageInfo*)(img1))->imageType != ((ImageInfo*)(img2))->imageType) ){ADV_SetLVError(ERR_INCOMP_TYPE, __func__, ErrorCluster); return;}

#define IS_SAME_TYPE3(img1,img2,img3) \
	if ((((ImageInfo*)(img1))->imageType != ((ImageInfo*)(img2))->imageType) )return; \
	if ((((ImageInfo*)(img1))->imageType != ((ImageInfo*)(img3))->imageType) )return;

#define LV_IS_SAME_TYPE3(img1,img2,img3,ErrorCluster) \
	if ((((ImageInfo*)(img1))->imageType != ((ImageInfo*)(img2))->imageType) ){ADV_SetLVError(ERR_INCOMP_TYPE, __func__, ErrorCluster); return;}; \
	if ((((ImageInfo*)(img1))->imageType != ((ImageInfo*)(img3))->imageType) ){ADV_SetLVError(ERR_INCOMP_TYPE, __func__, ErrorCluster); return;}



#define IS_U8_TYPE(img) \
	if (((ImageInfo*)(img))->imageType != IMAQ_IMAGE_U8)return ERR_INCOMP_TYPE	

#define LV_IS_U8_TYPE(img,ErrorCluster) \
	if (((ImageInfo*)(img))->imageType != IMAQ_IMAGE_U8){ADV_SetLVError(ERR_INCOMP_TYPE, __func__, ErrorCluster); return;} 


#define RESIZE_IF_NECESSARY(src,dst) \
	if ((((ImageInfo*)(src))->xRes != ((ImageInfo*)(dst))->xRes) || \
		(((ImageInfo*)(src))->yRes != ((ImageInfo*)(dst))->yRes) ){ \
		  imaqSetImageSize(dst, ((ImageInfo*)(src))->xRes, ((ImageInfo*)(src))->yRes); }

#define CHECK_TYPE(img, type)\
	if ( ((type) & U8)     	     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_U8 ));\
	else if ( ((type) & U16)     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_U16));\
	else if ( ((type) & I16)     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_I16));\
	else if ( ((type) & SGL)     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_SGL));\
	else if ( ((type) & COMPLEX) && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_COMPLEX));\
	else if ( ((type) & RGB_U8)     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_RGB));\
	else if ( ((type) & HSL)     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_HSL));\
	else if ( ((type) & RGB_U64) && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_RGB_U64 ));\
	else return ERR_INVALID_IMAGE_TYPE;

#define LV_CHECK_TYPE(img, type,ErrorCluster)\
	if ( ((type) & U8)     	     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_U8 ))\
	else if ( ((type) & U16)     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_U16))\
	else if ( ((type) & I16)     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_I16))\
	else if ( ((type) & SGL)     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_SGL))\
	else if ( ((type) & COMPLEX) && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_COMPLEX))\
	else if ( ((type) & RGB_U8)     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_RGB))\
	else if ( ((type) & HSL)     && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_HSL))\
	else if ( ((type) & RGB_U64) && ((((ImageInfo*)(img))->imageType) == IMAQ_IMAGE_RGB_U64 ))\
	else {ADV_SetLVError(ERR_INVALID_IMAGE_TYPE, ErrorCluster); return;}

Image* ADV_LVDTToAddress(const void* LVImageHdl);
int ADV_SetLVError(int ErrorCode, const char* errText, LVErrorCluster* ErrorCluster);
int ADV_ProcessLVError(int ErrorCode, const char* errText, NIErrorHandle errorHandle);
int ADV_SetLVErrorWithCallChain(int ErrorCode, LVErrorCluster* ErrorCluster);

#endif
