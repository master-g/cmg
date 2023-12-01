/*
 *  list.h
 *  DSAAC
 *
 *  Created by Master.G on 23-11-30.
 *  Copyright (c) 2023 Master.G. All rights reserved.
 */
#ifndef LIST_H_
#define LIST_H_

#include "payload.h"

typedef struct list_t list_t;

typedef int (*list_find_func)(const pdata *data, const pdata *key);

typedef pdata *(*list_sum_fun)(
    const list_t *l, int index, const pdata *data, const pdata *acc);

typedef struct list_iter_t list_iter_t;

list_iter_t *list_iter_alloc(const list_t *l);
const pdata *list_iter_next(list_iter_t *iter);
const pdata *list_iter_prev(list_iter_t *iter);
void list_iter_reset(list_iter_t *iter);
void list_iter_free(list_iter_t *iter);

/**
 * \brief allocate a list
 * \return a pointer to the list
 */
list_t *list_alloc(void);

/**
 * \brief free a list
 * each node in the list will be freed
 * if the payload of the node is not NULL, and is owned by the node,
 * will be freed too
 * \param l the list to be freed
 */
void list_free(list_t *l);

/**
 * \brief check if the list is empty
 * \param l the list to be checked
 * \return 1 if the list is empty, 0 otherwise
 */
int list_is_empty(const list_t *l);

/**
 * \brief insert a node into the list
 * \param l the list to be inserted
 * \param where position to be inserted
 * \param data what to be inserted
 * \return 0 if success
 */
int list_insert(list_t *l, int where, const pdata *data);

/**
 * \brief push a node to the end of the list
 * \param l the list to be pushed
 * \param data what to be pushed
 * \return 0 if success
 */
int list_push(list_t *l, const pdata *data);

/**
 * \brief unshift a node to the head of the list
 * \param l the list to be unshifted
 * \param data what to be unshifted
 * \return 0 if success
 */
int list_unshift(list_t *l, const pdata *data);

/**
 * \brief remove a node from the list
 * \param l the list to be removed
 * \param data what to be removed
 * \return 0 if success
 */
int list_remove(const list_t *l, const pdata *data);

/**
 * \brief remove a node from the list at the specified position
 * \param l the list to be removed
 * \param index position to be removed
 * \return NULL if failed, otherwise the payload of the removed node
 */
const pdata *list_remove_at(const list_t *l, int index);

/**
 * \brief remove the first node from the list
 * \param l the list to be removed
 * \return NULL if failed, otherwise the payload of the removed node
 */
const pdata *list_remove_first(list_t *l);

/**
 * \brief remove the last node from the list
 * \param l the list to be removed
 * \return NULL if failed, otherwise the payload of the removed node
 */
const pdata *list_remove_last(list_t *l);

/**
 * \brief return the first node of the list
 * \param l the list to be checked
 * \return NULL if failed, otherwise the payload of the first node
 */
const pdata *list_first(const list_t *l);

/**
 * \brief return the last node of the list
 * \param l the list to be checked
 * \return NULL if failed, otherwise the payload of the last node
 */
const pdata *list_last(const list_t *l);

/**
 * \brief return the node at the specified position
 * \param l the list to be checked
 * \param index position to be checked
 * \return NULL if failed, otherwise the payload of the node
 */
const pdata *list_get(const list_t *l, int index);

/**
 * \brief return the length of the list
 * \param l the list to be checked
 * \return 0 if failed or empty, otherwise the length of the list
 */
size_t list_len(const list_t *l);

/**
 * \brief find the index of the specified payload
 * \param l the list to be checked
 * \param data what to be checked
 * \return -1 if failed, otherwise the index of the payload
 */
int list_index_of(const list_t *l, const pdata *data);

/**
 * \brief find the node with the specified payload
 * \param l the list to be checked
 * \param finder the function to be used to find the node
 * \param key the key to be used to find the node
 * \return -1 if failed, otherwise the index of the payload
 */
int list_find(const list_t *l, list_find_func finder, const pdata *key);

/**
 * \brief iterate the list
 * \param l the list to be iterated
 * \param ctx the context to be passed to the function
 * \param func the function to be used to iterate the list
 */
void list_foreach_self_ctx_indexed(
    const list_t *l, void *ctx,
    void (*func)(const list_t *l_, void *ctx_, int index, const pdata *data));

void list_foreach_ctx_indexed(
    const list_t *l, void *ctx,
    void (*func)(void *ctx_, int index, const pdata *data));

void list_foreach_self_indexed(
    const list_t *l, void *ctx,
    void (*func)(const list_t *l_, int index, const pdata *data));

void list_foreach_self_ctx(
    const list_t *l, void *ctx,
    void (*func)(const list_t *l_, void *ctx_, const pdata *data));

void list_foreach_self(
    const list_t *l, void (*func)(const list_t *l_, const pdata *data));

void list_foreach_ctx(
    const list_t *l, void *ctx, void (*func)(void *ctx_, const pdata *data));

void list_foreach_indexed(
    const list_t *l, void (*func)(int index, const pdata *data));

/**
 * \brief iterate the list
 * \param l the list to be iterated
 * \param func the function to be used to iterate the list
 */
void list_foreach(const list_t *l, void(func)(const pdata *));

#endif /* LIST_H_ */
