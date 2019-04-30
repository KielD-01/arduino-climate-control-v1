#include <Wire.h>
#include <DHT.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <IRremote.h>
#include <LiquidCrystal_I2C.h>

/** Main Configurations **/
#define SERIAL_SPEED 115200

/** Pins configurations **/
#define IR_REMOTE_PIN 6
#define DHT_11_PIN 7
#define BUZZER 4

// Temperature LED Pins
#define T_R 13
#define T_G 12
#define T_B 11

// Humidity LED Pins
#define H_R 9
#define H_G 8
#define H_B 7

int mode = 1;
int tLED[] = {T_R, T_G, T_B};
int hLED[] = {H_R, H_G, H_B};
unsigned long irValue = 0;

int measurements[] = {0, 0};

bool tOrHChange = true;
bool alChange = true;
bool dndChange = true;

bool dnd = false;
bool debug = true;
bool modeSwitch = false;
bool allowedLights = true;

/** Classes Init **/
DHT dht(DHT_11_PIN, DHT11);
LiquidCrystal_I2C liquidCrystal(0x27, 16, 2);
IRrecv IR_Receiver(IR_REMOTE_PIN);
decode_results IR_Decode;

/** Custom Characters Start **/

byte degreeChar[] = {
        0x06,
        0x09,
        0x09,
        0x06,
        0x00,
        0x00,
        0x00,
        0x00
};

byte humidityChar[] = {
        0x00,
        0x04,
        0x0E,
        0x1F,
        0x1F,
        0x1F,
        0x0E,
        0x00
};

/** Custom Characters End **/

/**

   @param position
*/
void turnBackLight(bool position = true) {
    !allowedLights ?
    (position ? liquidCrystal.backlight() : liquidCrystal.noBacklight()) :
    liquidCrystal.noBacklight();
}

/**

   Sets the LCD display text

   @param text
   @param cursorPos
   @param cursorLine
   @param clear
*/
void setDisplayText(long text, int cursorPos = 0, int cursorLine = 0, bool clear = true) {
    if (clear) {
        liquidCrystal.clear();
    }
    liquidCrystal.setCursor(cursorPos, cursorLine);
    liquidCrystal.print(text);
}

void setDisplayText(bool text, int cursorPos = 0, int cursorLine = 0, bool clear = true, char *yes = "Yes",
                    char *no = "No") {
    if (clear) {
        liquidCrystal.clear();
    }
    liquidCrystal.setCursor(cursorPos, cursorLine);
    liquidCrystal.print(text ? yes : no);
}

void setDisplayText(char *text, int cursorPos = 0, int cursorLine = 0, bool clear = true) {
    if (clear) {
        liquidCrystal.clear();
    }
    liquidCrystal.setCursor(cursorPos, cursorLine);
    liquidCrystal.print(text);
}

void setDisplayText(int text, int cursorPos = 0, int cursorLine = 0, bool clear = true) {
    if (clear) {
        liquidCrystal.clear();
    }
    liquidCrystal.setCursor(cursorPos, cursorLine);
    liquidCrystal.print(text);
}

void setDisplayText(float text, int cursorPos = 0, int cursorLine = 0, bool clear = true) {
    if (clear) {
        liquidCrystal.clear();
    }
    liquidCrystal.setCursor(cursorPos, cursorLine);
    liquidCrystal.print(text);
}

void setDisplayText(unsigned long int text, int cursorPos = 0, int cursorLine = 0, bool clear = true) {
    if (clear) {
        liquidCrystal.clear();
    }
    liquidCrystal.setCursor(cursorPos, cursorLine);
    liquidCrystal.print(text);
}

void setDisplayTextChar(int index, int cursorPos = 0, int cursorLine = 0) {
    liquidCrystal.setCursor(cursorPos, cursorLine);
    liquidCrystal.write(index);
}

/**
   Sets LED Color

   @param rgb
   @param red
   @param green
   @param blue
*/
void setLED(int *rgb = {}, int red = 0, int green = 0, int blue = 0) {
    analogWrite(rgb[0], red);
    analogWrite(rgb[1], green);
    analogWrite(rgb[2], blue);
}

void setCustomCharacters() {
    liquidCrystal.createChar(0x1, degreeChar);
    liquidCrystal.createChar(0x2, humidityChar);
}

int getTemperature() {
    return dht.readTemperature();
}

int getHumidity() {
    return dht.readHumidity();
}

void setTemperature() {
    int temperatureLevel = getTemperature();


    if (measurements[0] != temperatureLevel && !isnan(temperatureLevel)) {
        measurements[0] = temperatureLevel;
        tOrHChange = true;
    }
}

void setHumidity() {
    int humidityLevel = getHumidity();

    if (measurements[1] != humidityLevel && !isnan(humidityLevel)) {
        measurements[1] = humidityLevel;
        tOrHChange = true;
    }
}

/**

   Sets Pin Modes
*/
void setPinModes() {
    pinMode(T_R, OUTPUT);
    pinMode(T_G, OUTPUT);
    pinMode(T_B, OUTPUT);
    pinMode(H_R, OUTPUT);
    pinMode(H_G, OUTPUT);
    pinMode(H_B, OUTPUT);
    pinMode(BUZZER, OUTPUT);
}

void switchMode(bool clear = false) {
    if (clear) {
        liquidCrystal.clear();
    }

    if (modeSwitch) {
        Serial.print("Switched to Mode No.");
        Serial.println(mode);
    }

    switch (mode) {
        case 1:
            if (tOrHChange || modeSwitch) {
                setDisplayText("Dashboard");
                setDisplayText(measurements[0], 0, 1, false);
                setDisplayTextChar(0x1, 3, 1);
                setDisplayText("C", 4, 1, false);

                setDisplayTextChar(0x2, 11, 1);
                setDisplayText(measurements[1], 13, 1, false);
                setDisplayText("%", 15, 1, false);

                tOrHChange = false;
            }
            break;
        case 2:
            if (alChange || modeSwitch) {
                setDisplayText("Settings", 0, 0);
                setDisplayText("AL : ", 0, 1, false);
                setDisplayText(allowedLights, 5, 1, false);
                alChange = false;
            }
            break;
        case 3:
            if (dndChange || modeSwitch) {
                setDisplayText("DND Mode");
                setDisplayText(dnd, 0, 1, false, "Active", "Inactive");
                dndChange = false;
            }
            break;
        default:
            mode = 1;
            switchMode();
            break;
    }

    modeSwitch = false;
}

void irRemoteDecode() {
    if (IR_Receiver.decode(&IR_Decode)) {

        if (IR_Decode.value == 0XFFFFFFFF)
            IR_Decode.value = irValue;
        liquidCrystal.setCursor(0, 0);
        liquidCrystal.clear();

        tone(BUZZER, 250, 200);

        switch (IR_Decode.value) {
            case 0xFFA25D: // CH-
                mode = mode == 1 ? 3 : mode - 1;
                Serial.println("Switch mode back");
                modeSwitch = true;
                break;
            case 0xFF629D: // CH
                mode = 1;
                Serial.println("Go to the Dashboard");
                modeSwitch = true;
                break;
            case 0xFFE21D: // CH+
                mode = mode == 3 ? 1 : mode + 1;
                Serial.println("Switch mode forward");
                modeSwitch = true;
                break;
            case 0xFFE01F: // -
                turnBackLight(false);
                Serial.println("Turn the backlight off");
                break;
            case 0xFFA857: // +
                turnBackLight(true);
                Serial.println("Turn the backlight on");
                break;
            case 0xFF906F: // EQ
                switch (mode) {
                    default:
                        allowedLights = !allowedLights;
                        alChange = true;
                        Serial.print("Force Lights Allowance : ");
                        Serial.println(allowedLights ? "Yes" : "No");
                        break;
                    case 3:
                        dnd = !dnd;
                        dndChange = true;
                        Serial.print("Do Not Disturb Mode : ");
                        Serial.println(dnd ? "Yes" : "No");
                        break;
                }
                break;
            case 0xFF22DD: // |<<

                break;
            case 0xFF02FD: // >>|

                break;
            case 0xFFC23D: // >|
                switch (mode) {
                    case 1:
                        Serial.println("Temperature force re-fresh");
                        setTemperature();

                        Serial.println("Humidity force re-fresh");
                        setHumidity();
                        break;
                    default:
                        Serial.println("No Action is defined for this mode");
                        break;
                }
                break;

            case 0xFF6897: // 0

                break;
            case 0xFF9867: // 100+

                break;
            case 0xFFB04F: // 200+

                break;
            case 0xFF30CF: // 1

                break;
            case 0xFF18E7: // 2

                break;
            case 0xFF7A85: // 3

                break;
            case 0xFF10EF: // 4

                break;
            case 0xFF38C7: // 5

                break;
            case 0xFF5AA5: // 6

                break;
            case 0xFF42BD: // 7

                break;
            case 0xFF4AB5: // 8

                break;
            case 0xFF52AD: // 9

                break;
        }

        switchMode();

        irValue = IR_Decode.value;
        IR_Receiver.resume();
    }
}

void setup() {
    if (debug) Serial.begin(SERIAL_SPEED); // Enables the Serial output, if You're within the development mode
    liquidCrystal.init();
    liquidCrystal.backlight();

    setDisplayText("Booting...");
    setCustomCharacters();
    attachInterrupt(digitalPinToInterrupt(IR_REMOTE_PIN), irRemoteDecode, RISING);
    setPinModes();

    setDisplayText("Enabling IR...");
    IR_Receiver.enableIRIn();

    liquidCrystal.clear();
    switchMode();
}

void loop() {
    interrupts();
    irRemoteDecode();

    setTemperature();
    setHumidity();

    switchMode(false);
    noInterrupts();
}
