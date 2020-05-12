/*
 *  Camera.h
 *  MEDSR
 *
 *  Created by Master.G on 14-4-16.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef MEDSR_CAMERA_H_
#define MEDSR_CAMERA_H_

#include "Math/Vector.h"

#define Camera_SetPosition(c, p) (Vec3f_Copy((c)->position, (p)))

#define Camera_SetTarget(c, t) (Vec3f_Copy((c)->target, (t)))

typedef struct camera_s {
  vec3f_t position;
  vec3f_t target;

} camera_t;

#endif /* MEDSR_CAMERA_H_ */
