// Morse code generator
// enter text into serial monitor, press return to send Morse on LED
// handles upper and lower case letters, numbers, and spaces.
// Also period "." comma "," at sign "@" and question mark "?"

// Define globals

int ch;  // used to hold each incoming character
const int ledPin = 13;  // define led pin

// define everything from length of a dit.  Change dit and everythign changes with it.

const int dit = 150;
const int dah = dit * 3;

// intra character space (space between the dits and dahs of a single character)
const int intra = dit; // between dits, dahs, and added between letters
// inter character space (space between seperate characters)
const int inter = 2 * dit; // 3 dits, one dit already included
// actual space (between words)
const int space = 4 * dit; // 7 dits, because 3 dits already included

// The letters array holds values for all letters A-Z and numbers 0-9
// Lower case is automatically handled (converted to upper case)

const int letters[] = {

  // 3 upper bits for length of code 1-5
  // 5 lower bits for 1-5 values:  1 = dah, 0 = dit

  0x48, // A: 2 .-    010-0 1000 = 48 hex
  0x90, // B: 4 -...  100-1 0000 = 90 hex
  0x94, // C: 4 -.-.  100-1 0100 = 94 hex
  0x70, // D: 3 -..   011-1 0000 = 70 hex
  0x20, // E: 1 .     001-0 0000 = 20 hex
  0x84, // F: 4 ..-.  100-0 0100 = 84 hex
  0x78, // G: 3 --.   011-1 1000 = 78 hex
  0x80, // H: 4 ....  100-0 0000 = 80 hex
  0x40, // I: 2 ..    010-0 0000 = 40 hex
  0x8E, // J: 4 .---  100-0 1110 = 8E hex
  0x74, // K: 3 -.-   011-1 0100 = 74 hex
  0x88, // L: 4 .-..  100-0 1000 = 88 hex
  0x58, // M: 2 --    010-1 1000 = 58 hex
  0x50, // N: 2 -.    010-1 0000 = 50 hex
  0x7C, // O: 3 ---   011-1 1100 = 7C hex
  0x8C, // P: 4 .--.  100-0 1100 = 8C hex
  0x9A, // Q: 4 --.-  100-1 1010 = 9A hex
  0x68, // R: 3 .-.   011-0 1000 = 68 hex
  0x60, // S: 3 ...   011-0 0000 = 60 hex
  0x30, // T: 1 -     001-1 0000 = 30 hex
  0x64, // U: 3 ..-   011-0 0100 = 64 hex
  0x82, // V: 4 ...-  100-0 0010 = 82 hex
  0x6C, // W: 3 .--   011-0 1100 = 6C hex
  0x92, // X: 4 -..-  100-1 0010 = 92 hex
  0x96, // Y: 4 -.--  100-1 0110 = 96 hex
  0x98, // Z: 4 --..  100-1 1000 = 98 hex
  0xBF, // 0: 5 ----- 101-1 1111 = BF hex
  0xAF, // 1: 5 .---- 101-0 1111 = AF hex
  0xA7, // 2: 5 ..--- 101-0 0111 = A7 hex
  0xA3, // 3, 5 ...-- 101-0 0011 = A3 hex
  0xA1, // 4, 5 ....- 101-0 0001 = A1 hex
  0xA0, // 5, 5 ..... 101-0 0000 = A0 hex
  0xB0, // 6, 5 -.... 101-1 0000 = B0 hex
  0xB8, // 7, 5 --... 101-1 1000 = B8 hex
  0xBC, // 8, 5 ---.. 101-1 1100 = BC hex
  0xBE  // 9, 5 ----. 101-1 1110 = BE hex
};

// initialize the led pin, set it low, and start the serial monitor port

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
}

// main loop.  Receive characters from the serial port, and convert to morse.

void loop() {
  // check for input character
  if (Serial.available()) {
    // input char available - read it in
    ch = Serial.read();
    // check to see if it is a uppercase letter
    if ('A' <= ch && ch <= 'Z') {
      flashLetter (letters[ch - 'A']);
    }
    // check to see if it is a lowercase letter
    else if ('a' <= ch && ch <= 'z') {
      flashLetter (letters[ch - 'a']);  // convert to upper case
    }
    // check to see if it is a number
    else if ('0' <= ch && ch <= '9') {
      flashLetter (letters[ch - '0' + 26]); //add alphabet offset into array
    }
    // check for space
    else if (ch == ' ') {  
      delay(space);
    }
    // check for various punctuations...
    else if (ch == '.') {  // dit dah dit dah dit dah
      flashPunc(0x15);
    }
    else if (ch == ',') {  // dah dah dit dit dah dah
      flashPunc(0x33);
    }
    else if (ch == '?') {  // dit dit dah dah dit dit
      flashPunc(0x0C);
    }
    else if (ch == '@') {  // dit dah dah dit dah dit
      flashPunc(0x1A);
    }
    // ignore any other characters
  }
}

// process array value into number of ditdahs and what they are
// call FlashDitOrDah to flash the LED appropriatelly

void flashLetter (int lettercode) {
  int ditdahcount;  // number of dis and dahs
  int ditdahs;  // actual dits and dahs in binary, lower 5 bits
  // get top 3 bits for ditdahcount for number of ditdahs
  ditdahcount = lettercode / 32;
  //get bottom 5 bits for ditdahs
  ditdahs = lettercode - (ditdahcount * 32);
  int mask = 16;  // mask first dit or dah in lower 5 bits
  while (ditdahcount > 0) {  // only do ditdahcount times
    flashDitOrDah(mask & ditdahs);  //flash the dit or dah
    mask = mask >> 1;  // shift the mast to the next dit or dah
    ditdahcount--;  // decrement the count
  }
  delay(inter);  // add the inter-character space (one dit already there)
}

// handle punctuations
// Enter with 6 bit values for the appropriate Morse code in bottom 6 bits
void flashPunc (int ditdahs) {
  int ditdahcount = 6;  // punctuations are 6 ditdahs long
  int mask = 32;  // mask for first dit or day
  while (ditdahcount > 0) {  // only do ditdahcount times
    flashDitOrDah(mask & ditdahs);  //flash the dit or dah and one dit space
    mask = mask >> 1;  // shift the mast to the next dit or dah
    ditdahcount--;  // decrement the count
  }
  delay(inter);  // add the inter-character space (one dit space already there)
}

// flashes the LED with either a dit or a dah, with a following
// intra-character spacing (a dit)
// Enter with a zero parameter for a dit, a non-zero for a dah

void flashDitOrDah(int ditDah) {
  int useDelay = dah;
  if (ditDah == 0) {
    useDelay = dit;
  }
  digitalWrite(ledPin, HIGH);
  delay(useDelay);  // use the defined dit or dah delay
  digitalWrite(ledPin, LOW);
  delay(intra);  // add the intra letter space
}

