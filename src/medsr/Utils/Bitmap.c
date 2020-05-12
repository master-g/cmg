/*
 *  Bitmap.c
 *  MEDSR
 *
 *  Created by Master.G on 14-4-16.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "Bitmap.h"

void BMPHeader_Setup(bmp_header_t *bh, int w, int h) {
  int bytesPerLine;

  memset(bh, 0, sizeof(bmp_header_t));
  bh->id[0] = 'B';
  bh->id[1] = 'M';
  bh->reserved[0] = 0;
  bh->reserved[1] = 0;
  bh->headerSize = 54;
  bh->infoSize = 0x28;
  bh->width = w;
  bh->depth = h;
  bh->biPlanes = 1;
  bh->bits = 24;
  bh->biCompression = 0;

  bytesPerLine = bh->width * 3;
  if (bytesPerLine & 0x0003) {
    bytesPerLine |= 0x0003;
    bytesPerLine++;
  }

  bh->filesize = bh->headerSize + (int32_t)bytesPerLine * bh->depth;
}

void BMPHeader_Dump(bmp_header_t *bh, char *buffer) {
  int offset = 0;
  memcpy(buffer + offset, bh->id, 2);
  offset += 2;
  memcpy(buffer + offset, &bh->filesize, sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer + offset, bh->reserved, sizeof(int16_t) * 2);
  offset += sizeof(int16_t) * 2;
  memcpy(buffer + offset, &bh->headerSize, sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer + offset, &bh->infoSize, sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer + offset, &bh->width, sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer + offset, &bh->depth, sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer + offset, &bh->biPlanes, sizeof(int16_t));
  offset += sizeof(int16_t);
  memcpy(buffer + offset, &bh->bits, sizeof(int16_t));
  offset += sizeof(int16_t);
  memcpy(buffer + offset, &bh->biCompression, sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer + offset, &bh->biSizeImage, sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer + offset, &bh->biXpelsPerMeter, sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer + offset, &bh->biYpelsPerMeter, sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer + offset, &bh->biClrUsed, sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer + offset, &bh->biClrImportant, sizeof(int32_t));
}

int32_t BMPHeader_BytesPerLine(bmp_header_t *bh) {
  int bytesPerLine = bh->width * 3;
  if (bytesPerLine & 0x0003) {
    bytesPerLine |= 0x0003;
    bytesPerLine++;
  }

  return bytesPerLine;
}

void Bitmap_Dump(const char *filename, int w, int h, char *buffer) {
  int x, y;
  int bytesPerLine = 0;
  FILE *bmpfile;
  char bhbuf[BMP_HEADER_SIZE];
  char *linebuf;

  bmp_header_t bh;

  BMPHeader_Setup(&bh, w, h);
  BMPHeader_Dump(&bh, bhbuf);

  bmpfile = fopen(filename, "wb");
  if (bmpfile == NULL)
    return;

  fwrite(bhbuf, 1, BMP_HEADER_SIZE, bmpfile);

  bytesPerLine = BMPHeader_BytesPerLine(&bh);
  linebuf = (char *)malloc(bytesPerLine);

  for (y = bh.depth - 1; y >= 0; y--) {
    for (x = 0; x < bh.width; x++) {
      linebuf[x * 3 + 0] = buffer[(y * bh.width + x) * 4 + 2];
      linebuf[x * 3 + 1] = buffer[(y * bh.width + x) * 4 + 1];
      linebuf[x * 3 + 2] = buffer[(y * bh.width + x) * 4 + 0];
    }

    fwrite(linebuf, 1, bytesPerLine, bmpfile);
  }

  fclose(bmpfile);
  free(linebuf);
}
