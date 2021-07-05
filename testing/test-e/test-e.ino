// test-e.ino
// Mon Jul  5 11:48:25 UTC 2021
// Mon Jul  5 07:49:00 UTC 2021

// minimally functional - many questions unanswered.  But 'works' sort of.

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

void waxitloop(void) {
    for (volatile int i=WAIT_COUNT;i>0;i--); // do not optimize
}

void led_bar(void) {
    digitalWrite(LED_BUILTIN, 1);
    ThisThread::sleep_for(MS(90));
    digitalWrite(LED_BUILTIN, 0);
    ThisThread::sleep_for(MS(500));
}

void led_foo(void) {
    ThisThread::sleep_for(MS(11));
    digitalWrite(LED_BUILTIN, 1);
    ThisThread::sleep_for(MS(11));
    digitalWrite(LED_BUILTIN, 0);
    ThisThread::sleep_for(MS(711));
}

uint8_t count_here = 0;

// void led_red_function() // element14
void fn_print_beacon_thread(void) { // continuous message
    for (int i=33;i>0;i--) {
    count_here++;
    Serial.print("  BEACON ");
    Serial.print(count_here);
    digitalWrite(LED_BUILTIN, 1);
    ThisThread::sleep_for(MS(5));

    digitalWrite(LED_BUILTIN, 0);
    ThisThread::sleep_for(MS(92));
    }
}

void setup_serial(void) {
    Serial.begin(115200);
}

void setup_gpio(void) {
    pinMode(LED_BUILTIN, 1);
}

void p_setup(void) {
    setup_gpio();
    setup_serial();
    for (int i=5;i>0;i--) { led_foo(); }
}

void setup() { // int main()
    p_setup();
    int timeout_count = 9; // reflash timing by counting
    bool times_up = TRUE_P;

    the_only_thread.start(fn_print_beacon_thread); // element14

    do {
        timeout_count--;
        // ATTN // print_beacon();
        led_bar();
        Serial.print(" KXG");
        if (timeout_count == 0) {
            times_up = FALSE_P; // loop exit mechanism
        }
    } while (times_up);

    Serial.println(" E4C9A unique message!");

    for (int i=12;i>0;i--) { led_foo(); }

    reflash(); // RPI_RP2 thumb-drive like entity exposed to operating system via USB
}


void loop() {
    Serial.println("NEVER SEEN.");
    while (1);
}

