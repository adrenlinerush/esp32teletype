#include <fabgl.h>
#include <Arduino.h>
#include "esp_intr_alloc.h"
#include "driver/gpio.h"
#include "soc/soc.h"
/*  

VGA Pins used by FabGL
    HSync => 23
    VSync => 15
    R0    => 21
    R1    => 22
    G0    => 18
    G1    => 19
    B0    => 4
    B1    => 5

*/


// Data Pins ASCII code 
int DATA[] = {32, 33, 25, 26, 27, 14, 12, 13};
int CLOCK = 34;


fabgl::VGATextController VGAText;
fabgl::Terminal Term;


#define BUFFER_SIZE 255
volatile uint8_t circularBuffer[BUFFER_SIZE];
volatile uint8_t readPointer = 0;
volatile uint8_t writePointer = 0;

#define INTR_NUM 31
#define CPU_CORE 1
//int clockPrevState;
//int clockState;


void IRAM_ATTR receive_vcbits(void* arg) {
    unsigned int data = 0;
    for (int d = 0; d < 8; d++) {
      int bit = digitalRead(DATA[d]) ? 1 : 0;
      data = (data << 1) + bit;
    }
    circularBuffer[writePointer] = data;
    writePointer = (writePointer + 1) % BUFFER_SIZE;
    //Term.write(data);
}


/*
void checkForInt(void* p) {
    while(1) {
      clockState = digitalRead(CLOCK);
      if (clockPrevState != clockState ) {
        if (clockState == HIGH) {
          receive_vcbits();
        }
        clockPrevState = clockState;
      }
    }
}
*/
void setup() {
  VGAText.begin();
  VGAText.setResolution(VESA_640x480_75Hz);
  Term.begin(&VGAText);
  Term.enableCursor(true);
  Term.write("\e[40;92m"); // fefault to Green on BlacK
  Term.write("\e[2J");     // clear screen
  Term.write("\e[1;1H");   // move cursor to 1,1

  for (int p = 0; p < 8; p+=1) {
    pinMode(DATA[p], INPUT);
  } 

  pinMode(CLOCK, INPUT);
  //attachInterrupt(digitalPinToInterrupt(CLOCK), receive_vcbits, RISING);

  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_POSEDGE;
  io_conf.pin_bit_mask = (1ULL << CLOCK);
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
  gpio_config(&io_conf);

  esp_err_t int_err = gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
  if (int_err != ESP_OK) { Term.write("Error installing isr service."); }
  int_err = gpio_isr_handler_add(GPIO_NUM_34, receive_vcbits, NULL);
  if (int_err != ESP_OK) { Term.write("Error adding isr handler."); }

  //ESP_INTR_DISABLE(INTR_NUM);
  //intr_matrix_set(CPU_CORE, ETS_GPIO_INTR_SOURCE, INTR_NUM);
  //ESP_INTR_ENABLE(INTR_NUM);
  
 // xTaskCreatePinnedToCore(checkForInt,"checkForInt",10000,NULL,1,NULL,1);
  Term.write("TELETYPE READY");
  Term.write(10);
  Term.write(13);
}

void loop() {
  if (readPointer != writePointer) {
    uint8_t data = circularBuffer[readPointer];
    readPointer = (readPointer + 1) % BUFFER_SIZE;
    Term.write(data);
  }
}
