# DACX1416_lib
A basic library for TI's DAC81416, DAC71416 and DAC61416 DACs and the associated DAC81416EVM.

**See `main.cpp` for an working example. Compile with Arduino IDE or PlatformIO.**

**Platforms:**

I tested the code with Arduino Due and DAC81416EVM. The code should work for other platforms as well. Please report if something is amiss.

**Limitations:**
1. Some bugs may exist. Please report if you encounter one. 
2. LDAC pin is not handled by the library.
3. "Advanced" features like broadcast, toggle, offset, etc. don't have a helper function in this library. They can however be accessed with the `write_reg()` and `read_reg()` methods.

**TODO:**
1. Code cleanup
2. Add more examples and follow standard Arduino library format

**Credits:**

Thanks to [sphCow](https://github.com/sphCow) for the [DAC81404 library](https://github.com/sphCow/DAC81404_lib).