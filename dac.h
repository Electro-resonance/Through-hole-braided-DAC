// Copyright 2012 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
// Rewritten and modified for Soundforce Throughhole Braids by Martin Timms.
// Implementing a 16bit DAC using two channels of the MCP4822 dual 12bit DAC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Driver for DAC.

#ifndef BRAIDS_DRIVERS_DAC_H_
#define BRAIDS_DRIVERS_DAC_H_

#include <stm32f10x_conf.h>
#include "stmlib/stmlib.h"

#define NOP1 __asm__ volatile ("nop");
#define NOP2 NOP1 NOP1
#define NOP4 NOP2 NOP2
#define NOP8 NOP4 NOP4
#define NOP16 NOP8 NOP8
#define NOP32 NOP16 NOP16
#define NOP64 NOP32 NOP32
#define NOP128 NOP64 NOP64

#define NOP96 NOP64 NOP32
#define NOP7 NOP4 NOP2 NOP1
#define NOP13 NOP8 NOP4 NOP1

#define NOP_N(N) for(register unsigned i=0; i<(N); i++) __asm__ volatile ("nop");

namespace braids {

class Dac {
 public:
  Dac() { }
  ~Dac() { }
  
  void Init();
  inline void Write(uint16_t value) {
    uint16_t val2;
 
    GPIOB->BSRR = GPIO_Pin_12;
    NOP7 //14ns * 7

    GPIOB->BRR = GPIO_Pin_12;
    NOP13 //14ns * 13

    //step 1 convert 16 bits data to 11 bits
    val2 = value >> 5;
    val2+=1024;
    //step 2 add the configuration bit_mask
    val2 &= 0x0FFF;
    //val2 |= 0x3000;
    val2 |= 0x1000; //2x gain
    SPI2->DR = val2;

    NOP128 //14ns * 128

    //calculate first 5 bits
    val2 = value & 0x001F;
    val2 =val2 << 3;
    val2+=1920; //1024; Value to add to 8 bit to get to centre

    val2 &= 0x0FFF;
    val2 |= 0x9000; //2x gain

    GPIOB->BSRR = GPIO_Pin_12;
    NOP7 //14ns * 7

    GPIOB->BRR = GPIO_Pin_12;
    NOP13 //14ns * 13

    //Write to second DAC channel
    SPI2->DR = val2;
}
 
 private:
  
  DISALLOW_COPY_AND_ASSIGN(Dac);
};

}  // namespace braids

#endif  // BRAIDS_DRIVERS_DAC_H_
