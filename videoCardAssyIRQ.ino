#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_intr_alloc.h"
#include <fabgl.h>

fabgl::VGATextController VGAText;
fabgl::Terminal Term;


#define BUFFER_SIZE 128
volatile uint8_t circularBuffer[BUFFER_SIZE];
volatile uint8_t readPointer = 0;
volatile uint8_t writePointer = 0;

typedef gpio_int_type_t o_gpio_int_type_t;

uint8_t readFromCircularBuffer() {
  if (readPointer != writePointer) {
    uint8_t data = circularBuffer[readPointer];
    readPointer = (readPointer + 1) % BUFFER_SIZE;
    return data;
  }
  return 0; // Indicate buffer is empty
}

void setupTask(void *pvParameters) {
  VGAText.begin();
  VGAText.setResolution(VESA_640x480_75Hz);
  Term.begin(&VGAText);
  Term.enableCursor(true);
  Term.write("\e[40;92m"); // fefault to Green on BlacK
  Term.write("\e[2J");     // clear screen
  Term.write("\e[1;1H");   // move cursor to 1,1

  // Define GPIO pins to be configured as inputs
  const gpio_num_t inputPins[] = {GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26,
                                  GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12, GPIO_NUM_13};

    // Configure GPIO pins as inputs
  gpio_config_t io_conf;
  io_conf.mode = GPIO_MODE_INPUT;

  for (int i = 0; i < sizeof(inputPins) / sizeof(gpio_num_t); i++) {
    io_conf.pin_bit_mask = (1ULL << inputPins[i]);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);
  }

  // Install ISR service with default configuration
  //gpio_install_isr_service(0);

  // Attach interrupt to pin 34
  gpio_set_intr_type(GPIO_NUM_34, GPIO_INTR_POSEDGE); // or GPIO_INTR_NEGEDGE for falling edge
  //gpio_isr_handler_add(GPIO_NUM_34, gpio_isr_handler, NULL);

  while (true) {
    uint8_t c = readFromCircularBuffer();
    if ( c > 0 ) {
      Term.write(c);
    }
  }
}

void setup() {
  xTaskCreatePinnedToCore(setupTask, "setupTask", 4096, NULL, 1, NULL, 0);
}

void loop() {
}
