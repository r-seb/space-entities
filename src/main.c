#include "active_object.h"
#include "app.h"
#include "assert_handler.h"
#include "delay.h"
#include "led.h"
#include "mpu6050_i2c.h"
#include "tx_api.h"
#include <stdbool.h>
#include <stdint.h>

static TX_TIMER l_timer_tick;
static void l_timer_tick_fn(ULONG id)
{
    TimeEvent_tick();
    static Event const time_tick_evt = {TIME_TICK_SIG};
    Active_post_nonthread(AO_Passageway, &time_tick_evt);
}

/*
 * https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/threadx/chapter4.md#tx_block_pool_create
 * Total number of blocks = 1440 / (256 + 4) = 5.54 = 5 blocks for 5 AO threads
 */
#define THREAD_STACK_SIZE 256U
#define THREAD_BLOCK_POOL_SIZE 1440U
TX_BLOCK_POOL thread_block_pool;

/*
 * https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/threadx/chapter4.md#tx_queue_create
 * Each message will be 1 32-bit word.
 * Queue size = 20 messages * 4 bytes-per-msg = 80 bytes.
 */
#define MSG_QUEUE_SIZE 80U
#define MSG_EVT_BYTE_POOL_SIZE 1800U // sized for 5 AO queues + remaining for events
TX_BYTE_POOL msg_evt_byte_pool;

int main()
{
    SystemCoreClockUpdate();
    led_init();
    delay_init();
    // mpu6050_init(&i2c1_write, &i2c1_read);

    /* Enter the ThreadX kernel. */
    tx_kernel_enter();
    // The function below should never execute

    // Ship should spawn in the middle of screen
    uint8_t x = 128 / 2 - 8;
    uint8_t y = 64 / 2 - 8;
    mpu6050_data imu;
    float move_thres = 0.02f; // acc approx value between -1g to 1g
    while (1) {
        // Check for imu readings
        if (mpu6050_is_data_ready()) {
            mpu6050_read_data(&imu);
        }

        // UP-DOWN Movement
        if (imu.accx > move_thres) {
            if (y > 0) {
                y--;
            } else {
                y = 0;
            }
        } else if (imu.accx < -move_thres) {
            if (y < 56) {
                y++;
            } else {
                y = 56;
            }
        }
        // LEFT-RIGHT Movement
        if (imu.accy > move_thres) {
            if (x > 0) {
                x--;
            } else {
                x = 0;
            }
        } else if (imu.accy < -move_thres) {
            if (x < 118) {
                x++;
            } else {
                x = 118;
            }
        }
    }

    return 0;
}

void tx_application_define(void* first_unused_memory)
{
    UINT status;

    // Timer responsible for calling all active objects TimeEvent
    status = tx_timer_create(&l_timer_tick, "timer tick", &l_timer_tick_fn, 0U, 17U, 17U,
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

    CHAR* stack_ptr;
    CHAR* queue_ptr;

    // Create AO and start them
    Passageway_ctor_call();
    Active_start(AO_Passageway, 1, &thread_block_pool, THREAD_STACK_SIZE, &stack_ptr,
                 &msg_evt_byte_pool, MSG_QUEUE_SIZE, &queue_ptr);

    I2CManager_ctor_call();
    Active_start(AO_I2CManager, 0, &thread_block_pool, THREAD_STACK_SIZE, &stack_ptr,
                 &msg_evt_byte_pool, MSG_QUEUE_SIZE, &queue_ptr);

    UARTManager_ctor_call();
    Active_start(AO_UARTManager, 5, &thread_block_pool, THREAD_STACK_SIZE, &stack_ptr,
                 &msg_evt_byte_pool, MSG_QUEUE_SIZE, &queue_ptr);
}
