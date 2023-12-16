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
#define STRIP_1_PIN 11
#define STRIP_2_PIN 10
#define STRIP_3_PIN 9
#define LED_LINE_LEN 20
#define LED_LINE_COUNT 4
#define LED_COUNT LED_LINE_LEN*LED_LINE_COUNT

int KillSwitchState;

// 0 = No Light, 1 = Christmas Light, 2 = SMD LED, 3 = Light Strip
int pixelType[LED_LINE_COUNT][LED_LINE_LEN] = {
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // Strip 0: Buttons + Driver's Side Doors
    {2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // Strip 1: Dash + Steering Wheel
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
float pixelBrightness[LED_LINE_COUNT][LED_LINE_LEN] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 0: Buttons + Driver's Side Doors
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 1: Dash + Steering Wheel
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
};
float defaultBrightness[LED_LINE_COUNT][LED_LINE_LEN] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 0: Buttons + Driver's Side Doors
    {1, 1, 1, 1, 1, 0.25, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Strip 1: Dash + Steering Wheel
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
};

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
            // Don't waste clock cycles if there is no input
            if(!digitalRead(up) && !digitalRead(dn)) {
                return;
            }

            // Ensures single button press => single input
            if(millis() - lastInputMillis > inputSensitivity) {
                if(digitalRead(dn)) {
                   selectorDnFunc();
                }

                if(digitalRead(up)) {
                   selectorUpFunc();
                }

            }
            lastInputMillis = millis();
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

        // 0 = cycle
        // 1 = sparkle
        // 2 = matrix
        // 3 = wave
        // 4 = music
        // int style = 0;
        int style = 1;
        int numStyles = 5;

        unsigned long lastUpdateMillis;

        // TEMP
        bool notYetImplementedFlash = true;


        bool readyToUpdate() {
            return millis() - lastUpdateMillis > speed;
        }

        // STYLES
        void cycle() {
            if(readyToUpdate()) {
                colour++;
                if(colour >= 1530) {
                    colour = 0;
                }

                generateRGBFromCurrentState();
                setAllFromCurrentState();

                lastUpdateMillis = millis();
            }
        }

        void sparkle() { // TODO implement
            if(readyToUpdate()) {
                colour++;
                if(colour >= 1530) {
                    colour = 0;
                }

                generateRGBFromCurrentState();
                setAllFromCurrentState();

                set(20,255,255,255);
                set(21,255,255,255);
                set(27,255,255,255);
                set(28,255,255,255);
                show();

                lastUpdateMillis = millis();
            }
        }

        void matrix() {
            if(lastUpdateMillis + speed * 25 < millis()) {
                if(notYetImplementedFlash) {
                    setAll(0, 255, 0);
                    notYetImplementedFlash = false;
                } else {
                    setAll(0, 0, 0);
                    notYetImplementedFlash = true;
                }
                lastUpdateMillis = millis();
            }
        }

        void wave() {
            if(lastUpdateMillis + speed * 25 < millis()) {
                colour = random(0,1530);
                generateRGBFromCurrentState();

                for (int i = 0; i < LED_COUNT; i++) {
                    set(i, Red, Green, Blue);
                    strip0.show();
                    strip1.show();
                    delay(25);
                }
                lastUpdateMillis = millis();
            }
        }

        void music() {
            if(lastUpdateMillis + speed * 25 < millis()) {
                if(notYetImplementedFlash) {
                    setAll(255, 0, 175);
                    notYetImplementedFlash = false;
                } else {
                    setAll(0, 0, 0);
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

            // Handle brightness
            float pb = Brightness * pixelBrightness[stripNum][ledIndex];
            r = ceil(r * pb);
            g = ceil(g * pb);
            b = ceil(b * pb);


            // TODO handle kill switch states
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
                    Strips[stripNum].setPixelColor(ledIndex, Strips[stripNum].Color(r, g, b));
                    break;
            }
        }

        void setAll(int r, int g, int b) {
            for(int i = 0; i < LED_COUNT; i++) {
                set(i, r, g, b);
            }
            show();
        }

       void setAllFromCurrentState() {
            for(int i = 0; i < LED_LINE_COUNT; i++) { // Possibly remove
                for(int j = 0; j < LED_COUNT; j++) {
                    set(i+j, Red, Green, Blue);
                }
            }
            show();
        }

        void show() {
            for(int i = 0; i < LED_LINE_COUNT; i++) {
                Strips[i].show();
            }
        }

        void update() {
            if(KillSwitchState == 1) {
                setAll(255,255,255);
            } else if (KillSwitchState == 2) {
                setAll(0,0,0);
            } else {
                if(style == 0) {
                    cycle();
                } else if(style == 1) {
                    sparkle();
                } else if(style == 2) {
                    matrix();
                } else if(style == 3) {
                    wave();
                } else if(style == 4) {
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

        // Store animation in 2d array: animation[frame][pixel, colour] = {{3, 1232}, {4, 1232}, {5, 63}}; (using full pixel index and 0-1530 colour)

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
    led.changeStyleDn();
}

void selectorUpFunc() {
    led.changeStyleUp();
}

void lightBarCallback(bool detected) {
    int dashLightStrip = 1;
    int dashLightIndex = 5;

    if(detected) {
        Serial.println("lightBar");
        pixelBrightness[dashLightStrip][dashLightIndex] = defaultBrightness[dashLightStrip][dashLightIndex];
    } else {
        pixelBrightness[dashLightStrip][dashLightIndex] = 0;
    }
}

void bumperLightCallback(bool detected) {
    int dashLightStrip = 1;
    int dashLightIndex = 6;

    if(detected) {
        Serial.println("bumperLight");
        pixelBrightness[dashLightStrip][dashLightIndex] = defaultBrightness[dashLightStrip][dashLightIndex];
    } else {
        pixelBrightness[dashLightStrip][dashLightIndex] = 0;
    }
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


