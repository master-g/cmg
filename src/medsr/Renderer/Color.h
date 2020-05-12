/*
 *  Color.h
 *  MEDSR
 *
 *  Created by Master.G on 14-4-16.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef MEDSR_COLOR_H_
#define MEDSR_COLOR_H_

#include "Common.h"

#define Color4i_Format(r, g, b, a)                                             \
  (((r)&0xFF) << 24 | ((g)&0xFF) << 16 | ((b)&0xFF) << 8 | ((a)&0xFF))

#define Color4i_Unpack(c, r, g, b, a)                                          \
  (*(r) = (((c) >> 24) & 0xFF), *(g) = (((c) >> 16) & 0xFF),                   \
   *(b) = (((c) >> 8) & 0xFF), *(a) = ((c)&0xFF))

#define Color4f_Format(c, r, g, b, a)                                          \
  ((c)[0] = (r) / 255.0f, (c)[1] = (g) / 255.0f, (c)[2] = (b) / 255.0f,        \
   (c)[3] = (a) / 255.0f)

/* http://www.colourlovers.com/blog/2007/07/24/32-common-color-names-for-easy-reference
 */

#define ColorPreset4i_Red 0xFF0000FF
#define ColorPreset4i_Green 0x00FF00FF
#define ColorPreset4i_Blue 0x0000FFFF
#define ColorPreset4i_White 0xFFFFFFFF
#define ColorPreset4i_Black 0x000000FF

#define ColorPreset4i_Ivory 0xFFFFF0FF
#define ColorPreset4i_Beige 0xF5F5DCFF
#define ColorPreset4i_Wheat 0xF5DEB3FF
#define ColorPreset4i_Tan 0xD2B48CFF
#define ColorPreset4i_Khaki 0xC3B091FF
#define ColorPreset4i_Silver 0xC0C0C0FF
#define ColorPreset4i_Gray 0x808080FF
#define ColorPreset4i_Charcoal 0x464646FF
#define ColorPreset4i_NavyBlue 0x000080FF
#define ColorPreset4i_RoyalBlue 0x084C9EFF
#define ColorPreset4i_MediumBlue 0x0000CDFF
#define ColorPreset4i_Azure 0x007FFFFF
#define ColorPreset4i_Cyan 0x00FFFFFF
#define ColorPreset4i_Aquamarine 0x7FFFD4FF
#define ColorPreset4i_Teal 0x008080FF
#define ColorPreset4i_ForestGreen 0x228B22FF
#define ColorPreset4i_Olive 0x808000FF
#define ColorPreset4i_Chartreuse 0x7FFF00FF
#define ColorPreset4i_Lime 0xBFFF00FF
#define ColorPreset4i_Golden 0xFFD700FF
#define ColorPreset4i_Goldenrod 0xDAA520FF
#define ColorPreset4i_Coral 0xFF7F50FF
#define ColorPreset4i_Salmon 0xFA8072FF
#define ColorPreset4i_HotPink 0xFC0FC0FF
#define ColorPreset4i_Fuchsia 0xFF77FFFF
#define ColorPreset4i_Puce 0xCC8899FF
#define ColorPreset4i_Mauve 0xE0B0FFFF
#define ColorPreset4i_Lavender 0xB57EDCFF
#define ColorPreset4i_Plum 0x843179FF
#define ColorPreset4i_Indigo 0x4B0082FF
#define ColorPreset4i_Maroon 0x800000FF
#define ColorPreset4i_Crimson 0xDC143CFF

#endif /* MEDSR_COLOR_H_ */
