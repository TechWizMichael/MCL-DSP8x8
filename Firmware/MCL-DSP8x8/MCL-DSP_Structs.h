#include <Arduino.h>
#include <Preferences.h>
#include "MCL-DSP_ADAU1452.h"

#define STRUCT_VERSION 1

const uint16_t mix_addr[9] = {
    MOD_MIXER_ALG0_AUDIOSIGNALROUTER34S300ALG1CROSSGAIN00_ADDR,
    MOD_MIXER_ALG0_AUDIOSIGNALROUTER34S300ALG1CROSSGAIN01_ADDR,
    MOD_MIXER_ALG0_AUDIOSIGNALROUTER34S300ALG1CROSSGAIN02_ADDR,
    MOD_MIXER_ALG0_AUDIOSIGNALROUTER34S300ALG1CROSSGAIN03_ADDR,
    MOD_MIXER_ALG0_AUDIOSIGNALROUTER34S300ALG1CROSSGAIN04_ADDR,
    MOD_MIXER_ALG0_AUDIOSIGNALROUTER34S300ALG1CROSSGAIN05_ADDR,
    MOD_MIXER_ALG0_AUDIOSIGNALROUTER34S300ALG1CROSSGAIN06_ADDR,
    MOD_MIXER_ALG0_AUDIOSIGNALROUTER34S300ALG1CROSSGAIN07_ADDR,
    MOD_MIXER_ALG0_AUDIOSIGNALROUTER34S300ALG1CROSSGAIN08_ADDR
};

const uint16_t general_eq_addr[8][10] = {
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE0_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE1_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE2_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE3_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE4_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE5_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE6_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE7_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE8_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE9_B2_ADDR},
    {},
    {},
    {},
    {},
    {},
    {},
    {}
};

typedef struct {
    uint8_t type; // 0 = All-Pass, 1 = Low-Pass, 2 = High-Pass, 3 = PEQ, 4 = Low-Shelf, 5 - High-Shelf
    float boost; // dB
    float Q; // Unitless
    float frequency; // Hz
    bool en; // determines if filter is enabled
} peq_t;

typedef struct {
    float gain;
    float ref_voltage;
    float peak_thresh;
    float rms_thresh;
} gain_t;

typedef struct {
    peq_t eq[10];
    peq_t hipass[2];
    peq_t lopass[2];
    uint16_t delay;
    float FIR[512];
    gain_t gain;
} channel_t;

typedef struct {
    float gain[9][8]; // gain of each input to each output as [input][output] with input 9 being the tone generator.
    float output_gain[8];
} router_t;

typedef struct {
    float gen_freq;
    bool gen_switch;
    float heartbeat_period;
    bool heartbeat_switch;
} admin_settings_t;

typedef struct {
    int version; // largely used to check if valid data is stored in the non-volatile memory
    channel_t ch[8]; // eight channels
    router_t mixer; // one mixer
    admin_settings_t misc_settings; // non-audio settings
} dsp_settings_t;

dsp_settings_t dsp_settings;

// Initialize all settings so that the device acts as pass-through
void init_param_struct(void) {

    dsp_settings.version = STRUCT_VERSION;

    // Each channel
    for (int i = 0; i < 8; i++) {
        
        // Each general eq filter
        for (int j = 0; j < 10; j++){
            dsp_settings.ch[i].eq[i].type = 5;
            dsp_settings.ch[i].eq[i].boost = 0;
            dsp_settings.ch[i].eq[i].Q = 0.707;
            dsp_settings.ch[i].eq[i].frequency = 1000;
            dsp_settings.ch[i].eq[i].en = 0;
        }
        
        // Each Low-Pass filter
        for (int j = 0; j < 2; j++){
            dsp_settings.ch[i].lopass[i].en = 0;
            dsp_settings.ch[i].lopass[i].type = 1;
            dsp_settings.ch[i].lopass[i].boost = 0;
            dsp_settings.ch[i].lopass[i].Q = 0.707;
            dsp_settings.ch[i].lopass[i].frequency = 1000;
        }

        // Each High-Pass filter.
        for (int j = 0; j < 2; j++){
            dsp_settings.ch[i].hipass[i].en = 0;
            dsp_settings.ch[i].hipass[i].type = 2;
            dsp_settings.ch[i].hipass[i].boost = 0;
            dsp_settings.ch[i].hipass[i].Q = 0.707;
            dsp_settings.ch[i].hipass[i].frequency = 1000;
        }

        // Delay samples - minimum is 1.
        dsp_settings.ch[i].delay = 1;

        // FIR taps, first tap is 1, rest are zero
        dsp_settings.ch[i].FIR[0] = 1.0;
        for(int j = 1; j < 512; j++) {
            dsp_settings.ch[i].FIR[j] = 0.0;
        }

        // All gains are 0dB
        dsp_settings.ch[i].gain.gain = 0.0;
        dsp_settings.ch[i].gain.peak_thresh = 0.0;
        dsp_settings.ch[i].gain.rms_thresh = 0.0;
        dsp_settings.ch[i].gain.ref_voltage = 1.0;
    }

    // Setting all gains to -inf dB
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 8; j++) {
            dsp_settings.mixer.gain[i][j] = 0;
        }
    }

    // Setting mixer so that every input channel is mapped to each output channel with gain of 0dB.
    for (int i = 0; i < 8; i++) {
        dsp_settings.mixer.gain[i][i] = 1.0;
    }

    // Setting miscellaneous settings
    dsp_settings.misc_settings.gen_freq = 1000.0;
    dsp_settings.misc_settings.gen_switch = 0;
    dsp_settings.misc_settings.heartbeat_period = 1.0;
    dsp_settings.misc_settings.heartbeat_switch = 0.0;
}

// Enable or disable filter
// fitl_num 0 through 9 are general parametric EQ
// filt_num 10 and 11 are dedicated low-pass
// filt_num 12 and 13  are dedicated high-pass
int toggle_eq(int channel, int filt_num, bool en) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) | (filt_num > 13)) {
        Serial.println("Invalid filter number");
        return -2;
    }

    if(filt_num > 11) {
        dsp_settings.ch[channel].lopass[filt_num-11].en = en;
        return 0;
    } else if(filt_num > 9) {
        dsp_settings.ch[channel].hipass[filt_num-9].en = en;
        return 0;
    }
    
    dsp_settings.ch[channel].eq[filt_num].en = en;
    return 0;
}

// set type of EQ filter
// type: 0 = All-Pass, 1 = Low-Pass, 2 = High-Pass, 3 = PEQ, 4 = Low-Shelf, 5 - High-Shelf
int set_eq_type(int channel, int filt_num, int type) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) | (filt_num > 9)) {
        Serial.println("Invalid filter number");
        return -2;
    }

    dsp_settings.ch[channel].eq[filt_num].type = type;
    return 0;
}

// set frequency for EQ filter
// fitl_num 0 through 9 are general parametric EQ
// filt_num 10 and 11 are dedicated low-pass
// filt_num 12 and 13  are dedicated high-pass
int set_eq_freq(int channel, int filt_num, float freq) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) | (filt_num > 13)) {
        Serial.println("Invalid filter number");
        return -2;
    }

    if(filt_num > 11) {
        dsp_settings.ch[channel].lopass[filt_num-11].frequency = freq;
        return 0;
    } else if(filt_num > 9) {
        dsp_settings.ch[channel].hipass[filt_num-9].frequency = freq;
        return 0;
    }
    
    dsp_settings.ch[channel].eq[filt_num].frequency = freq;
    return 0;
}

// set q for EQ filter
int set_eq_q(int channel, int filt_num, float Q) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) | (filt_num > 13)) {
        Serial.println("Invalid filter number");
        return -2;
    }

    if(filt_num > 11) {
        dsp_settings.ch[channel].lopass[filt_num-11].Q = Q;
        return 0;
    } else if(filt_num > 9) {
        dsp_settings.ch[channel].hipass[filt_num-9].Q = Q;
        return 0;
    }
    
    dsp_settings.ch[channel].eq[filt_num].Q = Q;
    return 0;
}

// set boost for EQ filter
int set_eq_boost(int channel, int filt_num, float boost) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) | (filt_num > 13)) {
        Serial.println("Invalid filter number");
        return -2;
    }

    if(filt_num > 11) {
        dsp_settings.ch[channel].lopass[filt_num-11].boost = boost;
        return 0;
    } else if(filt_num > 9) {
        dsp_settings.ch[channel].hipass[filt_num-9].boost = boost;
        return 0;
    }    

    dsp_settings.ch[channel].eq[filt_num].boost = boost;
    return 0;
}

// Set coefficients for FIR filters.
int set_fir_coef(int channel, int tap, float coef) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((coef < 0) | (coef > 512)) {
        Serial.println("Invalid coef number");
        return -2;
    }
    
    dsp_settings.ch[channel].FIR[tap] = coef;
    return 0;
}

int set_fir_coef_array(int channel, float* taps) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    int num_taps = sizeof(taps)/sizeof(taps[0]);

    if (num_taps > 512) {
        Serial.println("Too many coefficients in FIR coefficient array.");
        return -2;
    }

    for (int i = 0; i < num_taps; i++) {
        dsp_settings.ch[channel].FIR[i] = taps[i];
    }

    return 0;
}

int set_channel_gain(int channel, float gain) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    dsp_settings.ch[channel].gain.gain = gain;

    return 0;
}

// Set reference voltage for more human-readable limiter values
int set_channel_ref_voltage(int channel, float ref) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    dsp_settings.ch[channel].gain.ref_voltage = ref;

    return 0;
}

// Set threshold of peak limiter in dBFS
int set_channel_pk_thresh_db(int channel, float thresh) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    dsp_settings.ch[channel].gain.peak_thresh = thresh;

    return 0;
}

// Set threshold of RMS limiter in dBFS
int set_channel_rms_thresh_db(int channel, float thresh) {
    if ((channel < 0) | (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    dsp_settings.ch[channel].gain.rms_thresh = thresh;

    return 0;
}