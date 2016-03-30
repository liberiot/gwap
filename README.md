This library for Arduino IDE 1.6 implements GWAP on top of any existing ISM radio as far as the necessary hardware cores exist. Take a look at our repository containing the cores to see how to port your radio for GWAP.

GWAP stands for Global Wireless Abstract Protocol and is designed to turn simple wireless nodes into powerful interoperable devices, globally identifiable and programmable from the Arduino IDE:

panStamp is fully supporting GWAP, which is proving to be a serious alternative to other expensive proprietary technologies like Sigfox. GWAP usually requires less than 8 KB of flash space and less than 1 KB of RAM.

GWAP is a star-topology network protocol. All nodes transmit to a central gateway where the information is processed and then submitted to the cloud. Unlike SWAP, GWAP is not point-to-point. Only the gateway is able to contact any device in the network. On the other hand, GWAP needs a unique 12-bit addresses to be provided by each device. This feature makes any GWAP device uniquely identifiable around the world.

# Packet structure

GWAP packets need to comply witht he following packet structure:

[GWAP packet structure](http://panstamp.org/pictures/gwap_packet.jpg)

## 12-byte UID

This 12-byte field uniquely identifies the node around the planet. It consists of two shorter identifiers:

* Product ID (8 bytes) : this ID is unique for a given type of product (product code)
* Product code (4 bytes) : this code shows the type of device and manufacturer. Data coming from devices with the same product code will be processed and displayed in the same way.

The overall information forms a complete 12-byte UID which can be used only by a single device in the world.

## Cyclic nonce

This byte is incremented by the sender before every transmission. This makes that two consecutive frames, even containing the same information, will differ at least in one byte. this feature can be used as a simple protection against playback attacks.

## Function code

Type of GWAP packet. The following functions are supported:

* Status packet (function = 0) : packet containing the value of a register. Transmitted by an end node. This is the function to be used to inform about temperatures, levels, states, etc.
* Query packet (function = 1) : packet transmitted from a gateway to a node. Used to query about the contents of a register.
* Command packet (function = 2) : packet asking to change a register in a node. Always transmitted by a gateway.

## Register ID

Each register is identifiable by its ID. Any GWAP end-node contains seven mandatory (standard) registers and at least a custom (regular) register, containing endpoint information. Standard registers are listed below:

  REGI_PRODUCTCODE = 0,                 \
  REGI_HWVERSION,                       \
  REGI_FWVERSION,                       \
  REGI_SYSSTATE,                        \
  REGI_FREQCHANNEL,                     \
  REGI_NETWORKID,                       \
  REGI_TXINTERVAL,

### Product code

* Register ID : 0
* Length : 4 bytes
* Description : simple register containing the 4-byte product code explained above. 4-byte length.

### Hardware version

* Register ID : 1
* Length : 4 bytes
* Description : numeric version of hardware

### Hardware version

* Register ID : 2
* Length : 4 bytes
* Description : numeric version of firmware

### System state

* Register ID : 3
* Length : 1 byte
* Description : current state of the device. This state can be controlled by the gateway as far as the end-node remains in reception mode. Available states : 
 * state = 0 : restart device
 * state = 1 : RF reception enabled
 * state = 2 : RF reception disabled
 * state = 3 : Synchronization mode
 * state = 4 : Low battery state
 * state = 5 : Firmware upgrade mode

Additional states can be added by the developer.

### Frequency channel

* Register ID : 4
* Length : 1 byte
* Description : register used to alter change the frequency channel of the end-node.

### Network ID or synchronisation word

* Register ID : 5
* Length : 2 bytes
* Description : Multiple GWAP networks can be isolated one from the other by setting different network ID's. This practice should be avoided as much as possible in order to guarantee interoperbility between nodes and networks. The standard ID used by GWAP is 0x6A1C.

### Transmission interval

* Register ID : 6
* Length : 2 bytes
* Description : interval in seconds between periodic transmissions.

