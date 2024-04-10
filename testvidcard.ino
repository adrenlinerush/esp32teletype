

int DATA[] = {52,50,48,46,44,42,40,38};
int CLOCK = 36;
int ACK = 2;

int ready = 0;
int escape = 0;

void sendByte(char c) {
  
  for (int i = 7; i >= 0; --i) {
        unsigned char bit = (c >> i) & 1;
        Serial.print(bit);
        if (bit == 1) {
          digitalWrite(DATA[i], HIGH);
        } else {
          digitalWrite(DATA[i], LOW);
        }
    }
    Serial.print("\n");
    //delay(10);
    digitalWrite(CLOCK, HIGH);
    ready = 1;
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

void resetClock() {
  Serial.print("Recieved ACK\n");
  ready = 0;
  digitalWrite(CLOCK, LOW);
}

void setup() {
  for (int i=0; i < 8; i++) {
    pinMode(DATA[i], OUTPUT);
  }
  pinMode(CLOCK,OUTPUT);
  pinMode(ACK,INPUT_PULLUP);
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  if(ready == 0) {
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
  } else {
    if (digitalRead(ACK) == HIGH) {
      resetClock();
    } else {
      Serial.print("Not ready.\n");
    }
  }

}
