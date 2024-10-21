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

#include "homebus.h"

static hbs_err_e hbs_invoke_callback(adi_hbs_t* hbs, adi_hbs_Packet_t* packet)
{
    if (hbs->user_cb == NULL) {
        hbs->unhandled_pkt_cnt++;
        return HBS_ERR_OK;
    }
    hbs_err_e err = hbs->user_cb(hbs, packet);
    return err == HBS_ERR_OK ? HBS_ERR_OK : HBS_ERR_CB_FAILED;
}

static void reset_rxing_state(adi_hbs_t* hbs) {
    hbs->rx_state = HBS_RX_STATE_WAIT_FOR_SELF_ADDR;
    hbs->data_rxed = 0;
}

hbs_err_e adi_hbs_Init(adi_hbs_t* hbs, uint8_t self_addr, hbs_tx_cb_t tx_cb, void* tx_cb_state)
{
    if (hbs == NULL) {
        return HBS_ERR_BAD_PARAM;
    }

    hbs->unhandled_pkt_cnt = 0;
    hbs->tx_cb_state = tx_cb_state;
    hbs->self_addr = self_addr;
    hbs->tx_cb = tx_cb;
    reset_rxing_state(hbs);
    return HBS_ERR_OK;
}

static hbs_err_e process_packet(adi_hbs_t* hbs, adi_hbs_Packet_t* packet)
{
    if (packet->dest_addr == hbs->self_addr) {
        return hbs_invoke_callback(hbs, &hbs->rx_packet);
    }
    return HBS_ERR_OK;
}

hbs_err_e adi_hbs_Received(adi_hbs_t* hbs, uint8_t value)
{
    if (hbs == NULL) {
        return HBS_ERR_BAD_PARAM;
    }

    hbs_err_e err = HBS_ERR_OK;
    switch (hbs->rx_state) {
        case HBS_RX_STATE_WAIT_FOR_SELF_ADDR:
            hbs->rx_packet.self_addr = value;
            hbs->rx_state = HBS_RX_STATE_WAIT_FOR_DEST_ADDR;
            break;
        case HBS_RX_STATE_WAIT_FOR_DEST_ADDR:
            hbs->rx_packet.dest_addr = value;
            hbs->rx_state = HBS_RX_STATE_WAIT_FOR_OP_CODE;
            break;
        case HBS_RX_STATE_WAIT_FOR_OP_CODE:
            hbs->rx_packet.operation = value;
            hbs->rx_state = HBS_RX_STATE_WAIT_FOR_LEN;
            break;
        case HBS_RX_STATE_WAIT_FOR_LEN:
            hbs->rx_packet.len = value;
            if (hbs->rx_packet.len == 0) {
                err = process_packet(hbs, &hbs->rx_packet);
                reset_rxing_state(hbs);
            } else {
                hbs->rx_state = HBS_RX_STATE_WAIT_FOR_DATA;
            }
            break;
        case HBS_RX_STATE_WAIT_FOR_DATA:
            hbs->rx_packet.data[hbs->data_rxed++] = value;
            if (hbs->data_rxed == hbs->rx_packet.len) {
                err = process_packet(hbs, &hbs->rx_packet);
                reset_rxing_state(hbs);
            }
            break;
    }
    return err;
}

hbs_err_e adi_hbs_RegisterRxCb(adi_hbs_t* hbs, hbs_rx_cb_t cb)
{
    if (hbs == NULL || cb == NULL) {
        return HBS_ERR_BAD_PARAM;
    }

    hbs->user_cb = cb;
    return HBS_ERR_OK;
}

hbs_err_e adi_hbs_Send(adi_hbs_t* hbs, uint8_t dest, uint8_t opcode, uint8_t *data, size_t len)
{
    if (hbs == NULL) {
        return HBS_ERR_BAD_PARAM;
    }
    if (len != 0 && data == NULL) {
        return HBS_ERR_BAD_PARAM;
    }
    if (hbs->tx_cb == NULL) {
        return HBS_ERR_TX_UNREGISTERED;
    }

    const uint8_t header_size = 4;
    uint8_t raw_data[len + header_size];
    raw_data[0] = hbs->self_addr;
    raw_data[1] = dest;
    raw_data[2] = opcode;
    raw_data[3] = len;
    for (size_t i = 0; i < len; i++) {
        raw_data[header_size + i] = data[i];
    }
    return hbs->tx_cb(raw_data, sizeof raw_data / sizeof *raw_data, hbs->tx_cb_state);
}
