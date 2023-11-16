
#include "svc.h"
#include "investor.h"

void SVC_Handler_c (void *obj, unsigned int svc_number) {                    // SVC Interrupt Handler
  //printf("svc_number = %d\n", svc_number);                                     // Increment Counter
  switch (svc_number) {
    case 0: __SVC_0(obj); break;
    case 1: __SVC_1(obj); break;
    case 2: __SVC_2(obj); break;
    case 3: __SVC_3(obj); break;
  }
}



void __SVC_0( void *obj){
  char *str = (char*) obj;
  printf("svc_zero: %s\n", str);
}
void __SVC_1( void *obj){
  char *str = (char*) obj;
  printf("svc_one: %s\n", str);
}
void __SVC_2( void *obj){
	investor_stock_holding_t* ish = (investor_stock_holding_t*) obj;
	if(obj == NULL) {
		return;
	}
	investor_buy(ish->investor, ish->stock_holding);
}
void __SVC_3( void *obj){
	investor_stock_holding_t* ish = (investor_stock_holding_t*) obj;
	if(obj == NULL) {
		return;
	}
	investor_sell(ish->investor, ish->stock_holding);
}	

