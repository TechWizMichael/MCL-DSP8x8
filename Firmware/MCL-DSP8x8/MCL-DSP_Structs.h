#include <Arduino.h>
#include "MCL-DSP_ADAU1452.h"
#include <Preferences.h>

// version of struct that will be stored in non-volatile memory
// TODO: increment every time the structs change (once written to memory)
#define STRUCT_VERSION 1

// struct containing parameters for PEQ blocks
// used for general PEQ and IIR crossover filters.
typedef struct {
    uint8_t type; // 0 = All-Pass, 1 = Low-Pass, 2 = High-Pass, 3 = PEQ, 4 = Low-Shelf, 5 - High-Shelf
    float boost; // dB
    float Q; // Unitless
    float frequency; // Hz
    bool en; // determines if filter is enabled
} peq_t;

typedef struct {
    float gain; // Channel gain in dB
    float ref_voltage; // human-readable voltage reference for 0dBFS for limiters in volta or watts
    float peak_thresh; // Peak Limiter threshold in dBFS
    float rms_thresh; // RMS Limiter threshold in dBFS
} gain_t;

typedef struct {
    peq_t eq[10]; // 10 biquads
    peq_t hipass[4]; // up to 8th-order high-pass
    peq_t lopass[4]; // up to 8th-order low-pass
    uint16_t delay; // 1-2400 samples of delay (up to 50ms)
    float FIR[1024]; // taps of FIR filter
    gain_t gain; // dB gain
    bool mute; // 0 when muted, 1 when unmuted
    bool inv; // 0 if normal polarity, 1 if inverted
} channel_t;

typedef struct {
    float gain[9][8]; // gain of each input to each output as [input][output] with input 9 being the tone generator.
    float output_gain[8]; // optional gain to each channel
} router_t;

typedef struct {
    float gen_freq; // Frequency of the tone generator
    bool gen_switch; // Switch between tone generator and noise source
    float heartbeat_period; // period of the heartbeat LED flashing
    bool heartbeat_switch; // Enable and disable heartbeat
} admin_settings_t;

typedef struct {
    uint8_t version; // largely used to check if valid data is stored in the non-volatile memory
    channel_t ch[8]; // eight channels
    router_t mixer; // one mixer
    admin_settings_t misc_settings; // non-audio settings
} dsp_settings_t;

dsp_settings_t dsp_settings; // decalre single struct to store settings.

Preferences prefs;

// Initialize all settings so that the device acts as pass-through
void init_param_struct(void) {

    // Setting version number
    dsp_settings.version = STRUCT_VERSION;

    // Each channel
    for (int i = 0; i < 8; i++) {

        // mute channel
        dsp_settings.ch[i].mute = 0; // mute
        
        // Each general eq filter
        for (int j = 0; j < 10; j++){
            dsp_settings.ch[i].eq[j].en = 0; // Disable
            dsp_settings.ch[i].eq[j].type = 3; // PEQ
            dsp_settings.ch[i].eq[j].boost = 0.0; // 0 dB boost
            dsp_settings.ch[i].eq[j].Q = 0.707; // Q of 1/sqrt(2)
            dsp_settings.ch[i].eq[j].frequency = 1000.0; // 1 kHz
        }
        
        // Each Low-Pass filter will be set as PEQ with 0dB gain, aka THRU
        for (int j = 0; j < 4; j++){
            dsp_settings.ch[i].lopass[j].en = 0; // Disable
            dsp_settings.ch[i].lopass[j].type = 1; // Low-Pass
            dsp_settings.ch[i].lopass[j].boost = 0.0; // 0 dB boost
            dsp_settings.ch[i].lopass[j].Q = 0.707; // Q of 1/sqrt(2)
            dsp_settings.ch[i].lopass[j].frequency = 1000.0; // 1 kHz
        }

        // Each High-Pass filter will be set as PEQ with 0dB gain, aka THRU
        for (int j = 0; j < 4; j++){
            dsp_settings.ch[i].hipass[j].en = 0; // Disable
            dsp_settings.ch[i].hipass[j].type = 2; // high-Pass
            dsp_settings.ch[i].hipass[j].boost = 0.0; // 0 dB boost
            dsp_settings.ch[i].hipass[j].Q = 0.707; // Q of 1/sqrt(2)
            dsp_settings.ch[i].hipass[j].frequency = 1000.0; // 1 kHz
        }

        // Delay samples - minimum is 1.
        dsp_settings.ch[i].delay = 1; // minimum allowed delay

        // FIR taps, first tap is 1, rest are zero
        dsp_settings.ch[i].FIR[0] = 1.0;
        for(int j = 1; j < 1024; j++) {
            dsp_settings.ch[i].FIR[j] = 0.0;
        }

        // All gains are 0dB
        dsp_settings.ch[i].gain.gain = 0.0; // dB
        dsp_settings.ch[i].gain.peak_thresh = 0.0; // dB
        dsp_settings.ch[i].gain.rms_thresh = 0.0; // dB
        dsp_settings.ch[i].gain.ref_voltage = 1.0; // dB
    }

    // Setting all gains to -inf dB
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 8; j++) {
            dsp_settings.mixer.gain[i][j] = -144.0; // dB
        }
    }

    // Setting mixer so that every input channel is mapped to each output channel with gain of 0dB.
    for (int i = 0; i < 8; i++) {
        dsp_settings.mixer.gain[i][i] = 0.0; // dB
        dsp_settings.mixer.output_gain[i] = 0.0; // dB
    }

    // Setting miscellaneous settings
    dsp_settings.misc_settings.gen_freq = 1000.0;
    dsp_settings.misc_settings.gen_switch = 0;
    dsp_settings.misc_settings.heartbeat_period = 1.0;
    dsp_settings.misc_settings.heartbeat_switch = 0.0;
}

int toggle_mute(int channel, bool en) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }
    
    dsp_settings.ch[channel].mute = !en;
    return 0;
}
// Enable or disable filter
// fitl_num 0 through 9 are general parametric EQ
// filt_num 10 and 11 are dedicated low-pass
// filt_num 12 and 13  are dedicated high-pass
int toggle_eq(int channel, int filt_num, bool en) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) || (filt_num > 17)) {
        Serial.println("Invalid filter number");
        return -2;
    }

    if(filt_num > 13) {
        dsp_settings.ch[channel].lopass[filt_num-14].en = en;
        return 0;
    } else if(filt_num > 9) {
        dsp_settings.ch[channel].hipass[filt_num-10].en = en;
        return 0;
    }
    
    dsp_settings.ch[channel].eq[filt_num].en = en;
    return 0;
}

// set type of EQ filter
// type: 0 = All-Pass, 1 = Low-Pass, 2 = High-Pass, 3 = PEQ, 4 = Low-Shelf, 5 - High-Shelf
int set_eq_type(int channel, int filt_num, int type) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) || (filt_num > 9)) {
        Serial.println("Invalid filter number");
        return -2;
    }

    dsp_settings.ch[channel].eq[filt_num].type = type;
    return 0;
}

// set frequency for EQ filter
// filt_num 0 through 9 are general parametric EQ
// filt_num 10 and 11 are dedicated low-pass
// filt_num 12 and 13  are dedicated high-pass
int set_eq_freq(int channel, int filt_num, float freq) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) || (filt_num > 17)) {
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
// filt_num 0 through 9 are general parametric EQ
// filt_num 10 and 11 are dedicated low-pass
// filt_num 12 and 13  are dedicated high-pass
int set_eq_q(int channel, int filt_num, float Q) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) || (filt_num > 17)) {
        Serial.println("Invalid filter number");
        return -2;
    }

    if(filt_num > 13) {
        dsp_settings.ch[channel].lopass[filt_num-14].Q = Q;
        return 0;
    } else if(filt_num > 9) {
        dsp_settings.ch[channel].hipass[filt_num-10].Q = Q;
        return 0;
    }
    
    dsp_settings.ch[channel].eq[filt_num].Q = Q;
    return 0;
}

// set boost for EQ filter
int set_eq_boost(int channel, int filt_num, float boost) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) || (filt_num > 17)) {
        Serial.println("Invalid filter number");
        return -2;
    }

    dsp_settings.ch[channel].eq[filt_num].boost = boost;
    return 0;
}

// Set coefficients for FIR filters.
int set_fir_coef(int channel, int tap, float coef) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((coef < 0) || (coef > 1023)) {
        Serial.println("Invalid coef number");
        return -2;
    }
    
    dsp_settings.ch[channel].FIR[tap] = coef;
    return 0;
}

// Set delay in samples
int set_delay(int channel, int delay) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((delay < 1) || (delay > 2400)) {
        Serial.println("Invalid delay number");
        return -2;
    }
    
    dsp_settings.ch[channel].delay = delay;
    return 0;
}

int set_fir_coef_array(int channel, float* taps) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    int num_taps = sizeof(taps)/sizeof(taps[0]);

    if (num_taps > 1024) {
        Serial.println("Too many coefficients in FIR coefficient array.");
        return -2;
    }

    for (int i = 0; i < num_taps; i++) {
        dsp_settings.ch[channel].FIR[i] = taps[i];
    }

    return 0;
}

// Setting channel gain
int set_gain(int channel, float gain) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    dsp_settings.ch[channel].gain.gain = gain;

    return 0;
}

// Set reference voltage for more human-readable limiter values
int set_ref_voltage(int channel, float ref) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    dsp_settings.ch[channel].gain.ref_voltage = ref;

    return 0;
}

// Set threshold of peak limiter in dBFS
int set_peak_gain(int channel, float thresh) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    dsp_settings.ch[channel].gain.peak_thresh = thresh;

    return 0;
}

// Set threshold of RMS limiter in dBFS
int set_rms_gain(int channel, float thresh) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    dsp_settings.ch[channel].gain.rms_thresh = thresh;

    return 0;
}

int set_invert(int channel, bool invert) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return -1;
    }

    dsp_settings.ch[channel].inv = invert;
    return 0;
}

int toggle_HB(bool enable) {
    dsp_settings.misc_settings.heartbeat_switch = enable;
    return 0;
}

int setup_param(void) {
    init_param_struct();
    return 0;

    prefs.begin("DSP_SETTINGS"); // Capital letters to not be confuse with data key

    size_t buff_len = prefs.getBytesLength("DSP_settings");

    if (buff_len != sizeof(dsp_settings_t)) {
        init_param_struct();
        return 1;
    }

    // char buffer[buff_len];
    prefs.getBytes("dsp_settings", &dsp_settings, buff_len); // needs verification

    if (dsp_settings.version != STRUCT_VERSION) {
        init_param_struct();
        return 2;
    }

    //TODO: read settings from non-volatile memory, check if it's valid, initialize if it's not valid, then write those parameters
    return 0;
}