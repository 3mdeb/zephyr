/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <zephyr/irq.h>
#include <cmsis_core.h>

#define IPC_IRQ_ID          63      //79 on Bao config
#define BAO_IMAGE_START     0x10000000UL
#define BAO_HC_OFF          0x41UL
#define BAO_HC_ADDR         ((uintptr_t)BAO_IMAGE_START + BAO_HC_OFF)
#define BAO_HC_IPC_ID       0x1
#define VMS_IPC_BASE        0x20017000UL
#define VMS_IPC_SIZE        0x1000

void (*bao_hypercall)(unsigned int, unsigned int, unsigned int) =
    (void (*)(unsigned int, unsigned int, unsigned int))BAO_HC_ADDR;

char* const message1 = (char*)VMS_IPC_BASE;
const size_t shmem_channel_size = VMS_IPC_SIZE/2;

void ipc_notify(int ipc_id, int event_id)
{
    bao_hypercall(BAO_HC_IPC_ID, ipc_id, event_id);
}

void ipc_irq_handler(void)
{
    static unsigned int counter = 0;

    printf("Writing to shared memory...\n");
    sprintf(message1, "Counter: %d\r\n",++counter);
    ipc_notify(0, 0);
}

int main(void)
{
    printf("VM1 (Zephyr): Hello World! %s\n", CONFIG_BOARD_TARGET);

    memset(message1, 0, shmem_channel_size);

    IRQ_CONNECT(IPC_IRQ_ID, 0, ipc_irq_handler, NULL, 0);
    irq_enable(IPC_IRQ_ID);

    while(1);
}
