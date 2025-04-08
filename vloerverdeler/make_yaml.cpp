#include "Esp.h"
#include "make_yaml.h"
#include "settings.h"

double* correction_sensors[AMOUNT_PINS];
double* measurements[AMOUNT_MEASUREMENTS][AMOUNT_PINS];
double avarage[AMOUNT_MEASUREMENTS];


static void fix_data_yaml(void);


static void fix_data_yaml(void)
{
    double avar;
    for(int m = 0; m < AMOUNT_MEASUREMENTS; m++)
    {
        int right = 0;
        for(int p = 0; p < AMOUNT_PINS; p++)
        {
            sensors[p].requestTemperatures();
            for (int i = 0;  i < device_count[p];  i++)
            {
                //Serial.printf("Pin %d, sensor %d : %.2f %sC 0x%s\n", wire_pins[p], i + 1, sensors[p].getTempCByIndex(i), "\xC2\xB0", device_addresses[p][i]);
                int tmp = sensors[p].getTempC(device_addresses[p][i]);
                if(tmp < 70)
                {
                    right++;
                    measurements[m][p][i] = tmp;
                    avarage[m] += tmp;
                }
            }
        }
        if(right)
            avarage[m] /= right;
        delay((int)SLEEP_TIME_S);
    }

    avar = 0;
    for(int m = 0; m < AMOUNT_MEASUREMENTS; m++)
    {
        avar += avarage[m];
    }
    avar /= AMOUNT_MEASUREMENTS;
    
    for(int p = 0; p < AMOUNT_PINS; p++)
    {
        for (int i = 0;  i < device_count[p];  i++)
        {
            int right = 0;
            int sum = 0;
            for(int m = 0; m < AMOUNT_MEASUREMENTS; m++)
            {
                if(measurements[m][p][i] < 70)
                {
                    sum += measurements[m][p][i];
                    right++;
                }
            }
            if(right)
                sum /= right;
            correction_sensors[p][i] = sum - avar;
        }
    }
}

void print_yaml(void)
{
    fix_data_yaml();
}
