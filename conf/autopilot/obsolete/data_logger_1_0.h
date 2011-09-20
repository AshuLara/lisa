#ifndef CONFIG_DATA_LOGGER_H
#define CONFIG_DATA_LOGGER_H

/* Master oscillator freq.       */
#define FOSC (12000000) 

/* PLL multiplier                */
#define PLL_MUL (5)         

/* CPU clock freq.               */
#define CCLK (FOSC * PLL_MUL) 

/* Peripheral bus speed mask 0x00->4, 0x01-> 1, 0x02 -> 2   */
#define PBSD_BITS 0x00    
#define PBSD_VAL 4

/* Peripheral bus clock freq. */
#define PCLK (CCLK / PBSD_VAL) 

/* LEDs */
#define LED_1_BANK 0
#define LED_1_PIN 21

#define LED_2_BANK 0
#define LED_2_PIN 22

#define LED_3_BANK 1
#define LED_3_PIN 19

/* ADC */
#define ADC_CHANNEL_VSUPPLY AdcBank0(6)
#ifndef USE_AD0
#define USE_AD0
#endif
#define USE_AD0_6

/* SPI for SD card */
#define SPI1_DRDY_PINSEL PINSEL1
#define SPI1_DRDY_PINSEL_BIT   0
#define SPI1_DRDY_PINSEL_VAL   1
#define SPI1_DRDY_EINT         0
#define SPI1_DRDY_VIC_IT       VIC_EINT0

#endif /* CONFIG_DATA_LOGGER_H */
