// test-a.ino
// Mon Jul  5 07:49:00 UTC 2021

// REFLASH testing/compiling-tests in progress.  No expected result.

// status: rom_func stuff (lifted) compiles cleanly. No tests on reflash (at all).
// Prints continuously. Experimental reflash() in progress - not compiling yet.
// Blinks visibly at least one time. LED builtin remains ON.

#include "pico/bootrom.h" // why does this not fail to an orange error message

// COMPILES CLEAN - seriously this is strange to see. ;)

// lifted code pico-sdk (included copy during mbed arduino system install):
typedef void *(*rom_table_lookup_fn)(uint16_t *table, uint32_t code);
#define rom_hword_as_ptr(rom_address) (void *)(uintptr_t)(*(uint16_t *)rom_address)

void *rom_func_lookup(uint32_t code) {
    rom_table_lookup_fn rom_table_lookup = (rom_table_lookup_fn) rom_hword_as_ptr(0x18) ; // added tail
    uint16_t *func_table = (uint16_t *) rom_hword_as_ptr(0x14);
    return rom_table_lookup(func_table, code);
}

void reflash(void) {
    reset_usb_boot(0, 0);
    Serial.println("placeholder");
}

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
    int timeout_count = 222; // reflash timeout functionality planned
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

/*
    do {
        timeout_count--;
        print_beacon();
        led_foo();
        waitloop(); waitloop(); waitloop(); waitloop();
        if (timeout_count == 0) { reflash(); } // no expectations yet.  Testing.
    } while (1);
*/
    longwaitloop();
    led_foo(); longwaitloop();
    led_foo(); longwaitloop();
    led_foo(); longwaitloop();
    longwaitloop(); longwaitloop();
    longwaitloop(); longwaitloop();
    reflash(); // may be a while before this activates!
}
void loop() { while (1); }
