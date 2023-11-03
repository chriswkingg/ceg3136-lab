#include "stm32f4xx.h"
#include "sensor.h"
#include "alarm.h"
#include "user.h"
#include "super_user.h"
#include "alarm_system.h"

#include "stdio.h"
#include "stdlib.h"
#include <string.h>

void alarm_system_display (alarm_system_t *system){
  printf ("The system is now ");
  switch (system->state) {
    case UNARMED:         printf ("UNARMED");         break;
    case WAITING_TO_ARM:  printf ("WAITING_TO_ARM");  break;
    case ARMED:           printf ("ARMED");           break;
    case ALARMED:         printf ("ALARMED");           break;
  }
  printf ("\n");
}


void alarm_system_init (alarm_system_t *system){
  memset(system->sensor_list, 0, sizeof(system->sensor_list));
  memset(system->alarm_list, 0, sizeof(system->alarm_list));
  memset(system->user_list, 0, sizeof(system->user_list));
  memset(system->super_list, 0, sizeof(system->super_list));
  memset(system->fsm_coverage, 0, sizeof(system->fsm_coverage));
  system->fsm_coverage[UNARMED][UNARMED] = 1;  // start at idle state
  {
    int i, j;
    char buffer[28];
    char passcode[8];
    sensor_t *sensor = system->sensor_list;
    alarm_t *alarm = system->alarm_list;
    user_t * user = system->user_list;
    super_user_t *super = system->super_list;

    system->state = UNARMED;

    for (i=0; i<8; i++) {
      for (j=0; j<8; j++) {
        sprintf(buffer, "FLOOR%d-ROOM#%d", i+1, j+1);
        sensor_init ( sensor, 2001+i*10+j, buffer);
        alarm_init (alarm, 4001+i*10+j, buffer);
        sensor++;
        alarm++;
      }
      sprintf(buffer, "user#%d", i+1);
      sprintf(passcode, "user%d%d%d", i+1, i+2, i+3);
      user_init (user, 1000+i+1, passcode, buffer);
      user++;
      sprintf(buffer, "super%d", i+1);
      sprintf(passcode, "super%d%d", i+1, i+2);
      super_user_init (super, i+1, passcode, buffer);
      super++;
    }
  }
  system->usr_logged_in = NULL;
  alarm_system_display (system);
}

user_t *system_get_user(alarm_system_t *system, char* passcode){
  int i;
  char str[10];
  char *c;
  user_t *user = system->user_list;
  super_user_t *super = system->super_list;

  strncpy(str, passcode, 10);
  c = str;
  for (i=0; i<10; i++) {
    if ((*c) == 0x0A) *c = '\0';  // replace line-feed with end-of-string
    c++;
  }

  for (i=0; i<8; i++) {
    if (strcmp(str, super->usr.pass_code) == 0) break;
    super++;
  }
  if (i < 8) return &(super->usr);

  for (i=0; i<64; i++) {
    if (strcmp(str, user->pass_code) == 0) break;
    user++;
  }
  if (i >= 64) user = NULL;
  return user;
}

void system_user_login_event(alarm_system_t *system, user_t *user){
	printf("user %s has just logged in \n", user->name);
  system->usr_logged_in = user;
}

void system_fsm_coverage_update (alarm_system_t *system){
  if (system->state != system->prev_state) {  // transition detected
    alarm_system_display (system);
    system->fsm_coverage[system->state][system->state]++;       // state coverage
    system->fsm_coverage[system->prev_state][system->state]++;  // transition coverage
  }
  system->prev_state = system->state; // update prev_state
}

void system_fsm_coverage_display (alarm_system_t *system){
  char state_names[4][20] = {"UNARMED", "ARMING ", "ARMED  ", "ALARMED"};
  int i, j;
  printf ("FSM State Coverage:\n");
  for (i=0; i<4; i++) {
    printf ("%s ", state_names[i]);
    for (j=0; j<4; j++) {
      printf ("%d\t", system->fsm_coverage[i][j]);
    }
    printf("\n");
  }
}

void system_alarm_set (alarm_system_t *system){
  int i;
  alarm_t *alarm = system->alarm_list;
  for (i=0; i<64; i++) {
    alarm_set (alarm);
    alarm++;
  }
}
void system_alarm_reset (alarm_system_t *system){
  int i;
  alarm_t *alarm = system->alarm_list;
  for (i=0; i<64; i++) {
    alarm_reset (alarm);
    alarm++;
  }
}

void system_update_state(alarm_system_t *system){
  int i;
	switch (system->state){
    case UNARMED: 
			if(system->usr_logged_in != NULL) {
				system->tick_cnt = 0;
				system->state = WAITING_TO_ARM;
			}
      break;
    case WAITING_TO_ARM:
		if(system->tick_cnt > 5000) {
				system->state = ARMED;
				system->usr_logged_in = NULL;
			}
			system->tick_cnt++;
      break;
    case ARMED: 
			if(*system->sensor_states > 0) {
				system->state = ALARMED;
				for(i = 0; i < 64; i++) {
					system->alarm_list[i].state = ALARM_ON;
				}
			}
			if(system->usr_logged_in != NULL) {
				system->state = UNARMED;
				system->usr_logged_in = NULL;
			}
      break;
    case ALARMED: 
			if(system->usr_logged_in != NULL && system->usr_logged_in->is_super_user) {
				system->state = UNARMED;
				system->usr_logged_in = NULL ;
				for(i = 0; i < 64; i++) {
					system->alarm_list[i].state = ALARM_OFF;
				}
			}
      break;
  }
  system_fsm_coverage_update (system);
}
