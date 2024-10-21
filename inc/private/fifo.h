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

/**
 * @file fifo.h
 * First-in, first-out queue used by the MAX22088/MAX22288 driver internals.
 */

#ifndef FIFO_H
#define FIFO_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/** FIFO status codes. */
typedef enum {
    FIFO_ERR_OK, /*!< Success. */
    FIFO_ERR_INTERNAL, /*!< Internal error. */
    FIFO_ERR_BAD_PARAM, /*!< Bad parameter. */
    FIFO_ERR_BUFFER_EMPTY, /*!< Buffer is empty. */
    FIFO_ERR_BUFFER_FULL  /*!< Buffer is full. */
} _adi_fifo_Result_e;

/**
 * The FIFO queue.
 * @note The fields shouldn't be accessed directly. Use the `fifo_*` functions to interact with the object.
 */
typedef struct {
    void *buf;
    size_t buf_size;
    size_t elem_size;
    size_t head;
    size_t tail;
} _adi_fifo_t;

/**
 * @brief Initializes the fifo object with fixed capacity for elements of specific size.
 * 
 * @param[in] queue Object to initialize.
 * @param[in] len the maximum capacity in number of elements
 * @param[in] elem_size the size, in bytes, of each element
 * @retval FIFO_ERR_OK Success.
 * @retval FIFO_ERR_BAD_PARAM
 * @retval FIFO_ERR_INTERNAL
 */
_adi_fifo_Result_e _adi_fifo_Init(volatile _adi_fifo_t *queue, size_t len, size_t elem_size);

/**
 * @brief Frees the memory managed by the queue.
 * 
 * @param[in] queue the FIFO queue.
 * @retval FIFO_ERR_OK
 */
_adi_fifo_Result_e _adi_fifo_Free(volatile _adi_fifo_t *queue);

/**
 * @brief Checks if queue is empty.
 * 
 * @param[in] queue the FIFO queue.
 * @retval true queue is empty.
 * @retval false queue is not empty.
 */
bool _adi_fifo_IsEmpty(volatile _adi_fifo_t* queue);

/**
 * @brief Checks if queue is full.
 * 
 * @param[in] queue the FIFO queue.
 * @retval true queue is full.
 * @retval false queue is not full.
 */
bool _adi_fifo_is_Full(volatile _adi_fifo_t* queue);

/**
 * @brief Writes an element into the queue.
 * 
 * @param[in] queue the FIFO queue.
 * @param[in] data Pointer to element to write to the queue. The element should match the `elem_size` set during initialization.
 * @retval FIFO_ERR_OK Success.
 * @retval FIFO_ERR_BUFFER_FULL
 */
_adi_fifo_Result_e _adi_fifo_Push(volatile _adi_fifo_t* queue, void* data);

/**
 * @brief Pops one element from the queue. If data is NULL, the removed value is discarded, otherwise it's copied to data.
 * 
 * @param[in] queue the FIFO queue.
 * @param[out] data Element popped from queue
 * @retval FIFO_ERR_OK Success.
 * @retval FIFO_ERR_BUFFER_EMPTY queue is empty, there is nothing to pop. data is unmodified.
 */
_adi_fifo_Result_e _adi_fifo_Pop(volatile _adi_fifo_t* queue, void* data);

/**
 * @brief Returns the capacity of the queue.
 * 
 * @param[in] queue the FIFO queue.
 * @return size_t capacity.
 */
size_t _adi_fifo_Capacity(volatile _adi_fifo_t* queue);

/**
 * @brief Returns the amount if elements currently stored in the queue.
 * 
 * @param[in] queue the FIFO queue.
 * @return size_t amount of elements.
 */
size_t _adi_fifo_Len(volatile _adi_fifo_t* queue);

/**
 * @brief Clears the queue.
 * 
 * @param[in] queue the FIFO queue.
 * @retval FIFO_ERR_OK Success.
 * @retval FIFO_ERR_BUFFER_EMPTY The queue is already empty, nothing has been done.
 */
_adi_fifo_Result_e _adi_fifo_Clear(volatile _adi_fifo_t* queue);

/**
 * @brief Reads an element from the queue.
 * 
 * @param[in] queue the FIFO queue.
 * @param[out] data the element.
 * @retval FIFO_ERR_OK Success.
 * @retval FIFO_ERR_BUFFER_EMPTY The queue is empty, nothing has been done.
 * @retval FIFO_ERR_BAD_PARAM
 */
_adi_fifo_Result_e _adi_fifo_Read(volatile _adi_fifo_t* queue, void* data);

/**
 * @brief Copies up to `len` elements into `out_buf`.
 * 
 * @param[in] queue the FIFO queue.
 * @param[out] out_buf where the elements are copied to
 * @param[in] len the length (in elements) of buf.
 * @param[out] read the number of elements that have been read
 * @retval FIFO_ERR_OK Success.
 * @retval FIFO_ERR_BUFFER_EMPTY The queue is empty, nothing has been done.
 * @retval FIFO_ERR_BAD_PARAM
 */
_adi_fifo_Result_e _adi_fifo_ReadN(volatile _adi_fifo_t* queue, void* out_buf, size_t len, size_t *read);

#endif
