/*
 * $Id$
 *
 * Copyright (C) 2010 The Paparazzi Team
 *
 * This file is part of paparazzi.
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
 */

/*
 *
 * simulator plug for the crista imu arch dependant functions
 *
 */
#ifndef IMU_CRISTA_ARCH_H
#define IMU_CRISTA_ARCH_H

#include "subsystems/imu.h"


#define ImuCristaArchPeriodic() {}

extern void imu_feed_gyro_accel(void);
extern void imu_feed_mag(void);


#endif /* IMU_CRISTA_HW_H */
