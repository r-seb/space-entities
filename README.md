# Display

[1.54inch I2C OLED Module](http://www.lcdwiki.com/1.54inch_IIC_OLED_Module_SKU:MC154GX)

## Selected Pins

| OLED | Tiva C |
|------|--------|
| GND  | GND    |
| VCC  | 3.3V   |
| SCL  | PA6    |
| SDA  | PA7    |

## Bitmap creation

- creator: <https://www.pixilart.com/draw?ref=home-page>
- png to bytes array: <https://projedefteri.com/en/tools/lcd-assistant>

## Build

After cloning repo, make sure to initialize the submodules and apply the patch.

```sh
git submodule update --init --recursive
git apply external/printf.patch
```
