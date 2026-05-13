# ST75160i-Driver

# ST75160i Arduino / ESP32 Driver

Driver and test code for the Newhaven Display `NHD-C160100DiZ-FSW-FBW Rev1C` LCD using the `ST75160i` controller over I2C.

This driver was developed after discovering that the newer Rev1C display is **not firmware compatible** with older `ST7528i` based modules.

---

## Background

Older revisions of the display used the `ST7528i` controller.

Newer Rev1C displays now use:

`ST75160i`

The new controller requires:

* Different initialization sequence
* OTP loading during startup
* Different power / booster configuration
* Different RAM addressing
* Different framebuffer packing

The original ST7528i driver would initialize I2C correctly but only produced random pixels or corrupted output.

---

## Hardware Used

### Display

`NHD-C160100DiZ-FSW-FBW Rev1C`

### MCU

`ESP32 Dev Board`

---

## Wiring

### ESP32 -> LCD

| ESP32  | LCD Pin | Description   |
| ------ | ------- | ------------- |
| GPIO21 | SDA     | I2C Data      |
| GPIO22 | SCL     | I2C Clock     |
| GPIO23 | RST     | Display Reset |
| 3.3V   | VDD     | Power         |
| GND    | VSS     | Ground        |

---

## Important Notes

### 3.3V Logic Required

The display uses 3.3V logic.

Attempting to use direct 5V I2C pullups from an Arduino Mega caused issues with:

* Bus ACK
* Incorrect logic levels
* 4V+ appearing on the 3.3V rail through I2C pullups

ESP32 worked immediately once proper 3.3V pullups and reset handling were implemented.

---

## Major Discovery

The display would ACK over I2C but initially showed only random pixel noise.

This turned out to be because:

`Old driver = ST7528i RAM/page format`

`New display = ST75160i RAM format`

The panel itself was healthy the entire time.

Seeing random but repeatable pixels was actually a very good sign:

* LCD glass alive
* Booster working
* Contrast working
* RAM writes functioning
* Addressing partially correct

---

## Reset Requirement

The display requires a real hardware reset pulse before initialization.

This was critical.

### Correct Reset Sequence

```cpp
digitalWrite(rst, LOW);
delay(20);

digitalWrite(rst, HIGH);
delay(200);
```

Without this, the display would not ACK correctly.

---

## Initialization Sequence

The working initialization sequence was derived from:

* Newhaven Rev1C transition guide
* ST75160i datasheet
* Newhaven Arduino example code

Key differences from ST7528i include:

* OTP loading
* Extension command modes
* Multi-byte analog configuration
* Different display RAM setup
* Different grayscale configuration

---

## Display Buffer Format

The ST75160i framebuffer format differs from the older ST7528i implementation.

### Current Buffer Layout

`1 byte = 4 vertical pixels`

Bit usage:

```text
0x80 = pixel row 0
0x40 = pixel row 1
0x20 = pixel row 2
0x10 = pixel row 3
```

---

## Useful References

### Newhaven Transition Guide

[Transition Guide](https://newhavendisplay.com/content/docs/NHD-C160100DiZ-FSW-FBW_TransitionGuide.pdf)

### Newhaven Arduino Example

[Arduino Example](https://support.newhavendisplay.com/hc/en-us/articles/25842742613911-NHD-C160100DiZ-FSW-FBW-Rev1C-with-Arduino)

### Example Source Code

See uploaded Newhaven sample code. 

---

## Current Status

### Working

* I2C communication
* Reset handling
* Display initialization
* Display enable
* RAM writes
* Pixel output
* Basic framebuffer operation

### In Progress

* Text rendering cleanup
* Final framebuffer packing verification
* Optimized Flush()
* Graphics primitives
* Grayscale mode testing

---

## Lessons Learned

Even when displays appear physically identical, controller revisions can completely change:

* Initialization
* Memory addressing
* Pixel packing
* Power sequencing

"Compatible" hardware revisions are not always firmware compatible.

Always verify:

* Controller IC revision
* Transition guides
* Datasheet updates
* Vendor example code

before assuming drop-in compatibility.
