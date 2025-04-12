#pragma once
#include "OneWire.h"
#include "DallasTemperature.h"
#include <stdint.h>

// define MAKE_YAML if you want to print the yaml, the AMOUNT_MEASUREMENTS is only used with the make yaml
#define MAKE_YAML
#define AMOUNT_MEASUREMENTS 10
#define DELAY_TIME_YAML "60s"

#define RESOLUTION 12
#define SLEEP_TIME_S 1.0

#define AMOUNT_PINS 5
#define WIRE_PINS 32, 33, 25, 26, 27



extern uint8_t wire_pins[];
extern int device_count[];

struct CharByte
{
    char* Char;
    byte* Hex;
};
extern struct CharByte* device_addresses[AMOUNT_PINS];


extern OneWire wires[];
extern DallasTemperature sensors[];
