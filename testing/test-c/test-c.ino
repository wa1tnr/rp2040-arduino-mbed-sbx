// test-c.ino
// Mon Jul  5 09:41:37 UTC 2021
// Mon Jul  5 07:49:00 UTC 2021

#include "pico/bootrom.h"
#define WAIT_COUNT 1200000
#define TRUE_P -1
#define FALSE_P 0

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
}

void waitloop(void) {
    for (volatile int i=WAIT_COUNT;i>0;i--); // do not optimize
}

void longwaitloop(void) {
    for (volatile int i=27;i>0;i--) {
        waitloop();
    }
}

void led_foo(void) {
    waitloop();
    digitalWrite(LED_BUILTIN, 1);
    waitloop(); waitloop();
    digitalWrite(LED_BUILTIN, 0);
    waitloop(); waitloop();
    digitalWrite(LED_BUILTIN, 1);
}

uint8_t count = 0;
void print_beacon(void) { // continuous message
    count++;
    Serial.print("  BEACON ");
    Serial.print(count);
}

void setup_serial(void) {
    Serial.begin(9600);
    longwaitloop();
}

void setup_gpio(void) {
    pinMode(LED_BUILTIN, 1);
    digitalWrite(LED_BUILTIN, 0);
    longwaitloop();
    digitalWrite(LED_BUILTIN, 1);
    longwaitloop();
    digitalWrite(LED_BUILTIN, 0);
}

void setup() {
    int timeout_count = 4; // reflash timing by counting
    bool times_up = TRUE_P;

    setup_gpio();
    setup_serial();

    do {
        timeout_count--;
        print_beacon();
        led_foo();
        waitloop();
        if (timeout_count == 0) {
            times_up = FALSE_P; // loop exit mechanism
        }
    } while (times_up);

    led_foo(); led_foo(); led_foo();

    Serial.println("7C9A2 unique message!");

    longwaitloop(); longwaitloop();

    reflash(); // RPI_RP2 thumb-drive like entity exposed to operating system via USB
}

void loop() {
    Serial.println("NEVER SEEN.");
    longwaitloop(); longwaitloop();
    while (1);
}
