// test-b.ino
// Mon Jul  5 08:46:18 UTC 2021
// Mon Jul  5 07:49:00 UTC 2021

#include "pico/bootrom.h"

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

    int timeout_count = 11; // reflash timeout functionality planned

    do {
        timeout_count--;
        print_beacon();
        led_foo();
        waitloop(); waitloop(); waitloop(); waitloop();
        if (timeout_count == 0) { reflash(); } // no expectations yet.  Testing.
    } while (1);

    led_foo();
    led_foo();
    led_foo();
    longwaitloop();
    // reflash(); // may be a while before this activates!
}
void loop() { while (1); }
