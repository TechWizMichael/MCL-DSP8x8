#ifndef AK4619VN
#define AK4619VN

#include <Arduino.h>
#include <Wire.h>

// Device Addresses
#define AK4619VN_1_ADDRESS 0x10
#define AK4619VN_2_ADDRESS 0x11

// I2C Pins
#define I2C_SDA 4
#define I2C_SCL 5

//Register Locations
#define PWRMGM 0x00     // {0, 0, PMAD2, PMAD1, 0, PMDA2, PMDA1, RSTN}
#define AUDFORM1 0x01   // {TDM, DCF[2], DCF[1], DCF[0], DSL[1], DSL[0], BCKP, SDOPH}
#define AUDFORM2 0x02   // {0, 0, 0, SLOT, DIDL[1], DIDL[0], DODL[1], DODL[0]}
#define SYSCLKSET 0x03  // {0, 0, 0, 0, 0, FS[2], FS[1], FS[0]}
#define MICGAIN1 0x04   // {MGN1L[3], MGN1L[2], MGN1L[1], MGN1L[0], MGN1R[3], MGN1R[2], MGN1R[1], MGN1R[0]}
#define MICGAIN2 0x05   // {MGN2L[3], MGN2L[2], MGN2L[1], MGN2L[0], MGN2R[3], MGN2R[2], MGN2R[1], MGN2R[0]}
#define ADC1LVOL 0x06   // {VOLAD1L[7], VOLAD1L[6], VOLAD1L[5], VOLAD1L[4], VOLAD1L[3], VOLAD1L[2], VOLAD1L[1], VOLAD1L[0]}
#define ADC1RVOL 0x07   // {VOLAD1R[7], VOLAD1R[6], VOLAD1R[5], VOLAD1R[4], VOLAD1R[3], VOLAD1R[2], VOLAD1R[1], VOLAD1R[0]}
#define ADC2LVOL 0x08   // {VOLAD2L[7], VOLAD2L[6], VOLAD2L[5], VOLAD2L[4], VOLAD2L[3], VOLAD2L[2], VOLAD2L[1], VOLAD2L[0]}
#define ADC2RVOL 0x09   // {VOLAD2R[7], VOLAD2R[6], VOLAD2R[5], VOLAD2R[4], VOLAD2R[3], VOLAD2R[2], VOLAD2R[1], VOLAD2R[0]}
#define ADCFILT 0x0A    // {0, AD2VO, AD2SD, AD2SL, 0, AD1VO, AD1SD, AD1SL}
#define ADCAIN 0x0B     // {ADC1LSEL[1], ADC1LSEL[0], ADC1RSEL[1], ADC1RSEL[0], ADC2LSEL[1], ADC2LSEL[0], ADC2RSEL[1], ADC2RSEL[0]}
#define ADCMUTEHPF 0x0D // {ATSPAD, AD2MUTE, AD1MUTE, 0, 0, AD2HPF, AD1HPF, 0}
#define DAC1LVOL 0x0E   // {VOLDA1L[7], VOLDA1L[6], VOLDA1L[5], VOLDA1L[4], VOLDA1L[3], VOLDA1L[2], VOLDA1L[1], VOLDA1L[0]}
#define DAC1RVOL 0x0F   // {VOLDA1R[7], VOLDA1R[6], VOLDA1R[5], VOLDA1R[4], VOLDA1R[3], VOLDA1R[2], VOLDA1R[1], VOLDA1R[0]}
#define DAC2LVOL 0x10   // {VOLDA12L[7], VOLDA2L[6], VOLDA2L[5], VOLDA2L[4], VOLDA2L[3], VOLDA2L[2], VOLDA2L[1], VOLDA2L[0]}
#define DAC2RVOL 0x11   // {VOLDA2R[7], VOLDA2R[6], VOLDA2R[5], VOLDA2R[4], VOLDA2R[3], VOLDA2R[2], VOLDA2R[1], VOLDA2R[0]}
#define DACDIN 0x12     // {0, 0, 0, 0,  DAC2SEL[1], DAC2SEL[0], DAC1SEL[1], DAC1SEL[0]}
#define DACDEEM 0x13    // {0, 0, 0, 0, DEM2[1], DEM2[0], DEM1[1], DEM1[0]}
#define DACMUTFLT 0x14  // {ATSPDA, 0, DA2MUTE, DA1MUTE, DA2SD, DA2SL, DA1SD, DA1SL}

// Write value to register on AK4619VN
uint8_t write_AK4619VN(uint8_t device_address, byte register_address, byte register_value) {
    Wire.beginTransmission(device_address);
    Wire.write(register_address);
    Wire.write(register_value);
    
    uint8_t transmission_result = Wire.endTransmission(true);
    
    String message = "transmitting 0x" + String(int(register_value), HEX) + " to register 0x" + String(int(register_address), HEX);
    Serial.println(message);

    Serial.println("0x" + String(int(transmission_result), HEX));

    return transmission_result;
}

// Read current value on specific AK4619VN register
uint8_t read_AK4619VN(uint8_t device_address, byte register_address, uint8_t * register_value) {
    Wire.beginTransmission(device_address);
    Wire.write(register_address);
    Wire.endTransmission(true);
    
    uint8_t numbytes = 0;
    numbytes = Wire.requestFrom(device_address, (uint8_t)1, (uint8_t)false);
    
    if((bool)numbytes){
        Wire.readBytes(register_value, numbytes);
    }

    uint8_t transmission_result = Wire.endTransmission(true); //Send STOP

    Serial.println("0x" + String(int(transmission_result), HEX));

    return transmission_result;
}

// unmute ADC output of the AK4619VN
void unmute_AK4619VN() {
    byte temp_data = 0x00;

    byte temp_address = DACMUTFLT;
    read_AK4619VN(AK4619VN_1_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data & 0xCF;
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    read_AK4619VN(AK4619VN_2_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data & 0xCF;
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);
}

// mute the AK4619VN ADC output
void mute_AK4619VN() {
    byte temp_data = 0x00;

    byte temp_address = DACMUTFLT;
    read_AK4619VN(AK4619VN_1_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data | 0x30;
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    read_AK4619VN(AK4619VN_2_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data | 0x30;
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);
}

// Initialize AK4619VN over I2C
// NOTE: many settings maintain their default values, but having them here already makes changes easier
void init_AK4619VN() {
    Wire.begin(I2C_SDA, I2C_SCL); // SDA, SCL
    Wire.setClock(400000); // 400 kHz (Fast Mode)
    byte temp_data = 0x00;
    byte temp_address = 0x00;

    // Reset both AK4619VN
    // Clearing last bit (RSTN)

    temp_address = PWRMGM; // {0, 0, PMAD2, PMAD1, 0, PMDA2, PMDA1, RSTN}
    read_AK4619VN(AK4619VN_1_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data & 0xFE;
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    read_AK4619VN(AK4619VN_2_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data & 0xFE;
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);
    
    // ADC/DAC Power
    // Set bits for ADC and DAC power (PMAD2, PMAD1, PMDA2, PMDA1)
    // HIGH (1) means enabled

    temp_address = PWRMGM; // {0, 0, PMAD2, PMAD1, 0, PMDA2, PMDA1, RSTN}
    temp_data = 0x36; // ADC and DAC on
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    mute_AK4619VN();

    // Audio Format Registers
    // TDM128 with four channels, 32-bit slot length, 24-bit data in and out, negative polatity on BICK, slow SDOUT.
    // Note: MSB first, left justified, delayed by 1 BICK period.

    temp_address = AUDFORM1; // {TDM, DCF[2], DCF[1], DCF[0], DSL[1], DSL[0], BCKP, SDOPH}
    temp_data = 0xAC; // TDM, I2S Compatible, 32 bit slot, falling edge of BICK, SDOUT slow mode
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = AUDFORM2; // {0, 0, 0, SLOT, DIDL[1], DIDL[0], DODL[1], DODL[0]}
    temp_data = 0x10; // Data start on LRCLK edge, 24 bit date in, 24-bit data out
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Clock Speed

    temp_address = SYSCLKSET; // {0, 0, 0, 0, 0, FS[2], FS[1], FS[0]}
    temp_data = 0x00; // MCLK = 256*Fs, BICK <= 48kHz
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);
    
    // Analog Input Gain
    // labeled as microphone gain in datasheet.
    // each register holds two gain values (4 bits each).
    // adjusted in 3dB steps.
    //      0x0 = +6dB
    //      0x2 = 0dB
    //      0xF = -24dB

    temp_address = MICGAIN1; // {MGN1L[3], MGN1L[2], MGN1L[1], MGN1L[0], MGN1R[3], MGN1R[2], MGN1R[1], MGN1R[0]}
    temp_data = 0x22; // 0dB gain on both channels
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = MICGAIN2; // {MGN2L[3], MGN2L[2], MGN2L[1], MGN2L[0], MGN2R[3], MGN2R[2], MGN2R[1], MGN2R[0]}
    temp_data = 0x22; // 0dB gain on both channels
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Digital Input Gain
    // Each Register holds one gain value (8 bits each).
    // adjust in 0.5dB steps.
    //      0x00 = +24.0dB
    //      0x30 = 0.0dB
    //      0xFE = -103.0dB
    //      0xFF = MUTE

    temp_address = ADC1LVOL; // {VOLAD1L[7], VOLAD1L[6], VOLAD1L[5], VOLADL1[4], VOLADL1[3], VOLADL1[2], VOLADL1[1], VOLADL1[0]}
    temp_data = 0x30; // 0dB digital gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = ADC1RVOL; // {VOLAD1R[7], VOLAD1R[6], VOLAD1R[5], VOLAD1R[4], VOLAD1R[3], VOLAD1R[2], VOLAD1R[1], VOLAD1R[0]}
    temp_data = 0x30; // 0dB digital gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = ADC2LVOL; // {VOLAD2L[7], VOLAD2L[6], VOLAD2L[5], VOLAD2L[4], VOLAD2L[3], VOLAD2L[2], VOLAD2L[1], VOLAD2L[0]}
    temp_data = 0x30; // 0dB digital gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = ADC2RVOL; // {VOLAD2R[7], VOLAD2R[6], VOLAD2R[5], VOLAD2R[4], VOLAD2R[3], VOLAD2R[2], VOLAD2R[1], VOLAD2R[0]}
    temp_data = 0x30; // 0dB digital gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set ADC input filter
    // ADxVO: Enable "voice" filter (if set, other bits are ignored)
    // ADxSD: Enable short delay (possibly minimum-phase vs linear-phase?)
    // ADxSL: Enable slow roll-off

    temp_address = ADCFILT; // {0, AD2VO, AD2SD, AD2SL, 0, AD1VO, AD1SD, AD1SL}
    temp_data = 0x00; // sharp roll-off on both channels
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set ADC input mode

    temp_address = ADCAIN; // {ADC1LSEL[1], ADC1LSEL[0], ADC1RSEL[1], ADC1RSEL[0], ADC2LSEL[1], ADC2LSEL[0], ADC2RSEL[1], ADC2RSEL[0]}
    temp_data = 0x55; // differential on all ADC inputs
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set Mute and HPF on ADC

    temp_address = ADCMUTEHPF; // {ATSPAD, AD2MUTE, AD1MUTE, 0, 0, AD2HPF, AD1HPF, 0}
    temp_data = 0x86; // Slow transition, unmute both ADC, enable HPF on both ADC
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set DAC digital gain

    temp_address = DAC1LVOL; // {VOLDA1L[7], VOLDA1L[6], VOLDA1L[5], VOLDA1L[4], VOLDA1L[3], VOLDA1L[2], VOLDA1L[1], VOLDA1L[0]}
    temp_data = 0x18; // 0dB gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = DAC1RVOL; // {VOLDA1R[7], VOLDA1R[6], VOLDA1R[5], VOLDA1R[4], VOLDA1R[3], VOLDA1R[2], VOLDA1R[1], VOLDA1R[0]}
    temp_data = 0x18; // 0dB gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = DAC2LVOL; // {VOLDA12L[7], VOLDA2L[6], VOLDA2L[5], VOLDA2L[4], VOLDA2L[3], VOLDA2L[2], VOLDA2L[1], VOLDA2L[0]}
    temp_data = 0x18; // 0dB gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = DAC2RVOL; // {VOLDA2R[7], VOLDA2R[6], VOLDA2R[5], VOLDA2R[4], VOLDA2R[3], VOLDA2R[2], VOLDA2R[1], VOLDA2R[0]}
    temp_data = 0x18; // 0dB gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set DAC input source

    temp_address = DACDIN; // {0, 0, 0, 0,  DAC2SEL[1], DAC2SEL[0], DAC1SEL[1], DAC1SEL[0]}
    // temp_data = 0x05; // SDIN2 on both outputs
    // temp_data = 0x00; // SDIN1 on both outputs
    temp_data = 0x04; // SDIN1 for ADC1, SDIN2 for ADC2
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // DAC De-Emphasis

    temp_address = DACDEEM; // {0, 0, 0, 0, DEM2[1], DEM2[0], DEM1[1], DEM1[0]}
    temp_data = 0x05; // Disable de-emphasis
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);
    

    // DAC Mute and Filter

    temp_address = DACMUTFLT; // {ATSPDA, 0, DA2MUTE, DA1MUTE, DA2SD, DA2SL, DA1SD, DA1SL}
    temp_data = 0x80; // Slow transition. Mute off. Filters with sharp roll off, long delay.
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set RSTN to re-enable devices

    temp_address = PWRMGM; // {0, 0, PMAD2, PMAD1, 0, PMDA2, PMDA1, RSTN}
    read_AK4619VN(AK4619VN_1_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data | 0x01;
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    read_AK4619VN(AK4619VN_2_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data | 0x01;
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    unmute_AK4619VN();

    return;
}

#endif