//==============================================================================
//
// Title:		SignalAnalyzerShared.h
// Purpose:		A short description of the interface.
//
// Created on:	10/30/2017 at 6:10:26 PM by Sergei Mistyuk.
// Copyright:	CSUS Metamaterials Group. All Rights Reserved.
//
//==============================================================================

#ifndef __SignalAnalyzerShared_H__
#define __SignalAnalyzerShared_H__

//Include statements
#include "cvidef.h"
#include "GlobalDefs.h"
#include <utility.h>
#include <userint.h>
#include <analysis.h>

#include "NIDAQmx.h"
#include "Multi-Function-Synch AI-AO_Fn.h"
#include "FileIO.h"

#ifdef __cplusplus
    extern "C" {
#endif
		
//Constants
#define WAVEFORMCYCLESHIFT 0.0//0.5

//Structs
typedef struct
{
	int currentBuffer;
	int switchBufferAfterLastSample;
	int error;
	float64 *outputBuffer;
	float64 *newDataAddress;
	unsigned int *outputBufferSize;
	double *settleTime;
} BufferControl;

typedef struct Range
{
	double start;
	double stop;
	double min;
	double max;
	unsigned short steps;
	struct Range* nextRange;
} Range;

//Function prototypes
double getNearestFreq(double frequency, double deviceOutRate);
unsigned int getCyclesOut (double frequency);
int getMaxBufferSizes (double *frequencyArray, int totalSteps, double rateIn, double rateOut,unsigned int *readInBufferSize, unsigned int *readOutBufferSize);
unsigned int getBufferSize(double cyclesOut, double adjustedSamplesPerCycle);
int getSamplesPerCycle (double signalFrequency, double deviceRate, double *samplesPerCycle, double *adjustedSamplesPerCycle);
double getAdjustedDeviceRate(double signalFrequency, double deviceMaxRate, double samplesPerCycle, double adjustedSamplesPerCycle);
unsigned int getCyclesToFit(unsigned int cyclesOut);
double normalizePhase(double phase);
double findTrueOutputVoltage(double voltage, int useExternalAmp, double gain);
double findTrueInputVoltage(double voltage, double frequency, int useDivider, double dividerRatio, Calibration *cal);
double findTrueInputPhase(double phase, double frequency, Calibration *cal);

int getTrace(int measurementType, int useCalibration, int trialsToRun, double avgStartFreq, BufferControl *bufferControl, int *panel, int status, int magChart, int phaseChart, int signalsPlot, int freqLabel, char inputChan[CHAN_NAME_LEN], char outputChan[CHAN_NAME_LEN], char refChan[CHAN_NAME_LEN], char trigChan[CHAN_NAME_LEN], uInt8 useRefChan, int useTrigChan, int inputConfigType, double startParam, double stopParam, int stepType, unsigned short stepsPerUnit, double freqOut, double inputVoltage, double outputVoltage, int inputVoltageFollowsOutput, int fitFromEnd, double delayTime, double settleTime, int *vnaRunning, int *requestStop, double **vnaFreqData, double **vnaOutputData, double **vnaMagData, double **vnaPhaseData, int *vnaArraySize, int *vnaDataPoints, int useExternalAmp, double ampGain, int useDivider, double dividerRatio);
int canUpdate(int *panel, element);
void determineUpdateElements(int panel, int status, int measurementLabel, int magChart, int phaseChart, int signalsChart, int *statusCanUpdate, int *measurementLableCanUpdate, int *magCanUpdate, int *phaseCanUpdate, int *signalsCanUpdate);
void determineElementUpdateStatus (int panel, int element, int *canUpdate);
int prepVNAData(int numDataPoints, double **vnaFreqData, double **vnaOutputData, double **vnaMagData, double **vnaPhaseData, int *vnaArraySize, int *vnaDataPoints);
unsigned int buildStepArray (double startFreq, double stopFreq, int stepType, unsigned short stepsPerUnit, double *stepArray);
int initVNA(char *inputChan, char *outputChan, char *refChan, char *trigChan, int useRefChan, int useTrigChan, double freqOut, double maxRateOut, double maxRateIn, double inputVoltage, double outputVoltage, unsigned int dataBufferSize, unsigned int outputBufferSize, unsigned int inputConfigType, TaskHandle *inputTaskHandle, TaskHandle *outputTaskHandle, TaskHandle *trigTaskHandle);
int getVNAPoint(double freqOut, double rateIn, double rateOut, unsigned int readInBufferSize, unsigned int dataBufferSize, unsigned int outputBufferSize, double inputVoltage, double outputVoltage, TaskHandle inputTaskHandle, TaskHandle outputTaskHandle, uInt8 useRefChan, TaskHandle triggerTaskHandle, uInt8 useTrigger, int triggerType, int fitFromEnd, unsigned int samplesToRead, unsigned int samplesToWrite, unsigned int *numRead, float64 *wave, float64 *data);
float64* findNewSRWaveAddr(BufferControl *bufferControl, int useTrigger);
int initSR(char *inputChan, char *outputChan, char *refChan, char *trigChan, int useRefChan, int useTrigChan, BufferControl *bufferControl, unsigned int dataBufferSize, double freqOut, double maxRateOut, double maxRateIn, double inputVoltage, double outputVoltage, int inputConfigType, TaskHandle *inputTaskHandle, TaskHandle *outputTaskHandle, TaskHandle *trigTaskHandle);
int getSRPoint(BufferControl *bufferControl, int run, int updateBuffer, double freqOut, unsigned int dataBufferSize, unsigned int samplesToRead, unsigned int samplesToWrite, TaskHandle inputTaskHandle, TaskHandle outputTaskHandle, uInt8 useRefChan, TaskHandle triggerTaskHandle, uInt8 useTrigger, int triggerType, double settleTime, float64 *wave, float64 *data, int *numRead, int *requestStop);
int waitSettleTime(double settleTime, int *requestStop);
int computeWaveParameters(int numRead, int useRefChan, int cyclesOut, double freqOut, double rateIn, double adjustedSamplesPerCycleIn, int fitFromEnd, double *computedSamplesPerCycleIn, float64 *data, float64 **dataToFit, float64 **refDataToFit, double *magDataPoint, double *phaseDataPoint, int *pointsToFit, double *mainDataPhase, double *refDataPhase);
int prepFitVars(double frequency, double samplesPerCycle, unsigned int cyclesOut, unsigned int cyclesToFit, unsigned int fitFromEnd, unsigned int sampleShift, float64 *data, unsigned int *dataAddrShift, float64 **dataToFit, unsigned int *samplesToFit, SearchType *search);

int prepAnalogTrigger(float64* buffer, unsigned int bufferSize, int measurementType);
int buildLinearArray(double m, double b, float64 *yVals, int arrayLength);

int32 CVICALLBACK updateSignalPlot (TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK updateOutputBuffer (TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);


//Range functions
Range* getNewRange(double start, double stop, double min, double max, unsigned short steps);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __SignalAnalyzerShared_H__ */
