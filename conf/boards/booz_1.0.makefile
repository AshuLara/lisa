#
# booz_1.0.makefile
#
# http://paparazzi.enac.fr/wiki/Booz
#
ARCH=lpc21
BOARD=booz
BOARD_VERSION=1.0
BOARD_CFG=\"boards/$(BOARD)_$(BOARD_VERSION).h\"

ifndef FLASH_MODE
FLASH_MODE = IAP
endif

#
#
# some default values shared between different firmwares
#
#

SYS_TIME_LED = 1 # not used on rotorcraft, only for tests

RADIO_CONTROL_LED  = 1
RADIO_CONTROL_LINK = UART0

BARO_LED = 2

ifndef MODEM_PORT
MODEM_PORT = UART1
endif
ifndef MODEM_BAUD
MODEM_BAUD = B57600
endif

AHRS_ALIGNER_LED = 3

ifndef GPS_PORT
GPS_PORT=UART0
endif
ifndef GPS_BAUD
GPS_BAUD=B38400
endif
GPS_LED=4
