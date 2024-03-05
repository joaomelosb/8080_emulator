#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "core.h"

#define TEST_ROM "cpudiag.bin"
#define OFFSET 0x100
#define putchar(x) (isprint(x) ? putchar(x) : 0)

static void port_out(uint8_t, uint8_t);

static _Bool exec = 1;
static i8080 state = {
	.pc = {OFFSET >> 8 & 0xff, OFFSET & 0xff},
	.ei = 1,
	.port_out = port_out,
	.memory = (uint8_t [0x10000]) {0}
};

static void port_out(uint8_t p, uint8_t acc) {
	if (!p) {
		exec = 0;
		return;
	}
	switch (state.c) {
	case 0x2:
		// print char
		putchar(state.e);
		break;
	case 0x9:
		// print str
		for (uint8_t *w = &state.memory[state.d << 8 | state.e];
			*w != '$'; w++)
			putchar(*w);
	}
}

int main() {
	FILE *fp;
	
	if (!(fp = fopen(TEST_ROM, "rb"))) {
		perror(TEST_ROM);
		return 1;
	}
	
	for (uint8_t *w = state.memory + OFFSET;;) {
		*w++ = fgetc(fp);
		
		if (feof(fp))
			break;
		
		if (ferror(fp)) {
			perror("can't read " TEST_ROM);
			return 1;
		}
	}
	
	// emulate an CP/M machine
	uint8_t bios[] = {
		// terminate program
		0xd3,
		0x00,
		0xc9,
		// print on screen
		[0x5] = 0xd3,
		0x01,
		0xc9
	};
	
	memcpy(state.memory, bios, sizeof bios);
	
	// step count
	for (; exec; )
		i8080_step(&state);

	return 0;
}