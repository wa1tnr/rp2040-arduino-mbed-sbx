// test-a.ino
// Mon Jul  5 07:03:40 UTC 2021

// status: Prints continuously.
// Blinks visibly at least one time. LED builtin remains ON.

void waitloop(void) {
    for (volatile int i=1800000;i>0;i--); // do not optimize
}

void longwaitloop(void){for (volatile int i=27;i>0;i--){ waitloop();}}

void led_foo(void) {
    waitloop(); digitalWrite(LED_BUILTIN, 1); waitloop(); waitloop(); digitalWrite(LED_BUILTIN, 0); waitloop(); waitloop(); digitalWrite(LED_BUILTIN, 1);
}

uint8_t count = 0;
void print_beacon(void) { // continuous message
    count++;
    Serial.print("  BEACON ");
    Serial.print(count);
}

void setup() {
    pinMode(LED_BUILTIN, 1);
    digitalWrite(LED_BUILTIN, 0);
    longwaitloop();
    digitalWrite(LED_BUILTIN, 1);
    longwaitloop();
    digitalWrite(LED_BUILTIN, 0);
    longwaitloop();
    Serial.begin(9600);
    longwaitloop();
    longwaitloop();

    do {
        print_beacon();
        led_foo();
        waitloop(); waitloop(); waitloop(); waitloop();
    } while (1);
}
void loop() { while (1); }
