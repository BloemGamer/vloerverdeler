sudo chmod a+rw /dev/ttyUSB0
arduino-cli compile vloerverdeler -v -b esp32:esp32:esp32 --warnings all -u -t -p /dev/ttyUSB0 
echo -e "\n\n\n"
python3 make_yaml.py /dev/ttyUSB0
