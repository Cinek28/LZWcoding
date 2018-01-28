#pragma once

#define MAX_OUTPUT_BIT_SIZE 32
#define BYTE_BITS 8
#define BYTE_MASK 0xFF
#define MAX_HISTOGRAM_SIZE 256*256*256

enum ReadTypeEnum {
	Linear,
	Split3Color,
	LinearAscii,
	Unimplemented
};
enum OverflowType
{
	Flushing,
	WorkingSet
};
struct LZWCompressHeader {
	uint32_t dictionary_size;
	//ReadTypeEnum data_order_type;
	OverflowType overflow_type;
};