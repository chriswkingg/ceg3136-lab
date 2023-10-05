#include "transmission_ctrl.h"

void transmission_ctrl_init(transmission_ctrl_t *ctrl){
  ctrl->gear = FIRST_GEAR;
  ctrl->GRT = 3.42;   // gear ratio transmission 
  ctrl->GRD[0] = 2.97;  // gear ratio differential
  ctrl->GRD[1] = 2.07;  // gear ratio differential
  ctrl->GRD[2] = 1.43;  // gear ratio differential
  ctrl->GRD[3] = 1;  // gear ratio differential
  ctrl->GRD[4] = 0.84;  // gear ratio differential
  ctrl->CT = 1.613679;  // wheal circumference in meters
}
void transmission_ctrl_update_state(transmission_ctrl_t *ctrl){
  switch(ctrl->gear) {
		case FIRST_GEAR:
			*ctrl->rpm = (*ctrl->speed * ctrl->GRT * ctrl->GRD[0]) / (ctrl->CT * 3.6);
			if(*(ctrl->rpm) >= 2500) {
				ctrl->gear = SECOND_GEAR;
				*ctrl->rpm = 1000;
			}
			break;			
		case SECOND_GEAR:
			*ctrl->rpm = (*ctrl->speed * ctrl->GRT * ctrl->GRD[1]) / (ctrl->CT * 3.6);
			if(*(ctrl->rpm) >= 2500) {
				ctrl->gear = THIRD_GEAR;
				*ctrl->rpm = 1000;
			} else if (*(ctrl->rpm) <= 700) {
					ctrl->gear = FIRST_GEAR;
					*ctrl->rpm = 1000;
			}
			break;
		case THIRD_GEAR:
			*ctrl->rpm = (*ctrl->speed * ctrl->GRT * ctrl->GRD[2]) / (ctrl->CT * 3.6);
			if(*(ctrl->rpm) >= 2500) {
				ctrl->gear = FOURTH_GEAR;
				*ctrl->rpm = 1000;
			} else if (*(ctrl->rpm) <= 700) {
					ctrl->gear = THIRD_GEAR;
					*ctrl->rpm = 1000;
			}
			break;			
		case FOURTH_GEAR: 
			*ctrl->rpm = (*ctrl->speed * ctrl->GRT * ctrl->GRD[3]) / (ctrl->CT * 3.6);
			if(*(ctrl->rpm) >= 2500) {
				ctrl->gear = FIFTH_GEAR;
				*ctrl->rpm = 1000;
			} else if (*(ctrl->rpm) <= 700) {
					ctrl->gear = FOURTH_GEAR;
					*ctrl->rpm = 1000;
			}
			break;
		case FIFTH_GEAR:
			*ctrl->rpm = (*ctrl->speed * ctrl->GRT * ctrl->GRD[4]) / (ctrl->CT * 3.6);
			if(*(ctrl->rpm) <= 2000) {
				ctrl->gear = FOURTH_GEAR;
				*ctrl->rpm = 1000;
			}
			break;
	}
}

uint32_t speed_2_rpm(float grd, float speed){
  uint32_t _rpm;
  // speed = rpm * CT * 3.6 / 60 / GRT / GRD
  // rpm = speed * GRT * GRD * 60 / CT / 3.6 = speed * GRD * 35.32
  _rpm = (uint32_t) (35.32f * speed * grd);
  return _rpm;
}


