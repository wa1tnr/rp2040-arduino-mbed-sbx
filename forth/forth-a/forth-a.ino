// forth-a.ino
// Mon Jul  5 15:04:56 UTC 2021
// Mon Jul  5 07:49:00 UTC 2021

// #include <stdio.h>
// #include "pico/stdio.h"
// #include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "mbed.h" // system_reset() requires this
#include "rtos.h" // no issue just including the .h file

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define TRUE_P -1
#define FALSE_P 0

#define MS(x) chrono::milliseconds(x) // element14

uint8_t count_here = 0;

/* Tiny interpreter,
   similar to myforth's Standalone Interpreter
   This example code is in the public domain */

typedef void (*func)(void);         // signature of functions in dictionary

/* Structure of a dictionary entry */
typedef struct {
  const char*    name;              // Pointer the Word Name in flash
  const func     function;          // Pointer to function
} entry;

/* Data stack for parameter passing
   This "stack" is circular,
   like a Green Arrays F18A data stack,
   so overflow and underflow are not possible
   Number of items must be a power of 2 */
const int STKSIZE = 8;
const int STKMASK = 7;
// int stack[STKSIZE];
int stack[8];
int p = 0;

int tickstate = 0; // used to differentiate keyboard input after a tick
int crlfstate = 0; // differentiate when ascii 13 is entered to the terminal

/* TOS is Top Of Stack */
#define TOS stack[p]
/* NAMED creates a string in flash */
#define NAMED(x, y) const char x[]=y

/* Terminal Input Buffer for interpreter */
// const byte maxtib = 16;
#define maxtib 16
char tib[maxtib];
/* buffer required for strings read from flash */
char namebuf[maxtib];
// byte pos;
uint8_t pos;

using namespace std; // we will be using std::chrono // element14
using namespace rtos; // we will be using rtos::ThisThread // element14
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

void led_batz(void) {
    digitalWrite(LED_BUILTIN, 1); ThisThread::sleep_for(MS(12)); digitalWrite(LED_BUILTIN, 0); ThisThread::sleep_for(MS(40));
    ThisThread::sleep_for(MS(300));
    digitalWrite(LED_BUILTIN, 1); ThisThread::sleep_for(MS(12)); digitalWrite(LED_BUILTIN, 0); ThisThread::sleep_for(MS(40));
    ThisThread::sleep_for(MS(700));
}

void led_bar(void) {
    digitalWrite(LED_BUILTIN, 1);
    ThisThread::sleep_for(MS(90));
    digitalWrite(LED_BUILTIN, 0);
    ThisThread::sleep_for(MS(500));
}

void led_pip(void) {
    ThisThread::sleep_for(MS(11));
    digitalWrite(LED_BUILTIN, 1);
    ThisThread::sleep_for(MS(3));
    digitalWrite(LED_BUILTIN, 0);
    ThisThread::sleep_for(MS(3711));
}

void led_foo(void) {
    ThisThread::sleep_for(MS(11));
    digitalWrite(LED_BUILTIN, 1);
    ThisThread::sleep_for(MS(11));
    digitalWrite(LED_BUILTIN, 0);
    ThisThread::sleep_for(MS(711));
}

void fn_print_beacon_thread(void) { // continuous message
    for (int i=23;i>0;i--) {
    count_here++;
    Serial.print("  BEACON ");
    Serial.print(count_here);
    digitalWrite(LED_BUILTIN, 1);
    ThisThread::sleep_for(MS(5));
    digitalWrite(LED_BUILTIN, 0);
    ThisThread::sleep_for(MS(92));
    }
}

void wait_serial(void) {
    for(;;) {
        if (Serial.available()) { break; }
        ThisThread::sleep_for(MS(128));
    }
}

void pre_serial(void) {
    led_pip();
}

void setup_serial(void) {
    Serial.begin(115200);
    while (!Serial) { pre_serial(); }
    ThisThread::sleep_for(MS(1128));
    Serial.print("Forth-like interpreter.  Please press any key to begin..");
    wait_serial();
}

void setup_gpio(void) {
    pinMode(LED_BUILTIN, 1);
    ThisThread::sleep_for(MS(1792));
}

void p_setup(void) {
    setup_gpio();
    setup_serial();
    for (int i=5;i>0;i--) {
        led_foo();
    }
}




// Mon Feb 15 00:59:04 UTC 2021
// wa1tnr
// interpret-a // forth-like interpreter

/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* push n to top of data stack */
void push(int n) {
  p = (p + 1)& STKMASK;
  TOS = n;
}

/* return top of stack */
int pop() {
  int n = TOS;
  p = (p - 1)& STKMASK;
  return n;
}


/* new forward declaration: xtTOadrs */

/*
NAMED(_xtTOadrs, "xt>adrs");
void xtTOadrs();
*/

/* new word: tick */
NAMED(_tick, "\'");
void tick() {
  tickstate = -1; // true: tickstate flag has been raised
}

/* Global delay timer */
int spd = 44;

/* top of stack becomes current spd */
NAMED(_speed, "speed");
void speed() {
  spd = pop();
}

/* discard top of stack */
NAMED(_drop, "drop");
void drop() {
  pop();
}

/* recover dropped stack item */
NAMED(_back, "back");
void back() {
  for (int i = 1; i < STKSIZE; i++) drop();
}

/* copy top of stack */
NAMED(_dup, "dup");
void dup() {
  push(TOS);
}

/* exchange top two stack items */
NAMED(_swap, "swap");
void swap() {
  int a;
  int b;
  a = pop();
  b = pop();
  push(a);
  push(b);
}

/* copy second on stack to top */
NAMED(_over, "over");
void over() {
  int a;
  int b;
  a = pop();
  b = pop();
  push(b);
  push(a);
  push(b);
}

/* add top two items */
NAMED(_add, "+");
void add() {
  int a = pop();
  TOS = a + TOS;
}

/* bitwise and top two items */
NAMED(_and, "and");
void and_() {
  int a = pop();
  TOS = a & TOS;
}

/* inclusive or top two items */
NAMED(_or, "or");
void or_() {
  int a = pop();
  TOS = a | TOS;
}

/* exclusive or top two items */
NAMED(_xor, "xor");
void xor_() {
  int a = pop();
  TOS = a ^ TOS;
}

/* invert all bits in top of stack */
NAMED(_invert, "invert");
void invert() {
  TOS = ~(TOS);
}

/* negate top of stack */
NAMED(_negate, "negate");
void n_negate() {
  TOS = -(TOS);
}

/* destructively display top of stack, decimal */
NAMED(_dot, ".");
void dot() {
  // Serial.print(pop());
  printf("\n%4X: ", pop());
  putchar(' ');
  // Serial.print(" ");
}

/* destructively display top of stack, hex */
NAMED(_dotHEX, ".h");
void dotHEX() {
  // Serial.print(0xffff & pop(), HEX);
  printf("\n%4X: ", pop());
  // Serial.print(" ");
  putchar(' ');
}

/* display whole stack, hex */
NAMED(_dotShex, ".sh");
void dotShex() {
  for (int i = 0; i < STKSIZE; i++) dotHEX();
}

/* display whole stack, decimal */
NAMED(_dotS, ".s");
void dotS() {
  for (int i = 0; i < STKSIZE; i++) dot();
}

/* new word: clearstack */
NAMED(_clearstack, "clearstack"); // ( n n n n ... -- ) 
void clearstack() {

  for (int i = 0; i < STKSIZE; i++) push(0);
  p = 0; // not especially required
}

/* delay TOS # of milliseconds */
NAMED(_delay, "delay");
void del() {
  // delay(pop());
  sleep_ms(pop());
}

void defspeed() {
    if ((spd < 2) || (spd > 22333) ) { spd = 100; }
}

/* dump 16 bytes of RAM in hex with ascii on the side */
void dumpRAM() {
  char buffer[5] = "";
  char *ram;
  int p = pop();
  ram = (char*)p;

  // sprintf(buffer, "%4x", p);
  printf(buffer, "%4x", p);

  // Serial.print(buffer);
  printf("%s", buffer);

  // Serial.print("   ");
  putchar(' '); putchar(' '); putchar(' ');

  for (int i = 0; i < 16; i++) {
    char c = *ram++;
    printf(buffer, " %2x", (c & 0xff));
    // Serial.print(buffer);
    printf("%s", buffer);
  }
  ram = (char*)p;
  // Serial.print("   ");
  putchar(' '); putchar(' '); putchar(' ');
  for (int i = 0; i < 16; i++) {
    buffer[0] = *ram++;
    if (buffer[0] > 0x7f || buffer[0] < ' ') buffer[0] = '.';
    buffer[1] = '\0';
    // Serial.print(buffer);
    printf("%s", buffer);
  }
  push(p + 16);
}

/* dump 256 bytes of RAM */
NAMED(_dumpr, "dump");
void rdumps() {
  for (int i = 0; i < 16; i++) {
    // Serial.println();
    putchar('\n'); putchar('\r');
    dumpRAM();
  }
}

/* End of Forth interpreter words */
/* ******************************************** */
/* Beginning of application words */


// yours here ;)


/* End of application words */
/* ******************************************** */
/* Now build the dictionary */

// -------------- bookmark mass paste in begins ----------------

/* everybody loves a nop */
NAMED(_nopp, "nop");
void nopp() { } // pad the dictionary

/* empty words don't cause an error */
NAMED(_nop, " ");
void nop() { }

/* new word: throw */
NAMED(_throw, "throw");
void throw_();

NAMED(_execadrs, "execadrs");
void execadrs(); // execute at address

NAMED(_execXT, "execxt"); // execute from an xt (execution token) lookup
void execXT();

NAMED(_xtTOword, "xt>word");
void xtTOword();

NAMED(_EXECUTE, "EXECUTE"); // ( xt -- ) action: execute
void EXECUTE(); // ( xt -- ) action: execute

NAMED(_xxt, "xxt"); // alias for EXECUTE - execute execution token
void xxt();

/* Forward declaration required here */
NAMED(_words, "words");
void words();

NAMED(_entries_, "entries");
void _entries();

/* table of names and function addresses in flash */
const entry dictionary[] = {
  {_nop, nop},
  {_throw, throw_},
  {_xtTOword, xtTOword},
/*
  {_xtTOadrs, xtTOadrs},
*/
  {_execadrs, execadrs},
  {_execXT, execXT},
  {_EXECUTE, EXECUTE},
  {_xxt, xxt},
  {_words, words},
  {_tick, tick},
  {_entries_, _entries},
  {_drop, drop},
  {_dup, dup},
  {_swap, swap},
  {_delay, del},
  {_nopp, nopp} // to pad dictionary
};

/* Number of words in the dictionary */
const int entries = sizeof dictionary / sizeof dictionary[0];

/*
void xtTOadrs() { // ( xt -- addrs )
  func function;
  int plc = pop();
  unsigned int adxrs;
  function = (func) pgm_read_word(&dictionary[plc].function);
  push((unsigned int) function);
  int a = pop();
  push(a - 1);
}
*/

void execadrs() { // ( adrs -- ) action: execute at adrs
  int a = pop();  // an address of a word's execution token
  push(a + 1);    // alignment - THUMB2 consequence per C.H. Ting, iirc - 30 Sep 2020
  func function = ((func) pop());
  function();
  // fix bottom of stack so that
  // adrs is reusable, with 'back EXECUTE':
  back(); push(1); // n_negate();
  /* NO NEGATE */
  add(); drop();
  // starts to look like forth, doesn't it.
}

// execute from an xt (execution token) lookup
/*
void execXT() { // ( xt -- ) action: execute    XT exec - the EXECUTE word
  xtTOadrs();  // ( xt -- addrs )
  execadrs();  // ( adrs -- ) action: execute at adrs
}
*/

/*
void EXECUTE() { // ( xt -- ) action: execute
  execXT();
}
*/

/* short alias for EXECUTE:  xxt 'execute execution token' */
/*
void xxt() {
  execXT();
}
*/

/* Display all words in dictionary */
void words() {
  for (int i = entries - 1; i >= 0; i--) {
    strcpy(namebuf, dictionary[i].name);
    Serial.print(namebuf);
    Serial.print(" ");
  }
}

/* How many words are in the dictionary? */
void _entries() {
  int a;
  a = entries;
  push(a);
}

void throw_() {
  Serial.print("\r\n        THROW:  -1 \r\n");
}

/* Find an xt of a word in the dictionary, returning its name */
void xtTOword() { // ( xt -- ) print: the words's name from the dictionary
  int i = pop();
  int j = entries - 1;
  if ( i < 0 or i > j ) { // bounds checking please
    throw_();
    return;
  }
  strcpy(namebuf, dictionary[i].name);
  Serial.print(namebuf);
  Serial.print(" ");
}

/* Find a word in the dictionary, returning its position */
int locate() {
  for (int i = entries - 1; i >= 0; i--) {
    strcpy(namebuf, dictionary[i].name);
    if (!strcmp(tib, namebuf)) return i;
  }
  return 0;
}

// -------------- bookmark mass paste in ends ----------------
void interpreter(void) { }

int setup_the_interpreter() {
    // sleep_ms(1800);
    // uart_init(UART_ID, BAUD_RATE);

    // gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    // gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // stdio_init_all();

    // sleep_ms(800);
    Serial.println("\r\n\r\n   fort-like interpreter r0.1.0-pre-alpha\r\n\r\n");

    while(1) {
        interpreter(); // camelforth
    }
}






void setup() {
    p_setup();
    int timeout_count = 9;
    bool times_up = TRUE_P;

    the_only_thread.start(fn_print_beacon_thread); // element14

    do {
        timeout_count--;
        led_batz();
        Serial.print(" KXG");
        if (timeout_count == 0) {
            times_up = FALSE_P;
        }
    } while (times_up);

    Serial.println(" B29DC unique message!");

    for (int i=7;i>0;i--) {
        led_bar();
    }

    reflash(); // RPI_RP2 exposed
    system_reset(); // no reflash
}

void loop() { while (1); } // required
