#include <analysis.h>
#include "Multi-Function-Synch AI-AO_Fn.h"  

int32 Configure_SingleChannelDigitalTriggerSynchAIAO(const char inputChan[], const char outputChan[], float64 minIn, float64 minOut, float64 maxIn, float64 maxOut, float64 rateIn, float64 rateOut, TaskHandle *inputTaskHandle, TaskHandle *triggerTaskHandle, TaskHandle *outputTaskHandle, int inputType, unsigned int inputBufferSize, unsigned int outputBufferSize, int inputTerminalConfiguration)
{
	int32 error=0;
	char outTrigger[CHAN_NAME_LEN];
	char outTriggerTemp[CHAN_NAME_LEN];
	char digitalChan[CHAN_NAME_LEN];
	int numBytes;
	char* inputChans;
	int inputChansLength;

/*********************************************************************
*    1. Create a task.
*    2. Create an Analog Input Voltage channel.
*	 3. Create a reference analog input voltage channel.
*    4. Define the Sample Clock source. Additionally, define the sample
*       mode to be continuous.
*********************************************************************/
	DAQmxErrChk (DAQmxCreateTask("Input",inputTaskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan (*inputTaskHandle, inputChan, "", inputTerminalConfiguration, minIn, maxIn,DAQmx_Val_Volts, NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming (*inputTaskHandle, "OnboardClock", rateIn, DAQmx_Val_Rising, inputType, inputBufferSize));

/*********************************************************************
*    1. Create a task.
*    2. Create an Analog Output Voltage channel.
*    3. Define the Sample Clock source. Additionally, define the sample
*       mode to be continuous.
*    4. Define the Triggering parameters: Software triggered by the 
* 		Analog Input Voltage channel.
*********************************************************************/
	DAQmxErrChk (DAQmxCreateTask("Output",outputTaskHandle));
	DAQmxErrChk (DAQmxCreateAOVoltageChan (*outputTaskHandle, outputChan, "", minOut, maxOut, DAQmx_Val_Volts, NULL));
	DAQmxErrChk (DAQmxSetSampTimingType (*outputTaskHandle,DAQmx_Val_OnDemand));
	DAQmxErrChk (DAQmxCfgSampClkTiming (*outputTaskHandle, "OnboardClock", rateOut, DAQmx_Val_Rising, DAQmx_Val_ContSamps, outputBufferSize));
	
	//Next let's set up the digital out channel
	numBytes = FindPattern (inputChan, 0, -1, "/", 0, 0);				
	CopyString (digitalChan, 0, inputChan, 0, numBytes);
	sprintf(digitalChan, "%s/PFI1", digitalChan);
	DAQmxErrChk (DAQmxCreateTask("Trigger", triggerTaskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(*triggerTaskHandle, digitalChan, "", DAQmx_Val_ChanPerLine));
	
	//Next let's set up triggering.
	sprintf(outTrigger,"/%s",digitalChan);
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig (*outputTaskHandle, outTrigger, DAQmx_Val_Rising));
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig (*inputTaskHandle, outTrigger, DAQmx_Val_Rising));
//	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig (*outputTaskHandle, "/

Error:
	return error;
}

int32 Configure_SingleChannelSynchAIAO(const char inputChan[], const char outputChan[], float64 minIn, float64 minOut, float64 maxIn, float64 maxOut, float64 rateIn, float64 rateOut, TaskHandle *inputTaskHandle, TaskHandle *outputTaskHandle, int inputType, int outputType, unsigned int inputBufferSize, unsigned int outputBufferSize, int inputTerminalConfiguration)
{
	int32 error=0;
	char outTrigger[256];
	char outTriggerTemp[256];
	int numBytes;
	char* inputChans;
	int inputChansLength;

/*********************************************************************
*    1. Create a task.
*    2. Create an Analog Input Voltage channel.
*	 3. Create a reference analog input voltage channel.
*    4. Define the Sample Clock source. Additionally, define the sample
*       mode to be continuous.
*********************************************************************/
	DAQmxErrChk (DAQmxCreateTask("Input",inputTaskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan (*inputTaskHandle, inputChan, "", inputTerminalConfiguration, minIn, maxIn,DAQmx_Val_Volts, NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming (*inputTaskHandle, "OnboardClock", rateIn, DAQmx_Val_Rising, inputType, inputBufferSize));

/*********************************************************************
*    1. Create a task.
*    2. Create an Analog Output Voltage channel.
*    3. Define the Sample Clock source. Additionally, define the sample
*       mode to be continuous.
*    4. Define the Triggering parameters: Software triggered by the 
* 		Analog Input Voltage channel.
*********************************************************************/
	DAQmxErrChk (DAQmxCreateTask("Output",outputTaskHandle));
	DAQmxErrChk (DAQmxCreateAOVoltageChan (*outputTaskHandle, outputChan, "", minOut, maxOut, DAQmx_Val_Volts, NULL));
	DAQmxErrChk (DAQmxSetSampTimingType (*outputTaskHandle,DAQmx_Val_OnDemand));
	DAQmxErrChk (DAQmxCfgSampClkTiming (*outputTaskHandle, "OnboardClock", rateOut, DAQmx_Val_Rising, outputType, outputBufferSize));
	
	//Next let's set up triggering.
	numBytes = FindPattern (inputChan, 0, -1, "/", 0, 0);				
	CopyString (outTriggerTemp, 0, inputChan, 0, numBytes);
	sprintf(outTrigger,"/%s/ai/StartTrigger",outTriggerTemp);
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig (*outputTaskHandle, outTrigger, DAQmx_Val_Rising));

Error:
	return error;
}

int32 Configure_FullChannelsConfig(const char inputChan[], const char refInputChan[], const char outputChan[], const char trigChan[], int useRefChan, int useTrigChan, int triggerInputWithTrigger, int triggerOutputWithTrigger, float64 minIn, float64 minOut, float64 maxIn, float64 maxOut, float64 rateIn, float64 rateOut, TaskHandle *inputTaskHandle, TaskHandle *outputTaskHandle, TaskHandle *trigTaskHandle, int inputType, int outputType, unsigned int inputBufferSize, unsigned int outputBufferSize, int inputTerminalConfiguration)
{
	int channelType;
	int32 error=0;
	char inputChans[2 * CHAN_NAME_LEN] = "";
	char outputChans[2 * CHAN_NAME_LEN] = "";
	char outTrigger[CHAN_NAME_LEN] = "";
	char outTriggerTemp[CHAN_NAME_LEN]  ="";
	int numBytes;
	int inputChansLength;
	
	///First let's build the input channels string
	strcat(inputChans, inputChan);
	strcat(outputChans, outputChan);
	
	if (useRefChan)
	{
		strcat(inputChans, ", ");
		strcat(inputChans, refInputChan);
	}
	
/*********************************************************************
*    1. Create a task.
*    2. Create an Analog Input Voltage channel.
*	 3. Create a reference analog input voltage channel.
*    4. Define the Sample Clock source. Additionally, define the sample
*       mode to be continuous.
*********************************************************************/
	DAQmxErrChk (DAQmxCreateTask("Input",inputTaskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan (*inputTaskHandle, inputChans, "", inputTerminalConfiguration, minIn, maxIn,DAQmx_Val_Volts, NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming (*inputTaskHandle, "OnboardClock", rateIn, DAQmx_Val_Rising, inputType, inputBufferSize));

/*********************************************************************
*    1. Create a task.
*    2. Create an Analog Output Voltage channel.
*    3. Define the Sample Clock source. Additionally, define the sample
*       mode to be continuous.
*    4. Define the Triggering parameters: Software triggered by the 
* 		Analog Input Voltage channel.
*********************************************************************/
	DAQmxErrChk (DAQmxCreateTask("Output",outputTaskHandle));

	channelType = getChanType(trigChan);
	if (useTrigChan && channelType == ANALOG_CHAN)
	{
		strcat(outputChans, ", ");
		strcat(outputChans, trigChan);
	}
	
	DAQmxErrChk (DAQmxCreateAOVoltageChan (*outputTaskHandle, outputChans, "", minOut, maxOut, DAQmx_Val_Volts, NULL));
	DAQmxErrChk (DAQmxSetSampTimingType (*outputTaskHandle,DAQmx_Val_OnDemand));
	DAQmxErrChk (DAQmxCfgSampClkTiming (*outputTaskHandle, "OnboardClock", rateOut, DAQmx_Val_Rising, outputType, outputBufferSize));
	
	//Next let's set up triggering for the channel.  If we're using an external trigger channel, let's set it up accordingly.  If not, we use the analog in as a trigger.
	//Either way we're going to have to have a trigger set up.
	if (useTrigChan)
	{
		//Our configuration from here will be different based on wether the trigger channel is analog or digital.
		if (channelType == ANALOG_CHAN)
		{
			//If we're going to use an analog trigger, we're going to do it by generating a square wave with some duty cycle 0<D<1 and will use the rising edge as a trigger.
			//Let's simply configure another output.
			//It is on the user to produce the square wave.
			//strcat(outputChans, ", ");
			//strcat(trigChan);
			
			/*//If it's analog there's more work that we need to do.  Than if we were in digital.
			//Let's just set up another output channel.
			DAQmxErrChk (DAQmxCreateAOVoltageChan (*trigTaskHandle, trigChan, "", 0.0, 1.0, DAQmx_Val_Volts, NULL));
			DAQmxErrChk (DAQmxSetSampTimingType (*trigTaskHandle,DAQmx_Val_OnDemand));
			DAQmxErrChk (DAQmxCfgSampClkTiming (*trigTaskHandle, "OnboardClock", rateOut, DAQmx_Val_Rising, outputType, outputBufferSize));
			
			//Let's set our output and input channel triggers to the analog channel.
			/*numBytes = FindPattern (trigChan, 0, -1, "/", 0, 0);				
			CopyString (outTriggerTemp, 0, trigChan, 0, numBytes);
			sprintf(outTrigger,"/%s/ao/StartTrigger",outTriggerTemp);*/
			
			//This will be useful for the static response measurements, where you want to trigger the input channel but the output runs continuously.
			if (triggerInputWithTrigger)
			{
				numBytes = FindPattern (inputChan, 0, -1, "/", 0, 0);				
				CopyString (outTriggerTemp, 0, inputChan, 0, numBytes);
				sprintf(outTrigger,"/%s/ao/StartTrigger",outTriggerTemp);
				DAQmxErrChk (DAQmxCfgDigEdgeStartTrig (*inputTaskHandle, outTrigger, DAQmx_Val_Rising));			
			}
		}
		else
		{
			//Otherwise let's set up a digital output channel.
			//Observe that even if we get a chan doesn't exist constant we should just execute this branch.
			//It might be a PFI, which we can't test for.
			//If it's not this will just fail, which is just fine for us.
			//First let's set up our trigger channel as another output channel.
			DAQmxErrChk (DAQmxCreateTask("Trigger", trigTaskHandle));
			DAQmxErrChk (DAQmxCreateDOChan(*trigTaskHandle, trigChan, "", DAQmx_Val_ChanForAllLines));
			
			if (triggerInputWithTrigger)
				DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(*inputTaskHandle, trigChan, DAQmx_Val_Rising));
		
			if (triggerOutputWithTrigger)
				;//DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(*outputTaskHandle, trigChan, DAQmx_Val_Rising));
		}
	}
	//Otherwise let's use the analog input channel as a trigger.
	else
	{
		numBytes = FindPattern (inputChan, 0, -1, "/", 0, 0);				
		CopyString (outTriggerTemp, 0, inputChan, 0, numBytes);
		sprintf(outTrigger,"/%s/ai/StartTrigger",outTriggerTemp);
		DAQmxErrChk (DAQmxCfgDigEdgeStartTrig (*outputTaskHandle, outTrigger, DAQmx_Val_Rising));
	}

Error:
	return error;
}

int32 ConfigIO(const char inputChan[], const char refInputChan[], const char outputChan[], const char trigChan[], int useRefChan, int useTrig, int useTrigChan, int triggerInputWithTrigger, int triggerOutputWithTrigger, float64 minIn, float64 minOut, float64 maxIn, float64 maxOut, float64 rateIn, float64 rateOut, TaskHandle *inputTaskHandle, TaskHandle *outputTaskHandle, TaskHandle *trigTaskHandle, int inputType, int outputType, unsigned int inputBufferSize, unsigned int outputBufferSize, int inputTerminalConfiguration)
{
	int channelType;
	int32 error=0;
	char inputChans[2 * CHAN_NAME_LEN] = "";
	char outputChans[2 * CHAN_NAME_LEN] = "";
	char outTrigger[CHAN_NAME_LEN] = "";
	char outTriggerTemp[CHAN_NAME_LEN]  ="";
	int numBytes;
	int inputChansLength;
	
	///First let's build the input channels string
	strcat(inputChans, inputChan);
	strcat(outputChans, outputChan);
	
	if (useRefChan)
	{
		strcat(inputChans, ", ");
		strcat(inputChans, refInputChan);
	}
	
/*********************************************************************
*    1. Create a task.
*    2. Create an Analog Input Voltage channel.
*	 3. Create a reference analog input voltage channel.
*    4. Define the Sample Clock source and sample mode.
*********************************************************************/
	DAQmxErrChk (DAQmxCreateTask("Input",inputTaskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan (*inputTaskHandle, inputChans, "", inputTerminalConfiguration, minIn, maxIn, DAQmx_Val_Volts, NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming (*inputTaskHandle, "OnboardClock", rateIn, DAQmx_Val_Rising, inputType, inputBufferSize));

/*********************************************************************
*    1. Create a task.
*    2. Create an Analog Output Voltage channel.
*    3. Define the Sample Clock source. Additionally, define the sample
*       mode to be continuous.
*    4. Define the Triggering parameters: Software triggered by the 
* 		Analog Input Voltage channel.
*********************************************************************/
	DAQmxErrChk (DAQmxCreateTask("Output",outputTaskHandle));
	DAQmxErrChk (DAQmxCreateAOVoltageChan (*outputTaskHandle, outputChans, "", minOut, maxOut, DAQmx_Val_Volts, NULL));
	DAQmxErrChk (DAQmxSetSampTimingType (*outputTaskHandle,DAQmx_Val_OnDemand));
	DAQmxErrChk (DAQmxCfgSampClkTiming (*outputTaskHandle, "OnboardClock", rateOut, DAQmx_Val_Rising, outputType, outputBufferSize));
	
	//Next let's set up triggering for the channel.  If we're using an external trigger channel, let's set it up accordingly.  If not, we use the analog in as a trigger.
	//Either way we're going to have to have a trigger set up.
	if (useTrig)
	{
		if (useTrigChan)
		{
			//If we're using triggering and are using an external channel, let's set up a digital output channel.
			DAQmxErrChk (DAQmxCreateTask("Trigger", trigTaskHandle));
			DAQmxErrChk (DAQmxCreateDOChan(*trigTaskHandle, trigChan, "", DAQmx_Val_ChanForAllLines));
			DAQmxErrChk (DAQmxSetSampTimingType (*trigTaskHandle,DAQmx_Val_OnDemand));
			sprintf(outTrigger, "/%s", trigChan);

			if (triggerInputWithTrigger)
				DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(*inputTaskHandle, outTrigger, DAQmx_Val_Rising));

			if (triggerOutputWithTrigger)
				DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(*outputTaskHandle, outTrigger, DAQmx_Val_Rising));
		}
		//Otherwise let's use one of the analog channels as a trigger.
		else
		{
			if (triggerOutputWithTrigger)
			{
				numBytes = FindPattern (inputChan, 0, -1, "/", 0, 0);				
				CopyString (outTriggerTemp, 0, inputChan, 0, numBytes);
				sprintf(outTrigger,"/%s/ai/StartTrigger",outTriggerTemp);
				DAQmxErrChk (DAQmxCfgDigEdgeStartTrig (*outputTaskHandle, outTrigger, DAQmx_Val_Rising));
			}
			else if (triggerInputWithTrigger)
			{
				numBytes = FindPattern (outputChan, 0, -1, "/", 0, 0);				
				CopyString (outTriggerTemp, 0, outputChan, 0, numBytes);
				sprintf(outTrigger,"/%s/ao/StartTrigger",outTriggerTemp);
				DAQmxErrChk (DAQmxCfgDigEdgeStartTrig (*inputTaskHandle, outTrigger, DAQmx_Val_Rising));
			}
				
		}
	}
	
Error:
	return error;
}

int32 Configure_MultiFunctionSynchAIAO(const char inputChan[], const char refInputChan[], const char outputChan[], float64 minIn, float64 minOut, float64 maxIn, float64 maxOut, float64 rateIn, float64 rateOut, TaskHandle *inputTaskHandle, TaskHandle *refInputTaskHandle, TaskHandle *outputTaskHandle, int inputType, int outputType, unsigned int inputBufferSize, unsigned int outputBufferSize, int inputTerminalConfiguration)
{
	int32 error=0;
	char outTrigger[256];
	char outTriggerTemp[256];
	int numBytes;
	char* inputChans = NULL;
	int inputChansLength;
	
	///First let's build the input channels string
	inputChansLength = strlen(inputChan) + strlen(refInputChan) + 3;
	inputChans = (char*) malloc(inputChansLength * sizeof(char));
	
	if (!inputChans)
		return -1;
	
	inputChans[0] = '\0';
	
	strcat(inputChans, inputChan);
	strcat(inputChans, ", ");
	strcat(inputChans, refInputChan);

/*********************************************************************
*    1. Create a task.
*    2. Create an Analog Input Voltage channel.
*	 3. Create a reference analog input voltage channel.
*    4. Define the Sample Clock source. Additionally, define the sample
*       mode to be continuous.
*********************************************************************/
	DAQmxErrChk (DAQmxCreateTask("Input",inputTaskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan (*inputTaskHandle, inputChans, "", inputTerminalConfiguration, minIn, maxIn,DAQmx_Val_Volts, NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming (*inputTaskHandle, "OnboardClock", rateIn, DAQmx_Val_Rising, inputType, inputBufferSize));

/*********************************************************************
*    1. Create a task.
*    2. Create an Analog Output Voltage channel.
*    3. Define the Sample Clock source. Additionally, define the sample
*       mode to be continuous.
*    4. Define the Triggering parameters: Software triggered by the 
* 		Analog Input Voltage channel.
*********************************************************************/
	DAQmxErrChk (DAQmxCreateTask("Output",outputTaskHandle));
	DAQmxErrChk (DAQmxCreateAOVoltageChan (*outputTaskHandle, outputChan, "", minOut, maxOut, DAQmx_Val_Volts, NULL));
	DAQmxErrChk (DAQmxSetSampTimingType (*outputTaskHandle,DAQmx_Val_OnDemand));
	DAQmxErrChk (DAQmxCfgSampClkTiming (*outputTaskHandle, "OnboardClock", rateOut, DAQmx_Val_Rising, outputType, outputBufferSize));
	
	numBytes = FindPattern (inputChan, 0, -1, "/", 0, 0);				
	CopyString (outTriggerTemp, 0, inputChan, 0, numBytes);
	sprintf(outTrigger,"/%s/ai/StartTrigger",outTriggerTemp);
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig (*outputTaskHandle, outTrigger, DAQmx_Val_Rising));

Error:
	if (inputChans)
		free(inputChans);
	return error;
}

int32 zeroOutputChan(TaskHandle outputTaskHandle, int outputBufferMultiplier, TaskHandle inputTaskHandle, int useRefChan, TaskHandle triggerTaskHandle, int useTrigger, int triggerType, float64 rateIn, float64 rateOut)
{
	//Declarations
	int32 error;
	int inputType = DAQmx_Val_FiniteSamps;
	int outputType = DAQmx_Val_FiniteSamps;
	float64 zeroOutBuffer[outputBufferMultiplier * 2];
	float64 zeroInBuffer[16];
	int numZerosRead;
	
	//First let's build our output buffer.
	for (int i = 0; i < outputBufferMultiplier * 2; i++)
		zeroOutBuffer[i] = 0.0;
	
	//First let's stop our task and clear the buffer updating function as a registered event.
	DAQmxErrChk (Stop_MultiFunctionSynchAIAO(outputTaskHandle));
	DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(outputTaskHandle, DAQmx_Val_Transferred_From_Buffer, 2, 0, NULL, NULL));

	//Next let's reconfigure our input/output channels to read/write only two samples.
	DAQmxErrChk (ReconfigureChannelClock(rateIn, inputTaskHandle, inputType, 2));
	DAQmxErrChk (ReconfigureChannelClock(rateOut, outputTaskHandle, outputType, 2));
	DAQmxErrChk (Write_MultiFunctionSynchAIAO(outputTaskHandle, zeroOutBuffer, 2));
	DAQmxErrChk (Start_MultiFunctionSynchAIAO(outputTaskHandle));
	DAQmxErrChk (Start_MultiFunctionSynchAIAO(inputTaskHandle));

	//Now let's fire up our trigger.
	if (useTrigger)
		DAQmxErrChk (setTrigger(triggerTaskHandle, triggerType));

	//Let's fire up our input just in case our output depends on our input.
	DAQmxErrChk (Read_MultiFunctionSynchAIAO(inputTaskHandle, 2, zeroInBuffer, 16, &numZerosRead, DAQmx_Val_GroupByChannel));
	DAQmxErrChk (Stop_MultiFunctionSynchAIAO(inputTaskHandle));
	DAQmxErrChk (Stop_MultiFunctionSynchAIAO(outputTaskHandle));

	//Now we reset our trigger and we're done.
	if (useTrigger)
		DAQmxErrChk (resetTrigger(triggerTaskHandle, triggerType));

Error:
	return error;
}

int32 ReconfigureChannelClock(float64 rate, TaskHandle taskHandle, int ioType, unsigned int bufferSize)
{
	return DAQmxCfgSampClkTiming (taskHandle, "OnboardClock", rate, DAQmx_Val_Rising, ioType, bufferSize);	
}

// Writes data from user allocated buffer.
// Recommended parameters:
//   bufferSize     = 1000
int32 Write_MultiFunctionSynchAIAO(TaskHandle taskHandle, float64 data[], uInt32 bufferSize)
{
	int32 written;
	return DAQmxWriteAnalogF64 (taskHandle, bufferSize, 0, 10.0, DAQmx_Val_GroupByChannel, data, &written, NULL);
}

// Reads data into user allocated buffer.
// Recommended parameters:
//   bufferSize     = 1000
int32 Read_MultiFunctionSynchAIAO(TaskHandle taskHandle, uInt32 sampsPerChan, float64 data[], uInt32 bufferSize, int32 *read, bool32 fillMode)
{
	//DAQmx_Val_GroupByScanNumber
	return DAQmxReadAnalogF64 (taskHandle, sampsPerChan, -1, fillMode, data, bufferSize, read, NULL);
}

// Starts the task.
int32 Start_MultiFunctionSynchAIAO(TaskHandle taskHandle)
{
	return DAQmxStartTask (taskHandle);
}

// Checks whether the task is done.
int32 IsDone_MultiFunctionSynchAIAO(TaskHandle taskHandle, bool32 *done)
{
	return DAQmxIsTaskDone(taskHandle,done);
}

// Stops the task.
int32 Stop_MultiFunctionSynchAIAO(TaskHandle taskHandle)
{
	int32	error=0;

	error = DAQmxStopTask(taskHandle);
	return error;
}

// Stops and clears the task.
int32 StopAndClear_MultiFunctionSynchAIAO(TaskHandle taskHandle)
{
	int32	error=0;

	error = DAQmxStopTask(taskHandle);
	error = DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Transferred_From_Buffer, 1, 0, NULL, NULL); 
	DAQmxClearTask(taskHandle);
	return error;
}

//Sets the trigger channel.
int32 setTrigger(TaskHandle task, int triggerType)
{
	int32 error = 0;
	bool32 done = 0;
	
	if (triggerType == ANALOG_CHAN)
	{
		;//DAQmxErrChk (DAQmxWriteAnalogScalarF64(task, 0, -1, TRIGGER_LEVEL, NULL));
	}
	else
	{
		DAQmxErrChk (Start_MultiFunctionSynchAIAO(task));
		DAQmxErrChk (DAQmxWriteDigitalScalarU32(task, 0, -1, 1, NULL));
		DAQmxErrChk (Stop_MultiFunctionSynchAIAO(task));
	}
	
Error:
	if (task != 0)
		Stop_MultiFunctionSynchAIAO(task);
	
	return error;
}

int32 resetTrigger(TaskHandle task, int triggerType)
{
	int32 error = 0;
	bool32 done = 0;
	
	if (triggerType == ANALOG_CHAN)
	{
		;//DAQmxErrChk (DAQmxWriteAnalogScalarF64(task, 0, -1, 0.0, NULL));
	}
	else
	{
		DAQmxErrChk (Start_MultiFunctionSynchAIAO(task));
		DAQmxErrChk (DAQmxWriteDigitalScalarU32(task, 0, -1, 1, NULL));
		DAQmxErrChk (Stop_MultiFunctionSynchAIAO(task));
	}
	
Error:
	if (task != 0)
		Stop_MultiFunctionSynchAIAO(task);
	
	return error;
}

int getAllDevicesInfo(Device **deviceList, int *numDevices)
{
	int arraySize;
	char *devicesString;
	char *deviceToken;
	int error;
	int newError;
	int i;
	
	//First let's see if the user passed everything in.
	if(deviceList == NULL || numDevices == NULL)
		return -1;
	
	//First let's get our array size
	*numDevices = 0;
	arraySize = DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, NULL);

	//If there are no connected devices let's exit.
	if (arraySize == 0)
	{
		return 0;
	}

	//If there are let's allocate our devices string and move on.
	devicesString = (char*) malloc(arraySize * sizeof(char));

	//If we couldn't allocate our string let's exit.
	if (!devicesString)
	{
		return -1;
	}

	//If we could let's get the names.
	error = DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, devicesString, arraySize);

	//If we didn't succeed in getting our names let's free our devcies string and exit.
	if (error < 0)
	{
		free(devicesString);
		return error;
	}

	//Now let's see how many devices we have.
	strtok(devicesString, ", ");
	*numDevices = 1;
	while ((deviceToken = strtok(NULL, ", ")) != NULL)
		(*numDevices)++;

	//And allocate our devices list accordingly.
	*deviceList = (Device*) malloc(*numDevices * sizeof(Device));

	//Let's make sure our device list has properly allocated.
	if (!(*deviceList))
	{
		free(devicesString);
		return -1;
	}

	//Now for each device in the list let's get it's properties.
	deviceToken = strtok(devicesString, ", ");
	i = 0;
	error = 0;

	do
	{
		newError = getDeviceInfo(deviceToken, &(*deviceList[i]));
		deviceToken = strtok(NULL, ", ");
		i++;

		if (newError)
			error = newError;
	}
	while(deviceToken);

	//Next let's free unused variables.
	free(devicesString);

	return error;
}	

int getDeviceInfo(char* device, Device *deviceInfo)
{
	char formattedDeviceName[CHAN_NAME_LEN];
	char* sPtr = NULL;
	uInt32 serial;
	int returnVal;
	int newReturnVal;
	int arraySize;
	ssize_t maxIndex;
	ssize_t minIndex;
	float64* voltages = NULL;
	char* devices = NULL;
	
	//If we have an invalid device let's just quit.
	if (device == NULL || deviceInfo == NULL || strcmp(device, "") == 0)
		return -1;
	
	//Let's copy the string over for safe manipulation.
	strcpy(formattedDeviceName, device);
	
	//Now let's see if our device string has a '/' character in it.
	sPtr = strstr(formattedDeviceName, "/");
	
	if (sPtr)
	{
		//If it does let's get rid of it and end the string in its place.
		sPtr[0] = '\0';
	}

	//Now let's populate the device info struct.
	//Let's first copy over the device location as defined by NI MAX.
	returnVal = 0;
	strcpy(deviceInfo->location, formattedDeviceName);
	
	//First things first let's see if this device is even connected. If it's not we can't get any informataion about it down the road.
	deviceInfo->connected = 0;
	arraySize = DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, NULL);
	
	if (arraySize > 0)
		devices = (char*) malloc(arraySize * sizeof(char));
	
	//We need to make sure our array was allocated.
	if (devices)
	{
		//If it was let's get the devices connected to our system.
		DAQmxGetSystemInfoAttribute(DAQmx_Sys_DevNames, devices, arraySize);
		
		if(strstr(devices, formattedDeviceName))
			deviceInfo->connected = 1;
		
		free(devices);
	}
	
	//If the device isn't connected let's init all our variables to 0 and get out of here.
	if (!deviceInfo->connected)
	{
		strcpy(deviceInfo->name, "\0");
		deviceInfo->inMaxV = -1;
		deviceInfo->inMinV = -1;
		deviceInfo->outMaxV = -1;
		deviceInfo->outMinV = -1;
		deviceInfo->rateIn = -1;
		deviceInfo->rateOut = -1;
		deviceInfo->sharedRateIn = -1;
		
		return -1;
	}
	
	//If we're here then the device is connected.  Next we'll get the device name.
	if((newReturnVal = DAQmxGetDeviceAttribute(formattedDeviceName, DAQmx_Dev_ProductType, deviceInfo->name, CHAN_NAME_LEN)) < 0)
		returnVal = newReturnVal;

	//Next let's get the serial number.
	if ((newReturnVal = DAQmxGetDeviceAttribute(formattedDeviceName, DAQmx_Dev_SerialNum, &(deviceInfo->serial))) < 0)
	{
		deviceInfo->serial = -1;
		returnVal = newReturnVal;
	}
	
	//Next is the max input sample rate.
	if((newReturnVal = DAQmxGetDeviceAttribute(formattedDeviceName, DAQmx_Dev_AI_MaxSingleChanRate, &(deviceInfo->rateIn))) < 0)
	{
		deviceInfo->rateIn = -1;
		deviceInfo->sharedRateIn = -1;
		returnVal = newReturnVal;
	}
	else
	{
		deviceInfo->sharedRateIn = deviceInfo->rateIn / 2.0;
	}
	
	//Then the max output sample rate.
	if((newReturnVal = DAQmxGetDeviceAttribute(formattedDeviceName, DAQmx_Dev_AO_MaxRate, &(deviceInfo->rateOut))) < 0)
	{
		deviceInfo->rateOut =-1;
		returnVal = newReturnVal;
	}
	
	//Next let's get the min/max input voltages.
	arraySize = DAQmxGetDeviceAttribute(formattedDeviceName, DAQmx_Dev_AI_VoltageRngs, NULL);
	
	if (arraySize > 0)
		voltages = (float64*) malloc(arraySize * sizeof(float64));
	
	if (voltages)
	{
		if((newReturnVal = DAQmxGetDeviceAttribute(formattedDeviceName, DAQmx_Dev_AI_VoltageRngs, voltages, arraySize)) < 0)
		{
			deviceInfo->inMaxV = deviceInfo->inMinV = -1;
			returnVal = newReturnVal;
		}
		else
		{
			MaxMin1D (voltages, arraySize, &(deviceInfo->inMaxV), &maxIndex, &(deviceInfo->inMinV), &minIndex);
		}
		
		free(voltages);
	}
	else
	{
		deviceInfo->inMaxV = deviceInfo->inMinV = -1;
		returnVal = -1;
	}
	
	//Finally let's get the min/max output voltages.
	arraySize = DAQmxGetDeviceAttribute(formattedDeviceName, DAQmx_Dev_AO_VoltageRngs, NULL);
	
	if (arraySize > 0)
		voltages = (float64*) malloc(arraySize * sizeof(float64));

	if (voltages)
	{
		if((newReturnVal = DAQmxGetDeviceAttribute(formattedDeviceName, DAQmx_Dev_AO_VoltageRngs, voltages, arraySize)) < 0)
		{
			deviceInfo->outMaxV = deviceInfo->outMinV = -1;
			returnVal = newReturnVal;
		}
		else
		{
			MaxMin1D(voltages, arraySize, &(deviceInfo->outMaxV), &maxIndex, &(deviceInfo->outMinV), &minIndex);
		}

		free(voltages);
	}
	else
	{
		deviceInfo->outMaxV = deviceInfo->outMinV = -1;
		returnVal = -1;
	}
	
	//If all our reading succeeded we'll return a 0.  If not, we'll return a negative value.
	return returnVal;
}

double findInputRate(Device inputDevice, Device rInputDevice)
{
	double rate;

	//If our input and reference input devices are different, let's just select the minimum of the two so both can collect data.
	if (strcmp(inputDevice.location, rInputDevice.location))
	{
		if (inputDevice.rateIn <= rInputDevice.rateIn)
			rate = inputDevice.rateIn;
		else
			rate = rInputDevice.rateIn;
	}
	//Otherwise let's use the shared input rate of the device.
	else
	{
		rate = inputDevice.sharedRateIn;
	}
	
	return rate;
}

int getChanType(char chanName[])
{
	//Declarations
	int error = 0;
	char deviceName[CHAN_NAME_LEN];
	char chan[CHAN_NAME_LEN] = "";
	char *devicePointer = NULL;
	int deviceFound;
	
	if (!chanName)
		return -1;
	
	//First let's figure out the device name
	strcpy(deviceName, chanName);
	devicePointer = strstr(deviceName,"/");
	
	//Let's make sure the '/' character was found.
	//If it hasn't the format passed in is wrong.  Let's get out of here.
	if (devicePointer)
	{
		strcpy(chan, devicePointer + 1);
		devicePointer[0] = '\0';
	}
	else
	{
		return CHAN_DNE;
	}
	
	//If we're here let's look up our channel in our device and see if it's an analog or a digital.
	DAQmxErrChk(testAgainstType(deviceName, chan, DAQmx_Dev_AI_PhysicalChans, &deviceFound));
	if (deviceFound)
		return ANALOG_CHAN;
		
	DAQmxErrChk(testAgainstType(deviceName, chan, DAQmx_Dev_AO_PhysicalChans, &deviceFound));
	if (deviceFound)
		return ANALOG_CHAN;

	DAQmxErrChk(testAgainstType(deviceName, chan, DAQmx_Dev_DI_Lines, &deviceFound));
	if (deviceFound)
		return DIGITAL_CHAN;
		
	DAQmxErrChk(testAgainstType(deviceName, chan, DAQmx_Dev_DI_Ports, &deviceFound));
	if (deviceFound)
		return DIGITAL_CHAN;
	
	DAQmxErrChk(testAgainstType(deviceName, chan, DAQmx_Dev_DO_Lines, &deviceFound));
	if (deviceFound)
		return DIGITAL_CHAN;
		
	DAQmxErrChk(testAgainstType(deviceName, chan, DAQmx_Dev_DO_Ports, &deviceFound));
	if (deviceFound)
		return DIGITAL_CHAN;
	
	//Can we test for PFI channels here too?
	 
Error:
	return CHAN_DNE;
}

int testAgainstType (char *deviceName, char *chanName, int chanCode, int *deviceFound)
{
	char *chansOnDevice = NULL;
	char *devicePointer = NULL;
	int error = 0;
	
	if (!deviceName || !chanName || !deviceFound)
		return -1;
	
	//First let's set up our device found variable.
	*deviceFound = 0;
	
	//First let's get the number of byte that we need to get all our analog input channel.
	int bufferSize = DAQmxGetDeviceAttribute(deviceName, chanCode, NULL);
	
	//If our buffer size is greater than 0 let's try to allocate the memory.
	//If not, let's skip it, we have no analog input channels.
	if (bufferSize > 0)
	{
		//Allocate our memory
		chansOnDevice = (char*) malloc(bufferSize * sizeof(char));
	
		//If our device has been allocated let's see if it's an analog channel.
		if (chansOnDevice)
		{
			//Let's get our list of analog input devices.
			DAQmxErrChk(DAQmxGetDeviceAttribute(deviceName, chanCode, chansOnDevice, bufferSize));
			
			//Let's see if our channel is in the string we just got back.
			devicePointer = strstr(chansOnDevice, chanName);

			//If we found it, that is, if  our device pointer isn't null, let's let the caller know that we did.
			if (devicePointer)
				*deviceFound = 1;
		}
		else
		{
			//If we're here it means that we couldn't allocate the memory for our buffer.  Let's exit since we don't know if our chan is an analog one or digital.
			return -1;
		}
	}
	

Error:
	if (chansOnDevice)
		free(chansOnDevice);
	
	return error;
}
