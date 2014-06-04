/* Copyright (C) 2014

    Contributor:  Pedro Cevallos

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Acknowledgements:
Fabian Greif for the initial MCP2515 library http://www.kreatives-chaos.com/artikel/universelle-can-bibliothek
David Harding for his version of the MCP2515 library http://forum.arduino.cc/index.php/topic,8730.0.html
Kyle Crockett CANduino library with 16Mhz oscillator. (http://code.google.com/p/canduino/)
Nuno Alves for the help on Extended ID messaging
Stevenh for his work on library and all of the MCP research/work http://modelrail.otenko.com/arduino/arduino-controller-area-network-can

-------------------------------------------------------------------------------------------------------------
Change Log

DATE		VER		WHO			WHAT
07/07/13	0.1		PC		Modified and merge all MCP2515 libraries found. Stripped away most unused functions and corrected MCP2515 defs
09/12/13	0.2		PC		Added selectable CS SPI for CAN controller to use 1 IC to control several mcp2515
02/05/14	0.3		PC		Added filter and mask controls
05/01/14	0.4		PC		Cleaned up functions, variables and added message structures for J1939, CANopen and CAN.
05/07/14	1.0		PC		Released library to public through GitHub. Changelog will be tracked through GitHub
-------------------------------------------------------------------------------------------------------------

Features:

CAN V2.0B
8 byte length in the data field
Standard and extended data frames
Two receive buffers
Three Transmit Buffers
SPI Interface

Supported Baud rates
10 kbps; 20 kbps; 50 kbps; 100 kbps; 125 kbps; 250 kbps; 500 kbps; 1000 kbps

Intended to be used with ATMEL ATMega328P with Arduino bootloader, MCP2515 Stand-Alone CAN Controller and MCP2561 High-Speed CAN Transceivers.
*/

#if defined(ARDUINO_ARCH_AVR)

#ifndef _CAN_MCP2515_H_
#define _CAN_MCP2515_H_

#include <Arduino.h>
#include <SPI.h>
#include "CAN.h"

//SPI Interface functions
#define MCP2515_SPI_RESET 0xC0
#define MCP2515_SPI_READ 0x03
#define MCP2515_SPI_WRITE 0x02
#define MCP2515_SPI_READ_STATUS 0xA0
#define MCP2515_SPI_RX_STATUS 0xB0
#define MCP2515_SPI_BIT_MODIFY 0x05


//SPI functions to read CAN RX Buffers
#define MCP2515_READ_RX_BUF_0_ID 0x90
#define MCP2515_READ_RX_BUF_0_DATA 0x92
#define MCP2515_READ_RX_BUF_1_ID 0x94
#define MCP2515_READ_RX_BUF_1_DATA 0x96

//SPI functions to load CAN TX Buffers
#define MCP2515_LOAD_TX_BUF_0_ID 0x40
#define MCP2515_LOAD_TX_BUF_0_DATA 0x41
#define MCP2515_LOAD_TX_BUF_1_ID 0x42
#define MCP2515_LOAD_TX_BUF_1_DATA 0x43
#define MCP2515_LOAD_TX_BUF_2_ID 0x44
#define MCP2515_LOAD_TX_BUF_2_DATA 0x45

//SPI functions RTS (Request to send) messages from CAN TX buffers.
#define MCP2515_SEND_TX_BUF_0 0x81
#define MCP2515_SEND_TX_BUF_1 0x82
#define MCP2515_SEND_TX_BUF_2 0x84
#define MCP2515_SEND_ALL 0X87


// MCP2515 CAN CONTROLLER REGISTERS. SEE MCP2515 DATASHEET SECTION 11.0 FOR FURTHER EXPLANATION
// http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en010406

#define MCP2515_RXF0SIDH	0x00
#define MCP2515_RXF0SIDL	0x01
#define MCP2515_RXF0EID8	0x02
#define MCP2515_RXF0EID0	0x03
#define MCP2515_RXF1SIDH	0x04
#define MCP2515_RXF1SIDL	0x05
#define MCP2515_RXF1EID8	0x06
#define MCP2515_RXF1EID0	0x07
#define MCP2515_RXF2SIDH	0x08
#define MCP2515_RXF2SIDL	0x09
#define MCP2515_RXF2EID8	0x0A
#define MCP2515_RXF2EID0	0x0B
#define MCP2515_BFPCTRL		0x0C
#define MCP2515_TXRTSCTRL	0x0D
#define MCP2515_CANSTAT		0x0E
#define MCP2515_CANCTRL		0x0F

#define MCP2515_RXF3SIDH	0x10
#define MCP2515_RXF3SIDL	0x11
#define MCP2515_RXF3EID8	0x12
#define MCP2515_RXF3EID0	0x13
#define MCP2515_RXF4SIDH	0x14
#define MCP2515_RXF4SIDL	0x15
#define MCP2515_RXF4EID8	0x16
#define MCP2515_RXF4EID0	0x17
#define MCP2515_RXF5SIDH	0x18
#define MCP2515_RXF5SIDL	0x19
#define MCP2515_RXF5EID8	0x1A
#define MCP2515_RXF5EID0	0x1B
#define MCP2515_TEC		0x1C
#define MCP2515_REC     	0x1D

#define MCP2515_RXM0SIDH	0x20
#define MCP2515_RXM0SIDL	0x21
#define MCP2515_RXM0EID8	0x22
#define MCP2515_RXM0EID0	0x23
#define MCP2515_RXM1SIDH	0x24
#define MCP2515_RXM1SIDL	0x25
#define MCP2515_RXM1EID8	0x26
#define MCP2515_RXM1EID0	0x27
#define MCP2515_CNF3		0x28
#define MCP2515_CNF2		0x29
#define MCP2515_CNF1		0x2A
#define MCP2515_CANINTE		0x2B
#define MCP2515_CANINTF		0x2C
#define MCP2515_EFLG		0x2D

#define MCP2515_TXB0CTRL	0x30
#define MCP2515_TXB0SIDH	0x31
#define MCP2515_TXB0SIDL	0x32
#define MCP2515_TXB0EID8	0x33
#define MCP2515_TXB0EID0	0x34
#define MCP2515_TXB0DLC		0x35
#define MCP2515_TXB0D0		0x36
#define MCP2515_TXB0D1		0x37
#define MCP2515_TXB0D2		0x38
#define MCP2515_TXB0D3		0x39
#define MCP2515_TXB0D4		0x3A
#define MCP2515_TXB0D5		0x3B
#define MCP2515_TXB0D6		0x3C
#define MCP2515_TXB0D7		0x3D

#define MCP2515_TXB1CTRL	0x40
#define MCP2515_TXB1SIDH	0x41
#define MCP2515_TXB1SIDL	0x42
#define MCP2515_TXB1EID8	0x43
#define MCP2515_TXB1EID0	0x44
#define MCP2515_TXB1DLC		0x45
#define MCP2515_TXB1D0		0x46
#define MCP2515_TXB1D1		0x47
#define MCP2515_TXB1D2		0x48
#define MCP2515_TXB1D3		0x49
#define MCP2515_TXB1D4		0x4A
#define MCP2515_TXB1D5		0x4B
#define MCP2515_TXB1D6		0x4C
#define MCP2515_TXB1D7		0x4D

#define MCP2515_TXB2CTRL	0x50
#define MCP2515_TXB2SIDH	0x51
#define MCP2515_TXB2SIDL	0x52
#define MCP2515_TXB2EID8	0x53
#define MCP2515_TXB2EID0	0x54
#define MCP2515_TXB2DLC		0x55
#define MCP2515_TXB2D0		0x56
#define MCP2515_TXB2D1		0x57
#define MCP2515_TXB2D2		0x58
#define MCP2515_TXB2D3		0x59
#define MCP2515_TXB2D4		0x5A
#define MCP2515_TXB2D5		0x5B
#define MCP2515_TXB2D6		0x5C
#define MCP2515_TXB2D7		0x5D

#define MCP2515_RXB0CTRL	0x60
#define MCP2515_RXB0SIDH	0x61
#define MCP2515_RXB0SIDL	0x62
#define MCP2515_RXB0EID8	0x63
#define MCP2515_RXB0EID0	0x64
#define MCP2515_RXB0DLC		0x65
#define MCP2515_RXB0D0		0x66
#define MCP2515_RXB0D1		0x67
#define MCP2515_RXB0D2		0x68
#define MCP2515_RXB0D3		0x69
#define MCP2515_RXB0D4		0x6A
#define MCP2515_RXB0D5		0x6B
#define MCP2515_RXB0D6		0x6C
#define MCP2515_RXB0D7		0x6D

#define MCP2515_RXB1CTRL	0x70
#define MCP2515_RXB1SIDH	0x71
#define MCP2515_RXB1SIDL	0x72
#define MCP2515_RXB1EID8	0x73
#define MCP2515_RXB1EID0	0x74
#define MCP2515_RXB1DLC		0x75
#define MCP2515_RXB1D0		0x76
#define MCP2515_RXB1D1		0x77
#define MCP2515_RXB1D2		0x78
#define MCP2515_RXB1D3		0x79
#define MCP2515_RXB1D4		0x7A
#define MCP2515_RXB1D5		0x7B
#define MCP2515_RXB1D6		0x7C
#define MCP2515_RXB1D7		0x7D

// MESSAGE TRANSMISSION REGISTER BIT DEFINITIONS.

// TXBnCTRL TRANSMIT BUFFER n CONTROL REGISTER (ADDRESS: 30h, 40h, 50h)
#define MCP2515_ABTF		6
#define MCP2515_MLOA		5
#define MCP2515_TXERR		4
#define MCP2515_TXREQ		3
#define MCP2515_TXP1		1
#define MCP2515_TXP0		0

// TXRTSCTRL TXnRTS PIN CONTROL AND STATUS REGISTER (ADDRESS: 0Dh)
#define MCP2515_B2RTS		5
#define MCP2515_B1RTS		4
#define MCP2515_B0RTS		3
#define MCP2515_B2RTSM		2
#define MCP2515_B1RTSM		1
#define MCP2515_B0RTSM		0

// TXBnSIDH TRANSMIT BUFFER n STANDARD IDENTIFIER HIGH (ADDRESS: 31h, 41h, 51h)
#define MCP2515_SID10		7
#define MCP2515_SID9		6
#define MCP2515_SID8		5
#define MCP2515_SID7		4
#define MCP2515_SID6		3
#define MCP2515_SID5		2
#define MCP2515_SID4		1
#define MCP2515_SID3		0

// TXBnSIDL TRANSMIT BUFFER n STANDARD IDENTIFIER LOW (ADDRESS: 32h, 42h, 52h)
#define MCP2515_SID2		7
#define MCP2515_SID1		6
#define MCP2515_SID0		5
#define MCP2515_EXIDE		3
#define MCP2515_EID17		1
#define MCP2515_EID16		0

// TXBnEID8 TRANSMIT BUFFER n EXTENDED IDENTIFIER HIGH (ADDRESS: 33h, 43h, 53h)
#define MCP2515_EID15		7
#define MCP2515_EID14		6
#define MCP2515_EID13		5
#define MCP2515_EID12		4
#define MCP2515_EID11		3
#define MCP2515_EID10		2
#define MCP2515_EID9		1
#define MCP2515_EID8		0

// TXBnEID0 TRANSMIT BUFFER n EXTENDED IDENTIFIER LOW (ADDRESS: 34h, 44h, 54h)
#define MCP2515_EID7		7
#define MCP2515_EID6		6
#define MCP2515_EID5		5
#define MCP2515_EID4		4
#define MCP2515_EID3		3
#define MCP2515_EID2		2
#define MCP2515_EID1		1
#define MCP2515_EID0		0

// TXBnDLC - TRANSMIT BUFFER n DATA LENGTH CODE (ADDRESS: 35h, 45h, 55h)
#define MCP2515_RTR		6
#define MCP2515_DLC3		3
#define MCP2515_DLC2		2
#define MCP2515_DLC1		1
#define MCP2515_DLC0		0


// MESSAGE RECEPTION REGISTER BIT DEFINITIONS.

// RXB0CTRL RECEIVE BUFFER 0 CONTROL (ADDRESS: 60h)
#define MCP2515_RXM1		6
#define MCP2515_RXM0		5
#define MCP2515_RXRTR		3
#define MCP2515_BUKT		2
#define MCP2515_BUKT1		1
#define MCP2515_FILHIT0		0

// RXB1CTRL RECEIVE BUFFER 1 CONTROL (ADDRESS: 70h). SAME AS RXB0CTRL BUT WITH THE FOLLOWING CHANGES
#define MCP2515_FILHIT2		2
#define MCP2515_FILHIT1		1

// BFPCTRL RXnBF PIN CONTROL AND STATUS (ADDRESS: 0Ch)
#define MCP2515_B1BFS		5
#define MCP2515_B0BFS		4
#define MCP2515_B1BFE		3
#define MCP2515_B0BFE		2
#define MCP2515_B1BFM		1
#define MCP2515_B0BFM		0

// RXBnSIDH RECEIVE BUFFER n STANDARD IDENTIFIER HIGH (ADDRESS: 61h, 71h)
#define MCP2515_SID10		7
#define MCP2515_SID9		6
#define MCP2515_SID8		5
#define MCP2515_SID7		4
#define MCP2515_SID6		3
#define MCP2515_SID5		2
#define MCP2515_SID4		1
#define MCP2515_SID3		0

// RXBnSIDL RECEIVE BUFFER n STANDARD IDENTIFIER LOW (ADDRESS: 62h, 72h)
#define MCP2515_SID2		7
#define MCP2515_SID1		6
#define MCP2515_SID0		5
#define MCP2515_SRR		4
#define MCP2515_IDE		3
#define MCP2515_EID17		1
#define MCP2515_EID16		0

// RXBnEID8 RECEIVE BUFFER n EXTENDED IDENTIFIER HIGH (ADDRESS: 63h, 73h)
#define MCP2515_EID15		7
#define MCP2515_EID14		6
#define MCP2515_EID13		5
#define MCP2515_EID12		4
#define MCP2515_EID11		3
#define MCP2515_EID10		2
#define MCP2515_EID9		1
#define MCP2515_EID8		0

// RXBnEID0 RECEIVE BUFFER n EXTENDED IDENTIFIER LOW (ADDRESS: 64h, 74h)
#define MCP2515_EID7		7
#define MCP2515_EID6		6
#define MCP2515_EID5		5
#define MCP2515_EID4		4
#define MCP2515_EID3		3
#define MCP2515_EID2		2
#define MCP2515_EID1		1
#define MCP2515_EID0		0

// RXBnDLC RECEIVE BUFFER n DATA LENGTH CODE (ADDRESS: 65h, 75h)
#define MCP2515_RTR		6
#define MCP2515_DLC3		3
#define MCP2515_DLC2		2
#define MCP2515_DLC1		1
#define MCP2515_DLC0		0

// CONFIGURATION REGISTER BIT DEFINITIONS.

// CNF1 CONFIGURATION 1 (ADDRESS: 2Ah)
#define MCP2515_SJW1		7
#define MCP2515_SJW0		6
#define MCP2515_BRP5		5
#define MCP2515_BRP4		4
#define MCP2515_BRP3		3
#define MCP2515_BRP2		2
#define MCP2515_BRP1		1
#define MCP2515_BRP0		0

// CNF2 CONFIGURATION 2 (ADDRESS: 29h)
#define MCP2515_BTLMODE		7
#define MCP2515_SAM		6
#define MCP2515_PHSEG12		5
#define MCP2515_PHSEG11		4
#define MCP2515_PHSEG10		3
#define MCP2515_PRSEG2		2
#define MCP2515_PRSEG1		1
#define MCP2515_PRSEG0		0

// CNF3 - CONFIGURATION 1 (ADDRESS: 28h)
#define MCP2515_SOF		7
#define MCP2515_WAKFIL		6
#define MCP2515_PHSEG22		2
#define MCP2515_PHSEG21		1
#define MCP2515_PHSEG20		0

// CANCTRL CAN CONTROL REGISTER (ADDRESS: 0Fh)
#define MCP2515_REQOP2		7
#define MCP2515_REQOP1		6
#define MCP2515_REQOP0		5
#define MCP2515_ABAT		4
#define MCP2515_OSM		3
#define MCP2515_CLKEN		2
#define MCP2515_CLKPRE1		1
#define MCP2515_CLKPRE0		0

// CANSTAT CAN STATUS REGISTER (ADDRESS: 0Eh)
#define MCP2515_OPMOD2		7
#define MCP2515_OPMOD1		6
#define MCP2515_OPMOD0		5
#define MCP2515_ICOD2		3
#define MCP2515_ICOD1		2
#define MCP2515_ICOD0		1

// EFLG ERROR FLAG (ADDRESS: 2Dh)
#define MCP2515_RX1OVR		7
#define MCP2515_RX0OVR		6
#define MCP2515_TXB0		5
#define MCP2515_TXEP		4
#define MCP2515_RXEP		3
#define MCP2515_TXWAR		2
#define MCP2515_RXWAR		1
#define MCP2515_EWARN		0

// CANINTE INTERRUPT ENABLE (ADDRESS: 2Bh)
#define MCP2515_MERRE		7
#define MCP2515_WAKIE		6
#define MCP2515_ERRIE		5
#define MCP2515_TX2IE		4
#define MCP2515_TX1IE		3
#define MCP2515_TX0IE		2
#define MCP2515_RX1IE		1
#define MCP2515_RX0IE		0

// CANINTF INTERRUPT FLAG (ADDRESS: 2Ch)
#define MCP2515_MERRF		7
#define MCP2515_WAKIF		6
#define MCP2515_ERRIF		5
#define MCP2515_TX2IF		4
#define MCP2515_TX1IF		3
#define MCP2515_TX0IF		2
#define MCP2515_RX1IF		1
#define MCP2515_RX0IF		0

//FILTER AND MASK ADDRESSES.

#define MCP2515_mask0 0x20
#define MCP2515_mask1 0x24
#define MCP2515_filter0 0x00
#define MCP2515_filter1 0x04
#define MCP2515_filter2 0x08
#define MCP2515_filter3 0x10
#define MCP2515_filter4 0x14
#define MCP2515_filter5 0x18


//MCP2515 MODES
#define MCP2515_MODE_NORMAL		0x00
#define MCP2515_MODE_SLEEP		0x20
#define MCP2515_MODE_LOOPBACK	0x40
#define MCP2515_MODE_LISTEN		0x60
#define MCP2515_MODE_CONFIG		0x80

//Define some responses
#define YES 		1
#define NO		0
#define FAIL 		0
#define INIT 		1
#define ERROR 		0

//buffer definitions
#define MCP2515_buffer1		1
#define MCP2515_buffer2		2
#define MCP2515_buffer3		3
#define MCP2515_allBuffers  	0

// MCP class
class CAN_MCP2515 : public CANClass
{
  public:
    CAN_MCP2515();// Use pin 10 for SPI CS. Allows multiple CAN channels.
    CAN_MCP2515(uint8_t CS_Pin);// SPI CS is selectable through sketch. Allows multiple CAN channels.
    void begin (uint32_t baud) {
      begin(baud, MCP2515_MODE_NORMAL);
    }; // Initializes CAN communications into Normal mode. Note it also starts SPI communications
    void begin (uint32_t baud, uint8_t mode);// Initializes CAN communications. Note it also starts SPI communications
    void end();
    uint8_t available(); // check if message has been received on any of the buffers
    CAN_FRAME read();
    uint8_t read(unsigned long *ID, byte *length_out, byte *data_out); // Receive and display any message (J1939, CANopen, CAN)
    //    uint8_t read(CAN_FRAME& message); //Receive and display CAN message and allows use of the message structure for easier message handling
    //    uint8_t read(CAN_DATA_FRAME_J1939 *message); //Receive and display J1939 message and allows use of the message structure for easier message handling
    //    uint8_t read(CAN_DATA_FRAME_CANopen *message); //Receive and display CANopen message and allows use of the message structure for easier message handling
    void flush();
    uint8_t write(CAN_FRAME&);
    uint8_t write(unsigned long ID, byte frameType, byte length, byte *data); // Load and send message. No RTS needed.


  private:
    void reset(); //CAN software reset. Also puts MCP2515 into config mode
    byte readMode(); // reads CAN mode
    unsigned long readRate(); // reads CANspeed

    void clearRxBuffers(); // clears all receive buffers
    void clearTxBuffers(); // clears all receive buffers
    void clearFilters();   // clears all filters and masks

    void loadMsg(byte buffer, unsigned long ID, byte frameType, byte length, byte *data); //Load Standard Data Frame Message into TX buffer X. Note this only load message to buffer. RTS is needed to send message

    void sendTx(byte buffer); //(RTS) Request to send individual TX buffers or all

    uint8_t CS; //SPI CS is selectable through sketch
    void _init();

    // Make setting baud and mode only part of the constructor
    void bitRate(unsigned long baud);//sets up CAN bit rate
    void setMode(byte mode);//puts CAN controller in one of five modes

    void writeAddress(byte address, byte value);// writes MCP2515 register addresses
    byte readAddress(byte address); //reads MCP2515 registers
    void modifyAddress(byte address, byte mask, byte value); // MCP2515 SPI bit modification commands

    byte readStatus(); //reads several status bits for transmit and receive functions.
    byte readRXStatus(); //reads receive functions and filhits

    // OTHER USEFUL FUNCTIONS

    void enableRTSPins (); // Enable hardware request to send (RTS) pins if they are available. It allows messages to be send by driving MCP2515 RTS pins low.
    void setInterrupts(byte mask, byte writeVal); //Enable/disable interrupts
    void setMask(byte mask, byte b0, byte b1, byte b2, byte b3); // Set Masks for filters
    void setFilter(byte filter, byte b0, byte b1, byte b2, byte b3); //Set Receive filters

};

#endif // _CAN_MCP2515_H_

#endif // defined(ARDUINO_ARCH_AVR)