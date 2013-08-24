<h1>Overview</h1>
This repository includes the code which will be put on the BIS KickSat sprite.

<h2>Dependecies</h2>
In order for the code to work the CCFlash, RadiationTest and TemperatureSensor directories need to be put into this directory:

    ~/EnergiaKickSatWindows/hardware/msp430/libraries

<h2>Code Structure</h2>
Our code has 3 main sections: 
<ol>
    <li>Transmitting the temperature readings from the main processor and the gyroscope in Kelvin so we don't have to deal with minus signs. (Line 372)</li>
    <li>Transmitting the session information. This includes the minimum, maximum and average session durations as well as the number of times the sprite has started up. (Line 260)</li>
    <li>Transmitting the results of a memory integrity test. Segment B of the flash memory is filled with 1's. The sprite will then transmit the number of bits which have been flipped to 0's, this can only be due to radiation. (Line 434)</li>
</ol>

<h2>Redundancy</h2>
The sprite is also programmed to log where it is in the code. 

For example, if the sprite loses power after transmitting the first section. Upon restart it will skip to transmitting the next section, in this case the second section (See the main loop on line 104.)

If there is a problem with the code in one of the three section due to radiation damage, upon the next restart the sprite is programmed to skip to the piece of code after the section it failed on.

In order to calculate the minimum, maximum and average session durations and the number of startups it stores this data in segment C of the flash memory. During every startup it recalculates this data with a few simple calculations (see lines 218-223.)

<h2>Encoding of Messages</h2>
The separate parts of the transmission (temperature, radiation test and session info) are separated by header characters: T, R and S. The actual values to be transmitted are converted into hexadecimal in an attempt to save time. 

<ol>
    <li>The session information is transmitted as 12 characters in sets of 4, each representing minimum, maximum and aveerage durations. The number of startups is then transmitted but is variable in length. For example, if the number of startups is large enough to require 3 hex characters it will send 3, if only 2 are needed, it will send 2. This is headed by a "S".</li>
    <li>The temperature is sent in variable length as well. However the length is always an even number, so, the first half is the main processor reading and the second half is the gyroscope reading. This is headed by a "T".</li>
    <li>The radiation test results are also sent in variable length. However, will usually be "0". This is headed by a "R".</li>
