#include "core.h"

extern int printf(char const *restrict, ...);

int main() {

	i8080 state = {
		.ei = 1,
		.pc = {0, 5},
		.sp = {0x24, 00},
		.memory = (uint8_t [0x10000]) {
			0x01,
			0xaa,
			0xbb,
			0xfb,
			0xc9,
			0x76,
			0x01,
			0xab,
			0xbc
		}
	};

	for (unsigned i = 15; i--; ) {
		if (i == 5)
			i8080_rst(&state, 0);
		i8080_step(&state);
	}

	return 0;
}
