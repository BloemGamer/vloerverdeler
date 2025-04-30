#include "Esp.h"
#include "make_yaml.h"
#include "settings.h"
#include "private_settings.h"

double* correction_sensors[AMOUNT_PINS];
double* measurements[AMOUNT_MEASUREMENTS][AMOUNT_PINS];
double average[AMOUNT_MEASUREMENTS] = { 0 };


static void fix_data_yaml(void);


static void fix_data_yaml(void)
{
    double aver;
    for(int m = 0; m < AMOUNT_MEASUREMENTS; m++)
    {
        int right = 0;
        for(int p = 0; p < AMOUNT_PINS; p++)
        {
            sensors[p].requestTemperatures();
            for (int i = 0;  i < device_count[p];  i++)
            {
                //Serial.printf("Pin %d, sensor %d : %.2f %sC 0x%s\n", wire_pins[p], i + 1, sensors[p].getTempCByIndex(i), "\xC2\xB0", device_addresses[p][i]);
                measurements[m][p][i] = sensors[p].getTempC(device_addresses[p][i].Hex);
                if(measurements[m][p][i] < 70)
                {
                    right++;
                    average[m] += measurements[m][p][i];
                }
            }
        }
        if(right)
            average[m] /= right;
    }

    aver = 0;
    for(int m = 0; m < AMOUNT_MEASUREMENTS; m++)
    {
        aver += average[m];
    }
    aver /= AMOUNT_MEASUREMENTS;
    
    for(int p = 0; p < AMOUNT_PINS; p++)
    {
        for (int i = 0;  i < device_count[p];  i++)
        {
            int right = 0;
            double sum = 0;
            for(int m = 0; m < AMOUNT_MEASUREMENTS; m++)
            {
                if(measurements[m][p][i] < 70)
                {
                    sum += measurements[m][p][i];
                    right++;
                }
            }
            if(right)
            {
                sum /= right;
                correction_sensors[p][i] = -(sum - aver);
                Serial.printf("%0.2lf, %0.2lf, HEX: %s\n", sum, aver, device_addresses[p][i].Char);
            }
        }
    }
}


// 0iSerial.printf(�kD"�kDA\n");V=j
// :%s/Serial.printf("\\n");/Serial.printf("\\n");\r
void print_yaml(void)
{
    fix_data_yaml();

    Serial.println("\n\n\n\n\n\n\n\n\n\n");
    Serial.printf("yaml_start\n");

    Serial.printf("substitutions:\n");

    Serial.printf((strncmp(WIFI_NAME,"!secret",7) == 0 ? "  wifi_name: %s\n" : "  wifi_name: \"%s\"\n"), WIFI_NAME);
    Serial.printf((strncmp(WIFI_PASSWORD,"!secret",7) == 0 ? "  wifi_password: %s\n" : "  wifi_password: \"%s\"\n"), WIFI_PASSWORD);
    Serial.printf((strncmp(ENCRYPTION_KEY,"!secret",7) == 0 ? "  encryption_key: %s\n" : "  encryption_key: \"%s\"\n"), ENCRYPTION_KEY);
    
    Serial.printf("  esp32_name: \"esphome-web-6be034\"\n");
    Serial.printf("  esp32_friendly_name: \"vloerverwarming\"\n");
    Serial.printf("\n");

    for(int p = 0; p < AMOUNT_PINS; p++)
    {
        for(int i = 0;  i < device_count[p];  i++)
        {
            Serial.printf("  name_group_%d_sensor_%c: \"one_wire_%d%c\"\n", p + 1, i + 'a', p + 1, i + 'a');
            Serial.printf("  corr_group_%d_sensor_%c: \"%.2lf\"\n", p + 1, i + 'a', correction_sensors[p][i]);
        }
    }
    Serial.printf("\n");

    Serial.printf("esphome:\n");
    Serial.printf("  name: ${esp32_name}\n");
    Serial.printf("  friendly_name: ${esp32_friendly_name}\n");
    Serial.printf("  min_version: 2024.11.0\n");
    Serial.printf("  name_add_mac_suffix: false\n");
    Serial.printf("\n");

    Serial.printf("esp32:\n");
    Serial.printf("  board: esp32dev\n");
    Serial.printf("  framework:\n");
    Serial.printf("    type: esp-idf\n");
    Serial.printf("\n");

    Serial.printf("# deep sleep config\n");
    Serial.printf("deep_sleep:\n");
    Serial.printf("  run_duration: 20s\n");
    Serial.printf("  sleep_duration: 300s\n");
    Serial.printf("  wakeup_pin: GPIO34\n");
    Serial.printf("  wakeup_pin_mode: KEEP_AWAKE\n");
    Serial.printf("\n");

    Serial.printf("output:\n");
    Serial.printf("  - platform: ledc\n");
    Serial.printf("    id: o_blue_led\n");
    Serial.printf("    pin:\n");
    Serial.printf("      number: GPIO2\n");
    Serial.printf("      inverted: false\n");
    Serial.printf("      mode: OUTPUT\n");
    Serial.printf("\n");

    Serial.printf("\n");

    Serial.printf("one_wire:\n");

    for(int p = 0; p < AMOUNT_PINS; p++)
    {
        Serial.printf("  - platform: gpio\n");
        Serial.printf("    pin: GPIO%d\n", wire_pins[p]);
        Serial.printf("    id: pin%d\n", wire_pins[p]);
    }
    Serial.printf("\n");

    Serial.printf("sensor:\n");
    for(int p = 0; p < AMOUNT_PINS; p++)
    {
        for (int i = 0;  i < device_count[p];  i++)
        {
            Serial.printf("- platform: dallas_temp\n");
            Serial.printf("  one_wire_id: pin%d\n", wire_pins[p]);
            Serial.printf("  address: 0x%s\n", device_addresses[p][i].Char);
            Serial.printf("  name: ${name_group_%d_sensor_%c}\n", p + 1, i + 'a');
            Serial.printf("  unit_of_measurement: \"°C\"\n");
            Serial.printf("  icon: \"mdi:thermometer\"\n");
            Serial.printf("  device_class: \"temperature\"\n");
            Serial.printf("  state_class: \"measurement\"\n");
            Serial.printf("  accuracy_decimals: 2\n");
            Serial.printf("  update_interval: %s\n", DELAY_TIME_YAML);
            Serial.printf("  resolution: %d\n", RESOLUTION);
            Serial.printf("  filters:\n");
            Serial.printf("  - offset: ${corr_group_%d_sensor_%c}\n", p + 1, i + 'a');
            Serial.printf("  - clamp:\n");
            Serial.printf("      min_value: -20\n");
            Serial.printf("      max_value: 75\n");
            Serial.printf("      ignore_out_of_range: true\n");
            Serial.printf("  on_value:\n");
            Serial.printf("      then:\n");
            Serial.printf("        - output.turn_on: o_blue_led\n");
            Serial.printf("        - delay: 0.2s\n");
            Serial.printf("        - output.turn_off: o_blue_led\n");
            Serial.printf("        - delay: 0.1s\n");
            Serial.printf("\n");
        }
    }

    Serial.printf("light:\n");
    Serial.printf("  - platform: monochromatic\n");
    Serial.printf("    id: blue_led\n");
    Serial.printf("    output: o_blue_led\n");
    Serial.printf("    name: \"blue led\"\n");
    Serial.printf("\n");
    Serial.printf("\n");

    Serial.printf("# Enable logging\n");
    Serial.printf("logger:\n");
    Serial.printf("\n");

    Serial.printf("# Enable Home Assistant API\n");
    Serial.printf("api:\n");
    Serial.printf("  encryption: \n");
    Serial.printf("    key: ${encryption_key}\n");
    Serial.printf("\n");

    Serial.printf("# Allow Over-The-Air updates\n");
    Serial.printf("ota:\n");
    Serial.printf("  - platform: esphome\n");
    Serial.printf("\n");

    Serial.printf("wifi:\n");
    Serial.printf("  ssid: ${wifi_name}\n");
    Serial.printf("  password: ${wifi_password}\n");

    Serial.printf("yaml_end\n");
}

