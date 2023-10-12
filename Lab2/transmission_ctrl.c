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
	ctrl->minSpeed[0] = 999;
	ctrl->minSpeed[1] = 999;
	ctrl->minSpeed[2] = 999;
	ctrl->minSpeed[3] = 999;
	ctrl->minSpeed[4] = 999;
	ctrl->minRPM[0] = 99999;
	ctrl->minRPM[1] = 99999;
	ctrl->minRPM[2] = 99999;
	ctrl->minRPM[3] = 99999;
	ctrl->minRPM[4] = 99999;
}
void transmission_ctrl_update_state(transmission_ctrl_t *ctrl){
  int rpm = speed_2_rpm(*ctrl->speed, ctrl->GRD[ctrl->gear]);
	int gear = (int)ctrl->gear;
	*(ctrl->rpm) = rpm;
	
	if(*ctrl->rpm > ctrl->maxRPM[ctrl->gear]) {
		ctrl->maxRPM[ctrl->gear] = *ctrl->rpm;
	}  
	if (*ctrl->rpm < ctrl->minRPM[ctrl->gear]) {
		ctrl->minRPM[ctrl->gear] = *ctrl->rpm;
	} 
	
	if (*ctrl->speed > ctrl->maxSpeed[ctrl->gear]) {
		ctrl->maxSpeed[ctrl->gear] = *ctrl->speed;
	}  
	if (*ctrl->speed < ctrl->minSpeed[ctrl->gear]) {
		ctrl->minSpeed[ctrl->gear] = *ctrl->speed;
	}
	
	switch(ctrl->gear) {
		case FIRST_GEAR:
			if(*(ctrl->rpm) >= 2500) {
				ctrl->upShifts[ctrl->gear]++;
				ctrl->gear = SECOND_GEAR;
			}
			break;			
		case SECOND_GEAR:
			if(*(ctrl->rpm) >= 2500) {
				ctrl->upShifts[ctrl->gear]++;
				ctrl->gear = THIRD_GEAR;

			} else if (*(ctrl->rpm) <= 700) {
				ctrl->downShifts[ctrl->gear]++;	
				ctrl->gear = FIRST_GEAR;
				
			}
			break;
		case THIRD_GEAR:
			if(*(ctrl->rpm) >= 2500) {
				ctrl->upShifts[ctrl->gear]++;
				ctrl->gear = FOURTH_GEAR;
				
			} else if (*(ctrl->rpm) <= 700) {
				ctrl->downShifts[ctrl->gear]++;
					ctrl->gear = SECOND_GEAR;
				

			}
			break;			
		case FOURTH_GEAR: 
			if(*(ctrl->rpm) >= 2500) {
				ctrl->upShifts[ctrl->gear]++;
				ctrl->gear = FIFTH_GEAR;
			
			} else if (*(ctrl->rpm) <= 700) {
				ctrl->downShifts[ctrl->gear]++;
					ctrl->gear = THIRD_GEAR;
			
			}
			break;
		case FIFTH_GEAR:
			if(*(ctrl->rpm) <= 2000) {
				ctrl->downShifts[ctrl->gear]++;
				ctrl->gear = FOURTH_GEAR;
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


