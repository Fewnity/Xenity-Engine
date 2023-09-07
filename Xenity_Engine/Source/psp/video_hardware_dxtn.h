/*
 * libtxc_dxtn
 * Version: 0.1b
 *
 * Fixed some bugs with dxt1 compression
 *
 * Copyright (C) 2004 Roland Scheidegger
 * All Rights Reserved.
 * Copyright (C) 2006-2008 Franck Charlet
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#if defined(__PSP__)
#ifndef _TXC_DXTN_H_
#define _TXC_DXTN_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char GLchan;

#define FALSE 0
#define TRUE 1

#define UBYTE_TO_CHAN(b) (b)
#define CHAN_MAX 255
#define RCOMP 0
#define GCOMP 1
#define BCOMP 2
#define ACOMP 3

extern int tx_compress_dxtn(int srccomps, int width, int height, const unsigned char *srcPixData, unsigned int destformat, unsigned char *dest);

#ifdef __cplusplus
}
#endif

#endif
#endif
