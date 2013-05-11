/* bithelp.h  -  Some bit manipulation helpers
 * Copyright (C) 2001 CryptNET, V. Alex Brennen
 * Copyright (C) 1999 Free Software Foundation, Inc.
 *
*/

/*  Rotate a 32 bit integer by n bytes  */
static inline unsigned int rol( int x, int n)
{
	__asm__("roll %%cl,%0"
		:"=r" (x)
		:"0" (x),"c" (n));
	
	return x;
}

