// test-d.ino
// Mon Jul  5 11:13:08 UTC 2021
// Mon Jul  5 07:49:00 UTC 2021

// kinda-sorta works - no serial at all

#include "pico/bootrom.h"
#include "mbed.h"
#include "rtos.h" // no issue just including the .h file

#define WAIT_COUNT 1200000
#define TRUE_P -1
#define FALSE_P 0

#define MS(x) chrono::milliseconds(x) // element14
using namespace std; // we will be using std::chrono // element14
using namespace rtos; // we will be using rtos::ThisThread // element14
using namespace arduino; // shot in the dark

Thread the_only_thread;

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

void led_bar(void) {
    digitalWrite(LED_BUILTIN, 1);
    waitloop();
    digitalWrite(LED_BUILTIN, 0);
    waitloop();
    waitloop();
    waitloop();
}

void led_foo(void) {
    waitloop();
    digitalWrite(LED_BUILTIN, 1);
    waitloop(); waitloop();
    digitalWrite(LED_BUILTIN, 0);
    waitloop(); waitloop();
}

uint8_t count_here = 0;

// void led_red_function() // element14
void fn_print_beacon_thread(void) { // continuous message
    // setup_serial();
    for (int i=55;i>0;i--) {
    count_here++;
    Serial.print("  BEACON ");
    // Serial.print(count_here);
    digitalWrite(LED_BUILTIN, 1);
    ThisThread::sleep_for(MS(32));

    digitalWrite(LED_BUILTIN, 0);
    ThisThread::sleep_for(MS(128));
    }
}

void setup_serial(void) {
    // longwaitloop();
}

void setup_gpio(void) {
    pinMode(LED_BUILTIN, 1);
}

void p_setup(void) {
    setup_gpio();
}

void setup() { // int main()
    p_setup();
    Serial.begin(115200);
    int timeout_count = 33; // reflash timing by counting
    bool times_up = TRUE_P;

    the_only_thread.start(fn_print_beacon_thread); // element14

    do {
        timeout_count--;
        // ATTN // print_beacon();
        led_bar(); waitloop();
        if (timeout_count == 0) {
            times_up = FALSE_P; // loop exit mechanism
        }
    } while (times_up);

    // Serial.println(" A252B unique message!");

    led_foo(); led_foo(); led_foo(); led_foo(); led_foo();

    reflash(); // RPI_RP2 thumb-drive like entity exposed to operating system via USB
}


void loop() {
    Serial.println("NEVER SEEN.");
    longwaitloop(); longwaitloop();
    while (1);
}

