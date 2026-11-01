#include <Arduino.h>

#define fs 48000

// Calculating PEQ coefficients
int* peq_coef(float freq, float q, float boost) {
  if (boost == 0) {
    return { 0, 0, 1, 0, 0 };
  }

  float Ax = pow(10, (boost / 40));
  float omega = 2 * M_PI * freq / fs;
  float sn = sin(omega);
  float cs = cos(omega);
  float alpha = sn / (2 * Q);

  float a0 = 1 + alpha / Ax;
  float a1 = (-2 * cs) / a0;
  float a2 = (1 - alpha / Ax) / a0;

  float b0 = (1 + alpha * Ax) / a0;
  float b1 = -2 * cs / a0;
  float b2 = (1 - alpha * Ax) / a0;

  return { b2, b1, b0, a2, a1 };
}

// all-pass filter coefficient calculation
int* ap_coef(float freq, float q, float boost) {
  omega = 2 * pi * freq / fs;
  sn = sin(omega);
  cs = cos(omega);
  alpha = sn / (s * q);
  norm = 1 + alpha;
  b0 = (1 - alpha) / norm;
  b1 = (-2 * cs) / norm;
  b2 = (1 + alpha) / norm;
  a1 = (-2 * cs) / norm;
  a2 = (1 - alpha) / norm;

  return { b2, b1, b0, a2, a1 };
}

// low-pass filter coefficient calculation
int* lp_coef(float freq, float q, float boost) {
  omega = 2*pi*freq/fs;
  sn = sine(omega);
  cs = cos(omega);

  alpha = sn/(2*q);

  a0 = 1 + alpha;
  a1 = -2*cs;
  b0 = (1-cs)/2;
  b1 = 1 - cs;
  b2 = (1-cs)/2;

  return { b2, b1, b0, a2, a1 };
}

// high-pass filter coefficient calculation
int* hp_coef(float freq, float q, float boost) {
  omega = 2*pi*freq/fs;
  sn = sine(omega);
  cs = cos(omega);

  alpha = sn/(2*q)/fs;

  a0 = 1 + alpha;
  a1 = -2*cs;
  b0 = (1+cs)/2;
  b1 = -1*(1 + cs);
  b2 = (1+cs)/2;

  return { b2, b1, b0, a2, a1 };
}

// low-shelf filter coefficient calculation
int* ls_coef(float freq, float q, float boost) {
  omega = 2*pi*freq/fs;
  sn = sine(omega);
  cs = cos(omega);

  alpha = sn/(s*q);


  return { b2, b1, b0, a2, a1 };
}

// high-shelf filter coefficient calculation
int* hs_coef(float freq, float q, float boost) {
  omega = 2*pi*freq/fs;
  sn = sine(omega);
  cs = cos(omega);

  alpha = sn/(2*q);

  return { b2, b1, b0, a2, a1 };
}