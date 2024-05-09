LIBS = /home/austin/src/esp32vga32/FabGL/src 
#EXTRA_SRCS = /home/austin/src/esp32VideoCard/highint5.S
SKETCH = /home/austin/src/esp32VideoCard/videoCard.ino
UPLOAD_PORT = /dev/ttyUSB0
CHIP = esp32
ESP_ROOT = /home/austin/src/esp32vga32/esp32
include /home/austin/src/esp32vga32/makeEspArduino/makeEspArduino.mk
