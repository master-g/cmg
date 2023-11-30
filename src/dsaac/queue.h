/*
 *  queue.h
 *  DSAAC
 *
 *  Created by Master.G on 23-11-30.
 *  Copyright (c) 2023 Master.G. All rights reserved.
 */
#ifndef QUEUE_H_
#define QUEUE_H_

#include "list.h"

typedef list_t queue_t;

#define queue_alloc list_alloc
#define queue_free list_free
#define queue_is_empty list_is_empty
#define queue_len list_len

#define queue_enqueue list_push
#define queue_dequeue list_remove_first
#define queue_front list_first
#define queue_rear list_last

#endif /* QUEUE_H_ */
