// /*
//  * Project test
//  * Description:
//  * Author:
//  * Date:
//  */
// #include "Particle.h"
// #include "dct.h"
// #include <stdio.h>
// #include <math.h>

// // SYSTEM_MODE(MANUAL);
// // SYSTEM_THREAD(ENABLED);
// SYSTEM_MODE(MANUAL);

// // View logs with CLI using 'particle serial monitor --follow'
// SerialLogHandler logHandler(LOG_LEVEL_INFO);

// // int a0 = A0; // Light Sensor

// // // setup() runs once, when the device is first turned on.
// // void setup()
// // {
// //     pinMode(a0, INPUT);
// //     pinMode(D2, INPUT);
// // }

// // // loop() runs over and over again, as quickly as it can execute.
// // void loop()
// // {
// //     // The core of your code will likely live here.
// //     // printf("Value: %d", analogRead(a1));

// //     // Light Sensor
// //     Log.info("Value: %ld", analogRead(a0));

// //     // Sound sensor
// //     // Log.info("Value: %ld", digitalRead(D2));
// //     delay(100);
// // }

// //----------------------------------------------

// // int led = A1; // A1 use for LED

// // void setup() {
// //   pinMode(led, OUTPUT); // A1 output
// // }

// // void loop() {
// //   digitalWrite(led, HIGH); // LED on
// //   delay(2000);
// //   digitalWrite(led, LOW);  // LED off
// //   delay(2000);
// // }

// //----------------------------------------------

// // int potenPin = A3; // potentiometer & White LED code
// // int LEDPin = D2;
// // // setup() runs once, when the device is first turned on.
// // void setup()
// // {
// //     pinMode(potenPin, INPUT);
// //     pinMode(LEDPin, OUTPUT);
// // }

// // // loop() runs over and over again, as quickly as it can execute.
// // void loop()
// // {
// //     // The core of your code will likely live here.
// //     int potenValue = analogRead(potenPin);
// //     int brightness = map(potenValue, 0, 4095, 0, 255);
// //     analogWrite(LEDPin, brightness);
// //     // printf("Value: %d", analogRead(a1));

// //     Log.info("Potentiometer Value: %ld, Brightness: %d", analogRead(A3), brightness);

// //     // Sound sensor
// //     // Log.info("Value: %ld", digitalRead(D2));
// //     delay(100);
// // }

// //----------------------------------------------
// // R/G LED CODE
// // int green = D6; // Green LED
// // int red = D7; // Red LED

// // void setup()
// // {
// //     pinMode(green, OUTPUT);
// //     pinMode(red, OUTPUT);
// // }

// // void loop()
// // {
// //     // The core of your code will likely live here.
// //     // printf("Value: %d", analogRead(a1));

// //     // Light Sensor
// //     // Log.info("Value: %ld", analogRead(a0));

// //     // Sound sensor
// //     // Log.info("Value: %ld", digitalRead(D2));
// //     // delay(100);

// //     // Green LED
// //     digitalWrite(green, HIGH);
// //     delay(1000);
// //     digitalWrite(green, LOW);
// //     delay(1000);

// //     // Red LED
// //     digitalWrite(red, HIGH);
// //     delay(1000);
// //     digitalWrite(red, LOW);
// //     delay(1000);
// // }

// // void setColor(int redValue, int greenValue)
// // {
// //     analogWrite(red, redValue);
// //     analogWrite(green, greenValue);
// // }
// //----------------------------------------------
// // red/green LED with potentiometer code
// // int potenPin = A3; // Potentiometer pin
// // int LEDPin = D2;   // White LED pin
// // // int greenLED = D5; // Green LED pin
// // int redgreenLED = D6;   // Red LED pin
// // int threshold = 2000; // Threshold for potentiometer value (midpoint of 0-4095)

// // void setup()
// // {
// //     pinMode(potenPin, INPUT);
// //     pinMode(LEDPin, OUTPUT);
// //     // pinMode(greenLED, OUTPUT);
// //     pinMode(redgreenLED, OUTPUT);
// // }

// // void loop()
// // {
// //     int potenValue = analogRead(potenPin);
// //     int brightness = map(potenValue, 0, 4095, 0, 255);
// //     analogWrite(LEDPin, brightness);

// //     // Control LEDs based on potentiometer value
// //     if (potenValue > threshold)
// //     {
// //         digitalWrite(redgreenLED, HIGH);
// //     }
// //     else
// //     {
// //         digitalWrite(redgreenLED, LOW);
// //     }

// //     Log.info("Potentiometer Value: %d, Brightness: %d", potenValue, brightness);
// //     delay(100);
// // }
// //----------------------------------------------
// // motion sensor PIR code
// // int motionSensor = D4; // Motion sensor pin

// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// // }

// // void loop() {
// //     int motionValue = digitalRead(motionSensor);  // Read motion sensor value

// //     // Print the motion sensor value
// //     Serial.print("Motion Sensor Value: ");
// //     Serial.println(motionValue);

// //     // Check motion detection and print alerts
// //     if (motionValue == HIGH) {
// //         Serial.println("Motion Detected!");
// //     } else {
// //         Serial.println("No Motion Detected.");
// //     }

// //     delay(100);  // Small delay for stability
// // }

// //----------------------------------------------
// // motion sensor PIR with red green LED code
// // int motionSensor = D4; // Motion sensor pin
// // int redLED = A0; // Red LED pin
// // int greenLED = A1; // Green LED pin
// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// //     pinMode(redLED, OUTPUT);
// //     pinMode(greenLED, OUTPUT);
// // }

// // void loop() {
// //     int motionValue = digitalRead(motionSensor);  // Read motion sensor value

// //     // Print the motion sensor value
// //     Serial.print("Motion Sensor Value: ");
// //     Serial.println(motionValue);

// //     // Check motion detection and print alerts
// //     if (motionValue == HIGH) {
// //         Serial.println("Motion Detected!");
// //         digitalWrite(greenLED, HIGH);
// //         digitalWrite(redLED, LOW);
// //     } else {
// //         Serial.println("No Motion Detected.");
// //         digitalWrite(greenLED, LOW);
// //         digitalWrite(redLED, HIGH);
// //     }

// //     delay(100);  // Small delay for stability
// // }
// //----------------------------------------------
// // motion sensor PIR with flashing green if motion detect wich turn off after 5 second of no motion detect LED code
// // int motionSensor = D4; // Motion sensor pin
// // int greenLED = D1; // Green LED pin
// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// //     pinMode(greenLED, OUTPUT);
// // }

// // void loop() {
// //     int motionValue = digitalRead(motionSensor);  // Read motion sensor value

// //     // Print the motion sensor value
// //     Serial.print("Motion Sensor Value: ");
// //     Serial.println(motionValue);

// //     // Check motion detection and print alerts
// //     if (motionValue == HIGH) {
// //         Serial.println("Motion Detected!");
// //         digitalWrite(greenLED, HIGH);
// //         delay(5000);
// //     } else {
// //         Serial.println("No Motion Detected.");
// //         digitalWrite(greenLED, LOW);
// //     }

// //     delay(100);  // Small delay for stability
// // }

// //----------------------------------------------
// // motion sensor PIR with flashing green if motion detect wich turn off after 5 second of no motion detect LED code

// // int motionSensor = D4; // Motion sensor pin
// // int greenLED = A0;     // Green LED pin

// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// //     pinMode(greenLED, OUTPUT);
// // }

// // void loop()
// // {
// //     int motionValue = digitalRead(motionSensor); // Read motion sensor value

// //     // Print the motion sensor value
// //     Serial.print("Motion Sensor Value: ");
// //     Serial.println(motionValue);

// //     // Check motion detection and print alerts
// //     if (motionValue == HIGH)
// //     {
// //         Serial.println("Motion Detected! Flashing Green LED for 5 seconds.");
// //         // Flash green LED for 5 seconds
// //         for (int i = 0; i < 5; i++)
// //         {
// //             digitalWrite(greenLED, 255);
// //             delay(500);
// //             digitalWrite(greenLED, 0);
// //             delay(500);
// //        }
// //     }
// //     delay(100); // Small delay for stability

// //     // If no motion detected, turn off the LED
// //     if (motionValue == LOW)
// //     {
// //         Serial.println("No Motion Detected. Turning off Green LED.");
// //         digitalWrite(greenLED, HIGH);
// //     }
// //     delay(100); // Small delay for stability
// // }

// //----------------------------------------------
// // Off if there is no motion detected, flashing green led if motion detected and light sensor detected certain lux level, turn green led off after 5 second of no motion detected code
// // int motionSensor = D4; // Motion sensor pin
// // int greenLED = A0;     // Green LED pin
// // int lightSensor = A2; // Light sensor pin
// // int luxThreshold = 500; // Lux threshold for light sensor

// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// //     pinMode(greenLED, OUTPUT);
// //     pinMode(lightSensor, INPUT);
// // }

// // void loop()
// // {
// //     int motionValue = digitalRead(motionSensor); // Read motion sensor value
// //     int lightValue = analogRead(lightSensor); // Read light sensor value

// //     // Print the motion sensor and light sensor values
// //     Serial.print("Motion Sensor Value: ");
// //     Serial.println(motionValue);
// //     Serial.print("Light Sensor Value: ");
// //     Serial.println(lightValue);

// //     // Check motion detection and light level
// //     if (motionValue == HIGH && lightValue > luxThreshold)
// //     {
// //         Serial.println("Motion Detected! Flashing Green LED for 5 seconds.");
// //         // Flash green LED for 5 seconds
// //         for (int i = 0; i < 5; i++)
// //         {
// //             digitalWrite(greenLED, HIGH);
// //             delay(500);
// //             digitalWrite(greenLED, LOW);
// //             delay(500);
// //         }
// //     }

// //     // If no motion detected, turn off the LED
// //     if (motionValue == LOW)
// //     {
// //         Serial.println("No Motion Detected. Turning off Green LED.");
// //         digitalWrite(greenLED, HIGH);
// //     }

// //     delay(100); // Small delay for stability
// // }
// //----------------------------------------------
// // Off if there is no motion detected, flashing green led if motion detected and light sensor detected certain lux level, turn green led off after 5 second of no motion detected, Flashing red led if movement is detected and the light sensor below a certain lux level, which turns red led off after 5 second of no motion detected code
// // int motionSensor = D4; // Motion sensor pin
// // int greenLED = A0;     // Green LED pin
// // int redLED = A1;       // Red LED pin
// // int lightSensor = A2; // Light sensor pin
// // int luxThreshold = 500; // Lux threshold for light sensor

// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// //     pinMode(greenLED, OUTPUT);
// //     pinMode(redLED, OUTPUT);
// //     pinMode(lightSensor, INPUT);
// // }

// // void loop()
// // {
// //     int motionValue = digitalRead(motionSensor); // Read motion sensor value
// //     int lightValue = analogRead(lightSensor); // Read light sensor value

// //     // Print the motion sensor and light sensor values
// //     Serial.print("Motion Sensor Value: ");
// //     Serial.println(motionValue);
// //     Serial.print("Light Sensor Value: ");
// //     Serial.println(lightValue);

// //     // Check motion detection and light level
// //     if (motionValue == HIGH && lightValue > luxThreshold)
// //     {
// //         Serial.println("Motion Detected! Flashing Green LED for 5 seconds.");
// //         digitalWrite(redLED, HIGH); // Turn off red LED if motion is detected and light level is above threshold
// //         // Flash green LED for 5 seconds
// //         for (int i = 0; i < 5; i++)
// //         {
// //             digitalWrite(greenLED, HIGH);
// //             delay(500);
// //             digitalWrite(greenLED, LOW);
// //             delay(500);
// //         }

// //         digitalWrite(greenLED, HIGH); // Turn off green LED after flashing
// //         delay(500); // Small delay for stability
// //     }

// //     else if (motionValue == HIGH && lightValue < luxThreshold)
// //     {
// //         Serial.println("Motion Detected! Flashing Red LED for 5 seconds.");
// //         digitalWrite(greenLED, HIGH); // Turn off green LED if motion is detected and light level is below threshold
// //         // Flash red LED for 5 seconds
// //         for (int i = 0; i < 5; i++)
// //         {
// //             digitalWrite(redLED, HIGH);
// //             delay(500);
// //             digitalWrite(redLED, LOW);
// //             delay(500);
// //         }
// //         digitalWrite(redLED, HIGH); // Turn off red LED after flashing
// //         delay(500); // Small delay for stability
// //     }

// //     // If no motion detected, turn off the LEDs
// //     if (motionValue == LOW)
// //     {
// //         Serial.println("No Motion Detected. Turning off LEDs.");
// //         digitalWrite(greenLED, HIGH);
// //         digitalWrite(redLED, HIGH);
// //     }

// //     delay(100); // Small delay for stability
// // }
// //----------------------------------------------
// // // Off if there is no motion detected, flashing green led if motion detected and light sensor detected certain lux level, turn green led off after 5 second of no motion detected, Flashing red led if movement is detected and the light sensor below a certain lux level, which turns red led off after 5 second of no motion detected code
// // // plus potentiometer & White LED code behave independently to the motion sensor code
// // // make anti blocking code using instead for loop
// // int motionSensor = D4; // Motion sensor pin
// // int greenLED = A0;     // Green LED pin
// // int redLED = A1;       // Red LED pin
// // int lightSensor = A2; // Light sensor pin
// // int luxThreshold = 500; // Lux threshold for light sensor
// // int potenPin = A3; // Potentiometer pin
// // int LEDPin = D2;   // White LED pin

// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// //     pinMode(greenLED, OUTPUT);
// //     pinMode(redLED, OUTPUT);
// //     pinMode(lightSensor, INPUT);
// //     pinMode(potenPin, INPUT);
// //     pinMode(LEDPin, OUTPUT);
// // }

// // void loop()
// // {
// //     int motionValue = digitalRead(motionSensor); // Read motion sensor value
// //     int lightValue = analogRead(lightSensor); // Read light sensor value
// //     int potenValue = analogRead(potenPin); // Read potentiometer value
// //     int brightness = map(potenValue, 0, 4095, 0, 255); // Map potentiometer value to brightness

// //     // Print the motion sensor and light sensor values
// //     Serial.print("Motion Sensor Value: ");
// //     Serial.println(motionValue);
// //     Serial.print("Light Sensor Value: ");
// //     Serial.println(lightValue);

// //     // Control White LED based on potentiometer value
// //     analogWrite(LEDPin, brightness);

// //     // Check motion detection and light level
// //     if (motionValue == HIGH && lightValue > luxThreshold)
// //     {
// //         Serial.println("Motion Detected! Flashing Green LED for 5 seconds.");
// //         digitalWrite(redLED, HIGH); // Turn off red LED if motion is detected and light level is above threshold
// //         // Flash green LED for 5 seconds
// //         for (int i = 0; i < 5; i++)
// //         {
// //             digitalWrite(greenLED, HIGH);
// //             delay(500);
// //             digitalWrite(greenLED, LOW);
// //             delay(500);
// //         }

// //         digitalWrite(greenLED, HIGH); // Turn off green LED after flashing
// //         delay(500); // Small delay for stability
// //     }

// //     else if (motionValue == HIGH && lightValue < luxThreshold)
// //     {
// //         Serial.println("Motion Detected! Flashing Red LED for 5 seconds.");
// //         digitalWrite(greenLED, HIGH); // Turn off green LED if motion is detected and light level is below threshold
// //         // Flash red LED for 5 seconds
// //         for (int i = 0; i < 5; i++)
// //         {
// //             digitalWrite(redLED, HIGH);
// //             delay(500);
// //             digitalWrite(redLED, LOW);
// //             delay(500);
// //         }
// //         digitalWrite(redLED, HIGH); // Turn off red LED after flashing
// //         delay(500); // Small delay for stability
// //     }

// //     // If no motion detected, turn off the LEDs
// //     if (motionValue == LOW)
// //     {
// //         Serial.println("No Motion Detected. Turning off LEDs.");
// //         digitalWrite(greenLED, HIGH);
// //         digitalWrite(redLED, HIGH);
// //     }

// //     delay(100); // Small delay for stability
// // }

// //----------------------------------------------
// // // Off if there is no motion detected, flashing green led if motion detected and light sensor detected certain lux level, turn green led off after 5 second of no motion detected, Flashing red led if movement is detected and the light sensor below a certain lux level, which turns red led off after 5 second of no motion detected code
// // // plus potentiometer & White LED code behave independently to the motion sensor code
// // // make anti blocking code using instead for loop
// // int motionSensor = D4; // Motion sensor pin
// // int greenLED = A0;     // Green LED pin
// // int redLED = A1;       // Red LED pin
// // int lightSensor = A2; // Light sensor pin
// // int luxThreshold = 15; // Lux threshold for light sensor
// // int potenPin = A3; // Potentiometer pin
// // int LEDPin = D2;   // White LED pin
// // int redLEDforbutton = D6;
// // int greenLEDforbutton = D5;
// // int pushbutton = D3;

// // bool previousMotionValue = LOW;
// // unsigned long currentMillis = 0;
// // unsigned long previousMillis = 0;   // Store the last time the LED was updated
// // const long interval = 500;  // Interval at which to blink (milliseconds)
// // bool flashing = false;  // initial state of flashing
// // bool ledState = LOW;    // initial state of led
// // unsigned long startTime = 0;    // Store the time when the LED started flashing
// // int currentLED = -1; // 0: green, 1: red, -1: none
// // int currentbutton = 0; // variable for button state
// // unsigned long currentbuttonMillis = 0; // Store the last time the button was pressed
// // unsigned long previousbuttonMillis = 0; // Store the last time the button was released

// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// //     pinMode(greenLED, OUTPUT);
// //     pinMode(redLED, OUTPUT);
// //     pinMode(lightSensor, INPUT);
// //     pinMode(potenPin, INPUT);
// //     pinMode(LEDPin, OUTPUT);
// //     pinMode(pushbutton, INPUT_PULLDOWN);
// //     pinMode(redLEDforbutton, OUTPUT);
// //     pinMode(greenLEDforbutton, OUTPUT); // LED for button
// //     digitalWrite(greenLED, HIGH);   // turn off the green LED at the beginning
// //     digitalWrite(redLED, HIGH); // turn off the red LED at the beginning
// //     digitalWrite(redLEDforbutton, HIGH); // turn off the red LED at the beginning
// //     digitalWrite(greenLEDforbutton, HIGH); // turn off the green LED at the beginning
// //     Serial.begin(9600); // Initialize serial communication
// // }

// // void loop()
// // {

// //     int potenValue = analogRead(potenPin); // Read potentiometer value
// //     int brightness = map(potenValue, 0, 4095, 0, 255); // Map potentiometer value to brightness

// //     // button state
// //     int buttonValue = digitalRead(pushbutton); // Read button state
// //     if (buttonValue == HIGH)
// //     currentbuttonMillis = millis(); // Store the time when the button was pressed
// //     if (currentbuttonMillis - previousbuttonMillis >= 500) // 0.5 seconds debouncing
// //     {
// //         if (currentbutton == 1) // if the button is pressed
// //         {
// //             currentbutton = 0;
// //             previousbuttonMillis = currentbuttonMillis; // Store the time when the button was released
// //             digitalWrite(greenLEDforbutton, HIGH); // turn on gr een LED
// //             digitalWrite(redLEDforbutton, LOW); // turn off red LED
// //             Serial.println("Button Pressed! Red LED ON.");

// //         }
// //         else // if the button is not pressed
// //         {
// //             currentbutton = 1;
// //             previousbuttonMillis = currentbuttonMillis; // Store the time when the button was released
// //             digitalWrite(greenLEDforbutton, LOW); // turn off green LED
// //             digitalWrite(redLEDforbutton, HIGH); // turn on red LED
// //             Serial.println("Button Released! Green LED ON.");
// //         }
// //     }

// //     // Control White LED based on potentiometer value
// //     analogWrite(LEDPin, brightness);

// //     // light sensor
// //     int lightValue = analogRead(lightSensor); // Read light sensor value
// //     float vout = (lightValue * 3.3) / 4096; //
// //     float lightresistance = ((1000*3.3)/(vout/2.35))-1000; // gain 1.4, use 1k ohm with lightsensor
// //     float lux = pow((lightresistance/80077.0),(1/-0.761)); // lux value

// //     int motionValue = digitalRead(motionSensor); // Read motion sensor value
// //     // bool motionValue = digitalRead(motionSensor);
// //     // Print the motion sensor and light sensor values
// //     Serial.print("Motion Sensor Value: ");
// //     Serial.println(motionValue);
// //     Serial.print("Lux Value: ");
// //     Serial.println(lux);

// //     // when detect motion and light level is above threshold, flash green LED

// //     if (motionValue == HIGH && !flashing)  // if when motion detected and led is not flashing
// //     {
// //         if (lux > luxThreshold)  // when light value is above set threshold (which is light sensor detect certain lux level(which is lights on somewhere))
// //         {
// //             Serial.println("Motion Detected! Flashing Green LED for 5 seconds.");
// //             flashing = true;
// //             currentLED = 0; // green
// //         }
// //         else    // when detect motion and light level is below threshold, flash red led
// //         {
// //             Serial.println("Motion Detected! Flashing Red LED for 5 seconds.");
// //             flashing = true;
// //             currentLED = 1; // red
// //         }

// //             }

// //     // when it flashing, change LED state every 500ms
// //     if (flashing)   // flashing state
// //     {
// //         currentMillis = millis();
// //         // when its true, every 500ms, change LED state
// //         if (currentMillis - previousMillis >= 500)    //  if the current time - previous time is greater than the interval, then we need to change the led state
// //         // this is how we can make the led flashing, because we need to check the time every 500ms, and if the time is up, we need to change the led state
// //         {
// //             previousMillis = currentMillis;
// //             ledState = !ledState;

// //             if (currentLED == 0) // green
// //             {
// //                 digitalWrite(greenLED, ledState);
// //                 digitalWrite(redLED, HIGH); // ensure red is off
// //             }
// //             else if (currentLED == 1) // red
// //             {
// //                 digitalWrite(redLED, ledState);
// //                 digitalWrite(greenLED, HIGH); // ensure green is off
// //             }
// //         }

// //         // after 5 seconds of no motion detected, turn off the LED
// //         if (currentMillis - startTime >= 5000)
// //         {
// //             startTime = currentMillis;
// //             flashing = false;
// //             digitalWrite(greenLED, HIGH);
// //             digitalWrite(redLED, HIGH);
// //             currentLED = -1;
// //         }
// //     }

// //     // when no motion detected, turn off the LED
// //     if (motionValue == LOW && !flashing)
// //     {
// //         digitalWrite(greenLED, HIGH);
// //         digitalWrite(redLED, HIGH);
// //     }

// //     delay(100); // delay for stability
// // }
// //----------------------------------------------
// // new motion + light sensor test, added opamp with gain of 2, G = 1 + R2/R1, light sensor with extra resistance 1kohm
// // int motionSensor = D4; // Motion sensor pin
// // int greenLED = A0;     // Green LED pin
// // int redLED = A1;       // Red LED pin
// // int lightSensor = A2; // Light sensor pin
// // int luxThreshold = 500; // Lux threshold for light sensor

// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// //     pinMode(greenLED, OUTPUT);
// //     pinMode(redLED, OUTPUT);
// //     pinMode(lightSensor, INPUT);
// //     digitalWrite(greenLED, HIGH);   // turn off the green LED at the beginning
// //     digitalWrite(redLED, HIGH); // turn off the red LED at the beginning
// //     Serial.begin(9600); // Initialize serial communication
// // }

// // void loop()
// // {

// //     int lightvalue = analogRead(lightSensor); // Read light sensorConvert to voltage
// //     float vout = (lightvalue * 3.3) / 4096; //
// //     float lightresistance = ((1000*3.3)/(vout/2.35))-1000; // gain 1.4, use 1k ohm with lightsensor
// //     float lux = pow((lightresistance/80077.0),(1/-0.761)); // lux value
// //     //float lux = 397500.12*exp(lightresistance*-0.1);
// //     // int motionValue = digitalRead(motionSensor); // Read motion sensor value

// //     // make code to see what lux value
// //     Serial.print("Photo vout value: ");
// //     Serial.println(vout);
// //     Serial.print("Photo R value: ");
// //     Serial.println(lightresistance);
// //     Serial.print("Lux Value: ");
// //     Serial.println(lux);
// //     delay(1000); // delay for stability

// // }
// //----------------------------------------------------
// // int lightsensor = A5; // Light Sensor

// // // setup() runs once, when the device is first turned on.
// // void setup()
// // {
// //     pinMode(lightsensor, INPUT);
// //     // pinMode(D2, INPUT);
// // }

// // // loop() runs over and over again, as quickly as it can execute.
// // void loop()
// // {
// //     // The core of your code will likely live here.
// //     // printf("Value: %d", analogRead(a1));
// //     int lightvalue = analogRead(lightsensor); // Read light sensorConvert to voltage
// //     // Light Sensor
// //     Log.info("Value: %ld", analogRead(lightsensor));

// //     float vout = (lightvalue * 3.3) / 4096; //
// //     // Sound sensor
// //     // Log.info("Value: %ld", digitalRead(D2));
// //     delay(100);
// // }

// //----------------------------------------------------
// // int motionSensor = D4; // Motion sensor pin
// // int greenLED = A0;     // Green LED pin
// // int redLED = A1;       // Red LED pin
// // int lightSensor = A2; // Light sensor pin
// // int luxThreshold = 15; // Lux threshold for light sensor
// // int potenPin = A3; // Potentiometer pin
// // int LEDPin = D2;   // White LED pin
// // int redLEDforbutton = D6;
// // int greenLEDforbutton = D5;
// // int pushbutton = D3;

// // unsigned long currentMillis = 0;
// // unsigned long previousMillis = 0;   // Store the last time the LED was updated
// // const long interval = 500;  // Interval at which to blink (milliseconds)
// // bool flashing = false;  // initial state of flashing
// // bool ledState = LOW;    // initial state of led
// // unsigned long startTime = 0;    // Store the time when the LED started flashing
// // int currentLED = -1; // 0: green, 1: red, -1: none
// // int currentbutton = 0; // variable for button state
// // unsigned long currentbuttonMillis = 0; // Store the last time the button was pressed
// // unsigned long previousbuttonMillis = 0; // Store the last time the button was released

// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// //     pinMode(greenLED, OUTPUT);
// //     pinMode(redLED, OUTPUT);
// //     pinMode(lightSensor, INPUT);
// //     pinMode(potenPin, INPUT);
// //     pinMode(LEDPin, OUTPUT);
// //     pinMode(pushbutton, INPUT_PULLDOWN);
// //     pinMode(redLEDforbutton, OUTPUT);
// //     pinMode(greenLEDforbutton, OUTPUT); // LED for button
// //     digitalWrite(greenLED, HIGH);   // turn off the green LED at the beginning
// //     digitalWrite(redLED, HIGH); // turn off the red LED at the beginning
// //     digitalWrite(redLEDforbutton, HIGH); // turn off the red LED at the beginning
// //     digitalWrite(greenLEDforbutton, HIGH); // turn off the green LED at the beginning
// //     Serial.begin(9600); // Initialize serial communication
// // }

// // void loop()
// // {
// //     int potenValue = analogRead(potenPin); // Read potentiometer value
// //     int brightness = map(potenValue, 0, 4095, 0, 255); // Map potentiometer value to brightness

// //     // button state
// //     int buttonValue = digitalRead(pushbutton); // Read button state
// //     if (buttonValue == HIGH)
// //     currentbuttonMillis = millis(); // Store the time when the button was pressed
// //     if (currentbuttonMillis - previousbuttonMillis >= 500) // 0.5 seconds debouncing
// //     {
// //         if (currentbutton == 1) // if the button is pressed
// //         {
// //             currentbutton = 0;
// //             previousbuttonMillis = currentbuttonMillis; // Store the time when the button was released
// //             digitalWrite(greenLEDforbutton, HIGH); // turn on gr een LED
// //             digitalWrite(redLEDforbutton, LOW); // turn off red LED
// //             Serial.println("Button Pressed! Red LED ON.");

// //         }
// //         else // if the button is not pressed
// //         {
// //             currentbutton = 1;
// //             previousbuttonMillis = currentbuttonMillis; // Store the time when the button was released
// //             digitalWrite(greenLEDforbutton, LOW); // turn off green LED
// //             digitalWrite(redLEDforbutton, HIGH); // turn on red LED
// //             Serial.println("Button Released! Green LED ON.");
// //         }
// //     }

// //     // Control White LED based on potentiometer value
// //     analogWrite(LEDPin, brightness);

// //     // light sensor
// //     int lightValue = analogRead(lightSensor); // Read light sensor value
// //     float vout = (lightValue * 3.3) / 4096; //
// //     float lightresistance = ((1000*3.3)/(vout/2.35))-1000; // gain 1.4, use 1k ohm with lightsensor
// //     float lux = pow((lightresistance/80077.0),(1/-0.761)); // lux value

// //     int motionValue = digitalRead(motionSensor); // Read motion sensor value
// //     // Print the motion sensor and light sensor values
// //     Serial.print("Motion Sensor Value: ");
// //     Serial.println(motionValue);
// //     Serial.print("Lux Value: ");
// //     Serial.println(lux);

// //     delay(10); // delay for stability
// // }

// //----------------------------------------------------

// // int motionSensor = D4; // Motion sensor pin
// // int greenLED = A0;     // Green LED pin
// // int redLED = A1;       // Red LED pin
// // int lightSensor = A2; // Light sensor pin
// // int luxThreshold = 15; // Lux threshold for light sensor
// // int potenPin = A3; // Potentiometer pin
// // int LEDPin = D2;   // White LED pin
// // int redLEDforbutton = D6;
// // int greenLEDforbutton = D5;
// // int pushbutton = D3;
// // int ledtime = 0; // variable for led time
// // int Led2 = 0; // variable for led state (0: off, 1: green, 2: red)

// // bool previousMotionValue = LOW;
// // unsigned long currentMillis = 0;
// // unsigned long previousMillis = 0;   // Store the last time the LED was updated
// // const long interval = 500;  // Interval at which to blink (milliseconds)
// // bool flashing = false;  // initial state of flashing
// // bool ledState = LOW;    // initial state of led
// // unsigned long startTime = 0;    // Store the time when the LED started flashing
// // int currentLED = -1; // 0: green, 1: red, -1: none
// // int currentbutton = 0; // variable for button state
// // unsigned long currentbuttonMillis = 0; // Store the last time the button was pressed
// // unsigned long previousbuttonMillis = 0; // Store the last time the button was released
// // unsigned long lastMotionTime = 0;
// // void setup()
// // {
// //     pinMode(motionSensor, INPUT);
// //     pinMode(greenLED, OUTPUT);
// //     pinMode(redLED, OUTPUT);
// //     pinMode(lightSensor, INPUT);
// //     pinMode(potenPin, INPUT);
// //     pinMode(LEDPin, OUTPUT);
// //     pinMode(pushbutton, INPUT_PULLDOWN);
// //     pinMode(redLEDforbutton, OUTPUT);
// //     pinMode(greenLEDforbutton, OUTPUT); // LED for button
// //     digitalWrite(greenLED, HIGH);   // turn off the green LED at the beginning
// //     digitalWrite(redLED, HIGH); // turn off the red LED at the beginning
// //     digitalWrite(redLEDforbutton, HIGH); // turn off the red LED at the beginning
// //     digitalWrite(greenLEDforbutton, HIGH); // turn off the green LED at the beginning
// //     Serial.begin(9600); // Initialize serial communication
// // }

// // void loop()
// // {

// //     int potenValue = analogRead(potenPin); // Read potentiometer value
// //     int brightness = map(potenValue, 0, 4095, 0, 255); // Map potentiometer value to brightness

// //     // button state
// //     int buttonValue = digitalRead(pushbutton); // Read button state
// //     if (buttonValue == HIGH)
// //     currentbuttonMillis = millis(); // Store the time when the button was pressed
// //     if (currentbuttonMillis - previousbuttonMillis >= 500) // 0.5 seconds debouncing
// //     {
// //         if (currentbutton == 1) // if the button is pressed
// //         {
// //             currentbutton = 0;
// //             previousbuttonMillis = currentbuttonMillis; // Store the time when the button was released
// //             digitalWrite(greenLEDforbutton, HIGH); // turn on gr een LED
// //             digitalWrite(redLEDforbutton, LOW); // turn off red LED
// //             Serial.println("Button Pressed! Red LED ON.");

// //         }
// //         else // if the button is not pressed
// //         {
// //             currentbutton = 1;
// //             previousbuttonMillis = currentbuttonMillis; // Store the time when the button was released
// //             digitalWrite(greenLEDforbutton, LOW); // turn off green LED
// //             digitalWrite(redLEDforbutton, HIGH); // turn on red LED
// //             Serial.println("Button Released! Green LED ON.");
// //         }
// //     }

// //     // Control White LED based on potentiometer value
// //     analogWrite(LEDPin, brightness);

// //     // light sensor
// //     int lightValue = analogRead(lightSensor); // Read light sensor value
// //     float vout = (lightValue * 3.3) / 4096; //
// //     float lightresistance = ((1000*3.3)/(vout/2.35))-1000; // gain 1.4, use 1k ohm with lightsensor
// //     float lux = pow((lightresistance/80077.0),(1/-0.761)); // lux value

// //     int motionValue = digitalRead(motionSensor); // Read motion sensor value
// //     // Print the motion sensor and light sensor values
// //     Serial.print("Motion Sensor Value: ");
// //     Serial.println(motionValue);
// //     Serial.print("Lux Value: ");
// //     Serial.println(lux);

// //     currentMillis = millis();

// // //
// // if (motionValue == HIGH) {
// //     lastMotionTime = currentMillis;

// //     if (!flashing) {
// //       flashing = true;
// //           ledState = LOW;
// //           previousMillis = currentMillis;
// //           if (lux > luxThreshold) {
// //             currentLED = 0;  // green
// //             Serial.println("Motion Detected! Flashing Green LED");
// //           } else {
// //             currentLED = 1;  // red
// //             Serial.println("Motion Detected! Flashing Red LED");
// //           }
// //         }
// //       }

// //       //
// //       if (flashing) {
// //         if (currentMillis - previousMillis >= 500) {
// //           previousMillis = currentMillis;
// //           ledState = !ledState;

// //           if (currentLED == 0) {
// //             digitalWrite(greenLED, ledState);
// //             digitalWrite(redLED, HIGH);
// //           } else if (currentLED == 1) {
// //             digitalWrite(redLED, ledState);
// //             digitalWrite(greenLED, HIGH);
// //           }
// //         }

// //         //
// //         if (currentMillis - lastMotionTime >= 5000) {
// //           flashing = false;
// //           digitalWrite(greenLED, HIGH);
// //           digitalWrite(redLED, HIGH);
// //           currentLED = -1;
// //         }
// //       }

// //     delay(100); // delay for stability
// // }
// //------------------------------------------

// // Pin Definitions
// int motionSensor = D4;      // Motion sensor pin
// int greenLED = A0;          // Green LED pin
// int redLED = A1;            // Red LED pin
// int lightSensor = A2;       // Light sensor pin
// int luxThreshold = 20;      // Lux threshold for light sensor
// int potenPin = A3;          // Potentiometer pin
// int LEDPin = D2;            // White LED pin
// int redLEDforbutton = D6;   // Red LED for button
// int greenLEDforbutton = D5; // Green LED for button
// int pushbutton = D3;        // Button pin

// // Motion sensor variables
// bool previousMotionValue = LOW;   // Store the previous state of the motion sensor
// unsigned long lastMotionTime = 0; // Store the last time motion was detected
// // LED flashing variables
// unsigned long currentMillis = 0;  // Store the current time
// unsigned long previousMillis = 0; // Store the last time the LED was updated
// const long interval = 500;        // Blink interval (500ms)
// bool flashing = false;            // Flag to indicate if the LED is flashing
// bool ledState = LOW;              // current state of blinking led (HIGH/LOW)
// int currentLED = -1;              // which LED is flashing (0: green, 1: red, -1: none)

// // Button state variables
// int currentbutton = 0;                  // variable for button state (0: released, 1: pressed)
// unsigned long currentbuttonMillis = 0;  // Store the last time the button was pressed
// unsigned long previousbuttonMillis = 0; // Store the last time the button was released

// void setup()
// {
//   pinMode(motionSensor, INPUT);        // Motion sensor as input
//   pinMode(greenLED, OUTPUT);           // Green LED as output
//   pinMode(redLED, OUTPUT);             // Red LED as output
//   pinMode(lightSensor, INPUT);         // Light sensor as input
//   pinMode(potenPin, INPUT);            // Potentiometer as input
//   pinMode(LEDPin, OUTPUT);             // White LED as output
//   pinMode(pushbutton, INPUT_PULLDOWN); // Pull-down resistor for button
//   pinMode(redLEDforbutton, OUTPUT);    // Red LED for button
//   pinMode(greenLEDforbutton, OUTPUT);  // Green LED for button

//   // Initialize LEDs to off state
//   digitalWrite(greenLED, HIGH);          // turn off the green LED at the beginning
//   digitalWrite(redLED, HIGH);            // turn off the red LED at the beginning
//   digitalWrite(redLEDforbutton, HIGH);   // turn off the red LED at the beginning
//   digitalWrite(greenLEDforbutton, HIGH); // turn off the green LED at the beginning
//   Serial.begin(9600);                    // Initialize serial communication
// }

// void loop()
// {
//   //===============================================================
//   // POTENTIOMETER AND LIGHT INTESNITY CONTROL
//   int potenValue = analogRead(potenPin);             // Read potentiometer value
//   int brightness = map(potenValue, 0, 4095, 0, 255); // Map potentiometer value to brightness(DutyCycle)
//   analogWrite(LEDPin, brightness);                   // set brightness of white LED
//   // Control White LED based on potentiometer value
//   Serial.print("Potentiometer Value: ");
//   Serial.println(potenValue);
//   Serial.print("Brightness(DutyCycle): ");
//   Serial.println(brightness);
//   //===============================================================

//   //===============================================================
//   // BUTTON STATE WITH DEBOUNCING
//   int buttonValue = digitalRead(pushbutton); // Read button state
//   if (buttonValue == HIGH)
//     currentbuttonMillis = millis();                      // Store the time when the button was pressed
//   if (currentbuttonMillis - previousbuttonMillis >= 500) // 0.5 seconds debouncing - only process button after 500ms of stable state
//   {
//     if (currentbutton == 1) // if the button was pressed
//     {
//       currentbutton = 0;
//       previousbuttonMillis = currentbuttonMillis; // Store the time when the button was released
//       digitalWrite(greenLEDforbutton, HIGH);      // turn on green LED
//       digitalWrite(redLEDforbutton, LOW);         // turn off red LED
//       Serial.println("Button Pressed! Red LED ON.");
//     }
//     else // if the button was released
//     {
//       currentbutton = 1;
//       previousbuttonMillis = currentbuttonMillis; // Store the time when the button was released
//       digitalWrite(greenLEDforbutton, LOW);       // turn off green LED
//       digitalWrite(redLEDforbutton, HIGH);        // turn on red LED
//       Serial.println("Button Released! Green LED ON.");
//     }
//   }
//   //===============================================================

//   //===============================================================
//   // LIGHT SENSOR
//   int lightValue = analogRead(lightSensor);                      // Read light sensor value(12bit,0-4095)
//   float vout = (lightValue * 3.3) / 4096;                        // convert to voltage (0 - 3.3V)
//   float lightresistance = ((1000 * 3.3) / (vout / 2.35)) - 1000; // gain 2.35, use 1k ohm with lightsensor(voltage divider)
//   float lux = pow((lightresistance / 80077.0), (1 / -0.761));    // calculate lux value based on the charactersitics of the light sensor
//   Serial.print("Lux Value: ");
//   Serial.println(lux);
//   //===============================================================

//   //===============================================================
//   // MOTION SENSOR AND LED FLASHING
//   int motionValue = digitalRead(motionSensor); // Read motion sensor value
//   Serial.print("Motion Sensor Value: ");
//   Serial.println(motionValue);

//   // Motion detected (PIR goes HIGH)
//   if (motionValue == HIGH)
//   {
//     lastMotionTime = millis();
//     // If motion is detected, update the last motion time

//     // if no flashing atm, start flashing
//     if (!flashing)
//     {
//       if (lux > luxThreshold)
//       { // Bright condition
//         // If the light level is above the threshold, flash GREEN LED
//         flashing = true;
//         currentLED = 0;
//         Serial.println("Motion Detected! Flashing GREEN LED.");
//       }
//       else
//       { // Dark condition
//         // If the light level is below the threshold, flash RED LED
//         flashing = true;
//         currentLED = 1;
//         Serial.println("Motion Detected! Flashing RED LED.");
//       }
//     }
//   }

//   // If in flashing state
//   if (flashing)
//   {
//     currentMillis = millis(); // Get current time

//     // Check for light condition changes during flashing
//     if (lux > luxThreshold && currentLED == 1)
//     {
//       // If the light level is above the threshold and the current LED is RED, switch to GREEN
//       Serial.println("Light turned ON during flashing. Switching to GREEN.");
//       currentLED = 0;
//     }
//     else if (lux <= luxThreshold && currentLED == 0)
//     {
//       // If the light level is below the threshold and the current LED is GREEN, switch to RED
//       Serial.println("Light turned OFF during flashing. Switching to RED.");
//       currentLED = 1;
//     }

//     // Blink the appropriate LED every 500ms
//     if (currentMillis - previousMillis >= 500)
//     {
//       previousMillis = currentMillis; // Update last action time to current time
//       // to set a new reference point for the next cycle calculation
//       // ex: If current time is 1200ms, next toggle occurs at 1700ms (1200+500)
//       ledState = !ledState; // Toggle LED state

//       // Control currently selected LED (GREEN/RED)
//       if (currentLED == 0) // When GREEN LED is selected
//       {
//         digitalWrite(greenLED, ledState); // Blink only GREEN
//         digitalWrite(redLED, HIGH);       // Keep RED OFF
//       }
//       else if (currentLED == 1) // When RED LED is selected
//       {
//         digitalWrite(redLED, ledState); // Blink only RED
//         digitalWrite(greenLED, HIGH);   // Keep GREEN OFF
//       }
//     }

//     // Timeout check - if no motion for 5 seconds(2.5s sensor + 2.5s delay), turn off
//     if (currentMillis - lastMotionTime >= 2500)
//     {
//       flashing = false;
//       digitalWrite(greenLED, HIGH); // Turn off green LED
//       digitalWrite(redLED, HIGH);   // Turn off red LED
//       currentLED = -1;              // Reset active LED state, -1 = No LED active, 0 = Green, 1 = Red
//     }
//   }
//   else
//   {
//     digitalWrite(greenLED, HIGH); // Ensure green OFF
//     digitalWrite(redLED, HIGH);   // Ensure red OFF
//   }
//   //===============================================================
  
//   delay(50); // Small delay for system stability
// }

// //----------------------------------------------------
