#include <Adafruit_NeoPixel.h>

// GLOBALS
//todo make #define
int selectorUp = 3;
int selectorDn = 4;
int killUp = 2;
int killDn = 5;
int lightBarDetectorPin = A0;
int bumperLightDetectorPin = A1;
#define STRIP_0_PIN 12
#define STRIP_1_PIN 6
#define STRIP_2_PIN 8
#define STRIP_3_PIN 7
#define LED_LINE_LEN 50
#define LED_LINE_COUNT 4
#define LED_COUNT LED_LINE_LEN*LED_LINE_COUNT

int KillSwitchState;
bool IsLightBarOn = false;
bool IsBumperLightOn = false;

// NEED TO HARDCODE ALL OF THESE - you only have 2kb of variable storage
// 0 = No Light, 1 = Christmas Light, 2 = SMD LED, 3 = Light Strip
int pixelType[LED_LINE_COUNT][LED_LINE_LEN] = {
    {2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // Strip 0: Buttons + Driver's Side Doors
    {2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // Strip 1: Dash + Steering Wheel
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // Strip 2: Footwells + Heated Seat
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // Strip 3: Passenger's Footwells, Radio, Passenger's Side Doors
};
// float pixelBrightness[LED_LINE_COUNT][LED_LINE_LEN] = {
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 0: Buttons + Driver's Side Doors
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 1: Dash + Steering Wheel
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 2: Footwells + Heated Seat
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 3: Passenger's Footwells, Radio, Passenger's Side Doors
// };
// float defaultBrightness[LED_LINE_COUNT][LED_LINE_LEN] = {
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 0: Buttons + Driver's Side Doors
//     {1, 1, 1, 1, 1, 0.25, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 1: Dash + Steering Wheel
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 2: Footwells + Heated Seat
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 3: Passenger's Footwells, Radio, Passenger's Side Doors
// };

Adafruit_NeoPixel strip0(LED_LINE_LEN, STRIP_0_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1(LED_LINE_LEN, STRIP_1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_LINE_LEN, STRIP_2_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3(LED_LINE_LEN, STRIP_3_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel Strips[LED_LINE_COUNT] = {strip0, strip1, strip2, strip3};

// FORWARD DECLARATIONS
void selectorDnFunc();
void selectorUpFunc();
void lightBarCallback(bool);
void bumperLightCallback(bool);


// CLASSES
class Switch {
    private:
        unsigned long lastInputMillis;
        unsigned long inputSensitivity = 50;
        int up, dn;
        bool isMomentary;
        int switchState;


        void handleInputMomentary() {
            // Ensures single button press => single input
            if(digitalRead(dn)) {
                if(millis() - lastInputMillis > inputSensitivity) {
                   selectorDnFunc();
                }
                lastInputMillis = millis();
            }

            if(digitalRead(up)) {
                if(millis() - lastInputMillis > inputSensitivity) {
                   selectorUpFunc();
                }
                lastInputMillis = millis();
            }
        }

        void handleInputStatic() {
            if(digitalRead(dn)) {
                switchState = 1;
                KillSwitchState = 1;
                return;
            }

            if(digitalRead(up)) {
                switchState = 2;
                KillSwitchState = 2;
                return;
            }

            switchState = 0;
            KillSwitchState = 0;
        }

    public:
        Switch(int u, int d, bool m) {
            up = u;
            dn = d;
            isMomentary = m;
        }

        void handleInput() {
            if(isMomentary) {
                handleInputMomentary();
            } else {
                handleInputStatic();
            }
        }

        int state() {
            if(isMomentary) {
                return -1;
            }
            return switchState;
        }
};


class Detector {
    private:
        int detectorPin;
        int defaultState;
        int detectorType;

    public:
        Detector(int dp, bool ds, int dt) {
            detectorPin = dp;
            defaultState = ds;
            detectorType = dt;
        }

        void handleState() {
            switch (detectorType) {
                case 0:
                    lightBarCallback(digitalRead(detectorPin) != defaultState);
                    break;
                case 1:
                    bumperLightCallback(digitalRead(detectorPin) != defaultState);
                    break;
            }
        }
};





class LED {
    private:
        float Brightness = 1; // 0.00-1.00
        unsigned long speed = 10;
        int colour = 0;
        int Red;
        int Green;
        int Blue;

        // 0 = cycleWhite
        // 1 = cycle
        // 2 = sparkle
        // 3 = matrix
        // 4 = wave
        // 5 = music
        int style = 0;
        int numStyles = 6;

        unsigned long lastUpdateMillis;

        // TEMP
        bool notYetImplementedFlash = true;


        bool readyToUpdate() {
            return millis() - lastUpdateMillis > speed;
        }

        // STYLES
        void cycleWhite() {
            if(readyToUpdate()) {
                colour++;
                if(colour >= 1530) {
                    colour = 0;
                }

                generateRGBFromCurrentState();
                setAllFromCurrentState();

                // TODO make relative based on num leds
                set(50, 255, 255, 255);
                set(51, 255, 255, 255);
                set(57, 255, 255, 255);
                set(58, 255, 255, 255);
                show();

                lastUpdateMillis = millis();
            }
        }

        void cycle() {
            if(readyToUpdate()) {
                colour++;
                if(colour >= 1530) {
                    colour = 0;
                }

                generateRGBFromCurrentState();
                setAllFromCurrentState();
                show();

                lastUpdateMillis = millis();
            }
        }

        void sparkle() {
            if(lastUpdateMillis + speed * 25 < millis()) {
                if(notYetImplementedFlash) {
                    setAll(255, 0, 0);
                    show();
                    notYetImplementedFlash = false;
                } else {
                    setAll(0, 0, 0);
                    show();
                    notYetImplementedFlash = true;
                }
                lastUpdateMillis = millis();
            }
        }

        void matrix() {
            if(lastUpdateMillis + speed * 25 < millis()) {
                if(notYetImplementedFlash) {
                    setAll(0, 255, 0);
                    show();
                    notYetImplementedFlash = false;
                } else {
                    setAll(0, 0, 0);
                    show();
                    notYetImplementedFlash = true;
                }
                lastUpdateMillis = millis();
            }
        }

        void wave() {
            if(lastUpdateMillis + speed * 25 < millis()) {
                if(notYetImplementedFlash) {
                    setAll(0, 0, 255);
                    show();
                    notYetImplementedFlash = false;
                } else {
                    setAll(0, 0, 0);
                    show();
                    notYetImplementedFlash = true;
                }
                lastUpdateMillis = millis();
            }
        }

        void music() {
            if(lastUpdateMillis + speed * 25 < millis()) {
                if(notYetImplementedFlash) {
                    setAll(255, 0, 175);
                    show();
                    notYetImplementedFlash = false;
                } else {
                    setAll(0, 0, 0);
                    show();
                    notYetImplementedFlash = true;
                }
                lastUpdateMillis = millis();
            }
        }

        // MISC
        void generateRGBFromCurrentState() {
            if(colour >= 0 && colour <= 255) {
                Red = 255;
                Green = 0;
                Blue = colour;
            }

            else if(colour > 255 && colour <= 510) {
                Red = 510-colour;
                Green = 0;
                Blue = 255;
            }

            else if(colour > 510 && colour <= 765) {
                Red = 0;
                Green = colour-510;
                Blue = 255;
            }

            else if(colour > 765 && colour <= 1020) {
                Red = 0;
                Green = 255;
                Blue = 1020-colour;
            }

            else if(colour > 1020 && colour <= 1275) {
                Red = colour-1020;
                Green = 255;
                Blue = 0;
            }

            else if(colour > 1275 && colour <= 1530) {
                Red = 255;
                Green = 1530-colour;
                Blue = 0;
            }
        }

    public:
        void set(int i, int r, int g, int b) {
            int stripNum = floor(i / LED_LINE_LEN);
            int ledIndex = i % LED_LINE_LEN;

            bool isLightBarDashLight = (stripNum == 1 && ledIndex == 5);
            bool isBumperLightDashLight = (stripNum == 1 && ledIndex == 6);

            // Dash Lights
            if(isLightBarDashLight) {
                if(IsLightBarOn) {
                    r = r*0.25; g = g*0.25; b = b*0.25;
                } else {
                    r = 0; g = 0; b = 0;
                }
            }
            else if(isBumperLightDashLight) {
                if(IsBumperLightOn) {
                    r = r; g = g; b = b;
                } else {
                    r = 0; g = 0; b = 0;
                }
            }
            
            // Normal
            else {
                r = ceil(r * Brightness);
                g = ceil(g * Brightness);
                b = ceil(b * Brightness);
            }


            switch (pixelType[stripNum][ledIndex]) {
                case 0:
                    break;
                case 1:
                    Strips[stripNum].setPixelColor(ledIndex, Strips[stripNum].Color(g, r, b));
                    break;
                case 2:
                    Strips[stripNum].setPixelColor(ledIndex, Strips[stripNum].Color(r, g, b));
                    break;
                case 3:
                    Strips[stripNum].setPixelColor(ledIndex, Strips[stripNum].Color(r, b, g));
                    break;
            }
        }

        void setAll(int r, int g, int b) {
            for(int i = 0; i < LED_COUNT; i++) {
                set(i, r, g, b);
            }
        }

       void setAllFromCurrentState() {
            for(int i = 0; i < LED_COUNT; i++) {
                set(i, Red, Green, Blue);
            }
        }

        void show() {
            for(int i = 0; i < LED_LINE_COUNT; i++) {
                Strips[i].show();
            }
        }

        void update() {
            // Kill Switch
            if (KillSwitchState == 0) {
                Brightness = 1;
            } else if(KillSwitchState == 1) {
                setAll(255,255,255);
                show();
            } else if (KillSwitchState == 2) {
                Brightness = 0;
            }

            // If we aren't overriding all, set to normal
            if(KillSwitchState != 1) {
                if(style == 0) {
                    cycleWhite();
                } else if(style == 1) {
                    cycle();
                } else if(style == 2) {
                    sparkle();
                } else if(style == 3) {
                    matrix();
                } else if(style == 4) {
                    wave();
                } else if(style == 5) {
                    music();
                } else {
                    style = 0;
                }
            }
        }

        // onAnimation() {
        //     set(i, r, g, b);
        //     set(i, r, g, b);
        //     set(i, r, g, b);
        //     set(i, r, g, b);
        //     set(i, r, g, b);
        // }

        // offAnimation() {
        //     set(i, r, g, b);
        //     set(i, r, g, b);
        //     set(i, r, g, b);
        //     set(i, r, g, b);
        //     set(i, r, g, b);
        // }

        void changeStyleUp() {
            style++;
            if(style >= numStyles) {
                style = 0;
            }
        }

        void changeStyleDn() {
            style--;
            if(style < 0) {
                style = numStyles - 1;
            }
        }
};




// MORE GLOBALS
Switch selectorSwitch = Switch(selectorUp, selectorDn, true);
Switch killSwitch = Switch(killUp, killDn, false);
Detector lightBarDetector = Detector(lightBarDetectorPin, false, 0);
Detector bumperLightDetector = Detector(bumperLightDetectorPin, false, 1);
LED led = LED();


// FUNCTIONS
void selectorDnFunc() {
    Serial.println("HERE1");
    led.changeStyleDn();
}

void selectorUpFunc() {
    Serial.println("HEREup");
    led.changeStyleUp();
}

void lightBarCallback(bool detected) {
    IsLightBarOn = detected;
}

void bumperLightCallback(bool detected) {
    IsBumperLightOn = detected;
}


// ARDUINO
void setup() {
    Serial.begin(9600);

    // Init Detectors
    lightBarCallback(false);
    bumperLightCallback(false);

    // Init Strips
    for(int i = 0; i < LED_LINE_COUNT; i++) {
        Strips[i].begin(); // INITIALIZE strip object
        Strips[i].show(); // Turn OFF all pixels
    }

    pinMode(selectorUp, INPUT);
    pinMode(selectorDn, INPUT);
    pinMode(STRIP_0_PIN, OUTPUT);
    pinMode(STRIP_1_PIN, OUTPUT);
    pinMode(STRIP_2_PIN, OUTPUT);
    pinMode(STRIP_3_PIN, OUTPUT);
}

void loop() {
    selectorSwitch.handleInput();
    killSwitch.handleInput();
    lightBarDetector.handleState();
    bumperLightDetector.handleState();
    led.update();
}


