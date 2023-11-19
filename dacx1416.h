#ifndef _DACX1416_H
#define _DACX1416_H

// #include <cstdint>
#include <Arduino.h>
#include <SPI.h>

// Registers
#define  R_NOP        0x00
#define  R_DEVICEID   0x01
#define  R_STATUS     0x02
#define  R_SPICONFIG  0x03
#define  R_GENCONFIG  0x04
#define  R_BRDCONFIG  0x05
#define  R_SYNCCONFIG 0x06
#define  R_TOGGCONFIG 0x07 // 0x07 to 0x08
#define  R_DACPWDWN   0x09
#define  R_DACRANGE   0x0A // 0x0A to 0x0D2
#define  R_TRIGGER    0x0E
#define  R_BRDCAST    0x0F
#define  R_DAC        0x10 // 0x10 to 0x1F
#define  R_OFFSET     0x20 // 0x20 to 0x23

// SPICONFIG masks
#define TEMPALM_EN(x) (x << 11)
#define DACBUSY_EN(x) (x << 10)
#define CRCALM_EN(x)  (x << 9)
#define SOFTTOGGLE_EN(x)  (x << 6)
#define DEV_PWDWN(x)  (x << 5)
#define CRC_EN(x)     (x << 4)
#define SDO_EN(x)     (x << 2)
#define FSDO(x)       (x << 1)

// read mask
#define RREG 0xC0

class DACX1416 {
    private:
        SPIClass *_spi;
        SPISettings _spi_settings;
        
        // pins
        int _cs_pin;
        int _rst_pin;
        int _ldac_pin;

        inline void cs_on();
        inline void cs_off();

        // DACRANGE is a write only register
        // Have to keep track of individual channels manually
        uint16_t dacrange_reg[4] = {0,0,0,0};
    
    public:
        // some enums
        enum ChannelRange {
                U_5  = 0b0000, // 0 to +5V
                U_10 = 0b0001, // 0 to +10V
                U_20 = 0b0010, // 0 to +20V
                U_40 = 0b0100, // 0 to +40V
                B_2V5 = 0b1110,// -2.5 to +2.5V
                B_5  = 0b1001, // -5 to +5V
                B_10 = 0b1010, // -10 to +10V
                B_20 = 0b1100};// -20 to +20V

        enum SyncMode {SYNC_SS=0, SYNC_LDAC};

        // constructor
        DACX1416(int cspin, int rstpin = -1, int ldacpin = -1,
                 SPIClass *spi = &SPI, uint32_t spi_clock_hz=8000000);

        bool init();

        void write_reg(uint8_t reg, uint16_t wdata);
        uint16_t read_reg(uint8_t reg);

        // Hard-Reset or soft-reset
        void reset();
        
        // DAC power up/down
        // TODO: set/clear DAC_PWDWN bit in SPICONFIG reg

        // DAC channel powerdown
        void set_ch_enabled(int ch, bool state); // true/false = power ON/OFF
        bool get_ch_enabled(int ch);

        // Reference
        void set_int_reference(bool state);
        bool get_int_reference();
        
        // Status

        // set/get range of a channel
        void set_range(int ch, ChannelRange range);
        int get_range(int ch);

        // write to a ch
        void set_out(int ch, uint16_t val);

        // sync 
        void set_sync(int ch, SyncMode mode);
};

#endif
