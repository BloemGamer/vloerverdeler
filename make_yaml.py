import sys, io
import serial

PORT = sys.argv[1]
BAUD_RATE = 9600
STOP_KEYWORD = "DONE"

def monitor_serial(port, baudrate, stop_keyword):
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            with io.open("yaml.yaml", "w+", encoding="utf-8") as yaml:
                print_yaml = False
                while True:
                    if ser.in_waiting:           
                        line = ser.readline().decode(errors='ignore')
                        if(line == "yaml_start\n"):
                            print_yaml = True
                            continue
                        if(line == "yaml_end\n"):
                            print_yaml = False
                            continue
                        if(print_yaml):
                            yaml.write(line)
                        if stop_keyword in line:
                            break
                        print(line, end="")


    except serial.SerialException as e:
        print(f"Serial error: {e}")

if __name__ == "__main__":
    monitor_serial(PORT, BAUD_RATE, STOP_KEYWORD)
