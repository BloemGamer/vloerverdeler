#include "OneWire.h"
#include "DallasTemperature.h"
#include <stdint.h>

#define MAKE_YAML

#define AMOUNT_PINS 2
#define RESOLUTION 9
#define SLEEP_TIME_S 1.0

#define WIRE_PINS 25, 26

#define AMOUNT_MEASUREMENTS 100


extern uint8_t wire_pins[];
extern int device_count[];
extern byte** device_addresses[];


extern OneWire wires[];
extern DallasTemperature sensors[];
