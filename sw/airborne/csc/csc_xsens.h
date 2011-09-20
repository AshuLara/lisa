/*
 * Copyright (C) 2008 Joby Energy
 *
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

/** \file xsens.h
 */

#ifndef __CSC_XSENS_H__
#define __CSC_XSENS_H__

#include "types.h"
#include "math/pprz_algebra_float.h"

#define XSENS_COUNT 1

extern uint8_t xsens_mode[XSENS_COUNT]; // Receiver status
extern volatile uint8_t xsens_msg_received[XSENS_COUNT];

extern float xsens_phi[XSENS_COUNT];
extern float xsens_theta[XSENS_COUNT];
extern float xsens_psi[XSENS_COUNT];
extern uint8_t xsens_msg_status[XSENS_COUNT];

extern float xsens_r_a[XSENS_COUNT];
extern float xsens_r_b[XSENS_COUNT];
extern float xsens_r_c[XSENS_COUNT];
extern float xsens_r_d[XSENS_COUNT];
extern float xsens_r_e[XSENS_COUNT];
extern float xsens_r_f[XSENS_COUNT];
extern float xsens_r_g[XSENS_COUNT];
extern float xsens_r_h[XSENS_COUNT];
extern float xsens_r_i[XSENS_COUNT];

extern float xsens_accel_x[XSENS_COUNT];
extern float xsens_accel_y[XSENS_COUNT];
extern float xsens_accel_z[XSENS_COUNT];
extern float xsens_mag_x[XSENS_COUNT];
extern float xsens_mag_y[XSENS_COUNT];
extern float xsens_mag_z[XSENS_COUNT];
extern float xsens_gyro_x[XSENS_COUNT];
extern float xsens_gyro_y[XSENS_COUNT];
extern float xsens_gyro_z[XSENS_COUNT];
extern float xsens_mag_heading[XSENS_COUNT];

// adjustable heading offset
extern float xsens_psi_offset[XSENS_COUNT];

extern uint16_t xsens_time_stamp[XSENS_COUNT];

extern int xsens_setzero;

extern struct FloatRMat xsens_rmat_neutral[XSENS_COUNT];

#define PERIODIC_SEND_IMU_GYRO(_chan) DOWNLINK_SEND_IMU_GYRO (_chan, 	\
  &xsens_gyro_x, &xsens_gyro_y, &xsens_gyro_z \
)

#define PERIODIC_SEND_IMU_ACCEL(_chan) DOWNLINK_SEND_IMU_ACCEL (_chan, 	\
  &xsens_accel_x, &xsens_accel_y, &xsens_accel_z \
)

#define PERIODIC_SEND_IMU_MAG(_chan) DOWNLINK_SEND_IMU_MAG (_chan, 	\
  &xsens_mag_x, &xsens_mag_y, &xsens_mag_z \
)

#define PERIODIC_SEND_ATTITUDE(_chan) DOWNLINK_SEND_ATTITUDE (_chan,	\
  &xsens_phi, &xsens_psi, &xsens_theta \
)

#define PERIODIC_SEND_RMAT_DEBUG(_chan) DOWNLINK_SEND_RMAT_DEBUG (_chan,  \
  &xsens_rmat_neutral[0].m[0], \
  &xsens_rmat_neutral[0].m[1], \
  &xsens_rmat_neutral[0].m[2], \
  &xsens_rmat_neutral[0].m[3], \
  &xsens_rmat_neutral[0].m[4], \
  &xsens_rmat_neutral[0].m[5], \
  &xsens_rmat_neutral[0].m[6], \
  &xsens_rmat_neutral[0].m[7], \
  &xsens_rmat_neutral[0].m[8] \
)

#include "std.h"

void xsens_init(void);
void xsens_parse_msg(uint8_t xsens_id);
void xsens_event_task(void);
void xsens_periodic_task(void);

#endif
