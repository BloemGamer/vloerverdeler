#include "OneWire.h"
#include "DallasTemperature.h"
#include "settings.h"
#include "make_yaml.h"

uint8_t wire_pins[AMOUNT_PINS] = {WIRE_PINS};
int device_count[AMOUNT_PINS] = { 0 };

struct CharByte* device_addresses[AMOUNT_PINS];

OneWire wires[AMOUNT_PINS];
DallasTemperature sensors[AMOUNT_PINS];

void setup()
{
    DeviceAddress device_address;

    Serial.begin(9600);
    Serial.println("");
    // [11:03:21][C][gpio.one_wire:020]: GPIO 1-wire bus:
    // [11:03:21][C][gpio.one_wire:021]:   Pin: GPIO25
    // [11:03:21][C][gpio.one_wire:080]:   Found devices:
    // [11:03:21][C][gpio.one_wire:082]:     0x240000005139c728 (DS18B20)
    // [11:03:21][C][dallas.temp.sensor:029]: Dallas Temperature Sensor:
    // [11:03:21][C][dallas.temp.sensor:034]:   Address: 0x240000005139c728 (DS18B20)
    // [11:03:21][C][dallas.temp.sensor:035]:   Resolution: 12 bits
    // [11:03:21][C][dallas.temp.sensor:036]:   Update Interval: 60.0s

    Serial.printf("\nGPIO 1-wire bus:\n");

    for(int p = 0; p < AMOUNT_PINS; p++)
    {
        wires[p].begin(wire_pins[p]);
        sensors[p].setOneWire(&wires[p]);
        sensors[p].begin();
        device_count[p] = sensors[p].getDeviceCount();
        sensors[p].setResolution(RESOLUTION);        
        if(device_count[p] > 0)
        {
            device_addresses[p] = (CharByte*)malloc(device_count[p] * sizeof(CharByte));
        #ifdef MAKE_YAML
            correction_sensors[p] = (double*)malloc(device_count[p] * sizeof(double));
            memset((void*)correction_sensors[p], 0, device_count[p] * sizeof(double));

            for(int i = 0; i < AMOUNT_MEASUREMENTS; i++)
            {
                measurements[i][p] = (double*)malloc(device_count[p] * sizeof(double));
            }
        #endif // MAKE_YAML

            if(device_addresses[p] == NULL)
            {
                Serial.printf("\nfailed on %d", p);
            }
            for(int i = 0; i < device_count[p]; i++)
            {
                
                sensors[p].getAddress(device_address, i);
                device_addresses[p][i] = device_address_to_char(device_address);
                if(device_addresses[p][i].Char == NULL)
                {
                    Serial.printf("\nfailed on %d, %d", p, i);
                }
            }
        }

        
        Serial.printf("Pin: GPIO%d\n", wire_pins[p]);
        if(device_count[p] > 0)
        {
            Serial.printf("\tFound devices:\n");
        }
        else
        {
            Serial.printf("\tNo devices found\n");
        }
        for(int i = 0; i < device_count[p]; i++)
        {
            Serial.printf("\t\tAddress: 0x%s\n", device_addresses[p][i].Char);
        }
        
        
    }
    Serial.printf("Resolution: %d bits\n", RESOLUTION);
    Serial.printf("Update Interval: %.01lf\n", (double)SLEEP_TIME_S);
#ifdef MAKE_YAML
    print_yaml();
    Serial.printf("\nDONE");
#endif
}

void loop()
{
#ifndef MAKE_YAML
    for(int p = 0; p < AMOUNT_PINS; p++)
    {
        sensors[p].requestTemperatures();
        for (int i = 0;  i < device_count[p];  i++)
        {
            Serial.printf("Pin %d, sensor %d : %.2f %sC 0x%s\n", wire_pins[p], i + 1, sensors[p].getTempCByIndex(i), "\xC2\xB0", device_addresses[p][i].Char);
        }
        // Serial.println();
    }
    delay((uint32_t)(1000 * SLEEP_TIME_S)); // replace with a good sleep function for longer delay times
#endif
}

struct CharByte device_address_to_char(DeviceAddress deviceAddress)
{
    struct CharByte address;
    address.Char = (char*)malloc((2 + (8 * 2) + 1) * sizeof(char));
    address.Hex = (byte*)malloc(8 * sizeof(byte));
    char tmp[3];

    memset(address.Char, 0, (2 + (8 * 2) + 1) * sizeof(char));
    memset(address.Hex, 0, 8 * sizeof(char));
    for(int8_t i = 7; i >= 0; i--)
    {
        address.Hex[i] = deviceAddress[i];
        sprintf(tmp, "%02x", deviceAddress[i]);
        strncat(address.Char, tmp, 2);
    }
    return address;
}
