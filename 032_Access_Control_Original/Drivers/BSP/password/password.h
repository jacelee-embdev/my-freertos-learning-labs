#ifndef __PASSWORD_H__
#define __PASSWORD_H__

#include "sys.h"


#define POUND_KEY   '#'
#define START_KEY   '*'

#define TRUE    1
#define FALSE   0

void password_init(void);
void password_input_clear(void);
void password_save(void);
uint8_t password_get_input(void);
uint8_t password_compare(void);
void password_set_input(uint8_t key_val);
void password_input_right_operate(void);
void password_input_wrong_operate(void);
void password_old_right_operate(void);
void password_old_wrong_operate(void);
void password_check(void);

#endif
