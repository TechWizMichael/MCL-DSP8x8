#include <Arduino.h>
#include <Preferences.h>
#include "MCL-DSP_ADAU1452.h"

#define PEAK 0
#define RMS 1

uint16_t mix_addr[9] = {
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

uint16_t ch1_eq_addr[10][5] = {
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE0_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE0_B1_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE0_B0_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE0_A2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE0_A1_ADDR},
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE1_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE1_B1_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE1_B0_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE1_A2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE1_A1_ADDR},
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE2_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE2_B1_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE2_B0_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE2_A2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE2_A1_ADDR},
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE3_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE3_B1_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE3_B0_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE3_A2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE3_A1_ADDR},
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE4_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE4_B1_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE4_B0_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE4_A2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE4_A1_ADDR},
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE5_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE5_B1_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE5_B0_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE5_A2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE5_A1_ADDR},
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE6_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE6_B1_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE6_B0_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE6_A2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE6_A1_ADDR},
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE7_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE7_B1_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE7_B0_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE7_A2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE7_A1_ADDR},
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE8_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE8_B1_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE8_B0_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE8_A2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE8_A1_ADDR},
    {MOD_CHANNEL_1_PEQ_ALG0_STAGE9_B2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE9_B1_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE9_B0_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE9_A2_ADDR, MOD_CHANNEL_1_PEQ_ALG0_STAGE9_A1_ADDR}
};

typedef struct {
  uint8_t type = 0; // 0=PEQ, 1 = low-shelf, 2 = high-shelf, 3 = low-pass, 4 = high-pass, 5 = all-pass
  float boost; // dB
  float Q; // Unitless
  float frequency; // Hz
} peq_t;

typedef struct {
  float gain;
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
  channel_t ch[8];
  router_t mixer;
} dsp_settings_t;


dsp_settings_t init_param_struct(void) {
    dsp_settings_t new_settings;

    for (int i = 0; i < 8; i++) {
        
        for (int j = 0; j < 10; j++){
            new_settings.ch[i].eq[i].type = 0;
            new_settings.ch[i].eq[i].boost = 0;
            new_settings.ch[i].eq[i].Q = 0.707;
            new_settings.ch[i].eq[i].frequency = 1000;
        }
        
        for (int j = 0; j < 2; j++){
            new_settings.ch[i].lopass[i].type = 0;
            new_settings.ch[i].lopass[i].boost = 0;
            new_settings.ch[i].lopass[i].Q = 0.707;
            new_settings.ch[i].lopass[i].frequency = 1000;
        }

        for (int j = 0; j < 2; j++){
            new_settings.ch[i].hipass[i].type = 0;
            new_settings.ch[i].hipass[i].boost = 0;
            new_settings.ch[i].hipass[i].Q = 0.707;
            new_settings.ch[i].hipass[i].frequency = 1000;
        }

        new_settings.ch[i].delay = 1;

        new_settings.ch[i].FIR[0] = 1.0;

        for(int j = 1; j < 512; j++) {
            new_settings.ch[i].FIR[j] = 0.0;
        }

        new_settings.ch[i].gain.gain = 0.0;
        new_settings.ch[i].gain.peak_thresh = 0.0;
        new_settings.ch[i].gain.rms_thresh = 0.0;
    }

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 8; j++) {
            new_settings.mixer.gain[i][j] = 0;
        }
    }

    for (int i = 0; i < 9; i++) {
        new_settings.mixer.gain[i][i] = 1.0;
    }

    
}