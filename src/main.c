#include "TM4C123GH6PM.h"
#include "active_object.h"
#include "assert_handler.h"
#include "assets.h"
#include "delay.h"
#include "i2c.h"
#include "led.h"
#include "mpu6050_i2c.h"
#include "printf.h"
#include "ssd1309_128x64_i2c.h"
#include "tx_api.h"
#include "u8g2.h"
#include "uart.h"
#include <stdbool.h>
#include <stdint.h>

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
#define MSG_EVT_BYTE_POOL_SIZE 800U // sized for 5 AO queues + remaining for events
TX_BYTE_POOL msg_evt_byte_pool;

u8g2_t oled;

enum GameSignals {
    TIME_TICK_SIG = USER_SIG,
    MINE_IMG_SIG,
    SHIP_IMG_SIG,

    PLAYER_SHIP_UP_SIG,
    PLAYER_SHIP_DOWN_SIG,
    PLAYER_SHIP_LEFT_SIG,
    PLAYER_SHIP_RIGHT_SIG,

    MAX_SIG
};

typedef struct {
    Event super;

    // public:
    uint8_t x;
    uint8_t y;
} PositionEvent;

typedef struct {
    Active super;

    // private:
    uint8_t* mines[5];
    uint8_t last_mine_x;
    uint8_t last_mine_y;
} Passageway;

static void Passageway_dispatch(Passageway* me, Event const* const e)
{
    UINT status;
    switch (e->sig) {
        case INIT_SIG: {
            printf_("Passageway Initialized\n\r");
        } break;
        case SHIP_IMG_SIG: {
            printf_("Mine PosX=%u, PosY=%u \n\r", EVENT_CAST(PositionEvent)->x,
                    EVENT_CAST(PositionEvent)->y);
            status = EVENT_HANDLED();
            printf_("Byte Released: %d\n\r", status);
        } break;
        default:
            printf_("Passageway Default State\n\r");
            break;
    }
}

static void Passageway_ctor(Passageway* const me)
{
    Active_ctor(&me->super, (ao_dispatch_handler)&Passageway_dispatch);
}

static Passageway passageway;
Active* AO_Passageway = &passageway.super; // Global pointer so that others can post events to it

typedef struct {
    Active super;

    // private:
} I2CManager;

static void I2CManager_dispatch(I2CManager* me, Event const* const e)
{
    switch (e->sig) {
        case INIT_SIG: {
            printf_("I2CManager Initialized\n\r");
            // static Event const mine_img_evt = {MINE_IMG_SIG};
            // Active_post(AO_Passageway, &mine_img_evt);
            PositionEvent* pos_evt;
            EVENT_ALLOCATE(msg_evt_byte_pool, pos_evt);
            pos_evt->super.sig = SHIP_IMG_SIG;
            pos_evt->x = 0xD;
            pos_evt->y = 0xB;
            Active_post(AO_Passageway, (Event*)pos_evt);
        } break;
        default:
            printf_("I2CManager Default State\n\r");
            break;
    }
}

static void I2CManager_ctor(I2CManager* const me)
{
    Active_ctor(&me->super, (ao_dispatch_handler)&I2CManager_dispatch);
}

static I2CManager i2c_manager;
Active* AO_I2CManager = &i2c_manager.super; // Global pointer so that others can post events to it

int main()
{
    SystemCoreClockUpdate();
    led_init();
    delay_init();
    i2c1_init(I2C_1Mbps);
    uart_init();
    ssd1309_128x64_init(&oled, &i2c1_write);
    mpu6050_init(&i2c1_write, &i2c1_read);
    printf_("Hello from Tiva!!\n\r");

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

        // Clear buffer and draw the player ship
        u8g2_ClearBuffer(&oled);
        u8g2_DrawXBM(&oled, x, y, 8, 8, player_ship1_bmp);
        u8g2_SendBuffer(&oled);

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
    // For AO threads
    tx_block_pool_create(&thread_block_pool, "ao block pool", THREAD_STACK_SIZE,
                         first_unused_memory, THREAD_BLOCK_POOL_SIZE);
    // For AO message queues and events
    void* next_unused_memory = (UCHAR*)first_unused_memory + THREAD_BLOCK_POOL_SIZE;
    tx_byte_pool_create(&msg_evt_byte_pool, "message event byte pool", next_unused_memory,
                        MSG_EVT_BYTE_POOL_SIZE);

    CHAR* stack_ptr;
    CHAR* queue_ptr;

    // Create AO and start them
    Passageway_ctor(&passageway);
    Active_start(AO_Passageway, 0, &thread_block_pool, THREAD_STACK_SIZE, &stack_ptr,
                 &msg_evt_byte_pool, MSG_QUEUE_SIZE, &queue_ptr);

    I2CManager_ctor(&i2c_manager);
    Active_start(AO_I2CManager, 0, &thread_block_pool, THREAD_STACK_SIZE, &stack_ptr,
                 &msg_evt_byte_pool, MSG_QUEUE_SIZE, &queue_ptr);
}
