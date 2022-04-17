// Generates Apollo ranging codes
// See "A study of the JPL Mark I Ranging Subsystem"
// https://ntrs.nasa.gov/citations/19660004934
//
// Ken Shirriff, righto.com

#define PIN_OUT 0 // Main output pin
#define PIN_A 1 // Subsequence output pins
#define PIN_B 2
#define PIN_C 3
#define PIN_X 4
#define PIN_CK 5

#undef DEBUG // Define DEBUG for serial console debugging output
#define LONG // LONG gives the normal long codes, SHORT gives shorter sequences

volatile uint32_t a, b, c, x; // Shift registers
volatile bool anew, bnew, cnew, xnew, ck, out; // Current values

IntervalTimer timer;

// Write the values to the output pins
void write() {
  digitalWriteFast(PIN_A, anew);
  digitalWriteFast(PIN_B, bnew);
  digitalWriteFast(PIN_C, cnew);
  digitalWriteFast(PIN_X, xnew);
  digitalWriteFast(PIN_CK, ck);
  digitalWriteFast(PIN_OUT, out);
}

// Calculate the next output values
void calc() {
  if (ck == 0) {
    // Recalculate everything
    bool af1 = a & 4;
    bool af3 = a & 1;
    anew = af1 ^ af3;
    a = (a >> 1) | (anew << 4);

#ifdef LONG
    bool bf3 = b & 2;
    bool bf4 = b & 1;
    bnew = bf3 ^ bf4;
#else
    bool bf = b & 16;
    bool bf1 = b & 8;
    bnew = bf ^ bf1;
#endif
    b = (b >> 1) | (bnew << 5);

#ifdef LONG
    bool cf4 = c & 2;
    bool cf5 = c & 1;
    cnew = cf4 ^ cf5;
#else
    bool cf1 = c & 16;
    bool cf2 = c & 8;
    cnew = cf1 ^ cf2;
#endif
    c = (c >> 1) | (cnew << 6);

    bool xg = x & 16;
    bool xf = x & 8;
    bool xf1 = x & 4;
    bool xf2 = x & 2;
    bool xf3 = x & 1;

    xnew = (!xg & !xf2) | (!xf & !xf3) | (!xf1 & xf2 & xf3);
    x = (x >> 1) | (xnew << 4);

    ck = 1;
    out = ((xnew ^ 1) & ((anew & bnew) | (anew & cnew) | (bnew & cnew))) ^ ck;
  } else {
    // Clock state changes but the subsequences don't.
    ck = 0;
    out = out ^ 1;
  }
}

// Interrupt handler, called at 1 MHz by the timer
void handler() {
  write();
  calc();
}

void setup() {
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  pinMode(PIN_X, OUTPUT);
  pinMode(PIN_CK, OUTPUT);
  pinMode(PIN_OUT, OUTPUT);

  a = 0x1f; // Start with all 1's. Doesn't really matter as long as not all 0's.
  b = 0x3f;
  c = 0x7f;
  x = 22;
  ck = 0;
  out = 0;

  timer.priority(10); // High priority
#ifndef DEBUG
  timer.begin(handler, 1000 / 996); // Clock is 996 kHZ, not exactly 1 MHz. See p16 of ranging doc. But Teensy only has resolution to generate 1 MHz anyway.
#endif
}

int count = 0; // Counter for debugging printing.

#ifdef LONG
int cycle_length = 5456682; // Length of the complete cycle, 5.4 seconds
#else
int cycle_length = 71610; // short code
#endif

// Print some data from the start and then some data after the repetition time. They should match.
void print_two_reps() {
  cycle_length = 71610;
  if (count < 100) {
    Serial.printf("%d", out);
  } else if (count == 100) {
    Serial.printf("\n");
  } else if (count >= cycle_length && count < cycle_length + 100) {
    Serial.printf("%d", out);
  } else if (count == cycle_length + 100) {
    Serial.printf("\n");
  }
  count++;
}

// Print the specified subsequence to make sure it has the right length.
void print_subseq() {
  if (ck == 0) {
    Serial.printf("%d", cnew); // Change as needed
    if (++count == 15) { // Change length as needed
      Serial.printf("\n");
      count = 0;
    }
  }
  delay(20);
}
  
void loop() {
#ifdef DEBUG
  calc();
  // print_subseq();
  print_two_reps();
#endif
}
