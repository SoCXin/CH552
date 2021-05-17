#include "ch552.h"

void init_clock(void) {
#if OSC_EN_XT
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	CLOCK_CFG |= bOSC_EN_XT; // Enable external oscillator
	CLOCK_CFG &= ~bOSC_EN_INT; // Disable internal oscillator
#endif
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
#if FREQ_SYS == 24000000
	CLOCK_CFG = (CLOCK_CFG & ~MASK_SYS_CK_SEL) | 0x06; // 24MHz
#endif
#if FREQ_SYS == 16000000
	CLOCK_CFG = (CLOCK_CFG & ~MASK_SYS_CK_SEL) | 0x05; // 16MHz
#endif
#if FREQ_SYS == 12000000
	CLOCK_CFG = (CLOCK_CFG & ~MASK_SYS_CK_SEL) | 0x04; // 12MHz
#endif
#if FREQ_SYS == 6000000
	CLOCK_CFG = (CLOCK_CFG & ~MASK_SYS_CK_SEL) | 0x03; // 6MHz
#endif
#if FREQ_SYS == 3000000
	CLOCK_CFG = (CLOCK_CFG & ~MASK_SYS_CK_SEL) | 0x02; // 3MHz
#endif
#if FREQ_SYS == 750000
	CLOCK_CFG = (CLOCK_CFG & ~MASK_SYS_CK_SEL) | 0x01; // 750KHz
#endif
#if FREQ_SYS == 187500
	CLOCK_CFG = (CLOCK_CFG & ~MASK_SYS_CK_SEL) | 0x00; // 187.5KHz
#endif
	SAFE_MOD = 0x00;
}
