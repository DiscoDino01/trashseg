#pragma once

struct Pin {
public:
	Pin(uint8_t);
	Pin(uint8_t, uint8_t);

	int read();
	void write(int);
	//void set_mode(int);

	operator int();

	operator uint32_t(); // or unsigned int ?

	operator uint8_t(); // uchar
	

private:
	uint8_t val;
	uint8_t mode;
	//bool is_analog;
};