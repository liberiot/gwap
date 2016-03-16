This library for Arduino IDE 1.6 implements GWAP on top of any existing
ISM radio as far as the necessary hardware cores exist. Take a look at
our repository containing the cores to see how to port your radio for SWAP.

https://github.com/panStamp/panstamp/wiki

GWAP stands for Global Wireless Abstract Protocol and is designed to turn
simple wireless nodes into powerful interoperable devices, globally identifiable
and programmable the Arduino IDE:

https://github.com/panStamp/panstamp/wiki/Global-Wireless-Abstract-Protocol

panStamp is fully supporting GWAP, which has proven to be a serious
alternative to other oversized and overkilled protocols like Zigbee.
GWAP usually requires less than 8 KB of flash space and less than 1 KB of RAM.

GWAP is a star-topology network protocol. All nodes transmit to a central
gateway where the information is processed and then submitted to the cloud. Unlike
SWAP, GWAP is not point-to-point. Only the gateway is able to contact any device
in the network. On the other hand, GWAP needs a unique 12-bit addresses to be
provided by each device. This feature makes any GWAP device uniquely identifiable
around the world.

# Packet structure

GWAP packets need to comply witht he following packet structure:

 -----------------------------------------------------------------------------
|length|    device UID    |nonce | fnct |Reg ID|     data payload      |  CRC |
 -----------------------------------------------------------------------------
|1-byte|     12-byte      |1-byte|1-byte|1-byte|   up to 47 bytes      |1-byte|

