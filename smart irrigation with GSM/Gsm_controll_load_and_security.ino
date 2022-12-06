#include <LiquidCrystal.h>  // Include the lcd library
#include <SoftwareSerial.h> // Library for using serial communication

const byte rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  // initialize the library with the numbers of the interface pins

SoftwareSerial gsm(9, 10);    // Pins 9 (RX), 10 (TX) are used as used as software serial pins

String incomingData = "";     // for storing incoming serial data
unsigned short lightOut = 13; // Initialized a pin for relay module
unsigned short fanOut = 12;
unsigned short tvOut = 8;
unsigned short input = 11;
int buzzer = 5;
int inputstate;
int varr = 0;
int parr;
void setup() {

  parr = 1;
  Serial.begin(9600); // baudrate for serial monitor
  gsm.begin(9600);    // baudrate for GSM shield

  // reserve 200 bytes for the incomingData:
  incomingData.reserve(200);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Config pin as output pin
  pinMode(lightOut, OUTPUT);
  pinMode(fanOut, OUTPUT);
  pinMode(tvOut, OUTPUT);
  pinMode(input, INPUT);
  pinMode(buzzer, OUTPUT);
 digitalWrite(buzzer, LOW);
  // Make pin initailly low
  digitalWrite(lightOut, LOW);
  digitalWrite(fanOut, LOW);
  digitalWrite(tvOut, LOW);
 
  // Print a message to the LCD.
  lcd.print("GSM Control Home");
  lcd.setCursor(0, 1);
  lcd.print("   Automation   ");
  delay(2000);
  showInitializingMsg();
  lcd.clear();
  lcd.print("System Ready");
  delay(1000);

  // set the GSM Module in Text Mode
  gsm.println("AT+CMGF=1");
  delay(200);

  // set gsm module to receive sms & show the output on serial
  gsm.println("AT+CNMI=2,2,0,0,0");
  delay(200);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Welcome     ");
  
}

void loop() {

  //Function for receiving sms
  receiveSMS();
  //sendMessage("start");
  
  inputstate = digitalRead(input);

  if (inputstate == 1 && parr == 1) {
     lcd.setCursor(0, 0);
     lcd.print("Sensor Detect    ");
      parr = 0;
     sendMessage("Soil Moisture Is Very Low ");
    
  }


  if (inputstate == 0 && varr == 1) {
     lcd.setCursor(0, 0);
     digitalWrite(tvOut, LOW);
     varr = 0;
     parr = 1;
     sendMessage("Motor Is OFF");
     lcd.setCursor(0, 0);
     lcd.print("    Welcome     ");
     lcd.setCursor(0, 1);
     lcd.print("Motor Is OFF    ");
     delay(1000);
  }

  if (incomingData.indexOf("On") >= 0) {
    digitalWrite(tvOut, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("ON     ");
   
    sendMessage("All Load is ON");

  } else if (incomingData.indexOf("Off") >= 0) {
    digitalWrite(tvOut, LOW);
    lcd.setCursor(0, 1);
    lcd.print("OFF    ");
    sendMessage("All Load is OFF");

  } else if (incomingData.indexOf("motor on") >= 0) {
    varr = 1;
    
    digitalWrite(tvOut, HIGH);
     
     lcd.setCursor(0, 0);
     lcd.print("    Welcome     ");
     lcd.setCursor(0, 1);
     lcd.print("Motor Is ON     ");
     
    sendMessage("Motor is ON");

  } else if (incomingData.indexOf("motor off") >= 0) {
     varr = 0;
    digitalWrite(tvOut, LOW);
    lcd.setCursor(0, 1);
    lcd.print("OFF ");
    sendMessage("Motor is OFF");
  }

  // clear the string:
  incomingData = "";
  //delay(500);
}

/*
   Function for loading animation
*/
void showInitializingMsg() {
  String msg = "Initializing";
  // Here i'm using 16x2 display. So, characters in every line is 16
  unsigned int restOfCellInLine = (16 - msg.length());
  lcd.clear();
  lcd.print(msg);

  for (int i = 0; i < restOfCellInLine; i++) {
    for (int j = 0; j < restOfCellInLine; j++) {
      lcd.print(".");
      delay(1000);
    }
    lcd.clear();
    lcd.print(msg);
  }
}

/*
   Function for receiving sms
*/
void receiveSMS() {
  while (gsm.available()) {
    incomingData = gsm.readStringUntil('\0'); // Get the data from the serial port.
    incomingData.toLowerCase();
    Serial.print(incomingData);
  }
}

/*
   Function for sending sms
*/
void sendMessage(String msg) {
  gsm.println("AT+CMGF=1");    // Set the GSM Module in Text Mode
  delay(1000);
  gsm.println("AT+CMGS=\"+8801933268874\""); // Replace it with your mobile number
  delay(1000);
  gsm.println(msg);       // The SMS text you want to send
  delay(1000);
  gsm.println((char)26);  // ASCII code of CTRL+Z
  delay(1000);
  gsm.println();
  delay(1000);
}
