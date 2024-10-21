/* 
 * Copyright 2024 Analog Devices, Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     https://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "private/fifo.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// Calculates the next value a queue buffer index will have. No assignemnts, and no side effects.
#define NEXT_IDX(queue, idx) (((idx) + (queue)->elem_size) % (queue)->buf_size)

// Assigns a queue buffer index to its next value, one element ahead.
#define ADVANCE_IDX(queue, idx) (idx) = NEXT_IDX(queue, idx)

static _adi_fifo_Result_e fifo_init_buf(volatile _adi_fifo_t *queue, void *buf, size_t buf_size, size_t elem_size)
{
    if (buf == NULL) {
        return FIFO_ERR_BAD_PARAM;
    }
    if (elem_size < 1) {
        return FIFO_ERR_BAD_PARAM;
    }
    if (buf_size < 2 * elem_size) {
        return FIFO_ERR_BAD_PARAM;
    }

    queue->head = 0;
    queue->tail = 0;
    queue->buf_size = buf_size;
    queue->elem_size = elem_size;
    queue->buf = buf;
    return FIFO_ERR_OK;
}

_adi_fifo_Result_e _adi_fifo_Init(volatile _adi_fifo_t *queue, size_t len, size_t elem_size)
{
    // overhead of 1 extra element for head/tail implementation logic
    if (queue == NULL || len == SIZE_MAX || elem_size == 0) {
        return FIFO_ERR_BAD_PARAM;
    }
    size_t buf_len = len + 1;

    // make sure the necessary buffer size doesn't overflow in size_t
    if (elem_size != 0 && buf_len > SIZE_MAX / elem_size) {
        return FIFO_ERR_BAD_PARAM;
    }
    size_t buf_size = buf_len * elem_size;

    // allocate buffer
    void* buf = malloc(buf_size);
    if (buf == NULL) {
        return FIFO_ERR_INTERNAL;
    }

    return fifo_init_buf(queue, buf, buf_size, elem_size);
}

_adi_fifo_Result_e _adi_fifo_Free(volatile _adi_fifo_t *queue)
{
    if (queue == NULL) {
        return FIFO_ERR_BAD_PARAM;
    }

    free(queue->buf);
    queue->buf = NULL;
    return FIFO_ERR_OK;
}

_adi_fifo_Result_e _adi_fifo_Push(volatile _adi_fifo_t* queue, void* data)
{
    if (queue == NULL || queue->buf == NULL || data == NULL) {
        return FIFO_ERR_BAD_PARAM;
    }

    if (_adi_fifo_is_Full(queue)) {
        return FIFO_ERR_BUFFER_FULL;
    }
    char *buf = queue->buf;
    memcpy(&buf[queue->head], data, queue->elem_size);
    ADVANCE_IDX(queue, queue->head);
    return FIFO_ERR_OK;
}

_adi_fifo_Result_e _adi_fifo_Pop(volatile _adi_fifo_t* queue, void* data)
{
    if (queue == NULL) {
        return FIFO_ERR_BAD_PARAM;
    }

    _adi_fifo_Result_e ret = FIFO_ERR_OK;
    if (data != NULL) {
        ret = _adi_fifo_Read(queue, data);
    }
    if (ret == FIFO_ERR_OK) {
        ADVANCE_IDX(queue, queue->tail);
    }
    return ret;
}

bool _adi_fifo_IsEmpty(volatile _adi_fifo_t* queue)
{
    if (queue == NULL) {
        return FIFO_ERR_BAD_PARAM;
    }
    return queue->head == queue->tail;
}

bool _adi_fifo_is_Full(volatile _adi_fifo_t* queue)
{
    if (queue == NULL) {
        return FIFO_ERR_BAD_PARAM;
    }

    return NEXT_IDX(queue, queue->head) == queue->tail;
}

size_t _adi_fifo_Capacity(volatile _adi_fifo_t* queue)
{
    if (queue == NULL) {
        return FIFO_ERR_BAD_PARAM;
    }

    return (queue->buf_size / queue->elem_size) - 1;
}

size_t _adi_fifo_Len(volatile _adi_fifo_t* queue)
{
    if (queue == NULL) {
        return FIFO_ERR_BAD_PARAM;
    }
    size_t head = queue->head;
    size_t tail = queue->tail;
    if (head >= tail) {
        return (head - tail) / queue->elem_size;
    } else {
        return ((queue->buf_size - tail) + head) / queue->elem_size;
    }
}

_adi_fifo_Result_e _adi_fifo_Clear(volatile _adi_fifo_t* queue)
{
    if (queue == NULL) {
        return FIFO_ERR_BAD_PARAM;
    }

    if (_adi_fifo_IsEmpty(queue)) {
        return FIFO_ERR_BUFFER_EMPTY;
    } else {
        queue->tail = queue->head;
        return FIFO_ERR_OK;
    }
}

_adi_fifo_Result_e _adi_fifo_Read(volatile _adi_fifo_t* queue, void* data)
{
    if (queue == NULL || queue->buf == NULL || data == NULL) {
        return FIFO_ERR_BAD_PARAM;
    }
    if (_adi_fifo_IsEmpty(queue)) {
        return FIFO_ERR_BUFFER_EMPTY;
    }
    char *buf = queue->buf;
    memcpy(data, &buf[queue->tail], queue->elem_size);
    return FIFO_ERR_OK;
}

_adi_fifo_Result_e _adi_fifo_ReadN(volatile _adi_fifo_t* queue, void* out_buf, size_t len, size_t *read)
{
    if (queue == NULL || queue->buf == NULL || out_buf == NULL || len == 0) {
        return FIFO_ERR_BAD_PARAM;
    }

    size_t head = queue->head;
    size_t tail = queue->tail;
    char* src_buf  = queue->buf;
    char* dest_buf = out_buf;

    // Don't try to read more elements than the current length
    if (len > _adi_fifo_Len(queue)) {
        len = _adi_fifo_Len(queue);
    }

    // Calculate how many elements are available in sequential storage, starting from the tail
    size_t first_part_count = ((head >= tail) ? (head - tail) : (queue->buf_size - tail)) / queue->elem_size;
    
    // Read elements stored sequentially
    if (first_part_count > len) {
        first_part_count = len;
    }
    memcpy(&dest_buf[0], &src_buf[tail], first_part_count * queue->elem_size);
    
    // Read any remaining elements from the start of the buffer
    if (len > first_part_count) {
        memcpy(&dest_buf[first_part_count * queue->elem_size], &src_buf[0], (len - first_part_count) * queue->elem_size);
    }

    if (read != NULL) {
        *read = len;
    }
    return FIFO_ERR_OK;
}
