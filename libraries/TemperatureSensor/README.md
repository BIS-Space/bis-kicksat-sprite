TemperatureSensor
=================

Library to read the internal temperature sensor in the TI cc430f5137 and the InvenSense ITG-3200 used on the Kicksat Sprite.

Install to the Energia Libraries folder.

Add `#include <temp.h>` to the top of your code to use it.

`int32_t mainTempRead(void)` and `int32_t gyroTempRead(void)` give the temperature in units of degrees Centigrade, from the cc430 processor and the itg3200 gyroscope, in integer form. `temp.h` has a global variable `premult` which can be changed to multiply the reading, in case you want temperature to higher precision

`mainTempCalibrate()` and `setMainGainOffset(gain, offset)` adjust the mapping from ADUs to degrees for the cc430. The function is a simple linear fit, `Temperature = ADU * gain + offset`.

Examples are included and can be opened using Energia, simply go to `File->Examples->TemperatureSensor`.
