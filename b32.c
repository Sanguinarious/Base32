/*
* Copyright (C) 2012, William H. Welna All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY William H. Welna ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL William H. Welna BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pstdint.h>
#include <windows.h>

/* Encoding/Decoding Tables */
static unsigned char b32e_crockford[] = "0123456789abcdefghjkmnpqrstvwxyz";
static unsigned char b32d_crockford[] = {
/*                                                Special Shit                                          */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
/*      @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0,10,11,12,13,14,15,16,17, 0,18,19, 0,20,21, 0, 22,23,24,25,26,0,27,28,29,30,31, 0, 0, 0, 0, 0,
/*      `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0,10,11,12,13,14,15,16,17, 0,18,19, 0,20,21, 0, 22,23,24,25,26,0,27,28,29,30,31, 0, 0, 0, 0, 0,
/*                                                      ‘  ’                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char b32e_rfc4648[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
static unsigned char b32d_rfc4648[] = {
/*                                                Special Shit                                          */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,26,27,28,29,30,31, 0, 0, 0, 0, 0, 0, 0, 0,
/*      @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
/*      `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
/*                                                      ‘  ’                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char b32e_zbase32[] = "ybndrfg8ejkmcpqxot1uwisza345h769";
static unsigned char b32d_zbase32[] = {
/*                                                Special Shit                                          */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,18, 0,25,26,27,30,29, 7,31, 0, 0, 0, 0, 0, 0,
/*      @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*      `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0,24, 1,12, 3, 8, 5, 6,28,21, 9,10, 0,11, 2,16,13,14, 4,22,17,19, 0,20,15, 0,23, 0, 0, 0, 0, 0,
/*                                                      ‘  ’                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char b32e_triacontakia[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
static unsigned char b32d_triacontakia[] = {
/*                                                Special Shit                                          */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
/*      @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*      `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                                      ‘  ’                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char b32e_nintendo[] = "0123456789BCDFGHJKLMNPQRSTVWXYZ.";
static unsigned char b32d_nintendo[] = {
/*                                                Special Shit                                          */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,31, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
/*      @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0, 0,10,11,12, 0,13,14,15, 0,16,17,18,19,20, 0,21,22,23,24,25, 0,26,27,28,29,30, 0, 0, 0, 0, 0,
/*      `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0, 0,10,11,12, 0,13,14,15, 0,16,17,18,19,20, 0,21,22,23,24,25, 0,26,27,28,29,30, 0, 0, 0, 0, 0,
/*                                                      ‘  ’                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                          Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Encoding/Decoding Tables */

char *b32e_5(unsigned char *in, int len, unsigned char *out, unsigned char *table) { // in = 5 bytes, out = 8 bytes
	int x, y;
	uint64_t p, t=0;
	for(x=0, y=4; x < 5; ++x, --y) {
		p = in[x];
		t += p << ((+y)*8);
	}
	for(x=0, y=7; x < 8; ++x, --y) {
		out[x] = table[((t >> ((+y)*5)) & 0x1F)];
	}
	if(len<5)
		switch(len) { // why? because fuck it, thats why
			case 4: // 1
				out[7] = '=';
				break;
			case 3: // 3
				out[7] = '=';
				out[6] = '=';
				out[5] = '=';
				break;
			case 2: // 4
				out[7] = '=';
				out[6] = '=';
				out[5] = '=';
				out[4] = '=';
				break;
			case 1: // 6
				out[7] = '=';
				out[6] = '=';
				out[5] = '=';
				out[4] = '=';
				out[3] = '=';
				out[2] = '=';
				break;
			default:
				break;
		}
	return out;
}

char *b32d_8(unsigned char *in, unsigned char *out, unsigned char *table) { // in = 8 bytes, out = 5 bytes
	int x, y;
	uint64_t t=0, a;
	for(x=0; x < 8; ++x)
		if(in[x] == '=')
			in[x] = 0; // convert padding to zero;
	for(x=0, y=7; x < 8; ++x, --y) {
		a = table[in[x]];
		t += (a << ((+y)*5));
	}
	for(x=0, y=4; x < 5; ++x, --y)
		out[x] = (t >> ((+y)*8));
	return out;
}

char *b32e(char *in, int len, char *out, char *table) {
	int x, y;
	for(x=0, y=0; x < len; x+=5, y+=8)
		b32e_5(in+x, len-x, out+y, table);
	return out;
}

char *b32d(char *in, int len, char *out, char *table) {
	int x, y;
	for(x=0, y=0; x < len; x+=8, y+=5)
		b32d_8(in+x, out+y, table);
	return out;
}

void table_dump(char *table) {
	int x;
	for(x=0; x < 255; ++x)
		fprintf(stdout, "%i = %i\n", x, table[x]);
}

char *create_decode_table(char *encode_table, char *out) { // create simple decode table from char set, case sensitive
	int x;
	for(x=0; x < 32; ++x)
		out[encode_table[x]] = x;
	return out;
}

int main(int argc, char **argv) {
	char temp[1024], temp2[1512];
	ZeroMemory(temp, 1024);
	ZeroMemory(temp2, 1512);
	fprintf(stdout, "String> ");
	fgets(temp, 1023, stdin);
	fprintf(stdout, "crockford    -> %s\n", b32e(temp, strlen(temp), temp2, b32e_crockford));
	fprintf(stdout, "rfc4648      -> %s\n", b32e(temp, strlen(temp), temp2, b32e_rfc4648));
	fprintf(stdout, "zbase32      -> %s\n", b32e(temp, strlen(temp), temp2, b32e_zbase32));
	fprintf(stdout, "triacontakia -> %s\n", b32e(temp, strlen(temp), temp2, b32e_triacontakia));
	fprintf(stdout, "nintendo     -> %s\n", b32e(temp, strlen(temp), temp2, b32e_nintendo));
	return 0;
}
