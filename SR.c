#include <userint.h>
#include "Multi-Function-Synch AI-AO.h"
#include "FileIO.h"
#include "GlobalDefs.h"

//Main panel variables
static int sRespPanelHandle = -1;
static int srRunning = 0;
static int srRequestStop = 0;
static int srRequestQuit = 0;

//Measurment buffer access semaphores.
static BufferControl bufferController;

//Measurement storage data
static double *srFreqData = NULL;
static double *srOutputData = NULL;
static double *srMagData = NULL;
static double *srPhaseData = NULL;
static int srArraySize = 0;
static int srDataPoints = 0;

//Devices data
static Device srInDev;
static Device srOutDev;
//static Device srTrigDev;
static Device srRefDev;

//Function prototypes.
void updateSRPanelInputVoltages();
void updateSRPanelOutputVoltages();
void exitSR();

int CVICALLBACK startStaticResponse (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	char inputChan[CHAN_NAME_LEN];
	char outputChan[CHAN_NAME_LEN];
	char refChan[CHAN_NAME_LEN];
	char trigChan[CHAN_NAME_LEN];
	
	int inputConfig;
	int useTrigChan;
	double inputVoltage;
	int vInFollowsOut;
	double outMinV;
	double outMaxV;
	double freqOut;
	double gain;
	double ratio;
	unsigned short steps;
	int fitFromEnd;
	double settleTime;
	double delayTime;
	int trials;
	
	int error = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			if (srRunning)
				return 0;
			
			//First let's get our panel control values.
			GetCtrlVal(panel, STATIC_RES_INPUT_PHYS_CHANNEL, inputChan);
			GetCtrlVal(panel, STATIC_RES_OUT_PHYS_CHANNEL, outputChan);
			GetCtrlVal(panel, STATIC_RES_R_INPUT_PHYS_CHANNEL, refChan);
			GetCtrlVal(panel, STATIC_RES_TRIG_PHYS_CHANNEL, trigChan);
			GetCtrlVal(panel, STATIC_RES_INPUT_CONFIG, &inputConfig);
			GetCtrlVal(panel, STATIC_RES_USE_TRIG_CHAN, &useTrigChan);
			GetCtrlVal(panel, STATIC_RES_INPUT_VOLTAGE, &inputVoltage);
			GetCtrlVal(panel, STATIC_RES_SIGNAL_START_V, &outMinV);
			GetCtrlVal(panel, STATIC_RES_SIGNAL_STOP_V, &outMaxV);
			GetCtrlVal(panel, STATIC_RES_OUTPUT_FREQ, &freqOut);
			GetCtrlVal(panel, STATIC_RES_STEPS, &steps);
			GetCtrlVal(panel, STATIC_RES_FIT_POS, &fitFromEnd);
			GetCtrlVal(panel, STATIC_RES_SETTLE_TIME, &settleTime);
			GetCtrlVal(panel, STATIC_RES_DELAY_TIME, &delayTime);
			GetCtrlVal(panel, STATIC_RES_TRIALS, &trials);
			
			GetCtrlVal(panel, STATIC_RES_GAIN, &gain);
			GetCtrlVal(panel, STATIC_RES_RATIO, &ratio);
			
			outMinV /= gain;
			outMaxV /= gain;
			inputVoltage /= ratio;
			
			srRequestStop = 0;
			error = getTrace(VOLT_MEASUREMENT, 1, trials, 0, &bufferController, &sRespPanelHandle, STATIC_RES_STATUS_MSG, STATIC_RES_MAG_CHART, STATIC_RES_PHASE_CHART, STATIC_RES_SIGNALS_CHART, STATIC_RES_VOLT_MSG, inputChan, outputChan, refChan, trigChan, 1, useTrigChan, inputConfig, outMinV, outMaxV, 0, steps, freqOut, inputVoltage, outMinV, 0, fitFromEnd, delayTime, settleTime, &srRunning, &srRequestStop, &srFreqData, &srOutputData, &srMagData, &srPhaseData, &srArraySize, &srDataPoints, 1, gain, 1, ratio);
			break;
	}

	return 0;
}

int CVICALLBACK stopStaticResponse (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//First let's let the user know what's going on.
			if (srRunning)
				SetCtrlVal(sRespPanelHandle, STATIC_RES_STATUS_MSG, "Status:  Stopping");
			
			//Then let's stop our instrument.
			srRequestStop = 1;
			break;
	}
	return 0;
}

int CVICALLBACK saveStaticResponse (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	FILE* file;
	char filepath[MAX_PATHNAME_LEN] = "";
	int dialogReturn;
	int i;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//Let's just call the proper function
			saveInstrumentData(srRunning, srDataPoints, srFreqData, srOutputData, srMagData, srPhaseData);
			break;
	}
	return 0;
}

int CVICALLBACK quitStaticResp (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			exitSR();
			break;
	}
	return 0;
}

int CVICALLBACK staticResponsePanelCallback (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			exitSR();
			break;
	}
	return 0;
}

void exitSR()
{
	srRequestStop = 1;
	saveSRSettings(&settingsStruct, sRespPanelHandle);
	DiscardPanel(sRespPanelHandle);
	sRespPanelHandle = -1;
	
	if (srFreqData)
	{
		free (srFreqData);
		srFreqData = NULL;
	}
	
	if (srOutputData)
	{
		free (srOutputData);
		srOutputData = NULL;
	}
	
	if (srMagData)
	{
		free (srMagData);
		srMagData = NULL;
	}
	
	if (srPhaseData)
	{
		free (srPhaseData);
		srPhaseData = NULL;
	}
	
	srArraySize = 0;
	srDataPoints = 0;
	
	DisplayPanel(mainPanelHandle);
}

int CVICALLBACK saveSRPanel (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			saveSRSettings(&settingsStruct, sRespPanelHandle);
			break;
	}
	return 0;
}

int CVICALLBACK updateSRDevices (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	//Declarations
	double inV;
	double outV;
	double maxFreq;
	double temp;
	int inControl;
	int outControl;
	int refControl;
	int trigControl;
	int inVControl;
	int outVControl;
	int freqControl;
	char inputChan[CHAN_NAME_LEN];
	char outputChan[CHAN_NAME_LEN];
	char refChan[CHAN_NAME_LEN];
	char trigChan[CHAN_NAME_LEN];
	int inDevRead = 0;
	int outDevRead = 0;
	int refDevRead = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//First let's set our controls.  This is for cleanliness.
			inControl = STATIC_RES_INPUT_PHYS_CHANNEL;
			outControl = STATIC_RES_OUT_PHYS_CHANNEL;
			refControl = STATIC_RES_R_INPUT_PHYS_CHANNEL;
			trigControl = STATIC_RES_TRIG_PHYS_CHANNEL;
			
			//If an instrument has been updated let's update the UI information so the user can be made aware if their scan settings are invalid.
			//First things first let's get the infomation of all used devices.
			GetCtrlVal(panel, inControl, inputChan);
			GetCtrlVal(panel, outControl, outputChan);
			GetCtrlVal(panel, refControl, refChan);
			GetCtrlVal(panel, trigControl, trigChan);
			
			if (getDeviceInfo(inputChan, &srInDev) == 0)
				inDevRead = 1;
		
			if (getDeviceInfo(outputChan, &srOutDev) == 0)
				outDevRead = 1;
			
			if (getDeviceInfo(refChan, &srRefDev) == 0)
				refDevRead = 1;

			//getDeviceInfo(trigChan, &srTrigDev);
		
			//Let's see which control has been updated.
			//If it's an input control, let's update the input voltage.
			if ((control == inControl) && inDevRead)
			{
				//First let's update our max input voltage voltage.
				updateSRPanelInputVoltages();
			}
			//Otherwise if it's the out let's update the output voltage.
			//For this to work we need to have read in the output device info.
			else if (control == outControl && outDevRead)
			{
				//Let's get our max voltage.
				updateSRPanelOutputVoltages();
			}
			
			//Now let's calculate the minimum and maximum frequency the device can produce/capture.
			if (inDevRead && outDevRead && refDevRead)
			{
				//First let's see what needs to be done regarding the reference channel.
				maxFreq = findInputRate(srInDev, srRefDev);

				//Let's compare the sample frequencies of the input and output devices and select the smaller of fthe two.
				maxFreq = Min(maxFreq, srOutDev. rateOut);
				
				//Now let's divide our max frequency by 8, since that's realistically what we can read in.
				maxFreq /= MIN_SAMPS_PER_CYCLE;
				
				//Now let's update our frequency bounds control boxes accordingly.
				SetCtrlAttribute(panel, STATIC_RES_OUTPUT_FREQ, ATTR_MAX_VALUE, maxFreq);
				GetCtrlVal(panel, STATIC_RES_OUTPUT_FREQ, &temp);
				SetCtrlVal(panel, STATIC_RES_OUTPUT_FREQ, Min(temp, maxFreq));
			}

			saveSRSettings(&settingsStruct, sRespPanelHandle);
			break;
	}
	
	return 0;
}


int CVICALLBACK adjustSRVoltageBounds (int panel, int control, int event,
									   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			updateSRPanelInputVoltages();
			updateSRPanelOutputVoltages();
			saveSRSettings(&settingsStruct, sRespPanelHandle);
			break;
	}
	return 0;
}

void updateSRPanelInputVoltages()
{
	double ratio;
	double minV = 0;
	double maxV = 0;
	double currentV;
	
	if (srInDev.connected)
	{
		//First let's compute our new min and max
		GetCtrlVal(sRespPanelHandle, STATIC_RES_RATIO, &ratio);
		minV = srInDev.inMinV;
		maxV = srInDev.inMaxV;
		minV *= ratio;
		maxV *= ratio;
		
		//Then update the front panel with the new values.
		SetCtrlAttribute(sRespPanelHandle, STATIC_RES_INPUT_VOLTAGE, ATTR_MIN_VALUE, minV);
		SetCtrlAttribute(sRespPanelHandle, STATIC_RES_INPUT_VOLTAGE, ATTR_MIN_VALUE, minV);
	}
	
			//Then update the front panel with the new values.
		GetCtrlAttribute(sRespPanelHandle, STATIC_RES_INPUT_VOLTAGE, ATTR_MIN_VALUE, &minV);
		GetCtrlAttribute(sRespPanelHandle, STATIC_RES_INPUT_VOLTAGE, ATTR_MIN_VALUE, &minV);
		GetCtrlVal(sRespPanelHandle, STATIC_RES_INPUT_VOLTAGE, &currentV);
		currentV = Min(currentV, maxV);
		currentV = Max(currentV, minV);
		SetCtrlVal(sRespPanelHandle, STATIC_RES_INPUT_VOLTAGE, currentV);
}

void updateSRPanelOutputVoltages()
{
	double gain;
	double minV = 0;
	double maxV = 0;
	double currentV;
	
	if (srOutDev.connected)
	{
		//First let's compute our new min and max
		GetCtrlVal(sRespPanelHandle, STATIC_RES_GAIN, &gain);
		minV = srOutDev.outMinV;
		maxV = srOutDev.outMaxV;
		minV *= gain;
		maxV *= gain;
		
		//Then update the front panel with the new values.
		SetCtrlAttribute(sRespPanelHandle, STATIC_RES_SIGNAL_START_V, ATTR_MIN_VALUE, minV);
		SetCtrlAttribute(sRespPanelHandle, STATIC_RES_SIGNAL_STOP_V, ATTR_MIN_VALUE, minV);
		SetCtrlAttribute(sRespPanelHandle, STATIC_RES_SIGNAL_START_V, ATTR_MAX_VALUE, maxV);
		SetCtrlAttribute(sRespPanelHandle, STATIC_RES_SIGNAL_STOP_V, ATTR_MAX_VALUE, maxV);
	}
	
	//Now let's sure the panel values are within bounds.
	GetCtrlAttribute(sRespPanelHandle, STATIC_RES_SIGNAL_START_V, ATTR_MIN_VALUE, &minV);
	GetCtrlAttribute(sRespPanelHandle, STATIC_RES_SIGNAL_START_V, ATTR_MAX_VALUE, &maxV);
	GetCtrlVal(sRespPanelHandle, STATIC_RES_SIGNAL_START_V, &currentV);
	currentV = Min(currentV, maxV);
	currentV = Max(minV, currentV);
	SetCtrlVal(sRespPanelHandle, STATIC_RES_SIGNAL_START_V, currentV);

	GetCtrlAttribute(sRespPanelHandle, STATIC_RES_SIGNAL_STOP_V, ATTR_MIN_VALUE, &minV);
	GetCtrlAttribute(sRespPanelHandle, STATIC_RES_SIGNAL_STOP_V, ATTR_MAX_VALUE, &maxV);
	GetCtrlVal(sRespPanelHandle, STATIC_RES_SIGNAL_STOP_V, &currentV);
	currentV = Min(currentV, maxV);
	currentV = Max(currentV, minV);
	SetCtrlVal(sRespPanelHandle, STATIC_RES_SIGNAL_STOP_V, currentV);
}
