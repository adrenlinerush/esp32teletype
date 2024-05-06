#include <fabgl.h>
#include <Arduino.h>
//#include <ESP32.h>
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
int CLOCK = GPIO_NUM_34;
int ACK = 7;

fabgl::VGATextController VGAText;
fabgl::Terminal Term;

int ready = 0;

void IRAM_ATTR receive_vcbits() {
//void receive_vcbits() {
  //if (ready == 0) {
    ready = 1;
    unsigned int data = 0;
    for (int d = 0; d < 8; d++) {
      int bit = digitalRead(DATA[d]) ? 1 : 0;
      data = (data << 1) + bit;
    }
    Term.write(data);
  
   // digitalWrite(ACK, HIGH);
   // vTaskDelay(15);
   // digitalWrite(ACK, LOW);
  
  /*} else if (digitalRead(CLOCK) == HIGH) {
    ready = 0;
    digitalWrite(ACK, LOW);
  }*/
}

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
  attachInterrupt(digitalPinToInterrupt(CLOCK), receive_vcbits, RISING);
 
  //gpio_set_intr_type(CLOCK, GPIO_INTR_POSEDGE);
  //gpio_install_isr_service(ESP_INTR_FLAG_IRAM); 
  //gpio_isr_handler_add(CLOCK, receive_vcbits, NULL);
  //pinMode(ACK, OUTPUT);
  //digitalWrite(ACK, HIGH);

  Term.write("TELETYPE READY");
  Term.write(10);
  Term.write(13);
}

void loop() {
  /*if ((digitalRead(CLOCK) == HIGH) && (ready == 0)) {
    receive_vcbits();
  } else if ((digitalRead(CLOCK) == HIGH) && (ready == 1)) {
    ready = 0;
    digitalWrite(ACK, HIGH);
  } 
  //	TaskDelay(100);*/
  /*if (ready) {
    digitalWrite(ACK, LOW);
    vTaskDelay(15);
    digitalWrite(ACK, HIGH);
  }*/
}
