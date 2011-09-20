#ifndef SUPERVISION_H
#define SUPERVISION_H

#include "std.h"
#include "generated/airframe.h"

struct Supervision {
  int32_t commands[SUPERVISION_NB_MOTOR];
  int32_t trim[SUPERVISION_NB_MOTOR];
  bool_t override_enabled[SUPERVISION_NB_MOTOR];
  int32_t override_value[SUPERVISION_NB_MOTOR];
  uint32_t nb_failure;
};

extern struct Supervision supervision;

extern void supervision_init(void);
extern void supervision_run(bool_t motors_on, bool_t override_on, int32_t in_cmd[]);
extern void supervision_run_spinup(uint32_t counter, uint32_t max_counter);

#endif /* SUPERVISION_H */
