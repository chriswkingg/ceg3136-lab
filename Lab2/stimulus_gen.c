#include "stimulus_gen.h"
#include "math.h"

void stimulus_init(stimulus_gen_t *stim_gen, uint32_t seed){
  *stim_gen->speed = 0;
  stim_gen->time_step = 0;
  stim_gen->transmission_state = PARKING;
  stim_gen->shaping_param = (seed & 0xFF) / 0xFF; 
	stim_gen->ctrl->minSpeed[0] = 999;
	stim_gen->ctrl->minSpeed[1] = 999;
	stim_gen->ctrl->minSpeed[2] = 999;
	stim_gen->ctrl->minSpeed[3] = 999;
	stim_gen->ctrl->minSpeed[4] = 999;
	stim_gen->ctrl->minRPM[0] = 999;
	stim_gen->ctrl->minRPM[1] = 999;
	stim_gen->ctrl->minRPM[2] = 999;
	stim_gen->ctrl->minRPM[3] = 999;
	stim_gen->ctrl->minRPM[4] = 999;

}
void stimulus_update(stimulus_gen_t *stim_gen){
  *stim_gen->rpm = 0;
  switch(stim_gen->transmission_state) {
    case PARKING: {
      if (stim_gen->time_step < 10) stim_gen->time_step++;
      else {
        stim_gen->time_step = 0;
        stim_gen->transmission_state = REVERSE;
      }
      break;
    }
    case REVERSE: {
      if (stim_gen->time_step < 10) {
        stim_gen->time_step++;
        *stim_gen->speed = (stim_gen->time_step) * (-1.0f);
      }
      else if (stim_gen->time_step < 20) {
        stim_gen->time_step++;
        *stim_gen->speed = *stim_gen->speed + 1; 
      }
      else {
        stim_gen->time_step = 0;
        stim_gen->transmission_state = NEUTRAL;
      }
      *stim_gen->rpm = speed_2_rpm(1, *stim_gen->speed * -1);  // reverse has differential gear ration = 1
      break;
    }
    case NEUTRAL: {
      if (stim_gen->time_step < 3) stim_gen->time_step++;
      else {
        stim_gen->time_step = 0;
        stim_gen->transmission_state = DRIVING;
        transmission_ctrl_init(stim_gen->ctrl); // set to first gear
      }
      break;
    }
    case DRIVING: { 
      float tt;
      if (stim_gen->time_step < 62) {
        stim_gen->time_step++;
        tt = stim_gen->time_step * 0.05f;
        *stim_gen->speed = 100 * (sin(tt) + stim_gen->shaping_param * sin(3*tt));
        transmission_ctrl_update_state(stim_gen->ctrl);
      }
      else {
        stim_gen->time_step = 0;
        stim_gen->transmission_state = PARKING;
        stim_gen->shaping_param += 0.3f;
        if (stim_gen->shaping_param > 1) stim_gen->shaping_param = stim_gen->shaping_param - 1;
      }
      break;
    }
  }
}

void display_status(stimulus_gen_t *stim){
  char state_names[4][20] = { "PARKING", "REVERSE", "NEUTRAL", "DRIVING" };
  char sub_state_names[5][20] = { "FIRST_GEAR", "SECOND_GEAR", "THIRD_GEAR", "FOURTH_GEAR", "FIFTH_GEAR" };
  printf ("speed %3.2f RPM %4d ", *stim->speed, *stim->rpm);
  switch (stim->transmission_state) {
    case PARKING:
    case REVERSE:
    case NEUTRAL:
      printf ("state %s", state_names[stim->transmission_state]);
      break;
    case DRIVING:
      printf ("state %s", sub_state_names[stim->ctrl->gear]);
			printf ("\nupshifts %f %f %f %f %f", stim->ctrl->upShifts[0], stim->ctrl->upShifts[1], stim->ctrl->upShifts[2], stim->ctrl->upShifts[3], stim->ctrl->upShifts[4]);
			printf ("\ndownshifts %f %f %f %f %f", stim->ctrl->downShifts[0], stim->ctrl->downShifts[1], stim->ctrl->downShifts[2], stim->ctrl->downShifts[3], stim->ctrl->downShifts[4]);
			printf ("\nminspeed %f %f %f %f %f", stim->ctrl->minSpeed[0], stim->ctrl->minSpeed[1], stim->ctrl->minSpeed[2], stim->ctrl->minSpeed[3], stim->ctrl->minSpeed[4]);
			printf ("\nmaxspeed %f %f %f %f %f", stim->ctrl->maxSpeed[0], stim->ctrl->maxSpeed[1], stim->ctrl->maxSpeed[2], stim->ctrl->maxSpeed[3], stim->ctrl->maxSpeed[4]);
			printf ("\nminrpm %d %d %d %d %d", stim->ctrl->minRPM[0], stim->ctrl->minRPM[1], stim->ctrl->minRPM[2], stim->ctrl->minRPM[3], stim->ctrl->minRPM[4]);
			printf ("\nmaxrpm %d %d %d %d %d", stim->ctrl->maxRPM[0], stim->ctrl->maxRPM[1], stim->ctrl->maxRPM[2], stim->ctrl->maxRPM[3], stim->ctrl->maxRPM[4]);
      break;
  }
  printf ("\n");
}

