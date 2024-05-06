LIBS = /home/austin/src/esp32vga32/FabGL/src /home/austin/src/esp32VideoCard/6502TeletypeFasteInterrupt.S
SKETCH = /home/austin/src/esp32VideoCard/videoCardAssyIRQ.ino
UPLOAD_PORT = /dev/ttyUSB0
CHIP = esp32
ESP_ROOT = /home/austin/src/esp32vga32/esp32

include /home/austin/src/esp32vga32/makeEspArduino/makeEspArduino.mk
