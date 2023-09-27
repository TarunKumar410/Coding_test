#include <stdio.h>
#include <math.h>
#include <complex.h>

#define CYCLE 1
#define DATA_LENGTH 20
#define HARMONICS 5
#define SAMPLE_FREQ 1000

// Sample voltage data
float Va[] = {156.63, 246.59, 294.72, 305.51, 300.66,
268.03, 204.18, 125.41, 42.954, -48.322,
-154.08, -243.95, -293.12, -303.09, -297.98,
-264.13, -202.1, -122.25, -39.893, 51.818 };

float Vb[] = { -308.4, -280.19, -240.66, -186.6, -99.744,
-0.54547, 92.853, 181.46, 262.05, 312.39,
311.44, 283.76, 245.04, 188.62, 102.16,
2.9662, -89.395, -176.17, -259.16, -309.96 };

float Vc[] = {156.11, 82.694, -21.783, -128.37, -213.06,
-269.49, -309.58, -313.4, -273.73, -214.81,
-154.29, -79.64, 24.679, 132.16, 216.63,
274.14, 311.11, 315.76, 276.27, 216.22 };

// Structure to hold calculation parameters
typedef struct {
    float *in_a;
    float *in_b;
    float *in_c;
    float F_est;
    float Theta_est;
    float Harmonics[HARMONICS];
    float Ts;
    float Kc1;
    float Kc2;
    float Kc3;
} DDATA;

DDATA ddata = {
.in_a = &Va,
.in_b = &Vb,
.in_c = &Vc,
.Ts = 0.001,
};


void estimateFrequencyAndTheta(DDATA *d, int dataSize) {
    
    int index = 0;
    float max_amp = 0.0;
    for (int i = 1; i < dataSize/2 ; i++) {
        float amp = cabs(d->in_a[i]);
        
        if (amp > max_amp) {
            max_amp = amp;
            index = i;
        }
    }

    double datasize = (double)((double)dataSize * (double)d->Ts);
    d->F_est = ((float)index / (float)(datasize));
    
    //if we have fixed frequcency the we use below one to calulate the frequcency
    //sample frequcency = 1/d->Ts;
    //d->F_est = ((1/d->Ts)/dataSize);
    
    printf("phase : %f\n",d->F_est);
    
    int index_a = 0;
    int index_b = 0;
    int index_c = 0;
    float max_amp_a = 0.0;
    float max_amp_b = 0.0;
    float max_amp_c = 0.0;
    for (int i = 0; i < dataSize ; i++) {
        
        float amp_a = cabs(d->in_a[i]);
        float amp_b = cabs(d->in_b[i]);
        float amp_c = cabs(d->in_c[i]);

        if (amp_a > max_amp_a) {
            max_amp_a = amp_a;
            index_a = i;
        }
        if (amp_b > max_amp_b) {
            max_amp_b = amp_b;
            index_b = i;
        }
        if (amp_c > max_amp_c) {
            max_amp_c = amp_c;
            index_c = i;
        }
        //printf("index_a : %d\t b:%d\t c:%d\n",index_a,index_b,index_c);
    }
    
    //printf("a_i:%d,b_i:%d\n",index_a,index_b); 
    //Here calculated the phase angle b/w A and B
    
    d->Theta_est = (2 * 180*(index_b-index_a))/dataSize;
    
    printf("Phase Angle: %.2f degrees\n", ddata.Theta_est);
}


void getHarmonicAmplitudes(DDATA *d, int dataSize) {

    int max_amp_index = 0;
    float max_amp = 0.0;

    for (int i = 0; i < dataSize/2 ; i++) {
        float amp = cabs(d->in_a[i]);
        
        if (amp > max_amp) {
            max_amp = amp;
            max_amp_index = i;
           }
    }
    
    float fundamental_frequency = (float)max_amp_index / (dataSize * d->Ts);
   // printf("fundamental_frequency : %f\n",fundamental_frequency);
    for (int i = 1; i <= HARMONICS; i++) {
        float harmonic_frequency = i * fundamental_frequency;
        
        int harmonic_index = (int)(harmonic_frequency * dataSize * d->Ts);
        //printf("harmonic_index :%d",harmonic_index);
        float harmonic_amplitude_a = cabs(d->in_a[harmonic_index]);
        float harmonic_amplitude_b = cabs(d->in_b[harmonic_index]);
        float harmonic_amplitude_c = cabs(d->in_c[harmonic_index]);
        //printf("Harmonic Amplitudes : %f\t b:%f\t c:%f\n",harmonic_amplitude_a,harmonic_amplitude_b,harmonic_amplitude_c);
        
        d->Harmonics[i - 1] = harmonic_amplitude_a + harmonic_amplitude_b + harmonic_amplitude_c;
    }
	for (int j = 0; j < HARMONICS; j++) {
        printf("Harmonic %d: %.2f\n", j + 1, ddata.Harmonics[j]);
    }
}


int main() {

    int i = 0;
    for (i = 0; i < DATA_LENGTH * CYCLE; i++) {
        estimateFrequencyAndTheta(&ddata, DATA_LENGTH * CYCLE);
        getHarmonicAmplitudes(&ddata, DATA_LENGTH * CYCLE);
    }

    return 0;
    
}
