#include "active_object.h"
#include "app.h"
#include "assert_handler.h"
#include "delay.h"
#include "led.h"
#include "tx_api.h"
#include <stdbool.h>

static TX_TIMER l_timer_tick;
static void l_timer_tick_fn(ULONG id)
{
    TimeEvent_tick();
    static Event const time_tick_evt = {TIME_TICK_SIG};
    Active_post_nonthread(AO_Passageway, &time_tick_evt);
    Active_post_nonthread(AO_Ship, &time_tick_evt);
}

/*
 * https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/threadx/chapter4.md#tx_block_pool_create
 * Total number of blocks = 3520 / (640 + 4) = 5.50 = 5 blocks for 5 AO threads
 */
#define THREAD_STACK_SIZE 640U
#define THREAD_BLOCK_POOL_SIZE 3520U
TX_BLOCK_POOL thread_block_pool;

/*
 * https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/threadx/chapter4.md#tx_queue_create
 * Each message will be 1 32-bit word.
 * Queue size = 20 messages * 4 bytes-per-msg = 80 bytes.
 */
#define MSG_QUEUE_SIZE 80U
#define MSG_EVT_BYTE_POOL_SIZE 3000U // sized for 5 AO queues + remaining for events
#define UART_EVT_BYTE_POOL_SIZE 500U
TX_BYTE_POOL msg_evt_byte_pool;
TX_BYTE_POOL uart_evt_byte_pool;

int main()
{
    SystemCoreClockUpdate();
    led_init();
    delay_init();

    /* Enter the ThreadX kernel. */
    tx_kernel_enter();
    // Code below this line should never execute

    return 0;
}

void tx_application_define(void* first_unused_memory)
{
    UINT status;

    // Timer responsible for calling all active objects TimeEvent
    status = tx_timer_create(&l_timer_tick, "timer tick", &l_timer_tick_fn, 0U, 20U, 20U,
                             TX_AUTO_ACTIVATE);
    ASSERT(status == TX_SUCCESS);

    // For AO threads
    status = tx_block_pool_create(&thread_block_pool, "ao block pool", THREAD_STACK_SIZE,
                                  first_unused_memory, THREAD_BLOCK_POOL_SIZE);
    ASSERT(status == TX_SUCCESS);

    // For AO message queues and events
    void* next_unused_memory = (UCHAR*)first_unused_memory + THREAD_BLOCK_POOL_SIZE;
    status = tx_byte_pool_create(&msg_evt_byte_pool, "message event byte pool", next_unused_memory,
                                 MSG_EVT_BYTE_POOL_SIZE);
    ASSERT(status == TX_SUCCESS);

    // For uart events
    next_unused_memory = (UCHAR*)next_unused_memory + MSG_EVT_BYTE_POOL_SIZE;
    status = tx_byte_pool_create(&uart_evt_byte_pool, "uart event byte pool", next_unused_memory,
                                 UART_EVT_BYTE_POOL_SIZE);
    ASSERT(status == TX_SUCCESS);

    CHAR* stack_ptr;
    CHAR* queue_ptr;

    // Create AO and start them
    I2CManager_ctor_call();
    Active_start(AO_I2CManager, 0, &thread_block_pool, THREAD_STACK_SIZE, &stack_ptr,
                 &msg_evt_byte_pool, MSG_QUEUE_SIZE, &queue_ptr);

    Passageway_ctor_call();
    Active_start(AO_Passageway, 1, &thread_block_pool, THREAD_STACK_SIZE, &stack_ptr,
                 &msg_evt_byte_pool, MSG_QUEUE_SIZE, &queue_ptr);

    Ship_ctor_call();
    Active_start(AO_Ship, 2, &thread_block_pool, THREAD_STACK_SIZE, &stack_ptr, &msg_evt_byte_pool,
                 MSG_QUEUE_SIZE, &queue_ptr);

    UARTManager_ctor_call();
    Active_start(AO_UARTManager, 5, &thread_block_pool, THREAD_STACK_SIZE, &stack_ptr,
                 &msg_evt_byte_pool, MSG_QUEUE_SIZE, &queue_ptr);
}
