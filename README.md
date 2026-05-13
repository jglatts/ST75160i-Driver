# ST75160i-Driver

Arduino / ESP32 driver for the Newhaven Display `NHD-C160100DiZ-FSW-FBW Rev1C` LCD using the `ST75160i` controller over I2C.

This driver was written after discovering the newer Rev1C modules are not firmware compatible with older `ST7528i` based displays.

---

## Hardware

### Display

`NHD-C160100DiZ-FSW-FBW Rev1C`

### MCU

`ESP32 Dev Board`

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

## Notes

The display is 3.3V logic only.

Using an Arduino Mega directly caused issues due to 5V pullups on the I2C bus. ESP32 worked properly with 3.3V pullups and hardware reset handling.

The display also requires a proper reset pulse during initialization:

```cpp
digitalWrite(rst, LOW);
delay(20);

digitalWrite(rst, HIGH);
delay(200);
```

---

## ST7528i vs ST75160i

Older revisions of this display used the `ST7528i` controller.

Rev1C uses:

```text
ST75160i
```

The newer controller uses:

* Different initialization sequence
* OTP loading during startup
* Different RAM addressing
* Different framebuffer format
* Different analog / booster configuration

The original ST7528i driver would communicate over I2C correctly, but produced corrupted or random pixel output.

---

## Framebuffer Format

Current framebuffer format:

```text
1 byte = 4 vertical pixels
```

Bit layout:

```text
0x80 = row 0
0x40 = row 1
0x20 = row 2
0x10 = row 3
```



---

## References

### Transition Guide

https://newhavendisplay.com/content/docs/NHD-C160100DiZ-FSW-FBW_TransitionGuide.pdf

### Arduino Example

https://support.newhavendisplay.com/hc/en-us/articles/25842742613911-NHD-C160100DiZ-FSW-FBW-Rev1C-with-Arduino

### Datasheet

https://newhavendisplay.com/content/specs/NHD-C160100DiZ-FSW-FBW.pdf
