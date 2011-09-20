# Makefile for the Tiny board (1 arm7tdmi, 1 LEA-LA)

#
# can we remove this file?
# the makefiles under conf/boards/ should be used now
#

ARCHI=lpc21
ARCH=lpc21

ap.ARCHDIR = $(ARCHI)

fbw.ARCHDIR = $(ARCHI)

LPC21ISP_BAUD = 38400
LPC21ISP_XTAL = 12000


### default settings for tiny_2 and twog
GPS_UART_NR	= 0
GPS_BAUD	= 38400
MODEM_UART_NR = 1
MODEM_BAUD = 57600

ADC_IR1 = ADC_1
ADC_IR2 = ADC_2
ADC_IR_TOP = ADC_0
ADC_IR_NB_SAMPLES = 16
ADC_GYRO_ROLL = ADC_3
ADC_GYRO_NB_SAMPLES = 16

# a test program to setup actuators
setup_actuators.ARCHDIR = $(ARCHI)

setup_actuators.CFLAGS += -DFBW -DBOARD_CONFIG=\"tiny.h\" -DUSE_LED -DTIME_LED=1 -DACTUATORS=\"servos_4015_hw.h\" -DSERVOS_4015 -DUSE_UART0 -DUART0_BAUD=B9600 -DDATALINK=PPRZ -DPPRZ_UART=Uart0
setup_actuators.srcs += sys_time.c $(SRC_ARCH)/sys_time_hw.c $(SRC_ARCH)/armVIC.c pprz_transport.c setup_actuators.c $(SRC_ARCH)/mcu_periph/uart_arch.c $(SRC_ARCH)/servos_4015_hw.c main.c


# a test program to tunnel between both uart
tunnel.ARCHDIR = $(ARCHI)

tunnel.CFLAGS += -DFBW -DBOARD_CONFIG=\"tiny_2_1_1_usb.h\" -DUSE_LED
tunnel.srcs += $(SRC_ARCH)/uart_tunnel.c


# A test program to monitor the ADC values
test_adcs.ARCHDIR = $(ARCHI)

test_adcs.CFLAGS += -DBOARD_CONFIG=$(CONFIG) -DUSE_LED -DTIME_LED=1 -DADC -DUSE_ADC_0 -DUSE_ADC_1 -DUSE_ADC_2 -DUSE_ADC_3 -DUSE_ADC_4 -DUSE_ADC_5 -DUSE_ADC_6 -DUSE_ADC_7
test_adcs.CFLAGS += -DDOWNLINK -DUSE_UART0 -DDOWNLINK_TRANSPORT=XBeeTransport -DDOWNLINK_FBW_DEVICE=Uart0 -DDOWNLINK_AP_DEVICE=Uart0 -DXBEE_UART=Uart0 -DDATALINK=XBEE -DUART0_BAUD=B9600
test_adcs.srcs += downlink.c $(SRC_ARCH)/mcu_periph/uart_arch.c xbee.c

test_adcs.srcs += sys_time.c $(SRC_ARCH)/adc_hw.c $(SRC_ARCH)/sys_time_hw.c $(SRC_ARCH)/armVIC.c test_adcs.c
# pprz_transport.c


# a configuration program to access both uart through usb
usb_tunnel_0.ARCHDIR = $(ARCHI)
usb_tunnel_0.CFLAGS += -DFBW -DBOARD_CONFIG=\"tiny_2_1_1_usb.h\" -DUSE_UART0 -DUART0_BAUD=B115200
usb_tunnel_0.CFLAGS += -DUSE_USB_LINE_CODING -DUSE_USB_SERIAL -DUSE_LED
usb_tunnel_0.srcs += $(SRC_ARCH)/usb_tunnel.c $(SRC_ARCH)/usb_ser_hw.c $(SRC_ARCH)/mcu_periph/uart_arch.c
usb_tunnel_0.srcs += $(SRC_ARCH)/lpcusb/usbhw_lpc.c $(SRC_ARCH)/lpcusb/usbinit.c
usb_tunnel_0.srcs += $(SRC_ARCH)/lpcusb/usbcontrol.c $(SRC_ARCH)/lpcusb/usbstdreq.c
usb_tunnel_0.srcs += sys_time.c $(SRC_ARCH)/sys_time_hw.c $(SRC_ARCH)/armVIC.c

usb_tunnel_1.ARCHDIR = $(ARCHI)
usb_tunnel_1.CFLAGS += -DFBW -DBOARD_CONFIG=\"tiny_2_1_1_usb.h\" -DUSE_UART1 -DUART1_BAUD=B115200
usb_tunnel_1.CFLAGS += -DUSE_USB_LINE_CODING -DUSE_USB_SERIAL -DUSE_LED
usb_tunnel_1.srcs += $(SRC_ARCH)/usb_tunnel.c $(SRC_ARCH)/usb_ser_hw.c $(SRC_ARCH)/mcu_periph/uart_arch.c
usb_tunnel_1.srcs += $(SRC_ARCH)/lpcusb/usbhw_lpc.c $(SRC_ARCH)/lpcusb/usbinit.c
usb_tunnel_1.srcs += $(SRC_ARCH)/lpcusb/usbcontrol.c $(SRC_ARCH)/lpcusb/usbstdreq.c
usb_tunnel_1.srcs += sys_time.c $(SRC_ARCH)/sys_time_hw.c $(SRC_ARCH)/armVIC.c
