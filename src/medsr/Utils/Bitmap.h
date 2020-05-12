/*
 *  Bitmap.h
 *  MEDSR
 *
 *  Created by Master.G on 14-4-16.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef MEDSR_BITMAP_H_
#define MEDSR_BITMAP_H_

#include "Common.h"

#define BMP_HEADER_SIZE 54

typedef struct bmp_header_s {
  char id[2];
  int32_t filesize;
  int16_t reserved[2];
  int32_t headerSize;
  int32_t infoSize;
  int32_t width;
  int32_t depth;
  int16_t biPlanes;
  int16_t bits;
  int32_t biCompression;
  int32_t biSizeImage;
  int32_t biXpelsPerMeter;
  int32_t biYpelsPerMeter;
  int32_t biClrUsed;
  int32_t biClrImportant;

} bmp_header_t;

/**
 *	@brief	setup a 24bit bitmap image header
 *
 *	@param 	bmpheader 	bitmap file header structure
 *	@param 	w 	width of the image
 *	@param 	h 	height of the image
 */
void BMPHeader_Setup(bmp_header_t *bmpheader, int w, int h);

/**
 *	@brief	dump a bitmap header to bytes
 *
 *	@param 	bh 	bitmap header to dump
 *	@param 	buffer 	output buffer
 */
void BMPHeader_Dump(bmp_header_t *bh, char *buffer);

/**
 *	@brief	calculate bytes per line in bitmap image
 *
 *	@param 	bh 	bitmap header
 *
 *	@return	bytes per line
 */
int32_t BMPHeader_BytesPerLine(bmp_header_t *bh);

/**
 *	@brief	dump buffer to bitmap image
 *
 *	@param 	filename 	bitmap file name
 *	@param 	w 	width
 *	@param 	h 	height
 *	@param 	buffer 	back buffer
 */
void Bitmap_Dump(const char *filename, int w, int h, char *buffer);

#endif /* MEDSR_BITMAP_H_ */
