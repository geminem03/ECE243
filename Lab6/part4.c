/*******************************************************************************
 Write a C-language program, similar to the one in part II, except that it acts as an echo
- output = input = D*output(t-N)
- D is damping coeffcient (start with 0.5)
- N is number of samples that create 0.4 seconds delay (3200 samples)

 ******************************************************************************/

#define AUDIO_BASE 0xFF203040
#define MAX_SAMPLES 3200

struct audio_t {
    volatile unsigned int control;
    volatile unsigned char rarc;
    volatile unsigned char ralc;
    volatile unsigned char wsrc;
    volatile unsigned char wslc;
    volatile unsigned int ldata;
    volatile unsigned int rdata;
};

struct audio_t *const audiop = ((struct audio_t *)AUDIO_BASE);

// Arrays to store output samples
double out_left[MAX_SAMPLES] = {0};
double out_right[MAX_SAMPLES] = {0};


int main(void) {
    audiop->control = 0;  // Clear control register
    int left, right;

    // Counter variable for array index
    int count = 0;

    while (1) {
        // check if input and output fifo have space
        if (audiop->rarc > 0 && audiop->wsrc > 0) {
            // take in input from mic
            left = audiop->ldata;
            right = audiop->rdata;

            // add current input plus damped previous output
            out_left[count] = left + (0.3 * out_left[count]);
            out_right[count] = right + (0.3 * out_right[count]);

            // output all to speakers
            audiop->ldata = out_left[count];
            audiop->rdata = out_right[count];  

            // ouput count + 1 value 
            // when count + 1 = MAX_SAMPLES will reset to 0
            count = (count + 1) % MAX_SAMPLES;  
        }
    }

    return 0;
}