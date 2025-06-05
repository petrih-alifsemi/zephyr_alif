#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/sys/barrier.h>
#include <soc.h>

#define CGU_BASE 0x1A602000

#define PLL_CLK_SEL (CGU_BASE + 0x8)
#define PLL_CLK_SEL_RC (0x0 << 20 | 0x1 << 4 | 0x1)
#define PLL_CLK_SEL_PLL (0x1 << 20 | 0x1 << 4 | 0x1)

#define ESCLK_SEL (CGU_BASE + 0x10)
#define ESCLK_SEL_ES1_PLL_VAL(_v) (_v << 4)
#define ESCLK_SEL_ES1_PLL ESCLK_SEL_ES1_PLL_VAL(0x3) /* 160MHz PLL */
#define ESCLK_SEL_ES1_OSC (0x0 << 12) /* 76.8 MHz ring-oscillator */
#define ESCLK_SEL_REF_DIV_ENA (1 << 27)
#define ESCLK_SEL_REF_DIV(_div) (_div << 16)

#define ESCLK_SEL_BASE(_div) (ESCLK_SEL_ES1_PLL | ESCLK_SEL_ES1_OSC | ESCLK_SEL_REF_DIV_ENA | ESCLK_SEL_REF_DIV(_div))

#define USE_RC_CLOCK 1

#define WRITE_REG(_v, _r) (*(volatile uint32_t *)_r = _v)

#define INLINE_THESE //ALWAYS_INLINE



INLINE_THESE void cpu_idle_enter_clock(void)
{
#if USE_RC_CLOCK && 0
	uint32_t val = sys_read32(PLL_CLK_SEL);
	val &= ~PLL_CLK_SEL_PLL;
	// val |= PLL_CLK_SEL_RC;
	sys_write32(val, PLL_CLK_SEL);
#elif USE_RC_CLOCK
        WRITE_REG(PLL_CLK_SEL_RC, PLL_CLK_SEL);
#else
        //WRITE_REG(ESCLK_SEL_BASE(0x40), ESCLK_SEL); /* Clock divided by 64 */
        WRITE_REG(ESCLK_SEL_BASE(0x100), ESCLK_SEL); /* Clock divided by 256 */
        //WRITE_REG(ESCLK_SEL_ES1_PLL_VAL(0x0), ESCLK_SEL);
#endif
}

INLINE_THESE void cpu_idle_exit_clock(void)
{
#if USE_RC_CLOCK && 0
	uint32_t val = sys_read32(PLL_CLK_SEL);
	// val &= ~PLL_CLK_SEL_RC;
	val |= PLL_CLK_SEL_PLL;
	sys_write32(val, PLL_CLK_SEL);
#elif USE_RC_CLOCK
        WRITE_REG(PLL_CLK_SEL_PLL, PLL_CLK_SEL);
#else
        WRITE_REG(ESCLK_SEL_BASE(0), ESCLK_SEL);
        //WRITE_REG(ESCLK_SEL_ES1_PLL_VAL(0x3), ESCLK_SEL);
#endif
}
