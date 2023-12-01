/*
 *  dot.c
 *  DSAAC
 *
 *  Created by Master.G on 23-12-01.
 *  Copyright (c) 2023 Master.G. All rights reserved.
 */

#include "dot.h"
#include "common.h"
#include "list.h"

#define DOT_HEADER "digraph G {\n"
#define DOT_FOOTER "}\n"

struct dot_t {
  list_t *head;
  list_t *body;
  list_t *foot;
};

dot_t *dot_alloc(void) {
  dot_t *dot = malloc(sizeof(dot_t));
  dot->head = NULL;
  dot->body = NULL;
  dot->foot = NULL;

  return dot;
}

void _dot_free_pdata(const pdata *data) {
  char *content = pdata_raw(data);
  free(content);
  pdata_free(data);
}

void dot_free(dot_t *dot) {
  if (dot == NULL) {
    return;
  }

  if (dot->head != NULL) {
    list_foreach(dot->head, _dot_free_pdata);
    list_free(dot->head);
  }
  if (dot->body != NULL) {
    list_foreach(dot->body, _dot_free_pdata);
    list_free(dot->body);
  }
  if (dot->foot != NULL) {
    list_foreach(dot->foot, _dot_free_pdata);
    list_free(dot->foot);
  }

  free(dot);
}

int _dot_push(list_t **l, const char *content) {
  if (l == NULL || content == NULL) {
    return -1;
  }

  const size_t len = strlen(content);
  if (len == 0) {
    return -1;
  }

  if (*l == NULL) {
    *l = list_alloc();
  }

  char *buf = malloc(len);
  memcpy(buf, content, len);
  const pdata *data = pdata_from_ref(buf);
  list_push(*l, data);

  return 0;
}

int _dot_unshift(list_t **l, const char *content) {
  if (l == NULL || content == NULL) {
    return -1;
  }

  const size_t len = strlen(content);
  if (len == 0) {
    return -1;
  }

  if (*l == NULL) {
    *l = list_alloc();
  }

  char *buf = malloc(len);
  memcpy(buf, content, len);
  const pdata *data = pdata_from_ref(buf);
  list_unshift(*l, data);

  return 0;
}

int dot_push_head(dot_t *dot, const char *content) {
  if (dot == NULL) {
    return -1;
  }

  return _dot_push(&dot->head, content);
}

int dot_unshift_head(dot_t *dot, const char *content) {
  if (dot == NULL) {
    return -1;
  }

  return _dot_unshift(&dot->head, content);
}

int dot_push_body(dot_t *dot, const char *content) {
  if (dot == NULL) {
    return -1;
  }

  return _dot_push(&dot->body, content);
}

int dot_unshift_body(dot_t *dot, const char *content) {
  if (dot == NULL) {
    return -1;
  }

  return _dot_unshift(&dot->body, content);
}

int dot_push_foot(dot_t *dot, const char *content) {
  if (dot == NULL) {
    return -1;
  }

  return _dot_push(&dot->foot, content);
}

int dot_unshift_foot(dot_t *dot, const char *content) {
  if (dot == NULL) {
    return -1;
  }

  return _dot_unshift(&dot->foot, content);
}

ssize_t dot_dump(const dot_t *dot, char **buf, size_t *size) {
  if (dot == NULL || buf == NULL || size == NULL) {
    return -1;
  }

  size_t required_size = 0;
  list_iter_t *iter_head = NULL;
  list_iter_t *iter_body = NULL;
  list_iter_t *iter_foot = NULL;
  const pdata *d = NULL;
  if (dot->head != NULL) {
    iter_head = list_iter_alloc(dot->head);
    while ((d = list_iter_next(iter_head)) != NULL) {
      required_size += strlen(pdata_raw(d));
    }
    list_iter_reset(iter_head);
  }
  if (dot->body != NULL) {
    iter_body = list_iter_alloc(dot->body);
    while ((d = list_iter_next(iter_body)) != NULL) {
      required_size += strlen(pdata_raw(d));
    }
    list_iter_reset(iter_body);
  }
  if (dot->foot != NULL) {
    iter_foot = list_iter_alloc(dot->foot);
    while ((d = list_iter_next(iter_foot)) != NULL) {
      required_size += strlen(pdata_raw(d));
    }
    list_iter_reset(iter_foot);
  }

  required_size += strlen(DOT_HEADER) + strlen(DOT_FOOTER) + 1;
  if (*buf == NULL) {
    *size = required_size;
    *buf = malloc(required_size);
    if (*buf == NULL) {
      required_size = -1;
    }
  } else {
    if (*size < required_size) {
      required_size = -1;
    }
  }

  // actual print
  size_t pos = 0;
  if (required_size > 0) {
    memcpy(*buf + pos, DOT_HEADER, strlen(DOT_HEADER));
    pos += strlen(DOT_HEADER);
  }

  while ((d = list_iter_next(iter_head)) != NULL) {
    if (required_size > 0) {
      memcpy(*buf + pos, pdata_raw(d), strlen(pdata_raw(d)));
      pos += strlen(pdata_raw(d));
    }
  }
  list_iter_free(iter_head);

  while ((d = list_iter_next(iter_body)) != NULL) {
    if (required_size > 0) {
      memcpy(*buf + pos, pdata_raw(d), strlen(pdata_raw(d)));
      pos += strlen(pdata_raw(d));
    }
  }
  list_iter_free(iter_body);

  while ((d = list_iter_next(iter_foot)) != NULL) {
    if (required_size > 0) {
      memcpy(*buf + pos, pdata_raw(d), strlen(pdata_raw(d)));
      pos += strlen(pdata_raw(d));
    }
  }
  list_iter_free(iter_foot);

  if (required_size > 0) {
    memcpy(*buf + pos, DOT_FOOTER, strlen(DOT_FOOTER));
    pos += strlen(DOT_FOOTER);
    (*buf)[pos] = '\0';
  }

  return required_size;
}
