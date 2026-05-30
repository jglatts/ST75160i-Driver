# ST75160i Driver

Arduino / ESP32 driver for the Newhaven Display `NHD-C160100DiZ-FSW-FBW Rev1C` LCD using the `ST75160i` controller over I2C.

This project was created after discovering that newer Rev1C modules are not firmware compatible with older versions of the display that used the `ST7528i` controller. While the display retained the same part number, the controller architecture, initialization sequence, and memory organization changed significantly.

The goal of this project is to provide a lightweight, easy-to-understand driver for ESP32 and Arduino-based projects using the newer ST75160i-based displays.

---

## Features

* I2C interface
* Framebuffer-based graphics rendering
* Pixel drawing
* Text rendering using bitmap fonts
* Image display support
* ESP32 compatible
* Open and easy-to-modify codebase

---

## Supported Display

### Newhaven Display

`NHD-C160100DiZ-FSW-FBW Rev1C`

Controller:

```text
ST75160i
```

Resolution:

```text
160 x 100 pixels
```

Interface:

```text
I2C
```

---

## Tested Hardware

### MCU

* ESP32 DevKit
* ESP32-WROOM

### Display

* NHD-C160100DiZ-FSW-FBW Rev1C

---

## Wiring

| ESP32  | LCD Pin | Description   |
| ------ | ------- | ------------- |
| GPIO21 | SDA     | I2C Data      |
| GPIO22 | SCL     | I2C Clock     |
| GPIO23 | RST     | Display Reset |
| 3.3V   | VDD     | Power         |
| GND    | VSS     | Ground        |

---

## Important Notes

### Hardware Reset Required

The ST75160i requires a proper reset sequence before initialization.

```cpp
digitalWrite(rst, LOW);
delay(20);

digitalWrite(rst, HIGH);
delay(200);
```

Skipping the reset sequence can result in unreliable startup behavior.

---

## ST7528i vs ST75160i

Older versions of the display used the Sitronix ST7528i controller.

Although the display part number remained nearly identical, the newer Rev1C modules use the ST75160i controller.

Key differences include:

* Different initialization sequence
* Different analog power configuration
* Different RAM addressing
* Different framebuffer organization
* Different startup and OTP loading behavior

Because of these differences, existing ST7528i firmware may communicate successfully over I2C while still producing corrupted or unusable graphics output.

---

## Framebuffer Organization

The ST75160i implementation in this project uses a local framebuffer that is flushed to the display as a single transfer.

Framebuffer size:

```text
4000 bytes
```

Display resolution:

```text
160 x 100 pixels
```

Pixel mapping:

```text
1 byte = 8 vertical pixels
```

Example:

```text
Bit 7 = Y + 0
Bit 6 = Y + 1
Bit 5 = Y + 2
Bit 4 = Y + 3
Bit 3 = Y + 4
Bit 2 = Y + 5
Bit 1 = Y + 6
Bit 0 = Y + 7
```

This organization allows efficient rendering while maintaining compatibility with the controller's internal memory layout.

---

## Example

```cpp
#include "st75160i.h"
#include "font5x7.h"

ST75160i display(21, 22, 23);

void setup() {
    display.Init();

    display.Clear();

    display.PutStr(10, 20, "HELLO WORLD", fnt5x7);

    for (int x = 0; x < 160; x++) {
        display.SetPixel(x, 0, 1);
        display.SetPixel(x, 99, 1);
    }

    display.Flush();
}

void loop() {
}
```

---

## Example Animation

```cpp
void DancingText() {
    const char* msg = "HELLO JOHN";

    for (int frame = 0; frame < 100; frame++) {

        display.Clear();

        for (uint8_t i = 0; msg[i]; i++) {

            int wave = (frame + i * 3) % 16;

            uint8_t yOffset =
                (wave < 8) ? wave : (16 - wave);

            display.PutChar(
                20 + (i * 8),
                40 + yOffset,
                msg[i],
                fnt5x7
            );
        }

        display.Flush();
        delay(50);
    }
}
```

---

## Development Notes

One of the more interesting challenges during development was discovering that the display's memory organization differed significantly from the older ST7528i implementation. Initial assumptions about grayscale memory packing and page layout proved incorrect, requiring reverse engineering of the controller behavior through direct RAM writes and framebuffer experiments.

The final driver uses a framebuffer approach that dramatically improves rendering performance compared to writing individual pixels directly over I2C.

---

## References

### Transition Guide

https://newhavendisplay.com/content/docs/NHD-C160100DiZ-FSW-FBW_TransitionGuide.pdf

### Arduino Example

https://support.newhavendisplay.com/hc/en-us/articles/25842742613911-NHD-C160100DiZ-FSW-FBW-Rev1C-with-Arduino

### Datasheet

https://newhavendisplay.com/content/specs/NHD-C160100DiZ-FSW-FBW.pdf

---

## License

MIT License
