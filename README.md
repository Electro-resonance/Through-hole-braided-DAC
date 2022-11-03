# Throughhole_Braided_DAC
Suggested improvements to the DAC code for the Braids Through-hole – SoundForce Eurorack Modular Synth DIY module including upgrade from 12 bit to 16 bit mode using a small hardware modification without changing the digital to analogue converter(DAC).


Acknowledgements:
This repository was born after building the Braids Through-hole – SoundForce module which is a derrivative work of Mutable Instruments Braids by Emilie Gillet with original firmware: https://github.com/pichenettes/eurorack/tree/master/braids and also the with changes for the derivative firmware by Tim Churches: https://github.com/timchurches/Mutated-Mutables


Reason for Development:
On completion of the Soundforce Braids through hole it was noticed that the DAC output was very noisy. It sounded like it was dropping samples. The selection of the oscillator caused the unwanted noise and intereference to change character. Adding extra capacitors to the power supply lines of the DAC, STM32 "bluepill" module and the display driver ICs did not remove this unwanted noise.

Investigation revealed a few issues:
1) The MCP4822 chipselect CS was been driven low to load each new sample, but this was taking place only within a few clock cycles of the STM32 and the period was too short causing some loads to be unsuccessful. The CS line has to be held low for 40ns before loading a new sample (tCHS) as shown in figure 1-1: https://www.mouser.co.uk/datasheet/2/268/21953a-8929.pdf
Adding extra NOP instructions allows the timinig to be made more precise and allows a longer CS low time to be defined.
2) The SPI lines were set to 50MHz, however at this speed the lines interfere with each other especially crossing the inline connections from the STM32 bluepill to the main board. Dropping to 10MHz greatly reduced the number of dropped samples. 
3) The lines to the display were also set to 50MHz, however the display can operate with 2MHz. Selecting a slower output line adds extra capacitance and reduces the likelihood that the IO line will induce external interference across the board.


16bits from a dual 12bit DAC !!

The author wanted to extend the DAC bit rate beyond 12 bits resolution to closer match that of the original Braids module from Mutable Instruments. Fortunately the MCP4822 has two 12bit DAC channels. Using two DACs in parallel with differing size output resistors into a potential divider allows the chip to function up to 24bits. This technique is in fact described in the MCP4822 datasheet. The current firmware uses 16bit buffers which is adequate for the application. It was decided to aim so that the hardware would theoretically be able to achieve 20 bits possible accuracy. The internal accuracy of the DACs and external resistors is likely to limit what is achievable to around 15-16 bits. 

Simple Extra Resistor Modification

The extended bit depth resolution modification is achieved in hardware using a 10 Mohm resistor connected from pin 6 (VOUTB) of the MCP4822 to C21. VoutA is connected to the same capacitor via 39k. The ratio of 10Mohms/39Kohms is 256.4, so allows DACB to contribute approximately 1/256th of the voltage to the signal generated by DACA. The 16 bits can therefore be divided in software between the two DAC channels to emulate a single 16 bit DAC. The same combination of two resistors could be used to output 20 bits (or 19 bits across the smaller voltage range used to improve linearity). This could be useful for example for interpolation between samples.

Extra soiftware improvement

The DAC has a small linearity issue as defined in the datasheet for values between 0V and 0.01V (i.e. close to GND). To improve linearity, channel A is driven with 11 bits across the range 1.024V to 3.072V and channel B is driven with 5 bits across a smaller range centred about 2.048V. The DAC output is AC coupled through capacitor C24, so running nearer to the 2.048V mid point makes no difference to the overall DC output levels. This software modification with the two channels being used avoids operating the two DAC channels near to the area where the DAC is known to be non-linear.

Expectation of the combined DAC resolution

The upper limit of achievable resolution of the DAC will be limited by the input sample resolution, the system noise floor, the accuracy/tolerance of the internal DAC voltage dividers and the tolerance of the two external resistors (39k and 1 Mohm).


This repository provides the modified source files for fixing the DAC timings, changing the SPI and display timing and also for generating the dual DAC output for use as 16 bit output. It also includes the hex file for the modified Mutated-Mutables firmware.


Note that the very first time I flashed the firmware to a blank STM32 "bluepill" module, I had to first flash the braids bootloader:

git clone https://github.com/hadesbox/eurorack-with-binaries/braids_bootloader

st-flash --flash=128k --format ihex write braids_bootloader.hex 


The Mutated-Mutables firmware with the modifications from this repository can then be written to the module using: 

git clone https://github.com/Electro-resonance/Throughhole_Braided_DAC

st-flash --flash=128k --format ihex write mutated_dac_through_hole.hex

Note that upgrading firmware will remove any previous custom settings.


[THE SOFTWARE AND HARDWARE SUGGESTIONS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR SUGGESTED HARDWARE CHANGES OR THE USE OR OTHER DEALINGS IN THE SOFTWARE AND/OR HARDWARE.]




Useful Links and Related Forums:

https://sound-force.nl/?page_id=3179

https://www.modwiggler.com/forum/viewtopic.php?p=3254958#p3254958

https://lookmumnocomputer.discourse.group/t/braids-tht-in-kosmo-format/372/169

https://www.youtube.com/watch?v=EpqNQiMcVl8   (Kristian Blåsol "DIY Throughhole MI Braids - Modular in a Week" YouTube video)
