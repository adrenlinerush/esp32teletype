

int DATA[] = {30,32,34,36,38,40,42,44};
int CLOCK = 46;
int CS1 = 48;
int CS2B = 50;
int RWB = 52;
int RS[]= {22, 24, 26, 28};
int RESET = 2;
int IRQ = 3;

int dly = .016;
int tck = true;
int escape = 0;
int rel = false;
int shift = false;

char kbd[256];

char output[30];

char ps2ToAsciiConvert(int scanCode) {
  if (shift) {
    switch (scanCode) {
        case 0x76: return 27; //Escape 
        case 0x0E: return '~';
        case 0x16: return '!';
        case 0x1E: return '@';
        case 0x26: return '#';
        case 0x25: return '$';
        case 0x2E: return '%';
        case 0x36: return '^';
        case 0x3D: return '&';
        case 0x3E: return '*';
        case 0x46: return '(';
        case 0x45: return ')';
        case 0x4E: return '_';
        case 0x55: return '+';
        case 0x66: return  8; //Backspace
        case 0x0D: return 14;
        case 0x15: return 'Q';
        case 0x1D: return 'W';
        case 0x24: return 'E';
        case 0x2D: return 'R';
        case 0x2C: return 'T';
        case 0x35: return 'Y';
        case 0x3C: return 'U';
        case 0x43: return 'I';
        case 0x44: return 'O';
        case 0x4D: return 'P';
        case 0x54: return '{';
        case 0x5B: return '}';
        case 0x5D: return '|';
        case 0x1C: return 'A';
        case 0x1B: return 'S';
        case 0x23: return 'D';
        case 0x2B: return 'F';
        case 0x34: return 'G';
        case 0x33: return 'H';
        case 0x3B: return 'J';
        case 0x42: return 'K';
        case 0x4B: return 'L';
        case 0x4C: return ':';
        case 0x52: return '"';
        case 0x5A: return 12; //Enter
        case 0x1A: return 'Z';
        case 0x22: return 'X';
        case 0x21: return 'C';
        case 0x2A: return 'V';
        case 0x32: return 'B';
        case 0x31: return 'N';
        case 0x3A: return 'M';
        case 0x41: return '<';
        case 0x49: return '>';
        case 0x4A: return '?';
        case 0x29: return ' ';
        default: return '\0'; // Unrecognized scan code
    }
  }
    switch (scanCode) {
        case 0x76: return 27; //Escape 
        case 0x0E: return '`';
        case 0x16: return '1';
        case 0x1E: return '2';
        case 0x26: return '3';
        case 0x25: return '4';
        case 0x2E: return '5';
        case 0x36: return '6';
        case 0x3D: return '7';
        case 0x3E: return '8';
        case 0x46: return '9';
        case 0x45: return '0';
        case 0x4E: return '-';
        case 0x55: return '=';
        case 0x66: return  8; //Backspace
        case 0x0D: return '\t';
        case 0x15: return 'q';
        case 0x1D: return 'w';
        case 0x24: return 'e';
        case 0x2D: return 'r';
        case 0x2C: return 't';
        case 0x35: return 'y';
        case 0x3C: return 'u';
        case 0x43: return 'i';
        case 0x44: return 'o';
        case 0x4D: return 'p';
        case 0x54: return '[';
        case 0x5B: return ']';
        case 0x5D: return '\\';
        case 0x1C: return 'a';
        case 0x1B: return 's';
        case 0x23: return 'd';
        case 0x2B: return 'f';
        case 0x34: return 'g';
        case 0x33: return 'h';
        case 0x3B: return 'j';
        case 0x42: return 'k';
        case 0x4B: return 'l';
        case 0x4C: return ';';
        case 0x52: return '\'';
        case 0x5A: return 12; //Enter
        case 0x1A: return 'z';
        case 0x22: return 'x';
        case 0x21: return 'c';
        case 0x2A: return 'v';
        case 0x32: return 'b';
        case 0x31: return 'n';
        case 0x3A: return 'm';
        case 0x41: return ',';
        case 0x49: return '.';
        case 0x4A: return '/';
        case 0x29: return ' ';
        default: return '\0'; // Unrecognized scan code
    }
}

void addKbdBuffer(char c) {
  int len = strlen(kbd);
  kbd[len] = c;
  kbd[len+1]='\0';
}

char popKbdBuffer() {
  int len = strlen(kbd);
  char c = kbd[0];
  for (int i = 0; i < len -1; i++) {
    kbd[i] = kbd[i+1];
  }
  kbd[len-1] = '\0';
  return c;
}

void setRS(int rs3, int rs2, int rs1, int rs0) {
  digitalWrite(RS[3], rs3);
  digitalWrite(RS[2], rs2);
  digitalWrite(RS[1], rs1);
  digitalWrite(RS[0], rs0);

  //sprintf(output, "RS: %i%i%i%i\n", rs3, rs2, rs1, rs0);
  //Serial.print(output);
}

void setDataPins(int d7, int d6, int d5, int d4, int d3, int d2, int d1, int d0){
  //Serial.write("setDataPins\n");
  digitalWrite(DATA[7], d7);
  //Serial.write("Pin 7\n");
  digitalWrite(DATA[6], d6);
  //Serial.write("Pin 6\n");
  digitalWrite(DATA[5], d5);
  //Serial.write("Pin 5\n");
  digitalWrite(DATA[4], d4);
  //Serial.write("Pin 4\n");
  digitalWrite(DATA[3], d3);
  //Serial.write("Pin 3\n");
  digitalWrite(DATA[2], d2);
  //Serial.write("Pin 2\n");
  digitalWrite(DATA[1], d1);
  //Serial.write("Pin 1\n");
  digitalWrite(DATA[0], d0);
  //Serial.write("Pin 0\n");

  //sprintf(output, "DATA: %s%s%s%s%s%s%s%s", d7, d6, d5, d4, d3, d2, d1);
  //Serial.print(output);
}

void tick(){
  tck=!tck;
  //if (tck) {Serial.print("HIGH");}else{Serial.print("LOW");}
  //Serial.print("\n");
  digitalWrite(CLOCK,tck);
  delay(dly);
}

void setToRead() {
  tick();
  digitalWrite(RWB, HIGH);
  Serial.print("RWB HIGH\n");
  tick();
}

void setBToOutput() {
  tick();
  // Set Data direction to output all pins B
  setRS(0,0,1,0);
  setDataPins(1,1,1,1,1,1,1,1);
  tick();
}

void setToWrite() {
  tick();
  digitalWrite(RWB, LOW);
  Serial.print("RWB LOW\n");
  tick();
}

void setAToInput() {
  tick();
  // Set Data direction to read all pins A
  setRS(0,0,1,1);
  setDataPins(0,0,0,0,0,0,0,0);
  tick();
  tick();
  // enable the CA2 Interrupt
  setRS(1,1,1,0);
  setDataPins(0,0,0,0,0,0,1,0);
  tick();
}

void enableChip() {
  // set chip select
  digitalWrite(CS1, HIGH);
  Serial.print("CS1 HIGH\n");
  digitalWrite(CS2B, LOW);
  Serial.print("CS2B LOW\n");
}

void setDataDirection(int dir) {
  for (int n=0; n < 8; n++) {
    pinMode(DATA[n], dir);
  }
}

void readKeyboard() {
  tick();
  setDataDirection(INPUT);
  setToRead();
  setRS(0,0,0,1);
  tick();

  unsigned int data = 0;
  for (int n=7; n >= 0; n--) {
    int bit = digitalRead(DATA[n]) ? 1 : 0;
    Serial.print(bit);
    data = (data << 1) + bit;
  }
  sprintf(output, "\t%02x\n", data);
  Serial.print(output);

  tick();
  setToWrite();
  setDataDirection(OUTPUT);
  tick();
  if (data == 0x12 || data == 0x59) { shift = true; }
  if (data == 0xF0) { rel = true; } 
  char c = ps2ToAsciiConvert(data);
  if (rel == true && (data == 0x12 || data == 59)) { shift = false; rel = false;}
  if (c) {
    if ( !rel ) {
      Serial.print(c);
      Serial.print("\n");
      
      addKbdBuffer(c);
      if (c == 12) { addKbdBuffer('\r'); } 
    }
    rel = false;
  }
}

void setup() {
  Serial.begin(115200);
  setDataDirection(OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(CS1, OUTPUT);
  pinMode(CS2B, OUTPUT);
  delay(dly);
  pinMode(RWB, OUTPUT);
  for (int n=0; n < 4; n++) {
    pinMode(RS[n], OUTPUT);
  }
  pinMode(RESET, OUTPUT);

  pinMode(IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IRQ), readKeyboard, FALLING);

  //7 Clock Cycle reset
  digitalWrite(RESET, LOW);
  for (int i = 0; i < 14; i++) { tick(); }
  digitalWrite(RESET, HIGH);

  setToWrite();
  enableChip();
  setBToOutput();
  setAToInput();

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
  int len = strlen(kbd);
  if (len > 0) {
    char c = popKbdBuffer();
    sendByte(c);
  }
  tick();
  setRS(0,0,0,0);
  setDataPins(0,0,0,0,0,0,0,0);  
  tick();
 
   

}
