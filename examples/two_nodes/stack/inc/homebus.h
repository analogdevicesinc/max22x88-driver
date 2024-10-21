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
 * @file homebus.h
 * Protocol Stack API.
 */

#ifndef HOMEBUS_H
#define HOMEBUS_H

#include <stdint.h>
#include <stddef.h>

/** Maximum length of the payload data field. */
#define HBS_MAX_DATA_LEN (255)

/** Lenght of the packet header. */
#define HBS_HEADER_SIZE (4)

/**
 * Protocol stack status codes.
 * 
 */
typedef enum {
    HBS_ERR_OK, /*!< Success */
    HBS_ERR_BAD_PARAM, /*!< Error: bad parameter */
    HBS_ERR_CB_FAILED, /*!< Error: Rx callback execution failed */
    HBS_ERR_TX_UNREGISTERED /*!< Error: cannot transmit without configuring Tx callback */
} hbs_err_e;

/**
 * Internal use. State names for the Rx state machine.
 * 
 */
typedef enum {
    HBS_RX_STATE_WAIT_FOR_SELF_ADDR,
    HBS_RX_STATE_WAIT_FOR_DEST_ADDR,
    HBS_RX_STATE_WAIT_FOR_OP_CODE,
    HBS_RX_STATE_WAIT_FOR_LEN,
    HBS_RX_STATE_WAIT_FOR_DATA
} hbs_rx_state_machine_e;

/**
 * Representation of a packet.
 * 
 */
typedef struct {
    uint8_t self_addr; /*!< self address */
    uint8_t dest_addr; /*!< destination address */
    uint8_t operation; /*!< operation code */
    uint8_t len; /*!< payload data length*/
    uint8_t data[HBS_MAX_DATA_LEN]; /*!< payload data */
} adi_hbs_Packet_t;

/**
 * Typedef for protocol stack context.
 * 
 */
typedef struct adi_hbs_t adi_hbs_t;

/** Receiving callback */
typedef hbs_err_e (*hbs_rx_cb_t)(adi_hbs_t*, adi_hbs_Packet_t*);

/** Tx callback API */
typedef hbs_err_e (*hbs_tx_cb_t)(uint8_t*, size_t, void*);

/**
 * Protocol stack context.
 * 
 */
struct adi_hbs_t {
    uint8_t self_addr;
    hbs_rx_cb_t user_cb;
    hbs_tx_cb_t tx_cb;
    void* tx_cb_state;
    adi_hbs_Packet_t rx_packet;
    hbs_rx_state_machine_e rx_state;
    uint8_t data_rxed;
    unsigned int unhandled_pkt_cnt;
};

/**
 * @brief Initialize the protocol stack
 * 
 * @param hbs Protocol stck
 * @param self_addr Node address
 * @param tx_cb Driver Tx callback
 * @param tx_cb_state Tx callback context
 * @return hbs_err_e 
 */
hbs_err_e adi_hbs_Init(adi_hbs_t* hbs, uint8_t self_addr, hbs_tx_cb_t tx_cb, void* tx_cb_state);

/**
 * @brief Notify protocol stack of incoming data
 * 
 * @param hbs protocol stack
 * @param value incoming data
 * @return hbs_err_e 
 */
hbs_err_e adi_hbs_Received(adi_hbs_t* hbs, uint8_t value);

/**
 * @brief Register a callback to handle incoming packets.
 * 
 * @param hbs protoco stack
 * @param cb callback
 * @return hbs_err_e 
 */
hbs_err_e adi_hbs_RegisterRxCb(adi_hbs_t* hbs, hbs_rx_cb_t cb);

/**
 * @brief Send a packet.
 * 
 * @param hbs protocol stack
 * @param dest destination address
 * @param opcode operation code
 * @param data payload data
 * @param len length of payload data
 * @return hbs_err_e 
 */
hbs_err_e adi_hbs_Send(adi_hbs_t* hbs, uint8_t dest, uint8_t opcode, uint8_t *data, size_t len);

#endif
