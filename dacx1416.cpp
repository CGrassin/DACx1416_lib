#include "dacx1416.h"

// constructor 
DACX1416::DACX1416(int cspin, int rstpin, int ldacpin, SPIClass *spi, uint32_t spi_clock_hz) {
    _cs_pin = cspin;
    _spi = spi;

    if(rstpin != -1) {
        _rst_pin = rstpin;
        pinMode(_rst_pin, OUTPUT);
        digitalWrite(_rst_pin, HIGH);
    } 
    
    // LDAC operation is not implemented in the lib
    if(ldacpin != -1) _ldac_pin = ldacpin;
    
    _spi_settings = SPISettings(spi_clock_hz, MSBFIRST, SPI_MODE1);

    pinMode(_cs_pin, OUTPUT);

    _spi->begin();
}

inline void DACX1416::cs_on() { 
    digitalWrite(_cs_pin, LOW); 
}

inline void DACX1416::cs_off() { 
    digitalWrite(_cs_pin, HIGH); 
}

void DACX1416::reset() { 
    if(_rst_pin!=-1) { // HW reset
        digitalWrite(_rst_pin, LOW);
        delay(1); 
        digitalWrite(_rst_pin, HIGH); 
        delay(100);
    } else { // SW reset
        write_reg(R_TRIGGER, 0x000A);
        delay(100);
    }
}

bool DACX1416::init() {
    reset();

    // set SPICONFIG: DEV_PWDN=0
    uint16_t def = TEMPALM_EN(1) | DACBUSY_EN(0) | CRCALM_EN(1) | (1 << 7) | SOFTTOGGLE_EN(0) | DEV_PWDWN(0) | CRC_EN(0) | SDO_EN(1) | FSDO(0);
    write_reg(R_SPICONFIG, def);
    
    // reading SPICONFIG back
    return (read_reg(R_SPICONFIG) != def);
}

void DACX1416::write_reg(uint8_t reg, uint16_t wdata) {
    uint8_t lsb = ((uint16_t)wdata >> 0) & 0xFF;
    uint8_t msb = ((uint16_t)wdata >> 8) & 0xFF;

    _spi->beginTransaction(_spi_settings);
    cs_on();
    _spi->transfer(reg);
    _spi->transfer(msb);
    _spi->transfer(lsb);
    cs_off();
    _spi->endTransaction();
}

uint16_t DACX1416::read_reg(uint8_t reg) {
    uint8_t buf[3]; // 15-0

    _spi->beginTransaction(_spi_settings);
    cs_on();
    _spi->transfer( (RREG | reg) );
    _spi->transfer(0x00);
    _spi->transfer(0x00);
    cs_off();

    cs_on();
    buf[0] = _spi->transfer(0x00);
    buf[1] = _spi->transfer(0x00);
    buf[2] = _spi->transfer(0x00);
    cs_off();
    _spi->endTransaction();

    // TODO check buf[0]

    return ((buf[1] << 8) | buf[2]);
}

//******* Enable/Disable (Power up/down) a channel *******//
void DACX1416::set_ch_enabled(int ch, bool state) { // true/false = power ON/OFF
    uint16_t res = read_reg(R_DACPWDWN);
    
    // if state==true, power up the channel
    if(state) write_reg(R_DACPWDWN, res &= ~(1 << ch) );
    else write_reg(R_DACPWDWN, res |= (1 << ch) );
}

bool DACX1416::get_ch_enabled(int ch) {
    uint16_t res = read_reg(R_DACPWDWN);
    return !(bool(((res >> ch) & 1)));
}

//************** Set/Get internal reference **************//
void DACX1416::set_int_reference(bool state) {
    if(state) write_reg(R_GENCONFIG, (0 << 14) ); // Turn on
    else write_reg(R_GENCONFIG, (1 << 14) ); // Shutdown
}

// 0 => ref off, 1 => ref on
bool DACX1416::get_int_reference() {
    return !(read_reg(R_GENCONFIG) & 0x4000);
}

//**************** Set Range of a channel ***************//
void DACX1416::set_range(int ch, ChannelRange range) {
    uint8_t offset = 3 - ch/4;

    uint16_t mask = (0xffff >> (16-4)) << 4*(ch%4);
    uint16_t write = (dacrange_reg[offset] & ~mask) | (( range << 4*(ch%4) )&mask);
    dacrange_reg[offset] = write;

    write_reg(R_DACRANGE + offset, write );
}

int DACX1416::get_range(int ch) {
    return (dacrange_reg[3 - ch/4] >> 4*(ch%4)) & ((1UL << 4)-1);
}

//**************** Write value to a channel ***************//
void DACX1416::set_out(int ch, uint16_t val) {
    write_reg(R_DAC+ch, val);
}

//************* Set/get sync mode of a channel ************//
void DACX1416::set_sync(int ch, SyncMode mode) {
    uint16_t read = read_reg(R_SYNCCONFIG);
    //Serial.print("sync read -> "); Serial.println(read, HEX);
    if(mode==SYNC_LDAC) read |= 1UL << ch;
    else read &= ~(1UL << ch);
    write_reg(R_SYNCCONFIG, read);
}
