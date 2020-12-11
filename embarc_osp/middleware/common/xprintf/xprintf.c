/*------------------------------------------------------------------------/
/  Universal string handler for user console interface
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "xprintf.h"


#if _USE_XFUNC_OUT
#include <stdarg.h>
void (*xfunc_out)(unsigned char);	/* Pointer to the output stream */
static char *outptr;
static uint32_t buf_data_cnt = 0;

static int32_t (*pre_printf)(void);
static void (*post_printf)(int32_t, uint32_t);

static char *save_outptr;
static uint32_t save_buf_data_cnt = 0;
void xbuffer_install(char *buf)
{
	buf_data_cnt = 0;
	outptr = buf;
}
void xbuffer_save(void)
{
	save_outptr = outptr;
	save_buf_data_cnt = buf_data_cnt;
}

void xbuffer_restore(void)
{
	outptr = save_outptr;
	buf_data_cnt = save_buf_data_cnt;
}

uint32_t xget_buf_cnt(void)
{
	return buf_data_cnt;
}

int32_t xbuffer_idle(void)
{
	if (NULL == outptr) {
		return 1;
	} else {
		return 0;
	}
}

void xprintf_callback_install(int32_t (*pre)(void), void (*post)(int32_t, uint32_t))
{
	pre_printf = pre;
	post_printf = post;
}

void xfunc_out_set(void (*out)(unsigned char))
{
	xfunc_out = out;
}

/*----------------------------------------------*/
/* Put a character                              */
/*----------------------------------------------*/

void xputc (char c)
{
	if (_CR_CRLF && c == '\n') {
		xputc('\r');		/* CR -> CRLF */
	}

	if (outptr) {
		*outptr++ = (unsigned char)c;
		buf_data_cnt++;
		return;
	}

	if (xfunc_out) {
		xfunc_out((unsigned char)c);
	}
}


/*----------------------------------------------*/
/* Put a null-terminated string                 */
/*----------------------------------------------*/

void xputs (					/* Put a string to the default device */
	const char* str				/* Pointer to the string */
)
{
	while (*str) {
		xputc(*str++);
		//buf_data_cnt++;
	}
}


void xfputs (					/* Put a string to the specified device */
	void(*func)(unsigned char),	/* Pointer to the output function */
	const char*	str				/* Pointer to the string */
)
{
	void (*pf)(unsigned char);


	pf = xfunc_out;		/* Save current output device */
	xfunc_out = func;	/* Switch output to specified device */
	while (*str)		/* Put the string */
		xputc(*str++);
	xfunc_out = pf;		/* Restore output device */
}



/*----------------------------------------------*/
/* Formatted string output                      */
/*----------------------------------------------*/
/*  xprintf("%d", 1234);			"1234"
    xprintf("%6d,%3d%%", -200, 5);	"  -200,  5%"
    xprintf("%-6u", 100);			"100   "
    xprintf("%ld", 12345678L);		"12345678"
    xprintf("%04x", 0xA3);			"00a3"
    xprintf("%08LX", 0x123ABC);		"00123ABC"
    xprintf("%016b", 0x550F);		"0101010100001111"
    xprintf("%s", "String");		"String"
    xprintf("%-4s", "abc");			"abc "
    xprintf("%4s", "abc");			" abc"
    xprintf("%c", 'a');				"a"
    xprintf("%f", 10.0);            <xprintf lacks floating point support>
*/

void xvprintf (
	const char*	fmt,	/* Pointer to the format string */
	va_list arp			/* Pointer to arguments */
)
{
	unsigned int r, i, j, w, f;
	unsigned int f_pad_width = 6;
	unsigned long v = 0;
	char s[16], c, d, *p;
	char fract[16];

	for (;;) {
		c = *fmt++;					/* Get a char */
		if (!c) break;				/* End of format? */
		if (c != '%') {				/* Pass through it if not a % sequense */
			xputc(c); continue;
		}
		f = 0;
		c = *fmt++;					/* Get first char of the sequense */
		if (c == '0') {				/* Flag: '0' padded */
			f = 1; c = *fmt++;
		} else {
			if (c == '-') {			/* Flag: left justified */
				f = 2; c = *fmt++;
			}
		}
		for (w = 0; c >= '0' && c <= '9'; c = *fmt++)	/* Minimum width */
			w = w * 10 + c - '0';

		/* add to support float print. */
		if (c == '.') {
			f |= 16;
			c = *fmt++;
			for (f_pad_width = 0; c >= '0' && c <= '9'; c = *fmt++)
				f_pad_width = f_pad_width * 10 + c - '0';
		}

		if (c == 'l' || c == 'L') {	/* Prefix: Size is long int */
			f |= 4; c = *fmt++;
		}
		if (!c) break;				/* End of format? */
		d = c;
		if (d >= 'a') d -= 0x20;
		switch (d) {				/* Type is... */
		case 'S' :					/* String */
			p = va_arg(arp, char*);
			for (j = 0; p[j]; j++) ;
			while (!(f & 2) && j++ < w) xputc(' ');
			xputs(p);
			while (j++ < w) xputc(' ');
			continue;
		case 'C' :					/* Character */
			xputc((char)va_arg(arp, int)); continue;
		case 'B' :					/* Binary */
			r = 2; break;
		case 'O' :					/* Octal */
			r = 8; break;
		case 'D' :					/* Signed decimal */
		case 'U' :					/* Unsigned decimal */
			r = 10; break;
		case 'X' :					/* Hexdecimal */
			r = 16; break;

		/* add to support float print. */
		case 'F' :
			f |= 16;
			r = 10;
			break;

		default:					/* Unknown type (passthrough) */
			xputc(c); continue;
		}

		unsigned int valid_fraction = 0;
		unsigned int fract_end_idx = 0;
		unsigned int carry = 0;
		if (f & 16) {
			//unsigned char exponent;
			unsigned int fraction = 0;
			if (f & 4) {
				/* TODO: double float... */
				//double val = va_arg(arp, double);
			} else {
				float val = (float)va_arg(arp, double);
				int tmp = (int)val;
				float tmp1 = (val - tmp);
				if (val < 0) {
					f |= 8;
					v = 0 - tmp;
					fraction = (int)((0.0 - tmp1) * 1000000);
				} else {
					v = tmp;
					fraction = (int)(tmp1 * 1000000);
				}

				i = 0;
				do {
					d = fraction % r;
					fraction /= r;
					fract[i++] = d + '0';
				} while ((i < 6));
				valid_fraction = i;
				if (f_pad_width > valid_fraction) {
					f_pad_width -= valid_fraction;
				} else {
					fract_end_idx = valid_fraction - f_pad_width;
					if (fract[fract_end_idx - 1] >= '5') {
						int idx = fract_end_idx;
						while (idx < valid_fraction) {
							fract[idx] += 1;
							if (fract[idx] > '9') {
								fract[idx] = '0';
								carry = 1;
								idx++;
							} else {
								carry = 0;
								break;
							}
						}
					}
					f_pad_width = 0;
				}
			}
		} else {
			/* Get an argument and put it in numeral */
			v = (f & 4) ? va_arg(arp, long) : ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));
			if (d == 'D' && (v & 0x80000000)) {
				v = 0 - v;
				f |= 8;
			}
		}

		i = 0;
		do {
			d = (char)(v % r); v /= r;
			if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
			s[i++] = d + '0';
		} while (v && i < sizeof(s));
		if (carry) {
			int idx = 0;
			while (idx < i) {
				s[idx] += 1;
				if (s[idx] > '9') {
					s[idx++] = '0';
				} else {
					carry = 0;
					break;
				}
			}
			if (carry) {
				s[i++] = '1';
				carry = 0;
			}
		}

		if ((f & 8) && (i < sizeof(s))) s[i++] = '-';
		j = i; d = (f & 1) ? '0' : ' ';
		while (!(f & 2) && j++ < w) xputc(d);
		do xputc(s[--i]); while(i);

		if (valid_fraction) {
			xputc('.');
			do xputc(fract[--valid_fraction]); while((valid_fraction) && (valid_fraction > fract_end_idx));
			while (f_pad_width) {
				xputc('0');
				f_pad_width--;
			}
		} else {
			while (j++ < w) xputc(' ');
		}
	}
}


void xprintf (			/* Put a formatted string to the default device */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
	va_list arp;

	int32_t result = E_OK;

	if (pre_printf) {
		result = pre_printf();
	}
	if (result >= 0) {
		va_start(arp, fmt);
		xvprintf(fmt, arp);
		va_end(arp);

		if (post_printf) {
			post_printf(result, buf_data_cnt);
		}
	}
}

void xsprintf (			/* Put a formatted string to the memory */
	char* buff,			/* Pointer to the output buffer */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
	va_list arp;


	outptr = buff;		/* Switch destination for memory */

	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);

	*outptr = 0;		/* Terminate output string with a \0 */
	outptr = 0;			/* Switch destination for device */
}

void xfprintf (					/* Put a formatted string to the specified device */
	void(*func)(unsigned char),	/* Pointer to the output function */
	const char*	fmt,			/* Pointer to the format string */
	...							/* Optional arguments */
)
{
	va_list arp;
	void (*pf)(unsigned char);


	pf = xfunc_out;		/* Save current output device */
	xfunc_out = func;	/* Switch output to specified device */

	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);

	xfunc_out = pf;		/* Restore output device */
}



/*----------------------------------------------*/
/* Dump a line of binary dump                   */
/*----------------------------------------------*/

void put_dump (
	const void* buff,		/* Pointer to the array to be dumped */
	unsigned long addr,		/* Heading address value */
	int len,				/* Number of items to be dumped */
	int width				/* Size of the items (DF_CHAR, DF_SHORT, DF_LONG) */
)
{
	int i;
	const unsigned char *bp;
	const unsigned short *sp;
	const unsigned long *lp;


	xprintf("%08lX ", addr);		/* address */

	switch (width) {
	case DW_CHAR:
		bp = buff;
		for (i = 0; i < len; i++)		/* Hexdecimal dump */
			xprintf(" %02X", bp[i]);
		xputc(' ');
		for (i = 0; i < len; i++)		/* ASCII dump */
			xputc((bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.');
		break;
	case DW_SHORT:
		sp = buff;
		do								/* Hexdecimal dump */
			xprintf(" %04X", *sp++);
		while (--len);
		break;
	case DW_LONG:
		lp = buff;
		do								/* Hexdecimal dump */
			xprintf(" %08LX", *lp++);
		while (--len);
		break;
	}

	xputc('\n');
}

#endif /* _USE_XFUNC_OUT */



#if _USE_XFUNC_IN
unsigned char (*xfunc_in)(void);	/* Pointer to the input stream */

/*----------------------------------------------*/
/* Get a line from the input                    */
/*----------------------------------------------*/

int xgets (		/* 0:End of stream, 1:A line arrived */
	char* buff,	/* Pointer to the buffer */
	int len		/* Buffer length */
)
{
	int c, i;


	if (!xfunc_in) return 0;		/* No input function specified */

	i = 0;
	for (;;) {
		c = xfunc_in();				/* Get a char from the incoming stream */
		if (!c) return 0;			/* End of stream? */
		if (c == '\r') break;		/* End of line? */
		if (c == '\b' && i) {		/* Back space? */
			i--;
			if (_LINE_ECHO) xputc(c);
			continue;
		}
		if (c >= ' ' && i < len - 1) {	/* Visible chars */
			buff[i++] = c;
			if (_LINE_ECHO) xputc(c);
		}
	}
	buff[i] = 0;	/* Terminate with a \0 */
	if (_LINE_ECHO) xputc('\n');
	return 1;
}


int xfgets (	/* 0:End of stream, 1:A line arrived */
	unsigned char (*func)(void),	/* Pointer to the input stream function */
	char* buff,	/* Pointer to the buffer */
	int len		/* Buffer length */
)
{
	unsigned char (*pf)(void);
	int n;


	pf = xfunc_in;			/* Save current input device */
	xfunc_in = func;		/* Switch input to specified device */
	n = xgets(buff, len);	/* Get a line */
	xfunc_in = pf;			/* Restore input device */

	return n;
}


/*----------------------------------------------*/
/* Get a value of the string                    */
/*----------------------------------------------*/
/*	"123 -5   0x3ff 0b1111 0377  w "
	    ^                           1st call returns 123 and next ptr
	       ^                        2nd call returns -5 and next ptr
                   ^                3rd call returns 1023 and next ptr
                          ^         4th call returns 15 and next ptr
                               ^    5th call returns 255 and next ptr
                                  ^ 6th call fails and returns 0
*/

int xatoi (			/* 0:Failed, 1:Successful */
	char **str,		/* Pointer to pointer to the string */
	long *res		/* Pointer to the valiable to store the value */
)
{
	unsigned long val;
	unsigned char c, r, s = 0;


	*res = 0;

	while ((c = **str) == ' ') (*str)++;	/* Skip leading spaces */

	if (c == '-') {		/* negative? */
		s = 1;
		c = *(++(*str));
	}

	if (c == '0') {
		c = *(++(*str));
		switch (c) {
		case 'x':		/* hexdecimal */
			r = 16; c = *(++(*str));
			break;
		case 'b':		/* binary */
			r = 2; c = *(++(*str));
			break;
		default:
			if (c <= ' ') return 1;	/* single zero */
			if (c < '0' || c > '9') return 0;	/* invalid char */
			r = 8;		/* octal */
		}
	} else {
		if (c < '0' || c > '9') return 0;	/* EOL or invalid char */
		r = 10;			/* decimal */
	}

	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
		}
		if (c >= r) return 0;		/* invalid char for current radix */
		val = val * r + c;
		c = *(++(*str));
	}
	if (s) val = 0 - val;			/* apply sign if needed */

	*res = val;
	return 1;
}

#endif /* _USE_XFUNC_IN */
