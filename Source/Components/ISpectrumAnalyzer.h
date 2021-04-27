#pragma once

class ISpectrumAnalyzer
{
public:
	virtual void push_next_sample_into_fifo(float sample) noexcept = 0;
	virtual void calculate_next_frame_of_spectrum()= 0;

	virtual void calculate_spectrum()= 0;
	virtual void add_current_spectrum_to_accumulator_buffer()= 0;
	virtual void calculate_mean_fft_data()= 0;


	virtual int get_fft_mean_value()= 0;
	virtual void calculate_fft()= 0;
	virtual int get_fft_peak()= 0;

};
