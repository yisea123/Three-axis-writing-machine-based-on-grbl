/*
  print.c - Functions for formatting output strings
  Part of Grbl

  The MIT License (MIT)

  GRBL(tm) - Embedded CNC g-code interpreter and motion-controller
  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2011-2012 Sungeun K. Jeon

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

//#include <avr/pgmspace.h>
//#include "config.h"
//#include "serial.h"
//#include "settings.h"
#include "include.h"

void printString(const char *s)
{
  while (*s)
    serial_write(*s++);
}

// ��ӡ�洢��PGM-memory�е��ַ���
void printPgmString(const char *s)
{
//==========================================
//  char c;
//  while ((c = pgm_read_byte_near(s++)))
//    serial_write(c);
  char c;
  int i=0;
  while (s[i]!='\0')
  {
  	c=s[i];
    serial_write(c);
  	i++;
  }
//==========================================
}

// void printIntegerInBase(unsigned long n, unsigned long base)
// { 
// 	unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
// 	unsigned long i = 0;
// 
// 	if (n == 0) {
// 		serial_write('0');
// 		return;
// 	} 
// 
// 	while (n > 0) {
// 		buf[i++] = n % base;
// 		n /= base;
// 	}
// 
// 	for (; i > 0; i--)
// 		serial_write(buf[i - 1] < 10 ?
// 			'0' + buf[i - 1] :
// 			'A' + buf[i - 1] - 10);
// }

void print_uint8_base2(uint8_t n)
{ 
	unsigned char buf[8];
	uint8_t i = 0;

	for (; i < 8; i++) {
		buf[i] = n & 1;
		n >>= 1;
	}

	for (; i > 0; i--)
		serial_write('0' + buf[i - 1]);
}

static void print_uint32_base10(unsigned long n)
{ 
  unsigned char buf[10]; 
  uint8_t i = 0;
  
  if (n == 0) {
    serial_write('0');
    return;
  } 
  
  while (n > 0) {
    buf[i++] = n % 10 + '0';
    n /= 10;
  }
    
  for (; i > 0; i--)
    serial_write(buf[i-1]);
}

void printInteger(long n)
{
  if (n < 0) {
    serial_write('-');
    n = -n;
  }
  print_uint32_base10(n);
}

// Convert float to string by immediately converting to a long integer, which contains
// more digits than a float. Number of decimal places, which are tracked by a counter,
// may be set by the user. The integer is then efficiently converted to a string.
// NOTE: AVR '%' and '/' integer operations are very efficient. Bitshifting speed-up 
// techniques are actually just slightly slower. Found this out the hard way.
void printFloat(float n)
{
	uint8_t decimals;
	unsigned char buf[10];
	uint8_t i;
	uint32_t a;

  if (n < 0) {
    serial_write('-');
    n = -n;
  }

  decimals = settings.decimal_places;
  while (decimals >= 2) { // Quickly convert values expected to be E0 to E-4.
    n *= 100;
    decimals -= 2;
  }
  if (decimals) { n *= 10; }
  n += 0.5; // Add rounding factor. Ensures carryover through entire value.
    
  // Generate digits backwards and store in string.
  i = 0;
  a = (long)n;  
  buf[settings.decimal_places] = '.'; // Place decimal point, even if decimal places are zero.
  while(a > 0) {
    if (i == settings.decimal_places) { i++; } // Skip decimal point location
    buf[i++] = (a % 10) + '0'; // Get digit
    a /= 10;
  }
  while (i < settings.decimal_places) { 
     buf[i++] = '0'; // Fill in zeros to decimal point for (n < 1)
  }
  if (i == settings.decimal_places) { // Fill in leading zero, if needed.
    i++;
    buf[i++] = '0'; 
  }   
  
  // Print the generated string.
  for (; i > 0; i--)
    serial_write(buf[i-1]);
}
