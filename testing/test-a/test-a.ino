// test-a.ino
// Mon Jul  5 06:53:22 UTC 2021

// status: blinks visibly at least one time. LED builtin remains ON.

void waitloop(void) {
    for (volatile int i=1800000;i>0;i--); // do not optimize
}

void led_foo(void) {
    waitloop(); digitalWrite(LED_BUILTIN, 1); waitloop(); waitloop(); digitalWrite(LED_BUILTIN, 0); waitloop(); waitloop(); digitalWrite(LED_BUILTIN, 1);
}

void print_beacon(void) { // continuous message
    Serial.print("  CC");
}

void setup() {
    pinMode(LED_BUILTIN, 1);
    digitalWrite(LED_BUILTIN, 0);

    Serial.begin(9600);

    waitloop(); digitalWrite(LED_BUILTIN, 1); waitloop(); waitloop(); digitalWrite(LED_BUILTIN, 0); waitloop(); waitloop(); digitalWrite(LED_BUILTIN, 1);

    waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop();
    waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop();
    waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop();
    waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop(); waitloop();
    Serial.println("This is a serial println message.");
    Serial.print("\n\n  AA");
    Serial.print("  BB");
    do {
        print_beacon();
        waitloop(); waitloop(); waitloop(); waitloop();
    } while (1);
}
void loop() { while (1); }
