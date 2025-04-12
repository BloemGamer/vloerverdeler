#pragma once
#include "settings.h"

extern double* correction_sensors[];
extern double* measurements[AMOUNT_MEASUREMENTS][AMOUNT_PINS];
extern double avarage[];


void print_yaml(void);
