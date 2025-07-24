# TIVA C Project

## Display

[1.54inch I2C OLED Module](http://www.lcdwiki.com/1.54inch_IIC_OLED_Module_SKU:MC154GX)

### Selected Pins

| OLED | Tiva C |
| ---- | ------ |
| GND  | GND    |
| VCC  | 3.3V   |
| SCL  | PA6    |
| SDA  | PA7    |

### Bitmap creation

Use the creator to create an asset and save the drawing as a png. Then using the `png to bytes array`, upload the image and do the following:

- Choose Transparent background color
- Tick Invert Image Colors
- The output as Arduino code
- Tick Swap bits in byte

Generate the code. Then simply copy the array contents.

Creator: <https://www.pixilart.com/draw?ref=home-page>
PNG to bytes array: <https://projedefteri.com/en/tools/lcd-assistant>

## Build

After cloning repo, make sure to initialize the submodules.

```sh
git submodule update --init --recursive
```

## MPU6050

| MPU6050 | Tiva C |
| ------- | ------ |
| GND     | GND    |
| VCC     | 3.3V   |
| SCL     | PA6    |
| SDA     | PA7    |

[Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)
[Register Map and Descriptions](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf)

## Demo of current progress

[demo1.webm](https://github.com/user-attachments/assets/d200be0c-6b17-4e7a-802d-5a40fdb32990)

## References

- The starter template for this project, the Active Object and State Machines by Quantum Leaps: [#19 GNU-ARM Toolchain and Eclipse IDE](https://www.youtube.com/watch?v=BBF3ZMi8WK4), [Active Objects](https://youtu.be/o3eyz1gEqGU?si=rfhU0feqN9tC1W1J), [State Machines](https://youtu.be/EBSxZKjgBXI?si=AJyr5Zo38rtvkIOO)

- [eclipse-threadx/rtos-docs](https://github.com/eclipse-threadx/rtos-docs)

- Entity Component System (ECS): [ECS back and forth](https://skypjack.github.io/2019-05-06-ecs-baf-part-3/), [Building an ECS#3: Storage in Pictures](https://ajmmertens.medium.com/building-an-ecs-storage-in-pictures-642b8bfd6e04#5dfc)
