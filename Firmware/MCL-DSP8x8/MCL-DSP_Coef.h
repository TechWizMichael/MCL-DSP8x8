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
  float omega = 2 * pi * freq / fs;
  float sn = sin(omega);
  float cs = cos(omega);
  float alpha = sn / (s * q);
  float norm = 1 + alpha;
  float b0 = (1 - alpha) / norm;
  float b1 = (-2 * cs) / norm;
  float b2 = (1 + alpha) / norm;
  float a1 = (-2 * cs) / norm;
  float a2 = (1 - alpha) / norm;

  return { b2, b1, b0, a2, a1 };
}

// low-pass filter coefficient calculation
int* lp_coef(float freq, float q, float boost) {
  float omega = 2*pi*freq/fs;
  float sn = sine(omega);
  float cs = cos(omega);

  float alpha = sn/(2*q);

  float a0 = 1 + alpha;
  float a1 = -2*cs;
  float b0 = (1-cs)/2;
  float b1 = 1 - cs;
  float b2 = (1-cs)/2;

  return { b2, b1, b0, a2, a1 };
}

// high-pass filter coefficient calculation
int* hp_coef(float freq, float q, float boost) {
  float omega = 2*pi*freq/fs;
  float sn = sine(omega);
  float cs = cos(omega);
  float alpha = sn/(2*q)/fs;

  float a0 = 1 + alpha;
  float a1 = -2*cs;
  float b0 = (1+cs)/2;
  float b1 = -1*(1 + cs);
  float b2 = (1+cs)/2;

  return { b2, b1, b0, a2, a1 };
}

// high-shelf filter coefficient calculation
int* hs_coef(float freq, float q, float boost) {
  float omega = 2*pi*freq/fs;
  float sn = sine(omega);
  float cs = cos(omega);
  float Ax = pow(10, (boost/40));
  float alpha = sn/(s*q);

  float a0 = (Ax + 1) - (Ax - 1)*cs + 2*sqrt(Ax)*alpha;
  float a1 = 2*((Ax - 1) - (Ax + 1)*cs);
  float a2 = (Ax + 1) - (Ax - 1)*cs - 2*sqrt(Ax)*alpha;
  float b0 = Ax*((Ax + 1) + (Ax - 1)*cs + 2*sqrt(Ax)*alpha);
  float b1 = -2*Ax*((Ax - 1) + (Ax + 1)*cs);
  float b2 = Ax*((Ax + 1) + (Ax - 1)*cs - 2*sqrt(Ax)*alpha);

  // normalize to a0
  float a1 = a1/a0;
  float a2 = a2/a0;
  float b0 = b0/a0;
  float b1 = b1/a0;
  float b2 = b2/a0;

  return { b2, b1, b0, a2, a1 };
}

// low-shelf filter coefficient calculation
int* ls_coef(float freq, float q, float boost) {
  float omega = 2*pi*freq/fs;
  float sn = sine(omega);
  float cs = cos(omega);
  float Ax = pow(10, (boost/40));
  float alpha = sn/(s*q);

  float a0 = (Ax + 1) + (Ax - 1)*cs + 2*sqrt(Ax)*alpha;
  float a1 = -2*((Ax - 1) + (Ax + 1)*cs);
  float a2 = (Ax + 1) + (Ax - 1)*cs - 2*sqrt(Ax)*alpha;
  float b0 = Ax*((Ax + 1) - (Ax - 1)*cs + 2*sqrt(Ax)*alpha);
  float b1 = 2*Ax*((Ax - 1) - (Ax + 1)*cs);
  float b2 = Ax*((Ax + 1) - (Ax - 1)*cs - 2*sqrt(Ax)*alpha);

  // normalize to a0
  float a1 = a1/a0;
  float a2 = a2/a0;
  float b0 = b0/a0;
  float b1 = b1/a0;
  float b2 = b2/a0;

  return { b2, b1, b0, a2, a1 };
}