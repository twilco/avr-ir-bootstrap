# rc-car

DIY RC Car

#### To do

- [ ] Figure out how to unit test and set up CI - http://www.throwtheswitch.org
- [ ] Decode NEC infrared signals sent from remote
- [ ] Wire motor drivers and motors to ATMega328p and the breadboard
- [ ] Implement forward, back, left, right
- [ ] Implement three speed modes
- [ ] Integrate text-to-speech IC and wire it to the two remaining buttons on the remote
- [ ] Put the project on a protoboard and 3D print a casing for it

## Documentation

**Main Components**
* [AVR Pocket Programmer](https://www.sparkfun.com/products/9825)
* Remote control and TSOP38238 IR Receiver [from this kit](https://www.sparkfun.com/products/13235)
* ATMega328p
* (2) L298N Dual H Bridge DC Motor Controller Boards
* (4) DC stepper motors with attached wheels

#### High Level Overview

![NEC Protocol](https://github.com/twilco/rc-car/img/NEC_IR_Protocol.png)

Pictured above is the NEC protocol, which is one of many IR protocols, and the one I use in this project.  It was created by the major Japanese technology company formally known as Nippon Electric Company (hence the name NEC), and is one of the more popular protocols today.  The NEC protocol has a few distinct parts:

* A header
* An address, and the inverse (usually) of that address for redundancy
* A command, and the inverse of that command

A header is essentially the protocol's way of telling the receiver that they should be ready to receive data with specific timings.  It can also be used to convey message-wide configuration data - for example, the RC6 header has three bits that specify a mode and one bit that can signal a long button press or a double button press.

The header for the NEC protocol has none of that.  It is simply a 9ms (9000 microsecond) "on" pulse (a **mark**), followed by a 4.5ms "off" (a **space**).  This is important because it lets the microcontroller (or whatever may be listening to the receiver) know how to interpret the coming pulses so that they can be turned into something the microcontroller understands - bits.

When following the NEC protocol, a logical zero equates to a 562 microsecond mark followed by a 562 microsecond space.  Conversely, a logical one consists of a 562 microsecond mark followed up by a 1675 microsecond space.

The address portion of the NEC protocol is a unique value assigned to each manufacturer using the NEC protocol.  This protocol quickly became so popular that all of these addresses were used up - this led to what is known as the extended NEC protocol.  The extended NEC protocol forgos the redundancy provided by sending the inverse of the first 8 bits of the address, and instead allows the full 16-bit section to signify an address, extending the range from 256 possible addresses to 65536.

The command is exactly what you think it is - a unique value arbitrarily chosen by the IR transmitter.

#### RC Car Project Overview

**1.** IR signals will be sent from the remote and subsequently picked up by the TSOP38238

**2.** The TSOP38238 will directly output the received pulses (marks and spaces) on it's output pin

**3.** This output will be hooked up directly to the input capture pin of the ATMega328p pin

**4.** Using the input capture feature of the ATMega, one-by-one the length of each mark and space will be calculated in microseconds

**5.** Each new mark/space will be checked against a list of known header in an attempt to match to a protocol - NEC's header starts with a 9000 microsecond mark and follows up with a 4500 microsecond space

**6.** Once a header has been matched to a protocol, all following marks/spaces will be treated as logical data bits and stored

**7.** After receiving all the bits from the full IR burst, we will know what button was pressed on the remote since each button sends a different payload

**8.** The ATMega will respond to the received message in the appropriate way, be that sending power to the motors, changing a speed mode, etc.