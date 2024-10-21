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

#include <stdio.h>

#include "max22x88.h"
#include "max22x88_bitbang.h"
#include "homebus.h"
#include "homebus_max22x88.h"

#include "max32670.h"
#include "gpio.h"
#include "mxc_errors.h"
#include "board.h"
#include "pb.h"
#include "mxc_delay.h"
#include "nvic_table.h"

#include "max32670_inline_macros.h"

#define ROLE_MASTER 0
#define ROLE_SLAVE 1

#define ADDRESS_MASTER 0x10
#define ADDRESS_SLAVE 0x00

#define REQUEST_CODE 42
#define RESPONSE_CODE 90

#define DEBOUNCE_DELAY_MS 200

#define MAX22X88_RX_FIFO_LEN 256

#define HOMEBUS_BAUD 9600

#define GPIO_IRQn_DOUT MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO0))

static void print_info(const char* role)
{
    printf("ANALOG DEVICES MAX22x88 DRIVER DEMO\n");
    printf("Copyright 2024 Analog Devices, Inc.\n");
    printf("===================================\n");
    printf("Device role: ");
    printf(role);
    printf("\n");
    printf("===================================\n");
}

static const char * pb_status_to_str(int status)
{
    return status ? "pressed" : "released";
}

static bool master_received_response = false;

static void print_packet_content(adi_hbs_Packet_t* packet) {
    printf("Src\t%d\nDest\t%d\nOp\t%d\nLen\t%d\n", packet->self_addr, packet->dest_addr, packet->operation, packet->len);
    printf("Data");
    for (uint8_t i = 0; i < packet->len; i++) {
        printf("\t%d", packet->data[i]);
    }
    printf("\n");
}

static void print_rx_packet(adi_hbs_Packet_t* packet) {
    printf("=======   Received packet   =======\n");
    print_packet_content(packet);
    printf("===================================\n");
}

// Max22x88 DOUT is connected to GPIO0
void GPIO0_IRQHandler(void)
{
    // This is the only GPIO0 interrupt configured in this example, so checks for which pin triggered the interrupt are skipped.
    MXC_GPIO_ClearFlags(MXC_GPIO0, UINT32_MAX);
    adi_max22x88_FallingEdgeIntCallback();
}

hbs_err_e slavecb(adi_hbs_t* hbs, adi_hbs_Packet_t* packet)
{
    print_rx_packet(packet);
    if (packet->operation != REQUEST_CODE) {
        printf("Unexpected operation code: %d\n", packet->operation);
        printf("The expected operation code is: %d\n", REQUEST_CODE);
        return HBS_ERR_OK;
    }
    uint8_t src_addr = packet->self_addr;
    uint8_t status = PB_Get(0);
    printf("Received request from device %d\n", src_addr);
    printf("Button status is: %d - %s\n", status, pb_status_to_str(status));
    printf("Sending response... ");

    uint8_t data[] = {status};
    adi_hbs_Send(hbs, src_addr, RESPONSE_CODE, data, 1);
    printf("Response sent.\n");
    printf("Waiting for request...\n");
    return HBS_ERR_OK;
}

void run_slave(adi_hbs_t* hbs, adi_max22x88_t* driver)
{
    MXC_GPIO_OutClr(MXC_GPIO0, MXC_GPIO_PIN_22);  // Turn on red LED (P0_22)
    print_info("slave (red LED)");
    printf("This device reports the status of its push button (pressed/released).\n");
    printf("Waiting for request...\n");
    adi_hbs_RegisterRxCb(hbs, slavecb);
    while (1)
    {
        if (adi_max22x88_IsAvailable(driver)) {
            uint8_t data;
            adi_max22x88_Read(driver, &data);
            adi_hbs_Received(hbs, data);
        }
    }
}

hbs_err_e mastercb(adi_hbs_t* hbs, adi_hbs_Packet_t* packet)
{
    print_rx_packet(packet);
    if (packet->operation != RESPONSE_CODE) {
        printf("Unexpected operation code: %d\n", packet->operation);
        printf("The expected operation code is: %d\n", RESPONSE_CODE);
        return HBS_ERR_OK;
    }
    master_received_response = true;
    uint8_t status = packet->data[0];
    printf("Received status: %d - %s\n", status, pb_status_to_str(status));
    return HBS_ERR_OK;
}

void run_master(adi_hbs_t* hbs, adi_max22x88_t* driver)
{
    MXC_GPIO_OutClr(MXC_GPIO0, MXC_GPIO_PIN_23);  // Turn on green LED (P0_23)
    print_info("master (green LED)");
    printf("This device requests the push button status of another device in the network.\n");
    adi_hbs_RegisterRxCb(hbs, mastercb);

    while (1) {
        printf("Press push button SW3 to send request...\n");
        while (PB_Get(0) == 0)
            ;
        printf("Sending request to device %d\n", ADDRESS_SLAVE);
        MXC_Delay(MXC_DELAY_MSEC(DEBOUNCE_DELAY_MS));

        master_received_response = false;
        adi_hbs_Send(hbs, ADDRESS_SLAVE, REQUEST_CODE, NULL, 0);

        while (!master_received_response) {
            if (adi_max22x88_IsAvailable(driver)) {
                uint8_t data;
                adi_max22x88_Read(driver, &data);
                adi_hbs_Received(hbs, data);
            }
        }
        while (PB_Get(0) == 1)
            ;
        MXC_Delay(MXC_DELAY_MSEC(DEBOUNCE_DELAY_MS));  
    }
}

static const mxc_gpio_cfg_t gpio_role = {
    .drvstr = MXC_GPIO_DRVSTR_0,
    .func =MXC_GPIO_FUNC_IN,
    .mask = MXC_GPIO_PIN_12,
    .pad = MXC_GPIO_PAD_PULL_UP,
    .port = MXC_GPIO0,
    .vssel = MXC_GPIO_VSSEL_VDDIOH
};

int init(void)
{
    int err;
    if ((err = Board_Init()) != E_NO_ERROR) {
        printf("Internal error Board_Init\n");
        return err;
    }
    if ((err = MXC_GPIO_Config(&gpio_role)) != E_NO_ERROR) {
        printf("Internal error configuring GPIO\n");
        return err;
    }
    NVIC_EnableIRQ(GPIO_IRQn_DOUT);
    return 0;
}

int get_role() {
    return (MXC_GPIO_InGet(gpio_role.port, gpio_role.mask)) ? ROLE_SLAVE : ROLE_MASTER;
}

int main(void)
{
    int err;
    if ((err = init()) != 0) {
        printf("Internal error\n");
        while (1)
            ;
    }

    int role = get_role();

    adi_max22x88_t driver;
    adi_max22x88_bitbang_InitParams_t driver_param;
    driver_param.hbs_baud = HOMEBUS_BAUD;
    adi_max22x88_InitBitbang(&driver, &driver_param, MAX22X88_RX_FIFO_LEN);

    adi_hbs_t hbs;
    uint8_t address = (role == ROLE_MASTER ? ADDRESS_MASTER : ADDRESS_SLAVE);

    adi_hbs_InitMax22x88(&hbs, address, &driver);
    if (role == ROLE_MASTER) {
        run_master(&hbs, &driver);
    } else {
        run_slave(&hbs, &driver);
    }
    while (1)
        ;
}
