# M5DualPID
Two channel PID regulator. Currently used for controlling both temperature and humidity but should be(come) versatile enough to control any two things from different sensors. The general design goals are:
 * Least work hardware: Use existing, easily buyable parts like M5Stack.
 * Two channel PID
 * Two H-Bridge outputs able to control most things from heaters, peltiers, valves etc
 * Multiple sensors and compile-time configurable
 * Error graphs

Unfortunately M5Stack does not (yet) provide a high voltage H-Bridge module (right?). For this a design will be made that can stack with M5Stack and provide: LDO regulator for power and two directly controllable H-bridges.

## Dependencies

### Software
 * My fork of MiniPID: https://github.com/knifter/MiniPID
 * I2C Device library wrapper: https://github.com/knifter/lib-TwoWireDevice
 * SHT30 driver: https://github.com/knifter/lib-SHT3x
 * Tools https://github.com/knifter/lib-Tools

### Hardware
 * M5Stack Core: https://shop.m5stack.com/collections/m5-core/products/basic-core-iot-development-kit
 * M5Stack Din rail + LDO backplane (26mm): https://shop.m5stack.com/products/base-26proto-industrial-board-module or
 * M5Stack Din rail + LDO backplane (15mm): https://shop.m5stack.com/products/base15-proto-industrial-board-module
 * SHT31-D breakout board: https://www.adafruit.com/product/2857
 * M5Stack Cushion, mandatory lab equipment: https://shop.m5stack.com/collections/m5-accessory/products/m5stack-cushion

### Ideas which could be added/supported:
 * AC-socket: https://shop.m5stack.com/products/m5-ac-socket
 * 4Ch Lego Motor module: https://shop.m5stack.com/collections/m5-module/products/lego-module
 * 3Ch Stepmotor module: https://shop.m5stack.com/products/stepmotor-module-with-mega328p-drv8825
 * Base+4x Lego Motor Module: https://shop.m5stack.com/products/basex
 * Fan for motor module: https://shop.m5stack.com/collections/m5-module/products/fan-module-for-stepmotor

## Sensors
Currently the project supports the SHT30 (temp + hum) sensor. But other sensors can, and should, be added as well.

## Dependencies

## Credits
Originally this design was based upon https://github.com/sciunto-org/humidity_regulator, an arduino sketch for an Ardiuno Uno. But since I've replaced even the PID loop library, I don't think there is much left of that design. But the idea came from them so credit where credit is due ;)

They (sciunto) quoted this paper as well, I will too:
F. Boulogne, "Cheap and versatile humidity regulator for environmentally controlled experiments", The European Physical Journal E, vol. 42, no. 4, 2019, pp. 51.
* EPJE: <https://doi.org/10.1140/epje/i2019-11813-0>
* On arxiv: <https://arxiv.org/abs/1903.08952>
