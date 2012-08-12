/***************************************************************
 *  SmartRF Studio(tm) Export
 *
 *  Radio register settings specifed with C-code
 *  compatible #define statements.
 *
 ***************************************************************/

#ifndef SMARTRF_CC430_H
#define SMARTRF_CC430_H

// [BM] Modified radio settings for 433MHz, 868MHz, 915MHz 

// ISM_LF configuration
//
// Chipcon
// Product = CC1101
// Chip version = A   (VERSION = 0x04)
// Crystal accuracy = 10 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 232.142857 kHz
// Deviation = 32 kHz
// Datarate = 76.766968 kBaud
// Modulation = (1) GFSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 433.92 MHz
// Channel spacing = 199.951172 kHz
// Channel number = 0
// Optimization = -
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (41) CHIP_RDY

// ISM_EU configuration
//
// Chipcon
// Product = CC1101
// Chip version = A   (VERSION = 0x04)
// Crystal accuracy = 10 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 232.142857 kHz
// Deviation = 32 kHz
// Datarate = 76.766968 kBaud
// Modulation = (1) GFSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 869.524963 MHz
// Channel spacing = 199.951172 kHz
// Channel number = 0
// Optimization = -
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (41) CHIP_RDY

// ISM_US configuration
//
// Chipcon
// Product = CC1101
// Chip version = A   (VERSION = 0x04)
// Crystal accuracy = 10 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 232.142857 kHz
// Deviation = 32 kHz
// Datarate = 76.766968 kBaud
// Modulation = (1) GFSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 905.998993 MHz
// Channel spacing = 199.951172 kHz
// Channel number = 0
// Optimization = -
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (41) CHIP_RDY

#define SMARTRF_RADIO_CC430

#define SMARTRF_SETTING_FSCTRL1    0x08
#define SMARTRF_SETTING_FSCTRL0    0x00
#ifdef ISM_EU
	// 868 MHz
	#define SMARTRF_SETTING_FREQ2      0x21
	#define SMARTRF_SETTING_FREQ1      0x62
	#define SMARTRF_SETTING_FREQ0      0x76
#else
  #ifdef ISM_US  
    // 902MHz (CHANNR=20->906MHz)
    #define SMARTRF_SETTING_FREQ2      0x22
    #define SMARTRF_SETTING_FREQ1      0xB1
    #define SMARTRF_SETTING_FREQ0      0x3B
  #else
  	#ifdef ISM_LF
	  	// 433.92MHz
	    #define SMARTRF_SETTING_FREQ2      0x10
	    #define SMARTRF_SETTING_FREQ1      0xB0
	    #define SMARTRF_SETTING_FREQ0      0x71
  	#else
		#error "Wrong ISM band specified (valid are ISM_LF, ISM_EU and ISM_US)"
	#endif // ISM_LF
  #endif // ISM_US
#endif // ISM_EU

/*
 Deviation = 20.629883 
 Base frequency = 867.999939 
 Carrier frequency = 867.999939 
 Channel number = 0 
 Carrier frequency = 867.999939 
 Modulated = true 
 Modulation format = 2-GFSK 
 Manchester enable = false 
 Sync word qualifier mode = 30/32 sync word bits detected 
 Preamble count = 4 
 Channel spacing = 199.951172 
 Carrier frequency = 867.999939 
 Data rate = 38.3835 
 RX filter BW = 101.562500 
 Data format = Normal mode 
 Length config = Variable packet length mode. Packet length configured by the first byte after sync word 
 CRC enable = true 
 Packet length = 255 
 Device address = 0 
 Address config = No address check 
 CRC autoflush = false 
 PA ramping = false 
 TX power = 0
*/
#define SMARTRF_SETTING_FSCTRL1     0x08    // Frequency Synthesizer Control
#define SMARTRF_SETTING_IOCFG0      0x06    // GDO0 Output Configuration
#define SMARTRF_SETTING_FSCTRL0     0x00    // Frequency Synthesizer Control
#define SMARTRF_SETTING_MDMCFG4     0xCA    // Modem Configuration
#define SMARTRF_SETTING_MDMCFG3     0x83    // Modem Configuration
#define SMARTRF_SETTING_MDMCFG2     0x93    // Modem Configuration
#define SMARTRF_SETTING_MDMCFG1     0x22    // Modem Configuration
#define SMARTRF_SETTING_MDMCFG0     0xF8    // Modem Configuration
#define SMARTRF_SETTING_CHANNR      0x00    // Channel Number
#define SMARTRF_SETTING_DEVIATN     0x35    // Modem Deviation Setting
#define SMARTRF_SETTING_FREND1      0x56    // Front End RX Configuration
#define SMARTRF_SETTING_FREND0      0x10    // Front End TX Configuration
#define SMARTRF_SETTING_MCSM0       0x10    // Main Radio Control State Machine Configuration
#define SMARTRF_SETTING_FOCCFG      0x16    // Frequency Offset Compensation Configuration
#define SMARTRF_SETTING_BSCFG       0x6C    // Bit Synchronization Configuration
#define SMARTRF_SETTING_AGCCTRL2    0x43    // AGC Control
#define SMARTRF_SETTING_AGCCTRL1    0x40    // AGC Control
#define SMARTRF_SETTING_AGCCTRL0    0x91    // AGC Control
#define SMARTRF_SETTING_FSCAL3      0xE9    // Frequency Synthesizer Calibration
#define SMARTRF_SETTING_FSCAL2      0x2A    // Frequency Synthesizer Calibration
#define SMARTRF_SETTING_FSCAL1      0x00    // Frequency Synthesizer Calibration
#define SMARTRF_SETTING_FSCAL0      0x1F    // Frequency Synthesizer Calibration
#define SMARTRF_SETTING_FSTEST      0x59    // Frequency Synthesizer Calibration Control
#define SMARTRF_SETTING_TEST2       0x81    // Various Test Settings
#define SMARTRF_SETTING_TEST1       0x35    // Various Test Settings
#define SMARTRF_SETTING_TEST0       0x09    // Various Test Settings
#define SMARTRF_SETTING_FIFOTHR     0x47    // RX FIFO and TX FIFO Thresholds
#define SMARTRF_SETTING_IOCFG2      0x29    // GDO2 Output Configuration
#define SMARTRF_SETTING_PKTCTRL1    0x04    // Packet Automation Control
#define SMARTRF_SETTING_PKTCTRL0    0x05    // Packet Automation Control
#define SMARTRF_SETTING_ADDR        0x01    // Device Address
#define SMARTRF_SETTING_PKTLEN      0xFF    // Packet Length

#endif // SMARTRF_CC430_H
