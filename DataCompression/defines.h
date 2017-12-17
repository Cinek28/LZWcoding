#pragma once

#define MAX_OUTPUT_BIT_SIZE 32


enum ReadTypeEnum {
	Linear,
	Split3Color,
	LinearAscii,
	Unimplemented
};

struct LZWCompressHeader {
	uint8_t word_bit_count;
	uint8_t indx_bit_count;
	ReadTypeEnum data_order_type;
};