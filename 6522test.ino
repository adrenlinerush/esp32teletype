int DATA[] = {30,32,34,36,38,40,42,44};
int CLOCK = 46;
int CS1 = 48;
int CS2B = 50;
int RWB = 52;
int RS[]= {22, 24, 26, 28};
int RESET = 2;

int dly = 1;
int tck = true;
int escape = 0;

char output[30];

void setRS(int rs3, int rs2, int rs1, int rs0) {
  digitalWrite(RS[3], rs3);
  digitalWrite(RS[2], rs2);
  digitalWrite(RS[1], rs1);
  digitalWrite(RS[0], rs0);

  sprintf(output, "RS: %i%i%i%i\n", rs3, rs2, rs1, rs0);
  Serial.print(output);
}

void setDataPins(int d7, int d6, int d5, int d4, int d3, int d2, int d1, int d0){
  Serial.write("setDataPins\n");
  digitalWrite(DATA[7], d7);
  Serial.write("Pin 7\n");
  digitalWrite(DATA[6], d6);
  Serial.write("Pin 6\n");
  digitalWrite(DATA[5], d5);
  Serial.write("Pin 5\n");
  digitalWrite(DATA[4], d4);
  Serial.write("Pin 4\n");
  digitalWrite(DATA[3], d3);
  Serial.write("Pin 3\n");
  digitalWrite(DATA[2], d2);
  Serial.write("Pin 2\n");
  digitalWrite(DATA[1], d1);
  Serial.write("Pin 1\n");
  digitalWrite(DATA[0], d0);
  Serial.write("Pin 0\n");

  //sprintf(output, "DATA: %s%s%s%s%s%s%s%s", d7, d6, d5, d4, d3, d2, d1);
  //Serial.print(output);
}

void tick(){
  tck=!tck;
  if (tck) {Serial.print("HIGH");}else{Serial.print("LOW");}
  Serial.print("\n");
  digitalWrite(CLOCK,tck);
  delay(dly);
}

void setBToOutput() {
  tick();
  digitalWrite(CS1, HIGH);
  Serial.print("CS1 HIGH\n");
  digitalWrite(CS2B, LOW);
  Serial.print("CS2B LOW\n");
  digitalWrite(RWB, LOW);
  Serial.print("RWB LOW\n");

  tick();
  tick();

  // Set Data direction to output all pins 
  setRS(0,0,1,0);
  setDataPins(1,1,1,1,1,1,1,1);
  tick();

}
void setup() {
  Serial.begin(115200);
  for (int n=0; n < 8; n++) {
    pinMode(DATA[n], OUTPUT);
  }
  pinMode(CLOCK, OUTPUT);
  pinMode(CS1, OUTPUT);
  pinMode(CS2B, OUTPUT);
  delay(dly);
  pinMode(RWB, OUTPUT);
  for (int n=0; n < 4; n++) {
    pinMode(RS[n], OUTPUT);
  }
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  for (int i = 0; i < 14; i++) { tick(); }
  digitalWrite(RESET, HIGH);

  setBToOutput();
}

void sendByte(char c) { 
  tick();
  // Set RS back to Register B
  setRS(0,0,0,0);
  for (int i = 7; i >= 0; i--) {
        unsigned int bit = (c >> i) & 1;
        Serial.print(bit);
        if (bit == 1) {
          digitalWrite(DATA[i], HIGH);
        } else {
          digitalWrite(DATA[i], LOW);
        }
    }
    Serial.print("\n");
  tick();

  tick();
  // Set Peripheral Control Register to high
  setRS(1,1,0,0);
  setDataPins(1,1,1,0,0,0,0,0);
  tick();

  tick();
  // Set Peripheral Control Register to low
  setRS(1,1,0,0);
  setDataPins(1,1,0,0,0,0,0,0);
  tick();
}

void sendCode(unsigned char b) {
  switch (b) {
    // e
    case 101:
      //escape
      sendByte(27);
      break;
    // n
    case 110:
      //newline/linefeed
      sendByte(10);
      break;
    // b
    case 90:
      //backspace
      sendByte(8);
      break;
    //t
    case 116:
      //tab
      sendByte(9);
      break;
    //r
    case 114:
      //carriage return
      sendByte(13);
      break;
  } 
}

void loop() {
  
  if (Serial.available() > 0) {
    Serial.print("Got a Byte.\n");
    unsigned char b = Serial.read();
    if (b == 92) {
      Serial.print("Escape code detected.\n");
      escape = 1;
    } else if (escape == 1) {
      escape = 0;
      sendCode(b);
    } else {
      sendByte(b);
    }
  }
  tick();
  setRS(0,0,0,0);
  setDataPins(0,0,0,0,0,0,0,0);  
  tick();
 
   

}
