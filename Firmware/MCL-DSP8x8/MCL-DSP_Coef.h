#include <Arduino.h>

#define fs 48000

// all-pass filter coefficient calculation
void ap_coef(float coef[], float freq, float Q, float boost) {
  float omega = 2 * M_PI * freq / fs;
  float sn = sin(omega);
  float cs = cos(omega);
  float alpha = sn / (2 * Q);
  float norm = 1 + alpha;
  float b0 = (1 - alpha) / norm;
  float b1 = (-2 * cs) / norm;
  float b2 = (1 + alpha) / norm;
  float a1 = (-2 * cs) / norm;
  float a2 = (1 - alpha) / norm;

  coef[0] = b2;
  coef[1] = b1;
  coef[2] = b0;
  coef[3] = a2;
  coef[4] = a1;

  return;
}

// low-pass filter coefficient calculation
void lp_coef(float coef[], float freq, float Q, float boost) {
  float omega = 2*M_PI*freq/fs;
  float sn = sin(omega);
  float cs = cos(omega);

  float alpha = sn/(2*Q);

  float a0 = 1 + alpha;
  float a1 = -2*cs;
  float a2 = 1 - alpha;
  float b0 = (1-cs)/2;
  float b1 = 1 - cs;
  float b2 = (1-cs)/2;

  coef[0] = b2;
  coef[1] = b1;
  coef[2] = b0;
  coef[3] = a2;
  coef[4] = a1;

  return;
}

// high-pass filter coefficient calculation
void hp_coef(float coef[], float freq, float Q, float boost) {
  float omega = 2*M_PI*freq/fs;
  float sn = sin(omega);
  float cs = cos(omega);
  float alpha = sn/(2*Q);

  float a0 = 1 + alpha;
  float a1 = -2*cs;
  float a2 = 1 - alpha;
  float b0 = (1+cs)/2;
  float b1 = -1*(1 + cs);
  float b2 = (1+cs)/2;

  coef[0] = b2;
  coef[1] = b1;
  coef[2] = b0;
  coef[3] = a2;
  coef[4] = a1;

  return;
}

// Calculating PEQ coefficients
void peq_coef(float coef[], float freq, float Q, float boost) {
  if (boost == 0) {
    coef[0] = 0;
    coef[1] = 0;
    coef[2] = 1;
    coef[3] = 0;
    coef[4] = 0;
    return;
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

  coef[0] = b2;
  coef[1] = b1;
  coef[2] = b0;
  coef[3] = a2;
  coef[4] = a1;

  return;
}

// low-shelf filter coefficient calculation
void ls_coef(float coef[], float freq, float Q, float boost) {
  if (boost == 0) {
    coef[0] = 0;
    coef[1] = 0;
    coef[2] = 1;
    coef[3] = 0;
    coef[4] = 0;
    return;
  }

  float omega = 2*M_PI*freq/fs;
  float sn = sin(omega);
  float cs = cos(omega);
  float Ax = pow(10, (boost/40));
  float alpha = sn/(2*Q);

  float a0 = (Ax + 1) + (Ax - 1)*cs + 2*sqrt(Ax)*alpha;
  float a1 = -2*((Ax - 1) + (Ax + 1)*cs);
  float a2 = (Ax + 1) + (Ax - 1)*cs - 2*sqrt(Ax)*alpha;
  float b0 = Ax*((Ax + 1) - (Ax - 1)*cs + 2*sqrt(Ax)*alpha);
  float b1 = 2*Ax*((Ax - 1) - (Ax + 1)*cs);
  float b2 = Ax*((Ax + 1) - (Ax - 1)*cs - 2*sqrt(Ax)*alpha);

  // normalize to a0
  a1 = a1/a0;
  a2 = a2/a0;
  b0 = b0/a0;
  b1 = b1/a0;
  b2 = b2/a0;

  coef[0] = b2;
  coef[1] = b1;
  coef[2] = b0;
  coef[3] = a2;
  coef[4] = a1;

  return;
}

// high-shelf filter coefficient calculation
void hs_coef(float coef[], float freq, float Q, float boost) {
  if (boost == 0) {
    coef[0] = 0;
    coef[1] = 0;
    coef[2] = 1;
    coef[3] = 0;
    coef[4] = 0;
    return;
  }

  float omega = 2*M_PI*freq/fs;
  float sn = sin(omega);
  float cs = cos(omega);
  float Ax = pow(10, (boost/40));
  float alpha = sn/(2*Q);

  float a0 = (Ax + 1) - (Ax - 1)*cs + 2*sqrt(Ax)*alpha;
  float a1 = 2*((Ax - 1) - (Ax + 1)*cs);
  float a2 = (Ax + 1) - (Ax - 1)*cs - 2*sqrt(Ax)*alpha;
  float b0 = Ax*((Ax + 1) + (Ax - 1)*cs + 2*sqrt(Ax)*alpha);
  float b1 = -2*Ax*((Ax - 1) + (Ax + 1)*cs);
  float b2 = Ax*((Ax + 1) + (Ax - 1)*cs - 2*sqrt(Ax)*alpha);

  // normalize to a0
  a1 = a1/a0;
  a2 = a2/a0;
  b0 = b0/a0;
  b1 = b1/a0;
  b2 = b2/a0;

  coef[0] = b2;
  coef[1] = b1;
  coef[2] = b0;
  coef[3] = a2;
  coef[4] = a1;

  return;
}