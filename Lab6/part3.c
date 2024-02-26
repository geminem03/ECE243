/*Write a C-language program that will generate a square wave to go to the audio output device (no input). Make
it so that the frequency of the audio output is changeable (across the audible range from about 100Hz to 2KHz)
using the 10 switches on the DE1-Soc to create a fine-grained selection of the frequency.*/


#define AUDIO_BASE 0xFF203040
#define SAMPLING_RATE 1 / 8000 // 8000 Hz sampling rate

struct audio_t
	{
		volatile unsigned int control; // The control/status register
		volatile unsigned char rarc;   // the 8 bit RARC register
		volatile unsigned char ralc;   // the 8 bit RALC register
		volatile unsigned char wsrc;   // the 8 bit WSRC register
		volatile unsigned char wslc;   // the 8 bit WSLC register
		volatile unsigned int ldata;   // the 32 bit (really 24) left data register
		volatile unsigned int rdata;   // the 32 bit (really 24) right data register
	};

struct audio_t *const audiop = ((struct audio_t *)AUDIO_BASE);

int main(void)
{
	// Audio port structure
	volatile int *SWITCH = 0xFF200040;

	// enable audio
    audiop->control = 0;

	// initialize variables
	int amplitude = 0xFFFFFF;
	int cycles_counter = 0;
	int num_samples = 0;

	while (1)
	{
		int frequency = 100 + (*SWITCH) * 1.8; // formula that use 2^10 combinations of swithces to assign frequency
		int period = 1 / frequency;
		int cycles_on = period / 2;
	
		num_samples = (int)(cycles_on / SAMPLING_RATE ); // computes num of samples per wave period
		
		if (cycles_counter == num_samples) // checks if one half cycle has been completed
		{
			amplitude ^= 0xFFFFFF; // toggles between high on low frequency 
			cycles_counter = 0; // resets the counter
		}
		if (audiop->wsrc > 0){ // check for space in output fifo
			audiop->ldata = amplitude; 
			audiop->rdata = amplitude; 
			cycles_counter++; 
		}
	}
}