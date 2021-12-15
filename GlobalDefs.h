//==============================================================================
//
// Title:		GlobalDefs.h
// Purpose:		A short description of the interface.
//
// Created on:	11/1/2017 at 10:08:30 PM by Sergei Mistyuk.
// Copyright:	CSUS Metamaterials Group. All Rights Reserved.
//
//==============================================================================

#ifndef __GlobalDefs_H__
#define __GlobalDefs_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "cvidef.h"
#include <ansi_c.h>
		
#define MIN_SAMPS_PER_CYCLE 8
#define FREQ_MEASUREMENT 0
#define VOLT_MEASUREMENT 1
#define Min(x, y) (((x) <= (y)) ? (x) : (y))
#define Max(x, y) (((x) >= (y)) ? (x) : (y))
		
typedef struct Calibration
{
	double am;
	double bm;
	double cm;
	double dm;
	double ap;
	double bp;
	double cp;
	double dp;
	int completed;
	int refCal;
	uint32_t serial;
	uint32_t outSerial;
} Calibration;

//Global variables
char baseDir[MAX_PATHNAME_LEN];
char calDir[MAX_PATHNAME_LEN];

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __GlobalDefs_H__ */
