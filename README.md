# avr-ir-bootstrap

A barebones ATMega328p project that can decode NEC infrared signals with proper hardware setup.  This code is easily adaptable to any other AVR chip that has input capture capabilities.  Additional IR remotes and protocols are trivial to add.

### Technical Overview

**1.** IR signals will be sent from the remote and subsequently picked up by the TSOP38238 (the IR receiver hardware I'm using - you can use whatever you want that does the job)

**2.** The TSOP38238 will directly output the received pulses (marks and spaces) on it's output pin

**3.** This output will be hooked up directly to the input capture pin of the ATMega328p pin - the input capture interrupt will be triggered on each edge change

**4.** Edge change by edge change the length of each mark and space will be calculated in microseconds

**5.** Each new mark/space will be checked against a list of known headers in an attempt to match to a protocol - NEC's header starts with a 9000 microsecond mark and follows up with a 4500 microsecond space

**6.** Once a header has been matched to a protocol, all following marks/spaces will be treated as logical data bits and stored

**7.** After receiving all the bits from the full IR burst, we will know what button was pressed on the remote since each button sends a different series of data bits

**8.** The ATMega328p can then respond to the signal in whatever way you see fit

### High Level Overview of IR Protocols

![NEC Protocol](https://github.com/twilco/rc-car/blob/master/img/NEC_IR_Protocol.png)

Pictured above is the NEC protocol, which is one of many IR protocols, and the one I use in this project.  It was created by the major Japanese technology company formally known as Nippon Electric Company (hence the name NEC), and is one of the more popular protocols today.  The NEC protocol has a few distinct parts:

* A header
* An address, and the inverse (usually) of that address for redundancy
* A command, and the inverse of that command

The **header** is essentially the protocol's way of telling the receiver that they should be ready to receive data with specific timings.  It can also be used to convey message-wide configuration data - for example, the RC6 header has three bits that specify a mode and one bit that can signal a long button press or a double button press.

The header for the NEC protocol has none of that.  It is simply a 9ms (9000 microsecond) "on" pulse (a **mark**), followed by a 4.5ms "off" (a **space**).  This is important because it lets the microcontroller (or whatever may be listening to the receiver) know how to interpret the coming pulses so that they can be turned into something the microcontroller understands - bits.

When following the NEC protocol, a logical zero equates to a 562 microsecond mark followed by a 562 microsecond space.  Conversely, a logical one consists of a 562 microsecond mark followed up by a 1675 microsecond space.

The **address** portion of the NEC protocol is a unique value assigned to each manufacturer using the NEC protocol.  This protocol quickly became so popular that all of these addresses were used up - this led to what is known as the extended NEC protocol.  The extended NEC protocol forgos the redundancy provided by sending the inverse of the first 8 bits of the address, and instead allows the full 16-bit section to signify an address, extending the range from 256 possible addresses to 65536.

The **command** is exactly what you think it is - a unique value arbitrarily chosen by the IR transmitter that corresponds to whatever triggered the transmission (e.g. a button press on a remote).

So, for example, [Sparkfun's COM-11759](https://www.sparkfun.com/products/11759) remote's power button transmits `00010000111011111101100000100111` when pressed.  Let's break that down from left to right:

`00010000` - the 8-bit address specific to this remote

`11101111` - the inverse of this address (all buttons on this remote transmit these same 16 bits to start)

`11011000` - the command, arbitrarily chosen by Sparkfun for this button

`00100111` - the inverse of that command


