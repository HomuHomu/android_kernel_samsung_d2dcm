/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_fb.h"
#include "mipi_samsung_oled.h"

static struct msm_panel_info pinfo;
#define MAX_GAMMA_VALUE 25
static struct mipi_panel_data mipi_pd;

/*
 * GAMMA TABLE 2.2 Temporary...
 */
static char gamma_2_2[][26] = {
	{0xFA, /* 70 cd */
	 0x02, 0x10, 0x10, 0x10, 0xD1, 0x34, 0xD0, 0xD6, 0xBA, 0xDC,
	 0xE0, 0xD9, 0xE2, 0xC2, 0xC0, 0xBF, 0xD4, 0xD5, 0xD0, 0x00,
	 0x73, 0x00, 0x59, 0x00, 0x82},

	{0xFA, /* 80 cd */
	 0x02, 0x10, 0x10, 0x10, 0xD7, 0x39, 0xD6, 0xD6, 0xBF, 0xDD,
	 0xE1, 0xDA, 0xE2, 0xC0, 0xBF, 0xBD, 0xD3, 0xD5, 0xCF, 0x00,
	 0x78, 0x00, 0x5D, 0x00, 0x88},

	{0xFA, /* 90 cd */
	 0x02, 0x10, 0x10, 0x10, 0xD7, 0x39, 0xD5, 0xD5, 0xBF, 0xDC,
	 0xDF, 0xDA, 0xE0, 0xC1, 0xC0, 0xBD, 0xD2, 0xD4, 0xCF, 0x00,
	 0x7C, 0x00, 0x60, 0x00, 0x8C},

	{0xFA, /* 100 cd */
	 0x02, 0x10, 0x10, 0x10, 0xDD, 0x3A, 0xE3, 0xD7, 0xC5, 0xDD,
	 0xDF, 0xDA, 0xDF, 0xC0, 0xBF, 0xBC, 0xD0, 0xD3, 0xCD, 0x00,
	 0x81, 0x00, 0x64, 0x00, 0x92},

	{0xFA, /* 110 cd */
	 0x02, 0x10, 0x10, 0x10, 0xE1, 0x43, 0xE2, 0xD6, 0xC5, 0xDC,
	 0xDE, 0xDA, 0xDF, 0xBF, 0xBF, 0xBB, 0xD0, 0xD3, 0xCD, 0x00,
	 0x85, 0x00, 0x67, 0x00, 0x96},

	{0xFA, /* 120 cd */
	 0x02, 0x10, 0x10, 0x10, 0xE5, 0x48, 0xE4, 0xD5, 0xC5, 0xDB,
	 0xDE, 0xDA, 0xDD, 0xBE, 0xBF, 0xBB, 0xD0, 0xD2, 0xCC, 0x00,
	 0x88, 0x00, 0x6A, 0x00, 0x9A},

	{0xFA, /* 130 cd */
	 0x02, 0x10, 0x10, 0x10, 0xE6, 0x4D, 0xE3, 0xD5, 0xC5, 0xDA,
	 0xDD, 0xDA, 0xDD, 0xBE, 0xBE, 0xBA, 0xCE, 0xD1, 0xCA, 0x00,
	 0x8C, 0x00, 0x6D, 0x00, 0x9F},

	{0xFA, /* 140 cd */
	 0x02, 0x10, 0x10, 0x10, 0xE8, 0x51, 0xE4, 0xD6, 0xC9, 0xDB,
	 0xDC, 0xD9, 0xDC, 0xBE, 0xBF, 0xBA, 0xCD, 0xD0, 0xC9, 0x00,
	 0x90, 0x00, 0x70, 0x00, 0xA3},

	{0xFA, /* 150 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEA, 0x57, 0xE9, 0xD6, 0xC9, 0xDA,
	 0xDD, 0xDA, 0xDC, 0xBC, 0xBE, 0xB8, 0xCE, 0xD0, 0xCA, 0x00,
	 0x92, 0x00, 0x72, 0x00, 0xA6},

	{0xFA, /* 160 cd */
	 0x02, 0x10, 0x10, 0x10, 0xF0, 0x61, 0xEE, 0xD5, 0xC9, 0xD9,
	 0xDD, 0xDA, 0xDB, 0xBC, 0xBE, 0xB9, 0xCC, 0xCF, 0xC8, 0x00,
	 0x96, 0x00, 0x75, 0x00, 0xAA},

	{0xFA, /* 170 cd */
	 0x02, 0x10, 0x10, 0x10, 0xF1, 0x81, 0xEE, 0xD4, 0xC9, 0xD9,
	 0xDD, 0xDB, 0xDB, 0xBB, 0xBD, 0xB7, 0xCC, 0xCE, 0xC8, 0x00,
	 0x99, 0x00, 0x78, 0x00, 0xAE},

	{0xFA, /* 180 cd */
	 0x02, 0x10, 0x10, 0x10, 0xF0, 0x8C, 0xED, 0xD5, 0xCA, 0xD8,
	 0xDC, 0xDB, 0xDC, 0xBB, 0xBD, 0xB7, 0xCB, 0xCD, 0xC5, 0x00,
	 0x9C, 0x00, 0x7A, 0x00, 0xB2},

	{0xFA, /* 190 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0x90, 0xEC, 0xD4, 0xC9, 0xD7,
	 0xDD, 0xDB, 0xDB, 0xBA, 0xBD, 0xB7, 0xCA, 0xCD, 0xC5, 0x00,
	 0x9F, 0x00, 0x7C, 0x00, 0xB5},

	{0xFA, /* 200 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0x8F, 0xED, 0xD2, 0xC7, 0xD5,
	 0xDC, 0xDA, 0xDA, 0xBA, 0xBB, 0xB5, 0xCA, 0xCC, 0xC4, 0x00,
	 0xA0, 0x00, 0x7F, 0x00, 0xBB},

	{0xFA, /* 210 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEF, 0x96, 0xEF, 0xD1, 0xC7, 0xD3,
	 0xDB, 0xD9, 0xD9, 0xB9, 0xBB, 0xB4, 0xCA, 0xCC, 0xC6, 0x00,
	 0xA3, 0x00, 0x81, 0x00, 0xBD},

	{0xFA, /* 220 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0x99, 0xEE, 0xD3, 0xC8, 0xD3,
	 0xDB, 0xD9, 0xD9, 0xB8, 0xBB, 0xB4, 0xC9, 0xCC, 0xC4, 0x00,
	 0xA6, 0x00, 0x83, 0x00, 0xC1},

	{0xFA, /* 230 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEE, 0xA3, 0xEF, 0xD2, 0xC9, 0xD3,
	 0xDB, 0xD9, 0xD9, 0xB9, 0xBA, 0xB3, 0xC8, 0xCC, 0xC4, 0x00,
	 0xA8, 0x00, 0x85, 0x00, 0xC4},

	{0xFA, /* 240 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0xA4, 0xEE, 0xD2, 0xC9, 0xD3,
	 0xDC, 0xD9, 0xD9, 0xB7, 0xBA, 0xB3, 0xC8, 0xCB, 0xC2, 0x00,
	 0xAB, 0x00, 0x87, 0x00, 0xC8},

	{0xFA, /* 250 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0xA8, 0xEF, 0xD2, 0xC8, 0xD2,
	 0xDA, 0xD9, 0xD8, 0xB7, 0xB9, 0xB2, 0xC8, 0xCB, 0xC2, 0x00,
	 0xAD, 0x00, 0x89, 0x00, 0xCB},

	{0xFA, /* 260 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEB, 0xB2, 0xEF, 0xD2, 0xC9, 0xD3,
	 0xDB, 0xDA, 0xD9, 0xB6, 0xB9, 0xBA, 0xC7, 0xCA, 0xC1, 0x00,
	 0xB0, 0x00, 0x8B, 0x00, 0xCE},

	{0xFA, /* 270 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEC, 0xB0, 0xEF, 0xD1, 0xC9, 0xD2,
	 0xDB, 0xDA, 0xD9, 0xB7, 0xB9, 0xB1, 0xC6, 0xC9, 0xC0, 0x00,
	 0xB2, 0x00, 0x8D, 0x00, 0xD1},

	{0xFA, /* 280 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0xB6, 0xF0, 0xD1, 0xC9, 0xD2,
	 0xDB, 0xDA, 0xD9, 0xB6, 0xB8, 0xAF, 0xC7, 0xCA, 0xC2, 0x00,
	 0xB4, 0x00, 0x8F, 0x00, 0xD3},

	{0xFA, /* 290 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEC, 0xB7, 0xEE, 0xD0, 0xC8, 0xD1,
	 0xDB, 0xDA, 0xD9, 0xB6, 0xB8, 0xB0, 0xC5, 0xC9, 0xC1, 0x00,
	 0xB7, 0x00, 0x91, 0x00, 0xD5},

	{0xFA, /* 300 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEC, 0xB7, 0xEF, 0xD1, 0xCA, 0xD1,
	 0xDB, 0xDA, 0xD8, 0xB5, 0xB8, 0xB0, 0xC5, 0xC8, 0xBF, 0x00,
	 0xB9, 0x00, 0x93, 0x00, 0xD9},
};

/*
 * GAMMA TABLE 1.9 Temporary...
 */
static char gamma_1_9[][26] = {
	{0xFA, /* 70 cd */
	 0x02, 0x10, 0x10, 0x10, 0xD1, 0x34, 0xD0, 0xD6, 0xBA, 0xDC,
	 0xE0, 0xD9, 0xE2, 0xC2, 0xC0, 0xBF, 0xD4, 0xD5, 0xD0, 0x00,
	 0x73, 0x00, 0x59, 0x00, 0x82},

	{0xFA, /* 80 cd */
	 0x02, 0x10, 0x10, 0x10, 0xD7, 0x39, 0xD6, 0xD6, 0xBF, 0xDD,
	 0xE1, 0xDA, 0xE2, 0xC0, 0xBF, 0xBD, 0xD3, 0xD5, 0xCF, 0x00,
	 0x78, 0x00, 0x5D, 0x00, 0x88},

	{0xFA, /* 90 cd */
	 0x02, 0x10, 0x10, 0x10, 0xD7, 0x39, 0xD5, 0xD5, 0xBF, 0xDC,
	 0xDF, 0xDA, 0xE0, 0xC1, 0xC0, 0xBD, 0xD2, 0xD4, 0xCF, 0x00,
	 0x7C, 0x00, 0x60, 0x00, 0x8C},

	{0xFA, /* 100 cd */
	 0x02, 0x10, 0x10, 0x10, 0xDD, 0x3A, 0xE3, 0xD7, 0xC5, 0xDD,
	 0xDF, 0xDA, 0xDF, 0xC0, 0xBF, 0xBC, 0xD0, 0xD3, 0xCD, 0x00,
	 0x81, 0x00, 0x64, 0x00, 0x92},

	{0xFA, /* 110 cd */
	 0x02, 0x10, 0x10, 0x10, 0xE1, 0x43, 0xE2, 0xD6, 0xC5, 0xDC,
	 0xDE, 0xDA, 0xDF, 0xBF, 0xBF, 0xBB, 0xD0, 0xD3, 0xCD, 0x00,
	 0x85, 0x00, 0x67, 0x00, 0x96},

	{0xFA, /* 120 cd */
	 0x02, 0x10, 0x10, 0x10, 0xE5, 0x48, 0xE4, 0xD5, 0xC5, 0xDB,
	 0xDE, 0xDA, 0xDD, 0xBE, 0xBF, 0xBB, 0xD0, 0xD2, 0xCC, 0x00,
	 0x88, 0x00, 0x6A, 0x00, 0x9A},

	{0xFA, /* 130 cd */
	 0x02, 0x10, 0x10, 0x10, 0xE6, 0x4D, 0xE3, 0xD5, 0xC5, 0xDA,
	 0xDD, 0xDA, 0xDD, 0xBE, 0xBE, 0xBA, 0xCE, 0xD1, 0xCA, 0x00,
	 0x8C, 0x00, 0x6D, 0x00, 0x9F},

	{0xFA, /* 140 cd */
	 0x02, 0x10, 0x10, 0x10, 0xE8, 0x51, 0xE4, 0xD6, 0xC9, 0xDB,
	 0xDC, 0xD9, 0xDC, 0xBE, 0xBF, 0xBA, 0xCD, 0xD0, 0xC9, 0x00,
	 0x90, 0x00, 0x70, 0x00, 0xA3},

	{0xFA, /* 150 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEA, 0x57, 0xE9, 0xD6, 0xC9, 0xDA,
	 0xDD, 0xDA, 0xDC, 0xBC, 0xBE, 0xB8, 0xCE, 0xD0, 0xCA, 0x00,
	 0x92, 0x00, 0x72, 0x00, 0xA6},

	{0xFA, /* 160 cd */
	 0x02, 0x10, 0x10, 0x10, 0xF0, 0x61, 0xEE, 0xD5, 0xC9, 0xD9,
	 0xDD, 0xDA, 0xDB, 0xBC, 0xBE, 0xB9, 0xCC, 0xCF, 0xC8, 0x00,
	 0x96, 0x00, 0x75, 0x00, 0xAA},

	{0xFA, /* 170 cd */
	 0x02, 0x10, 0x10, 0x10, 0xF1, 0x81, 0xEE, 0xD4, 0xC9, 0xD9,
	 0xDD, 0xDB, 0xDB, 0xBB, 0xBD, 0xB7, 0xCC, 0xCE, 0xC8, 0x00,
	 0x99, 0x00, 0x78, 0x00, 0xAE},

	{0xFA, /* 180 cd */
	 0x02, 0x10, 0x10, 0x10, 0xF0, 0x8C, 0xED, 0xD5, 0xCA, 0xD8,
	 0xDC, 0xDB, 0xDC, 0xBB, 0xBD, 0xB7, 0xCB, 0xCD, 0xC5, 0x00,
	 0x9C, 0x00, 0x7A, 0x00, 0xB2},

	{0xFA, /* 190 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0x90, 0xEC, 0xD4, 0xC9, 0xD7,
	 0xDD, 0xDB, 0xDB, 0xBA, 0xBD, 0xB7, 0xCA, 0xCD, 0xC5, 0x00,
	 0x9F, 0x00, 0x7C, 0x00, 0xB5},

	{0xFA, /* 200 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0x8F, 0xED, 0xD2, 0xC7, 0xD5,
	 0xDC, 0xDA, 0xDA, 0xBA, 0xBB, 0xB5, 0xCA, 0xCC, 0xC4, 0x00,
	 0xA0, 0x00, 0x7F, 0x00, 0xBB},

	{0xFA, /* 210 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEF, 0x96, 0xEF, 0xD1, 0xC7, 0xD3,
	 0xDB, 0xD9, 0xD9, 0xB9, 0xBB, 0xB4, 0xCA, 0xCC, 0xC6, 0x00,
	 0xA3, 0x00, 0x81, 0x00, 0xBD},

	{0xFA, /* 220 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0x99, 0xEE, 0xD3, 0xC8, 0xD3,
	 0xDB, 0xD9, 0xD9, 0xB8, 0xBB, 0xB4, 0xC9, 0xCC, 0xC4, 0x00,
	 0xA6, 0x00, 0x83, 0x00, 0xC1},

	{0xFA, /* 230 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEE, 0xA3, 0xEF, 0xD2, 0xC9, 0xD3,
	 0xDB, 0xD9, 0xD9, 0xB9, 0xBA, 0xB3, 0xC8, 0xCC, 0xC4, 0x00,
	 0xA8, 0x00, 0x85, 0x00, 0xC4},

	{0xFA, /* 240 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0xA4, 0xEE, 0xD2, 0xC9, 0xD3,
	 0xDC, 0xD9, 0xD9, 0xB7, 0xBA, 0xB3, 0xC8, 0xCB, 0xC2, 0x00,
	 0xAB, 0x00, 0x87, 0x00, 0xC8},

	{0xFA, /* 250 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0xA8, 0xEF, 0xD2, 0xC8, 0xD2,
	 0xDA, 0xD9, 0xD8, 0xB7, 0xB9, 0xB2, 0xC8, 0xCB, 0xC2, 0x00,
	 0xAD, 0x00, 0x89, 0x00, 0xCB},

	{0xFA, /* 260 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEB, 0xB2, 0xEF, 0xD2, 0xC9, 0xD3,
	 0xDB, 0xDA, 0xD9, 0xB6, 0xB9, 0xBA, 0xC7, 0xCA, 0xC1, 0x00,
	 0xB0, 0x00, 0x8B, 0x00, 0xCE},

	{0xFA, /* 270 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEC, 0xB0, 0xEF, 0xD1, 0xC9, 0xD2,
	 0xDB, 0xDA, 0xD9, 0xB7, 0xB9, 0xB1, 0xC6, 0xC9, 0xC0, 0x00,
	 0xB2, 0x00, 0x8D, 0x00, 0xD1},

	{0xFA, /* 280 cd */
	 0x02, 0x10, 0x10, 0x10, 0xED, 0xB6, 0xF0, 0xD1, 0xC9, 0xD2,
	 0xDB, 0xDA, 0xD9, 0xB6, 0xB8, 0xAF, 0xC7, 0xCA, 0xC2, 0x00,
	 0xB4, 0x00, 0x8F, 0x00, 0xD3},

	{0xFA, /* 290 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEC, 0xB7, 0xEE, 0xD0, 0xC8, 0xD1,
	 0xDB, 0xDA, 0xD9, 0xB6, 0xB8, 0xB0, 0xC5, 0xC9, 0xC1, 0x00,
	 0xB7, 0x00, 0x91, 0x00, 0xD5},

	{0xFA, /* 300 cd */
	 0x02, 0x10, 0x10, 0x10, 0xEC, 0xB7, 0xEF, 0xD1, 0xCA, 0xD1,
	 0xDB, 0xDA, 0xD8, 0xB5, 0xB8, 0xB0, 0xC5, 0xC8, 0xBF, 0x00,
	 0xB9, 0x00, 0x93, 0x00, 0xD9},
};


/*
 * [1] ETC Condition Set 1
 */
static char etc_cond_set_1_1[] = {
	0xF0,
	0x5A, 0x5A
};
static char etc_cond_set_1_2[] = {
	0xF1,
	0x5A, 0x5A
};
static char etc_cond_set_1_3[] = {
	0xFC,
	0x5A, 0x5A
};

/*
 * [2] Gamma Condition Set
 */
static char gamma_cond_set[] = {
	0xFA, /* 70 cd */
	0x02, 0x58, 0x42, 0x56, 0xa8,
	0xc3, 0xae, 0xbd, 0xc6, 0xc2, 0xae,
	0xbe, 0xaf, 0x99, 0xa4, 0x99, 0xac,
	0xb3, 0xa8, 0x00, 0xc1, 0x00, 0xb7,
	0x00, 0xdf
};
static char gamma_set_update[] = {
	0xFA,
	0x03
};

/*
 * [3] Panel Condition Set
 */
static char panel_cond_set1[] = {
	0xF8,
	0x27, 0x27, 0x08, 0x08, 0x4E,
	0xAA, 0x5E, 0x8A, 0x10, 0x3F,
	0x10, 0x10, 0x00
};
static char panel_cond_set2[] = {
	0xF7,
	0x03
};
static char panel_cond_set3[] = {
	0xB3,
	0x63, 0x02, 0xC3, 0x32, 0xFF
};

/*
 * [4] ETC Condition Set 2
 */
static char etc_cond_set_2_1[] = {
	0xF6,
	0x00, 0x84, 0x09
};
static char etc_cond_set_2_2[] = {
	0xB0,
	0x09
};
static char etc_cond_set_2_3[] = {
	0xD5,
	0x64
};
static char etc_cond_set_2_4[] = {
	0xB0,
	0x0B
};
static char etc_cond_set_2_5[] = {
	0xD5,
	0xA4, 0x7E, 0x20
};
static char etc_cond_set_2_6[] = {
	0xB0,
	0x08
};
static char etc_cond_set_2_7[] = {
/* FDh is hidden command */
	0xFD,
	0xF8
};
static char etc_cond_set_2_8[] = {
	0xB0,
	0x04
};
static char etc_cond_set_2_9[] = {
	0xF2,
	0x4D
};

static char etc_cond_set_2_10[] = {
	0xB0,
	0x05
};
static char etc_cond_set_2_11[] = {
	0xFD,
	0x1F
};

static char etc_cond_set_2_12[] = {
	0xB1,
	0x01, 0x00, 0x16
};

static char etc_cond_set_2_13[] = {
	0xB2,
	0x15, 0x15, 0x15, 0x15
};
static char etc_cond_set_2_14[] = { 0x11, /* no param */ };

/*
 * [5] Memory window setting 1
 */
static char memory_window_set_1_1[] = {
	0x2A,
	0x00, 0x00, 0x02, 0x57
};
static char memory_window_set_1_2[] = {
	0x2B,
	0x00, 0x00, 0x03, 0xFF
};
static char memory_window_set_1_3[] = { 0x2C, /* no param */ };

/*
 * [6] Memory window setting 2
 */
static char memory_window_set_2_1[] = { 0x35, /* no param */ };
static char memory_window_set_2_2[] = {
	0x2A,
	0x00, 0x1E, 0x02, 0x39
};
static char memory_window_set_2_3[] = {
	0x2B,
	0x00, 0x00, 0x03, 0xBF
};
static char memory_window_set_2_4[] = {
	0xD1,
	0x8A
};

/*
 * [7] ACL ON/OFF setting
 */
#ifdef USE_ACL
static char acl_cond_set[] = {
	/*
	*	There is LDI defect on acl process.
	*	So setting 0xC1 register all zero instead of
	*	setting 0xC0 register to 0x00
	*/
	0xC1,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

static char acl_const_set[] = {
	/*
	*	There is LDI defect on acl process.
	*	So setting 0xC1 register all zero instead of
	*	setting 0xC0 register to 0x00
	*/
	0xC1,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

static char acl_on[] = {
	0xC0,
	0x01
};

static char acl_off[] = {
	/*
	*	There is LDI defect on acl process.
	*	So setting 0xC1 register all zero instead of
	*	setting 0xC0 register to 0x00
	*/
	0xC1,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};
#endif
#ifdef USE_ELVSS
static char each_elvss_cond_set[] = { 0xB2, 0x15, 0x15, 0x15, 0x15};
#endif
enum {
	GAMMA_30CD,
	GAMMA_40CD,
	GAMMA_50CD,
	GAMMA_60CD,
	GAMMA_70CD,
	GAMMA_80CD,
	GAMMA_90CD,
	GAMMA_100CD,
	GAMMA_110CD,
	GAMMA_120CD,
	GAMMA_130CD,
	GAMMA_140CD,
	GAMMA_150CD,
	GAMMA_160CD,
	GAMMA_170CD,
	GAMMA_180CD,
	GAMMA_190CD,
	GAMMA_200CD,
	GAMMA_210CD,
	GAMMA_220CD,
	GAMMA_230CD,
	GAMMA_240CD,
	GAMMA_250CD,
	GAMMA_300CD,
};

static int lux_tbl_acl[] = {
	  30, 40, 50, 60, 70, 80,
	  90, 100, 110, 120, 130, 140,
	  150, 160, 170, 180, 190, 200,
	  210, 220, 230, 240, 250, 300
};

static char gamma_cond_300cd[] = {
	0x58, 0x42, 0x56, 0xAA,
	0xC8, 0xAE, 0xB5, 0xC1, 0xBE,
	0xB4, 0xC0, 0xB2, 0x93, 0x9F,
	0x93, 0xA6, 0xAD, 0xA2, 0x00,
	0xE9, 0x00, 0xDB, 0x01, 0x0F
};

static char GAMMA_SmartDimming_COND_SET[] = {
	0xFA,
	0x02, 0x4A, 0x01, 0x4D, 0x7A,
	0x5D, 0xA5, 0x9C, 0xCA, 0xA4,
	0xBD, 0xDC, 0xBE, 0x93, 0xBD,
	0x95, 0xBA, 0xD2, 0xB7, 0x00,
	0x81, 0x00, 0x75, 0x00, 0xA5,
};

static char prepare_mtp_read1[] = {
	0xF0,
	0x5A, 0x5A
};

static char prepare_mtp_read2[] = {
	0xF1,
	0x5A, 0x5A
};

#ifdef USE_ACL
static struct dsi_cmd_desc samsung_panel_acl_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(acl_on), acl_on},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(acl_const_set), acl_const_set},
};

static struct dsi_cmd_desc samsung_panel_acl_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(acl_off), acl_off},
};
static struct dsi_cmd_desc samsung_panel_acl_update_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	 sizeof(acl_cond_set), acl_cond_set}
	,
};
#endif

/*
 * Display On, Stand-by Off Command
 */
static char all_pixel_off[] = { 0x22, /* no param */ };
static char normal_mode_on[] = { 0x13, /* no parm */ };
static char display_on[] = { 0x29, /* no param */ };
static char display_off[] = { 0x28, /* no param */ };
static char sleep_in[] = { 0x10, /* no param */ };

static struct dsi_cmd_desc samsung_mtp_read_cmds[] = {

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(prepare_mtp_read1), prepare_mtp_read1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(prepare_mtp_read2), prepare_mtp_read2},
};

static struct dsi_cmd_desc samsung_panel_ready_to_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_1), etc_cond_set_1_1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_2), etc_cond_set_1_2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_3), etc_cond_set_1_3},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set), gamma_cond_set},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_set_update), gamma_set_update},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(panel_cond_set1), panel_cond_set1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(panel_cond_set2), panel_cond_set2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(panel_cond_set3), panel_cond_set3},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_1), etc_cond_set_2_1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_2), etc_cond_set_2_2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_3), etc_cond_set_2_3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_4), etc_cond_set_2_4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_5), etc_cond_set_2_5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_6), etc_cond_set_2_6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_7), etc_cond_set_2_7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_8), etc_cond_set_2_8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_9), etc_cond_set_2_9},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_10), etc_cond_set_2_10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_11), etc_cond_set_2_11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_12), etc_cond_set_2_12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_13), etc_cond_set_2_13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_2_14), etc_cond_set_2_14},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 120,
		sizeof(memory_window_set_1_1), memory_window_set_1_1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(memory_window_set_1_2), memory_window_set_1_2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1,
		sizeof(memory_window_set_1_3), memory_window_set_1_3},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(memory_window_set_2_1), memory_window_set_2_1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(memory_window_set_2_2), memory_window_set_2_2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(memory_window_set_2_3), memory_window_set_2_3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(memory_window_set_2_4), memory_window_set_2_4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(acl_on), acl_on},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(acl_const_set), acl_const_set},
};

static struct dsi_cmd_desc samsung_panel_ready_to_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(display_off), display_off},
};

static struct dsi_cmd_desc samsung_panel_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc samsung_panel_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(sleep_in), sleep_in},
};

static struct dsi_cmd_desc samsung_panel_late_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(normal_mode_on), normal_mode_on},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 5,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc samsung_panel_early_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(all_pixel_off), all_pixel_off},
};

static struct dsi_cmd_desc samsung_panel_gamma_update_cmds[2] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_2_2[0]), gamma_2_2[0]},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_set_update), gamma_set_update},
};

static struct dsi_cmd_desc samsung_panel_elvss_update_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	 sizeof(each_elvss_cond_set), each_elvss_cond_set}
	,
};

#define LCD_ELVSS_DELTA_300CD (0)
#define LCD_ELVSS_DELTA_200CD (0x08)
#define LCD_ELVSS_DELTA_160CD (0x0B)
#define LCD_ELVSS_DELTA_100CD (0x10)
#define LCD_ELVSS_RESULT_LIMIT	(0x29)
static int GET_EACH_ELVSS_ID[] = {
	LCD_ELVSS_DELTA_100CD,/* 0 = 30_dimming,*/
	LCD_ELVSS_DELTA_100CD,/* 1 =  40*/
	LCD_ELVSS_DELTA_100CD,/* 2 =  50*/
	LCD_ELVSS_DELTA_100CD,/* 3 = 60,*/
	LCD_ELVSS_DELTA_100CD,/* 4 = 70,*/
	LCD_ELVSS_DELTA_100CD,/* 5 = 80,*/
	LCD_ELVSS_DELTA_100CD,/* 6 = 90,*/
	LCD_ELVSS_DELTA_100CD,/* 7 = 100,*/
	LCD_ELVSS_DELTA_160CD,/* 8 = 110,*/
	LCD_ELVSS_DELTA_160CD,/* 9 = 120,*/
	LCD_ELVSS_DELTA_160CD,/* 10 = 130,*/
	LCD_ELVSS_DELTA_160CD,/* 11 = 140,*/
	LCD_ELVSS_DELTA_160CD,/* 12 = 150,*/
	LCD_ELVSS_DELTA_160CD,/* 13 = 160,*/
	LCD_ELVSS_DELTA_200CD,/* 14 = 170,*/
	LCD_ELVSS_DELTA_200CD,/* 15 = 180,*/
	LCD_ELVSS_DELTA_200CD,/* 16 = 190,*/
	LCD_ELVSS_DELTA_200CD,/* 17 = 200,*/
	LCD_ELVSS_DELTA_300CD,/* 18 = 210,*/
	LCD_ELVSS_DELTA_300CD,/* 19 = 220,*/
	LCD_ELVSS_DELTA_300CD,/* 20 = 230,*/
	LCD_ELVSS_DELTA_300CD,/* 21 = 240,*/
	LCD_ELVSS_DELTA_300CD,/* 22 = 250,*/
	LCD_ELVSS_DELTA_300CD/* 23= 300,*/
};


#define LCD_ELVSS_DEFAULT_100CD (0x25)
#define LCD_ELVSS_DEFAULT_160CD (0x20)
#define LCD_ELVSS_DEFAULT_200CD (0x1D)
#define LCD_ELVSS_DEFAULT_300CD (0x15)
static int GET_DEFAULT_ELVSS_ID[] = {
	LCD_ELVSS_DEFAULT_100CD,/* 0 = 30_dimming,*/
	LCD_ELVSS_DEFAULT_100CD,/* 1 =  40*/
	LCD_ELVSS_DEFAULT_100CD,/* 2 =  50*/
	LCD_ELVSS_DEFAULT_100CD,/*  3 = 60,*/
	LCD_ELVSS_DEFAULT_100CD,/* 4 = 70,*/
	LCD_ELVSS_DEFAULT_100CD,/* 5 = 80,*/
	LCD_ELVSS_DEFAULT_100CD,/* 6 = 90,*/
	LCD_ELVSS_DEFAULT_100CD,/* 7 = 100,*/
	LCD_ELVSS_DEFAULT_160CD,/* 8 = 110,*/
	LCD_ELVSS_DEFAULT_160CD,/* 9 = 120,*/
	LCD_ELVSS_DEFAULT_160CD,/* 10 = 130,*/
	LCD_ELVSS_DEFAULT_160CD,/* 11 = 140,*/
	LCD_ELVSS_DEFAULT_160CD,/* 12 = 150,*/
	LCD_ELVSS_DEFAULT_160CD,/* 13 = 160,*/
	LCD_ELVSS_DEFAULT_200CD,/* 14 = 170,*/
	LCD_ELVSS_DEFAULT_200CD,/* 15 = 180,*/
	LCD_ELVSS_DEFAULT_200CD,/* 16 = 190,*/
	LCD_ELVSS_DEFAULT_200CD,/* 17 = 200,*/
	LCD_ELVSS_DEFAULT_300CD,/* 18 = 210,*/
	LCD_ELVSS_DEFAULT_300CD,/* 19 = 220,*/
	LCD_ELVSS_DEFAULT_300CD,/* 20 = 230,*/
	LCD_ELVSS_DEFAULT_300CD,/* 21 = 240,*/
	LCD_ELVSS_DEFAULT_300CD,/* 22 = 250,*/
	LCD_ELVSS_DEFAULT_300CD/* 23 = 300,*/
};


/********************* ACL *******************/
static char ACL_COND_SET_50[] = {
	0xC1, 0x05, 0x0A, 0x0F, 0x16, 0x1B, 0x21, 0x26, 0x2A, 0x2F, 0x35, 0x3A,
	0x3F, 0x44, 0x49, 0x49, 0x00, };
static char ACL_COND_SET_40[] = {
	0xC1, 0x03, 0x07, 0x0D, 0x12, 0x16, 0x1A, 0x1D, 0x21, 0x25, 0x28, 0x2D,
	0x31, 0x35, 0x38, 0x38, 0x00, };
static char ACL_COND_SET_33[] = {
	0xC1, 0x03, 0x05, 0x0A, 0x0F, 0x12, 0x16, 0x18, 0x1B, 0x1E, 0x21, 0x25,
	0x2A, 0x2B, 0x2E, 0x2E, 0x00, };
static char ACL_COND_SET_20[] = {
	0xC1, 0x02, 0x04, 0x06, 0x09, 0x0B, 0x0F, 0x10, 0x11, 0x12, 0x14, 0x17,
	0x18, 0x1A, 0x1B, 0x1B, 0x00, };
static char ACL_COND_SET_0[] = {
	0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, };

static struct dsi_cmd_desc DSI_CMD_ACL_50 = {
DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(ACL_COND_SET_50), ACL_COND_SET_50 };
static struct dsi_cmd_desc DSI_CMD_ACL_40 = {
DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(ACL_COND_SET_40), ACL_COND_SET_40 };
static struct dsi_cmd_desc DSI_CMD_ACL_33 = {
DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(ACL_COND_SET_33), ACL_COND_SET_33 };
static struct dsi_cmd_desc DSI_CMD_ACL_20 = {
DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(ACL_COND_SET_20), ACL_COND_SET_20 };
static struct dsi_cmd_desc DSI_CMD_ACL_0 = {
DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(ACL_COND_SET_0), ACL_COND_SET_0 };
static struct dsi_cmd_desc_LCD lcd_acl_table[] = {
	{30, "30", &DSI_CMD_ACL_0},
	{30, "40", &DSI_CMD_ACL_0},
	{40, "50", &DSI_CMD_ACL_40},
	{40, "60", &DSI_CMD_ACL_40},
	{40, "70", &DSI_CMD_ACL_40},
	{40, "80", &DSI_CMD_ACL_40},
	{40, "90", &DSI_CMD_ACL_40},
	{40, "100", &DSI_CMD_ACL_40},
	{40, "110", &DSI_CMD_ACL_40},
	{40, "120", &DSI_CMD_ACL_40},
	{40, "130", &DSI_CMD_ACL_40},
	{40, "140", &DSI_CMD_ACL_40},
	{40, "150", &DSI_CMD_ACL_40},
	{40, "160", &DSI_CMD_ACL_40},
	{40, "170", &DSI_CMD_ACL_40},
	{40, "180", &DSI_CMD_ACL_40},
	{40, "190", &DSI_CMD_ACL_40},
	{40, "200", &DSI_CMD_ACL_40},
	{40, "210", &DSI_CMD_ACL_40},
	{40, "220", &DSI_CMD_ACL_40},
	{40, "230", &DSI_CMD_ACL_40},
	{40, "240", &DSI_CMD_ACL_40},
	{40, "250", &DSI_CMD_ACL_40},
	{50, "300", &DSI_CMD_ACL_50},
};

static int get_candela_index(int bl_level)
{
	int backlightlevel;

	/* brightness setting from platform is from 0 to 255
	 * But in this driver, brightness is only supported from 0 to 24 */

	switch (bl_level) {
	case 0 ... 29:
		if (mipi_pd.msd->dstat.auto_brightness == 1)
			backlightlevel = GAMMA_40CD; /* 1 */
		else
			backlightlevel = GAMMA_30CD; /* 0*/
		break;
	case 30 ... 49:
		backlightlevel = GAMMA_40CD; /* 1 */
		break;
	case 50 ... 59:
		backlightlevel = GAMMA_50CD; /* 3 */
		break;
	case 60 ... 69:
		backlightlevel = GAMMA_60CD; /* 4 */
		break;
	case 70 ... 79:
		backlightlevel = GAMMA_70CD; /* 5 */
		break;
	case 80 ... 89:
		backlightlevel = GAMMA_80CD; /* 6 */
		break;
	case 90 ... 99:
		backlightlevel = GAMMA_90CD; /* 8 */
		break;
	case 100 ... 109:
		backlightlevel = GAMMA_100CD; /* 9 */
		break;
	case 110 ... 119:
		backlightlevel = GAMMA_110CD; /* 10 */
		break;
	case 120 ... 125:
		backlightlevel = GAMMA_120CD; /* 11 */
		break;
	case 126 ... 129:
		backlightlevel = GAMMA_130CD; /* 11 */
		break;
	case 130 ... 135:
		backlightlevel = GAMMA_140CD; /* 12 */
		break;
	case 136 ... 139:
		backlightlevel = GAMMA_150CD; /* 12 */
		break;
	case 140 ... 149:
		backlightlevel = GAMMA_160CD; /* 13 */
		break;
	case 150 ... 159:
		backlightlevel = GAMMA_170CD; /* 14 */
		break;
	case 160 ... 169:
		backlightlevel = GAMMA_180CD; /* 15 */
		break;
	case 170 ... 179:
		backlightlevel = GAMMA_190CD; /* 15 */
		break;
	case 180 ... 189:
		backlightlevel = GAMMA_200CD; /* 15 */
		break;
	case 190 ... 199:
		backlightlevel = GAMMA_210CD; /* 16 */
		break;
	case 200 ... 219:
		backlightlevel = GAMMA_220CD; /* 19 */
		break;
	case 220 ... 239:
		backlightlevel = GAMMA_230CD; /* 20 */
		break;
	case 240 ... 249:
		backlightlevel = GAMMA_240CD; /* 21 */
		break;
	case 250 ... 254:
		backlightlevel = GAMMA_250CD; /* 22 */
		break;
	case 255:
		if (mipi_pd.msd->dstat.auto_brightness == 1)
			backlightlevel = GAMMA_300CD; /* 23 */
		else
			backlightlevel = GAMMA_250CD; /* 22 */
		break;
	default:
		backlightlevel = GAMMA_40CD; /* 1 */
		break;
	}
	return backlightlevel;
}



static int set_acl_on_level(int bl_level)
{
	int cd;
	cd = get_candela_index(bl_level);

	if (!lcd_acl_table[cd].lux)
		return 1;

	if (lcd_acl_table[cd].lux) {
		samsung_panel_acl_update_cmds[0].dlen =
		    lcd_acl_table[cd].cmd->dlen;
		samsung_panel_acl_update_cmds[0].payload =
		    lcd_acl_table[cd].cmd->payload;
	}
	return 0;
}

static int set_elvss_level(int bl_level)
{
	unsigned char calc_elvss;
	int cd;
	int elvss_pulse;

	cd = get_candela_index(bl_level);
	elvss_pulse = mipi_pd.lcd_elvss_data[0] & 0x3F;
	if (elvss_pulse == 0)
		calc_elvss = GET_DEFAULT_ELVSS_ID[cd];
	else
		calc_elvss = elvss_pulse + GET_EACH_ELVSS_ID[cd];

	pr_debug("%s: elvss_pulse=%x,calc_elvss = %x\n", __func__, elvss_pulse,
		calc_elvss);
	if (calc_elvss > LCD_ELVSS_RESULT_LIMIT)
		calc_elvss = LCD_ELVSS_RESULT_LIMIT;
	each_elvss_cond_set[1] = calc_elvss;
	each_elvss_cond_set[2] = calc_elvss;
	each_elvss_cond_set[3] = calc_elvss;
	each_elvss_cond_set[4] = calc_elvss;

	return 0;
}

void reset_gamma_level(void)
{
	pr_info("reset_gamma_level\n");
	mipi_pd.lcd_current_cd_idx = -1;
}
static int set_gamma_level(int bl_level, enum gamma_mode_list gamma_mode)
{
	int cd;
	int *lux_tbl = lux_tbl_acl;

	cd = get_candela_index(bl_level);
	if (mipi_pd.lcd_current_cd_idx == cd) {
		pr_debug("mipi_pd.lcd_current_cd_idx :%d cd:%d\n",
			mipi_pd.lcd_current_cd_idx, cd);
		return -1;
		}
	else
		mipi_pd.lcd_current_cd_idx = cd;

	if (gamma_mode == GAMMA_2_2) {
		samsung_panel_gamma_update_cmds[0].dlen = sizeof(gamma_2_2[cd]);
		samsung_panel_gamma_update_cmds[0].payload = gamma_2_2[cd];
	} else if (gamma_mode == GAMMA_1_9) {
		samsung_panel_gamma_update_cmds[0].dlen = sizeof(gamma_1_9[cd]);
		samsung_panel_gamma_update_cmds[0].payload = gamma_1_9[cd];
	} else {
		/*  SMART Dimming gamma_lux;  */
		char pBuffer[256];
		int i;
		int gamma_lux;

		gamma_lux = lux_tbl[cd];

		if (gamma_lux > SmartDimming_CANDELA_UPPER_LIMIT)
			gamma_lux = SmartDimming_CANDELA_UPPER_LIMIT;

		/* Set Minimum Lux for Dimm */

		for (i = SmartDimming_GammaUpdate_Pos;
		     i < sizeof(GAMMA_SmartDimming_COND_SET); i++)
			GAMMA_SmartDimming_COND_SET[i] = 0;
		mipi_pd.smart_s6e39a0x02.brightness_level = gamma_lux;
		generate_gamma(&mipi_pd.smart_s6e39a0x02,
			&(GAMMA_SmartDimming_COND_SET[2]), GAMMA_SET_MAX);
		samsung_panel_gamma_update_cmds[0].dlen =
		    sizeof(GAMMA_SmartDimming_COND_SET);
		samsung_panel_gamma_update_cmds[0].payload =
		    GAMMA_SmartDimming_COND_SET;
		pBuffer[0] = 0;
		for (i = 0; i < sizeof(GAMMA_SmartDimming_COND_SET); i++) {
			snprintf(pBuffer + strnlen(pBuffer, 256), 256, " %02x",
				 GAMMA_SmartDimming_COND_SET[i]);
		}
		pr_info("SD: %03d %s\n", gamma_lux, pBuffer);
		pr_debug("bl_level:%d,cd:%d:Candela:%d\n", bl_level, cd,
			gamma_lux);
	}

	return 0;
}

static struct mipi_panel_data mipi_pd = {
	.panel_name = "SMD_AMS429QC01\n",
	.ready_to_on = {samsung_panel_ready_to_on_cmds
			, ARRAY_SIZE(samsung_panel_ready_to_on_cmds)},
	.ready_to_off = {samsung_panel_ready_to_off_cmds
			, ARRAY_SIZE(samsung_panel_ready_to_off_cmds)},
	.on = {samsung_panel_on_cmds
			, ARRAY_SIZE(samsung_panel_on_cmds)},
	.off = {samsung_panel_off_cmds
			, ARRAY_SIZE(samsung_panel_off_cmds)},
	.late_on = {samsung_panel_late_on_cmds
			, ARRAY_SIZE(samsung_panel_late_on_cmds)},
	.early_off = {samsung_panel_early_off_cmds
			, ARRAY_SIZE(samsung_panel_early_off_cmds)},
	.gamma_update = {samsung_panel_gamma_update_cmds
			, ARRAY_SIZE(samsung_panel_gamma_update_cmds)},
#ifdef USE_ACL
	.acl_on = {samsung_panel_acl_on_cmds
			, ARRAY_SIZE(samsung_panel_acl_on_cmds)},
	.acl_off = {samsung_panel_acl_off_cmds
			, ARRAY_SIZE(samsung_panel_acl_off_cmds)},
	.acl_update = {
	samsung_panel_acl_update_cmds,
	     ARRAY_SIZE(samsung_panel_acl_update_cmds)},
#endif
#ifdef USE_ELVSS
	.elvss_update = {
	samsung_panel_elvss_update_cmds,
	     ARRAY_SIZE(samsung_panel_elvss_update_cmds)},
#endif
	.set_elvss = set_elvss_level,
	.set_acl = set_acl_on_level,
	.set_gamma = set_gamma_level,
	.gamma_smartdim = gamma_cond_300cd,
	.lcd_current_cd_idx = -1,
	.mtp_read_enable = {samsung_mtp_read_cmds
				, ARRAY_SIZE(samsung_mtp_read_cmds)},
};

static struct mipi_dsi_phy_ctrl dsi_command_mode_phy_db = {
	/* DSI_BIT_CLK at 410MHz, 2 lane, RGB888 */
	{0x0F, 0x0a, 0x04, 0x00, 0x20}, /* regulator */
	/* timing   */
	{0xAD, 0x8B, 0x19, 0x00, 0x93, 0x96, 0x1C, 0x8D,
	 0x1C, 0x03, 0x04, 0xa0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x1, 0x1a, 0x00, 0x50, 0x48, 0x63,
	 0x41, 0x0f, 0x01,
	 0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01},
};

static int __init mipi_cmd_samsung_oled_qhd_pt_init(void)
{
	int ret;


#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
	if (msm_fb_detect_client("mipi_cmd_samsung_oled_qhd"))
		return 0;
#endif

	pinfo.xres = 540;
	pinfo.yres = 960;
	pinfo.mode2_xres = 0;
	pinfo.mode2_yres = 0;
	pinfo.mode2_bpp = 0;
	/*
	 *
	 * Panel's Horizontal input timing requirement is to
	 * include dummy(pad) data of 200 clk in addition to
	 * width and porch/sync width values
	 */
	pinfo.lcdc.xres_pad = 0;
	pinfo.lcdc.yres_pad = 0;

	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 32;
	pinfo.lcdc.h_back_porch = 0;
	pinfo.lcdc.h_front_porch = 0;
	pinfo.lcdc.h_pulse_width = 0;
	pinfo.lcdc.v_back_porch = 0x2;
	pinfo.lcdc.v_front_porch = 0x4D;
	pinfo.lcdc.v_pulse_width = 0x1;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 410000000;
	pinfo.lcd.v_back_porch = pinfo.lcdc.v_back_porch;
	pinfo.lcd.v_front_porch = pinfo.lcdc.v_front_porch;
	pinfo.lcd.v_pulse_width = pinfo.lcdc.v_pulse_width;
	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.refx100 = 4024; /* adjust refx100 to prevent tearing */
	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.t_clk_post = 0x19;
	pinfo.mipi.t_clk_pre = 0x2E;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;
	pinfo.mipi.dsi_phy_db = &dsi_command_mode_phy_db;

	ret = mipi_samsung_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_WVGA_PT,
						&mipi_pd);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}
module_init(mipi_cmd_samsung_oled_qhd_pt_init);
