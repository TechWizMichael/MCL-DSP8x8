// Values and functions for using Serial/USB
// Last updated October 26, 2026

#include <Arduino.h>
#include "MCL-DSP_Structs.h"
#include "MCL-DSP_HEARTBEAT.h"
#include "MCL-DSP_Coef.h"
#include "MCL-DSP_Param_Addr.h"

#define DELIM_CHAR '/'

// Get the indext where the delimiter character shows up
int next_delimiter(String input, char delim){
    if (input.length() < 2) {
        return -2;
    }

    for (int i = 0; i < input.length(); i++) {
        if (input[i] == delim) {
            return i;
        }
    }

    return -1;
}

// Get the portion of the input string that comes before the index
String former(String input, int index){
    if (index < 0) {
        return input;
    }

    return input.substring(0, index);
}

// Get the portion of the input string that comes after the index
String latter(String input, int index) {
    if (index < 0) {
        return "";
    }

    if (index >= input.length()){
        return "";
    }

    return input.substring(index+1, input.length());
}

// Write value to address in float form
void write_param(int address, float value) {
  SIGMA_WRITE_REGISTER_FLOAT(address, value);
  return;
}

// Write value to address in integer form
void write_param_int(int address, int value) {
  SIGMA_WRITE_REGISTER_INTEGER(address, value);
  return;
}

// Write gain to cell in mixer
int write_mixer(int input, int output) {
    if ((input < 0) || (input > 8)) {
        Serial.println("Invalid mixer input.");
        return -1;
    }

    if ((output < 0) || (output > 7)) {
        Serial.println("Invalid mixer output.");
        return -1;
    }

    float gain = dsp_settings.mixer.gain[input][output];
    gain = pow(10.0, (gain/20.0));

    uint16_t address = mix_addr[input] + output;

    write_param(address, gain);
    return 0;
}

// Write EQ coefficients given channel, eq number, and settings stored in the relevant eq struct
int write_eq(int channel, int filt_num) {    
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel number");
        return -1;
    }

    if ((filt_num < 0) || (filt_num > 17)) {
        Serial.println("Invalid filter number");
        return -2;
    }

    peq_t filter;
    uint16_t coef_addr;

    if (filt_num > 13) {
        filter = dsp_settings.ch[channel].lopass[filt_num - 14];
        coef_addr = lp_eq_addr[channel][filt_num - 14];
    } else if (filt_num > 9) {
        filter = dsp_settings.ch[channel].hipass[filt_num - 10];
        coef_addr = hp_eq_addr[channel][filt_num - 10];
    } else {
        filter = dsp_settings.ch[channel].eq[filt_num];
        coef_addr = general_eq_addr[channel][filt_num];
    }

    bool en = filter.en;
    int type = filter.type;
    float freq = filter.frequency;
    float Q = filter.Q;
    float boost = filter.boost;

    if(!en) {
        type = 5;
        freq = 1000;
        Q = 0.707;
        boost = 0;
    }

    if ((type < 0) || (type > 5)) {
        Serial.println("Invalid filter type.");
        return -3;
    }

    float* coef = new float[5];

    coef[0] = 0.0;
    coef[1] = 0.0;
    coef[2] = 1.0;
    coef[3] = 0.0;
    coef[4] = 0.0;
    
    switch (type) {
        case 0: // All-Pass
            ap_coef(coef, freq, Q, boost);
            break;
        case 1: // Low-Pass
            lp_coef(coef, freq, Q, boost);
            break;
        case 2: // High-Pass
            hp_coef(coef, freq, Q, boost);
            break;
        case 3: // PEQ
            peq_coef(coef, freq, Q, boost);
            break;
        case 4: // Low-Shelf
            ls_coef(coef, freq, Q, boost);
            break;
        case 5: // High-Shelf
            hs_coef(coef, freq, Q, boost);
            break;
    }

    // Serial.println("[b2, b1, b0, a2, a1] = deal(" + String(coef[0], 10) + ", " + String(coef[1], 10) + ", " + String(coef[2], 10) + ", " + String(coef[3], 10) + ", " + String(coef[4], 10) + ");");

    for (int i = 0; i < 5; i++) {
        //write each coefficient to each address
        write_param(safeload_data_addr + i, coef[i]);
    }

    write_param_int(safeload_addr_addr, coef_addr);

    write_param_int(safeload_count_addr, 5);

    delete[] coef;

    return 0;
}

// Write coefficients to FIR filter of given channel
int write_fir(int channel, int tap) {
    if((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return -1;
    }

    if ((tap < 0) || (tap > 1023)) {
        Serial.println("Invalid tap number.");
        return -2;
    }

    float coef = dsp_settings.ch[channel].FIR[tap];
    uint16_t address = fir_addr[channel] + tap;
    write_param(address, coef);

    return 0;
}

// Write delay settings to given channel
int write_delay(int channel) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return -1;
    }

    uint16_t value = dsp_settings.ch[channel].delay;
    uint16_t address = delay_addr[channel];

    write_param_int(address, value);
    return 0;
}

// Write gain to given channel
int write_gain(int channel) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return -1;
    }

    float value = dsp_settings.ch[channel].gain.gain;
    value = pow(10.0, (value/20.0)); // convert to linear gain

    uint16_t address = gain_addr[channel];

    write_param(address, value);

    return 0;
}

// Write peak sidechain gain to given channel
int write_peak_thresh(int channel) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return -1;
    }

    float value = dsp_settings.ch[channel].gain.peak_thresh;

    // Threshold cannot go above 0dBFS
    if(value > 0.0) {
        value = 0.0;
    }

    // Converting to positive gain
    value = (-1.0*value) - 20.0;

    value = pow(10.0, (value/20.0)); // convert to linear gain

    uint16_t address = peak_gain_addr[channel];

    write_param(address, value);

    return 0;
}

// Write rms sidechain gain to given channel
int write_rms_thresh(int channel) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return -1;
    }

    float value = dsp_settings.ch[channel].gain.rms_thresh;

    // Threshold cannot go above 0dBFS
    if(value > 0.0) {
        value = 0.0;
    }

    // Converting to positive gain
    value = (-1.0*value) - 20.0;

    value = pow(10.0, (value/20.0)); // convert to linear gain

    uint16_t address = rms_gain_addr[channel];

    write_param(address, value);

    return 0;
}

// Write invert setting to given channel
int write_invert(int channel) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return -1;
    }

    bool inv = dsp_settings.ch[channel].inv;
    uint16_t address = inv_addr[channel];

    if(inv) {
        write_param_int(address, 1);
    } else {
        write_param_int(address, 0);
    }

    return 0;
}

// Write mute setting to given channel
int write_mute(int channel) {
    if ((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return -1;
    }

    bool mute = dsp_settings.ch[channel].mute;
    uint16_t address = mute_addr[channel];

    if(mute) {
        write_param(address, 0.0);
    } else {
        write_param(address, 1.0);
    }

    return 0;
}

// Toggle heartbeat settings
int toggle_heartbeat(void) {
    bool enable = dsp_settings.misc_settings.heartbeat_switch;

    uint16_t address = heartbeat_switch_addr;

    if(enable) {
        write_param_int(address, 0);
    } else {
        write_param_int(address, 1);
    }

    return 0;
}

// pase mix parameters
String parse_mix(String input) {
    int index = next_delimiter(input, DELIM_CHAR);
    String temp_string = former(input, index);
    String residual_input = latter(input, index);

    return "INACTIVE";
}

// Parse heartbeat parameters
String parse_HB(String input) {
    // Getting specific heartbeat Command
    int index = next_delimiter(input, DELIM_CHAR);
    String temp_string = former(input, index);
    String residual_input = latter(input, index);

    if (temp_string.equals("EN")){
        set_heartbeat_mode(1);
        toggle_HB(1);
        toggle_heartbeat();
        
        return "READY";
    } else if(temp_string.equals("DIS")){
        set_heartbeat_mode(0);
        toggle_HB(0);
        toggle_heartbeat();

        return "READY";
    }

    return "INVALID";
}

// Parse generator parameters
String parse_gen(String input) {
    int index = next_delimiter(input, DELIM_CHAR);
    String temp_string = former(input, index);
    String residual_input = latter(input, index);

    return "INVALID";
}

// Parse EQ parameters
String parse_eq(int channel, int eq, String input) {
    if((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return "ERROR";
    }

    if((eq < 0) || (eq > 17)) {
        Serial.println("Invalid filter number.");
        return "ERROR";
    }
    int index = next_delimiter(input, DELIM_CHAR);
    String temp_string = former(input, index);
    String residual_input = latter(input, index);

    if(temp_string.equals("TYPE")) {
        temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
        int type = temp_string.toInt();

        if (eq > 13) {
            type = 1;
        } else if(eq > 9) {
            type = 1;
        }
        
        set_eq_type(channel, eq, type);
        write_eq(channel, eq);
        return "READY";
    } else if(temp_string.equals("BOOST")) {
        temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
        double boost = temp_string.toDouble();

        set_eq_boost(channel, eq, boost);
        write_eq(channel, eq);
        return "READY";
    } else if(temp_string.equals("Q")) {
        temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
        double Q = temp_string.toDouble();

        set_eq_q(channel, eq, Q);
        write_eq(channel, eq);
        return "READY";
    } else if(temp_string.equals("FREQ")) {
        temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
        double freq = temp_string.toDouble();

        set_eq_freq(channel, eq, freq);
        write_eq(channel, eq);
        return "READY";
    } else if(temp_string.equals("EN")) {
        toggle_eq(channel, eq, 1);
        write_eq(channel, eq);

        return "READY";
    } else if(temp_string.equals("DIS")) {
        toggle_eq(channel, eq, 0);
        write_eq(channel, eq);

        return "READY";
    }

    return "INVALID";
}

// Parse FIR parameters
String parse_fir(int channel, String input){
    if((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return "ERROR";
    }

    int index = next_delimiter(input, DELIM_CHAR);
    String temp_string = former(input, index);
    String residual_input = latter(input, index);

    return "INVALID";
}


String parse_channel(int channel, String input){
    if((channel < 0) || (channel > 7)) {
        Serial.println("Invalid channel.");
        return "ERROR";
    }

    int index = next_delimiter(input, DELIM_CHAR);
    String temp_string = former(input, index);
    String residual_input = latter(input, index);
    
    if(temp_string.equals("GAIN")) {
        temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
        double gain_val = temp_string.toDouble();

        set_gain(channel, gain_val);
        write_gain(channel);
        
        return "READY";
    } else if(temp_string.equals("PKGAIN")) {
        temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
        double thresh_val = temp_string.toDouble();

        set_peak_gain(channel, thresh_val);
        write_peak_thresh(channel);
        
        return "READY";
    } else if(temp_string.equals("RMSGAIN")) {
        temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
        double thresh_val = temp_string.toDouble();

        set_rms_gain(channel, thresh_val);
        write_rms_thresh(channel);
        
        return "READY";
    } else if(temp_string.equals("DELAY")) {
        temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
        int delay = temp_string.toInt();

        set_delay(channel, delay);
        write_delay(channel);
        
        return "READY";
    } else if(temp_string.equals("EN")) {
        toggle_mute(channel, 1);
        write_mute(channel);
        Serial.println("ENABLE CHANNEL 1");

        return "READY";
    } else if(temp_string.equals("DIS")) {
        toggle_mute(channel, 0);
        write_mute(channel);
        Serial.println("DISABLE CHANNEL 1");

        return "READY";
    } else if(temp_string.equals("EQ1")) {
        return parse_eq(channel, 0, residual_input);
    } else if(temp_string.equals("EQ2")) {
        return parse_eq(channel, 1, residual_input);
    } else if(temp_string.equals("EQ3")) {
        return parse_eq(channel, 2, residual_input);
    } else if(temp_string.equals("EQ4")) {
        return parse_eq(channel, 3, residual_input);
    } else if(temp_string.equals("EQ5")) {
        return parse_eq(channel, 4, residual_input);
    } else if(temp_string.equals("EQ6")) {
        return parse_eq(channel, 5, residual_input);
    } else if(temp_string.equals("EQ7")) {
        return parse_eq(channel, 6, residual_input);
    } else if(temp_string.equals("EQ8")) {
        return parse_eq(channel, 7, residual_input);
    } else if(temp_string.equals("EQ9")) {
        return parse_eq(channel, 8, residual_input);
    } else if(temp_string.equals("EQ10")) {
        return parse_eq(channel, 9, residual_input);
    } else if(temp_string.equals("HP1")) {
        return parse_eq(channel, 10, residual_input);
    } else if(temp_string.equals("HP2")) {
        return parse_eq(channel, 11, residual_input);
    } else if(temp_string.equals("HP3")) {
        return parse_eq(channel, 12, residual_input);
    } else if(temp_string.equals("HP4")) {
        return parse_eq(channel, 13, residual_input);
    } else if(temp_string.equals("LP1")) {
        return parse_eq(channel, 14, residual_input);
    } else if(temp_string.equals("LP2")) {
        return parse_eq(channel, 15, residual_input);
    } else if(temp_string.equals("LP3")) {
        return parse_eq(channel, 16, residual_input);
    } else if(temp_string.equals("LP4")) {
        return parse_eq(channel, 17, residual_input);
    }

    return "INVALID";
}

// Root parse function
String parse_command(String input) {
    // Skip if there is no command.
    if (input == ""){
        return "NOCOMMAND";
    }

    int index = next_delimiter(input, DELIM_CHAR);
    String temp_string = former(input, index);
    String residual_input = latter(input, index);

    if(temp_string.equals("MIX")){
        return parse_mix(residual_input);
    } else if(temp_string.equals("GEN")){
            return parse_gen(residual_input);
    } else if(temp_string.equals("HB")) {
        return parse_HB(residual_input);
    } else if(temp_string.equals("CH1")){
        return parse_channel(0, residual_input);
    } else if(temp_string.equals("CH2")){
        return parse_channel(1, residual_input);
    } else if(temp_string.equals("CH3")){
        return parse_channel(2, residual_input);
    } else if(temp_string.equals("CH4")){
        return parse_channel(3, residual_input);
    } else if(temp_string.equals("CH5")){
        return parse_channel(4, residual_input);
    } else if(temp_string.equals("CH6")){
        return parse_channel(5, residual_input);
    } else if(temp_string.equals("CH7")){
        return parse_channel(6, residual_input);
    } else if(temp_string.equals("CH8")){
        return parse_channel(7, residual_input);
    }
    
    Serial.println("Received invalid command.");

    return "ERROR";
}