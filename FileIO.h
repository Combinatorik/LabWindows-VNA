#ifndef __FileIO_H__
#define __FileIO_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "inifile.h"
#include "cvidef.h"
#include "Multi-Function-Synch AI-AO.h"
#include "GlobalDefs.h"
    	
typedef struct
{
	IniText settingsHandle;
	char iniFile[MAX_PATHNAME_LEN];
	int fileOpened;
} Settings;

static Settings settingsStruct;

int openIniFile(Settings *settings);

int saveFGenSettings(Settings *settings, int fgenPanelHandle);
int readFGenSettings(Settings *settings, int fgenPanelHandle);

int saveVNASettings(Settings *settings, int vnaPanelHandle);
int readVNASettings(Settings *settings, int vnaPanelHandle);

int saveSRSettings();
int readSRSettings();

void saveInstrumentData(int isRunning, int dataPoints, double *freqData, double *voutData, double *magData, double *phaseData);

int ReadCalFromFile (uint64_t inSerial, uint64_t outDevSerial, char calDir[MAX_PATHNAME_LEN], Calibration *cal);
int WriteCalToFile (char calDir[MAX_PATHNAME_LEN], Calibration *calRun);
		
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __FileIO_H__ */
