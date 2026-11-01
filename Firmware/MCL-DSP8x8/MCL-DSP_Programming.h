// Values and functions for using Serial/USB
// Last updated October 26, 2026

#include <Arduino.h>

#ifdef CUSTOM_DSP

void command_ADAU1452(String input){
    Serial.println("Changing parameters with Custom DSP not supported");
}

#else

#include "MCL-DSP_HEARTBEAT.h"
#include "MCL-DSP_Structs.h"

#define DELIM_CHAR '/'

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

void write_param(int address, float value) {
  SIGMA_WRITE_REGISTER_FLOAT(address, value);
  return;
}

String command_ADAU1452(String input) {
    // Skip if there is no command.
    if (input == ""){
        return "NOCOMMAND";
    }

    // Serial.println("\n====================\n");
    // Serial.println("Input: " + input + "\n");

    int index = next_delimiter(input, DELIM_CHAR);
    String temp_string = former(input, index);
    String residual_input = latter(input, index);

    if(temp_string.equals("MIX")){
        Serial.println("Received command for MIXER.");
        // TODO: Process command for mixer
        return "READY";
    } else if(temp_string.equals("GEN")){
        Serial.println("Received command for SIGNAL GENERATORS.");

        index = next_delimiter(residual_input, DELIM_CHAR);
        temp_string = former(residual_input, index);
        residual_input = latter(residual_input, index);

        if(temp_string.equals("GG")){
            Serial.println("Received Command for GEN GAIN.");
            //TODO: Adjust gain of  noise generator
            return "READY";
        } else if(temp_string.equals("GEN")){
            Serial.println("Recieved command for NOISE GEN ENABLE");

            return "READY";
        } else if(temp_string.equals("GDIS")){
            Serial.println("Recieved command for NOISE GEN DISABLE");

            return "READY";
        } else if(temp_string.equals("GN")){
            Serial.println("Selected NOISE GENERATOR.");
            return "READY";
        } else if(temp_string.equals("GT")){
            Serial.println("Selected TONE GENERATOR.");
            return "READY";
        } else if(temp_string.equals("GCON")){
            Serial.println("Received command for NOISE GEN CONNECTIONS.");
            command_ADAU1452("MIX/CH9/" + latter(residual_input, next_delimiter(residual_input, '/')));
            return "READY";
        }
    } else if(temp_string.equals("HB")) {
        Serial.println("Received command for heartbeat LED.");

        // Getting specific heartbeat Command
        index = next_delimiter(residual_input, DELIM_CHAR);
        temp_string = former(residual_input, index);
        residual_input = latter(residual_input, index);

        if (temp_string.equals("EN")){
            Serial.println("Heartbeat Enable Command.");
            set_heartbeat_mode(HIGH);

            SIGMA_WRITE_REGISTER_INTEGER(MOD_LED_HEARTBEAT_HEARTBEAT_EN_MONOMUXSIGMA300NS1INDEX_ADDR, 0);
            
            return "READY";
        } else if(temp_string.equals("DIS")){
            Serial.println("DSP Heartbeat Disable Command");
            set_heartbeat_mode(LOW);

            SIGMA_WRITE_REGISTER_INTEGER(MOD_LED_HEARTBEAT_HEARTBEAT_EN_MONOMUXSIGMA300NS1INDEX_ADDR, 1);

            return "READY";
        }
        
    } else if(temp_string.equals("CH1")){

        index = next_delimiter(residual_input, DELIM_CHAR);
        temp_string = former(residual_input, index);
        residual_input = latter(residual_input, index);

        if(temp_string.equals("GAIN")) {
            temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
            float temp_gain = strtod(temp_string.c_str(), nullptr);
            // double temp_gain = temp_string.toDouble();
            temp_gain = pow(10, temp_gain/20.0);

            SIGMA_WRITE_REGISTER_FLOAT(MOD_CHANNEL_1_CH_GAIN_ALG0_TARGET_ADDR, temp_gain);
            return "NEW_GAIN: " + String(temp_gain, 8);
        } else if(temp_string.equals("PKGAIN")) {
            temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
            double temp_thresh = temp_string.toDouble();
            temp_thresh = max(-20.0, (-temp_thresh) - 20);
            temp_thresh = pow(10, temp_thresh/20.0);

            SIGMA_WRITE_REGISTER_FLOAT(MOD_CHANNEL_1_PEAK_LIM_GAIN_ALG0_TARGET_ADDR, temp_thresh);
            return "NEW_GAIN: " + String(temp_thresh, 8);
        }else if(temp_string.equals("RMSGAIN")) {
            temp_string = former(residual_input, next_delimiter(residual_input, DELIM_CHAR));
            double temp_thresh = temp_string.toDouble();
            temp_thresh = max(-20.0, (-temp_thresh) - 20);
            temp_thresh = pow(10, temp_thresh/20.0);

            SIGMA_WRITE_REGISTER_FLOAT(MOD_CHANNEL_1_RMS_LIM_GAIN_ALG0_TARGET_ADDR, temp_thresh);
            return "NEW_GAIN: " + String(temp_thresh, 8);
        }
    }
    
    Serial.println("Received invalid command.");

    return "ERROR";
}

#endif