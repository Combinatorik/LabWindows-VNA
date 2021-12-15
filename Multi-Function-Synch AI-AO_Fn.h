#ifndef __MULTI-FUNCTION-SYNCH_AI-AO_FN_H__
#define __MULTI-FUNCTION-SYNCH_AI-AO_FN_H__

//Includes
#include <formatio.h>
#include <ansi_c.h>
#include "NIDAQmx.h"

//Macros
#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

//Constants
#define CHAN_NAME_LEN 256
#define CHAN_DNE 0
#define ANALOG_CHAN 1
#define DIGITAL_CHAN 2
#define TRIGGER_LEVEL 1.0

//Structure definitions
typedef struct
{
	char location[CHAN_NAME_LEN];
	int8 connected;
	uInt32 serial;
	char name[CHAN_NAME_LEN];
	double rateIn;
	double rateOut;
	double sharedRateIn;
	float64 inMaxV;
	float64 inMinV;
	float64 outMaxV;
	float64 outMinV;
	int inputBufferSize;
	int outputBuffersize;
} Device;

//Function definitions
int32 Configure_SingleChannelDigitalTriggerSynchAIAO(const char inputChan[], const char outputChan[], float64 minIn, float64 minOut, float64 maxIn, float64 maxOut, float64 rateIn, float64 rateOut, TaskHandle *inputTaskHandle, TaskHandle *triggerTaskHandle, TaskHandle *outputTaskHandle, int inputType, unsigned int inputBufferSize, unsigned int outputBufferSize, int inputTerminalConfiguration);
int32 Configure_SingleChannelSynchAIAO(const char inputChan[], const char outputChan[], float64 minIn, float64 minOut, float64 maxIn, float64 maxOut, float64 rateIn, float64 rateOut, TaskHandle *inputTaskHandle, TaskHandle *outputTaskHandle, int inputType, int outputType, unsigned int inputBufferSize, unsigned int outputBufferSize, int inputTerminalConfiguration);
int32 Configure_FullChannelsConfig(const char inputChan[], const char refInputChan[], const char outputChan[], const char trigChan[], int useRefChan, int useTrigChan, int triggerInputWithTrigger, int triggerOutputWithTrigger, float64 minIn, float64 minOut, float64 maxIn, float64 maxOut, float64 rateIn, float64 rateOut, TaskHandle *inputTaskHandle, TaskHandle *outputTaskHandle, TaskHandle *trigTaskHandle, int inputType, int outputType, unsigned int inputBufferSize, unsigned int outputBufferSize, int inputTerminalConfiguration);
int32 Configure_MultiFunctionSynchAIAO(const char inputChan[], const char refInputChan[], const char outputChan[], float64 minIn, float64 minOut, float64 maxIn, float64 maxOut, float64 rateIn, float64 rateOut, TaskHandle *inputTaskHandle, TaskHandle *refInputTaskHandle, TaskHandle *outputTaskHandle, int inputType, int outputType, unsigned int inputBufferSize, unsigned int outputBufferSize, int inputTerminalConfiguration);
int32 ConfigIO(const char inputChan[], const char refInputChan[], const char outputChan[], const char trigChan[], int useRefChan, int useTrig, int useTrigChan, int triggerInputWithTrigger, int triggerOutputWithTrigger, float64 minIn, float64 minOut, float64 maxIn, float64 maxOut, float64 rateIn, float64 rateOut, TaskHandle *inputTaskHandle, TaskHandle *outputTaskHandle, TaskHandle *trigTaskHandle, int inputType, int outputType, unsigned int inputBufferSize, unsigned int outputBufferSize, int inputTerminalConfiguration);
int32 zeroOutputChan(TaskHandle outputTaskHandle, int outputBufferMultiplier, TaskHandle inputTaskHandle, int useRefChan, TaskHandle triggerTaskHandle, int useTrigger, int triggerType, float64 rateIn, float64 rateOut);
int32 ReconfigureChannelClock(float64 rate, TaskHandle taskHandle, int ioType, unsigned int bufferSize);
int32 Write_MultiFunctionSynchAIAO(TaskHandle taskHandle, float64 data[], uInt32 bufferSize);
int32 Read_MultiFunctionSynchAIAO(TaskHandle taskHandle, uInt32 sampsPerChan, float64 data[], uInt32 bufferSize, int32 *read, bool32 fillMode);
int32 Start_MultiFunctionSynchAIAO(TaskHandle taskHandle);
int32 IsDone_MultiFunctionSynchAIAO(TaskHandle taskHandle, bool32 *done);
int32 Stop_MultiFunctionSynchAIAO(TaskHandle taskHandle);
int32 StopAndClear_MultiFunctionSynchAIAO(TaskHandle taskHandle);
int32 setTrigger(TaskHandle task, int triggerType);
int32 resetTrigger(TaskHandle task, int triggerType);

int getAllDevicesInfo(Device **deviceList, int *numDevices);
int getDeviceInfo(char* device, Device *deviceInfo);
double findInputRate(Device inputDevice, Device rInputDevice);
int getChanType(char chanName[]);
int testAgainstType (char *deviceName, char *chanName, int chanCode, int *deviceFound);

#endif
