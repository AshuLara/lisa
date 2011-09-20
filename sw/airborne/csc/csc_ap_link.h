#ifndef CSC_AP_LINK_H
#define CSC_AP_LINK_H

#include "std.h"
#include "string.h"
#include "csc_can.h"
#include "csc_msg_def.h"


extern int32_t csc_ap_link_error_cnt;

void csc_ap_link_init(void);
void csc_ap_send_msg(uint8_t msg_id, const uint8_t *buf, uint8_t len);
void can_write_csc(uint8_t board_id, uint8_t msg_id, const uint8_t *buf, uint8_t len);
void csc_ap_link_send_status(uint32_t loops, uint32_t msgs);
void csc_ap_link_send_adc(float adc1, float adc2);
void csc_ap_link_send_vane(float *vane_angle);
void csc_ap_link_send_pressure(float pressure1, float pressure2);
void csc_ap_link_send_airspeed(float airspeed, uint8_t sensor_id);
void csc_ap_link_send_baro(uint32_t pressure, uint16_t temp, uint8_t status, uint8_t sensor_addr);
void csc_ap_link_set_servo_cmd_cb(void (* cb)(struct CscServoCmd *cmd));
void csc_ap_link_set_motor_cmd_cb(void (* cb)(struct CscMotorMsg *msg));
void csc_ap_link_set_prop_cmd_cb(void (* cb)(struct CscPropCmd *cmd, int idx));
void csc_ap_link_set_rc_cmd_cb(void (* cb)(struct CscRCMsg *msg));
void csc_ap_link_set_gpsfix_cb(void (* cb)(struct CscGPSFixMsg *msg));
void csc_ap_link_set_gpspos_cb(void (* cb)(struct CscGPSPosMsg *msg));
void csc_ap_link_set_gpsacc_cb(void (* cb)(struct CscGPSAccMsg *msg));
void csc_ap_link_send_bat(uint16_t volts, uint16_t amps, uint8_t msgctr);

#endif /* CSC_AP_LINK_H */

