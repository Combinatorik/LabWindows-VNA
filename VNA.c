#include "Multi-Function-Synch AI-AO.h"
#include "FileIO.h"
#include <userint.h>

static int vnaPanelHandle = -1;
static int vnaRunning = 0;
static int vnaRequestStop = 0;
static int vnaRequestQuit = 0;

static double *vnaFreqData = NULL;
static double *vnaOutputData = NULL;
static double *vnaMagData = NULL;
static double *vnaPhaseData = NULL;
static int vnaArraySize = 0;
static int vnaDataPoints = 0;

static Device vnaInDev;
static Device vnaOutDev;
static Device vnaRefDev;
//static Device vnaTrigDev;

//Function prototypes.
void exitVNA();
void updateVNAPanelOutputVoltages();
void updateVNAPanelInputVoltages();

int CVICALLBACK saveVNADataCallback (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	FILE* file;
	char filepath[MAX_PATHNAME_LEN] = "";
	int dialogReturn;
	int i;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//Let's just call save data function.
			saveInstrumentData(vnaRunning, vnaDataPoints, vnaFreqData, vnaOutputData, vnaMagData, vnaPhaseData);
			break;
	}
	
	return 0;
}

int CVICALLBACK vnaPanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			exitVNA();
			break;
	}
	return 0;
}


int CVICALLBACK quitVNACallback (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			exitVNA();
			break;
	}

	return 0;
}

void exitVNA()
{
	//Let's stop the VNA
	vnaRequestStop = 1;

	//Next let's save the settings.
	saveVNASettings(&settingsStruct, vnaPanelHandle);
	
	//Then get rid of the panel.
	DiscardPanel(vnaPanelHandle);
	vnaPanelHandle = -1;

	//Finally let's deallocate any arrays that have been allocated.
	if (vnaFreqData)
	{
		free(vnaFreqData);
		vnaFreqData = NULL;
	}

	if (vnaOutputData)
	{
		free(vnaOutputData);
		vnaOutputData = NULL;
	}
	
	if (vnaMagData)
	{
		free(vnaMagData);
		vnaMagData = NULL;
	}

	if (vnaPhaseData)
	{
		free(vnaPhaseData);
		vnaPhaseData = NULL;
	}
	
	vnaArraySize = 0;
	vnaDataPoints = 0;
	
	//Now let's diaplay the main menu again.
	DisplayPanel(mainPanelHandle);
}


int CVICALLBACK startVNACallback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	//Declarations
	double startFreq;
	double stopFreq;
	double inputVoltage;
	double outputVoltage;
	double gain;
	double ratio;
	int stepType;
	unsigned short stepsPerUnit;
	int totalSteps = 0;
	char inputChan[CHAN_NAME_LEN];
	char outputChan[CHAN_NAME_LEN];
	char refChan[CHAN_NAME_LEN];
	char trigChan[CHAN_NAME_LEN];
	int useRefChan;
	int useTrigChan;
	int32 error = 0;
	int inputConfigType;
	char errBuff[2048]={'\0'};
	int fitStartPos;
	double delayTime;
	unsigned short numTrials;
				
	switch (event)
	{
		case EVENT_COMMIT:
			//Let's make sure the VNA isn't running.  If it is let's just quit.
			if (vnaRunning)
				return -1;
			
			//Before we begin let's update our status message.
			SetCtrlVal(panel, VNA_STATUS_MSG, "Status:  Initializing");
			
			//First things first, let's get our inputs.
			GetCtrlVal(panel, VNA_SIGNAL_START_FREQ, &startFreq);
			GetCtrlVal(panel, VNA_SIGNAL_STOP_FREQ, &stopFreq);
			GetCtrlVal(panel, VNA_SIG_TYPE, &stepType);
			GetCtrlVal(panel, VNA_STEPS, &stepsPerUnit);
			GetCtrlVal(panel, VNA_OUTPUT_VOLTAGE, &outputVoltage);
			GetCtrlVal(panel, VNA_INPUT_VOLTAGE, &inputVoltage);
			GetCtrlVal(panel, VNA_GAIN, &gain);
			GetCtrlVal(panel, VNA_RATIO, &ratio);
			GetCtrlVal(panel, VNA_AVERAGES, &numTrials);
			
			GetCtrlVal(panel, VNA_INPUT_PHYS_CHANNEL, inputChan);
			GetCtrlVal(panel, VNA_R_INPUT_PHYS_CHANNEL, refChan);
			GetCtrlVal(panel, VNA_OUT_PHYS_CHANNEL, outputChan);
			GetCtrlVal(panel, VNA_R_INPUT_PHYS_CHANNEL, refChan);
			GetCtrlVal(panel, VNA_USE_REF_CHAN, &useRefChan);
			GetCtrlVal(panel, VNA_TRIG_PHYS_CHANNEL, trigChan);
			GetCtrlVal(panel, VNA_USE_TRIG_CHAN, &useTrigChan);
			GetCtrlVal(panel, VNA_INPUT_CONFIG, &inputConfigType);
			
			GetCtrlVal(panel, VNA_DELAY_TIME, &delayTime);
			GetCtrlVal(panel, VNA_FIT_POS, &fitStartPos);
			
			outputVoltage /= gain;
			inputVoltage /= ratio;
			
			vnaRequestStop = 0;
			error = getTrace(FREQ_MEASUREMENT, 1, numTrials, 0, 0, &vnaPanelHandle, VNA_STATUS_MSG, VNA_MAG_CHART, VNA_PHASE_CHART, VNA_SIGNALS_CHART, VNA_FREQ_MSG, inputChan, outputChan, refChan, trigChan, useRefChan, useTrigChan, inputConfigType, startFreq, stopFreq, stepType, stepsPerUnit, startFreq, inputVoltage, outputVoltage, 0, fitStartPos, delayTime, 0, &vnaRunning, &vnaRequestStop, &vnaFreqData, &vnaOutputData, &vnaMagData, &vnaPhaseData, &vnaArraySize, &vnaDataPoints, 1, gain, 1, ratio);
			break;
	}
	
	if( DAQmxFailed(error) )
	{
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		MessagePopup("DAQmx Error", errBuff);
	}
	
	return 0;
}

int CVICALLBACK stopVNACallback (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//Before we begin let's update our status message.
			if(vnaRunning)
				SetCtrlVal(panel, VNA_STATUS_MSG, "Status:  Stopping");
			
			vnaRequestStop = 1;
			break;
	}
	return 0;
}

int CVICALLBACK vnaFreqBoundsCallback (int panel, int control, int event,
									   void *callbackData, int eventData1, int eventData2)
{
	double minFreq;
	double maxFreq;
	double temp;
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel, VNA_SIGNAL_START_FREQ, &minFreq);
			GetCtrlVal(panel, VNA_SIGNAL_STOP_FREQ, &maxFreq);
			
			if (minFreq > maxFreq)
			{
				temp = minFreq;
				minFreq = maxFreq;
				maxFreq = temp;
				
				SetCtrlVal(panel, VNA_SIGNAL_START_FREQ, minFreq);
				SetCtrlVal(panel, VNA_SIGNAL_STOP_FREQ, maxFreq);
			}
			
			saveVNASettings(&settingsStruct, panel);

			break;
	}
	return 0;
}






///////////////////////////
// Shared Callbacks      //
///////////////////////////
int CVICALLBACK updateVNAControllsCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
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
	int useRefChan;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//First let's set our controls.  This is for cleanliness.
			inControl = VNA_INPUT_PHYS_CHANNEL;
			outControl = VNA_OUT_PHYS_CHANNEL;
			refControl = VNA_R_INPUT_PHYS_CHANNEL;
			trigControl = VNA_TRIG_PHYS_CHANNEL;
			inVControl = VNA_INPUT_VOLTAGE;
			outVControl = VNA_OUTPUT_VOLTAGE;
			
			//If an instrument has been updated let's update the UI information so the user can be made aware if their scan settings are invalid.
			//First things first let's get the infomation of all used devices.
			GetCtrlVal(panel, inControl, inputChan);
			GetCtrlVal(panel, outControl, outputChan);
			GetCtrlVal(panel, refControl, refChan);
			GetCtrlVal(panel, trigControl, trigChan);
			GetCtrlVal(panel, VNA_USE_REF_CHAN, &useRefChan);
			
			if (getDeviceInfo(inputChan, &vnaInDev) == 0)
				inDevRead = 1;
		
			if (getDeviceInfo(outputChan, &vnaOutDev) == 0)
				outDevRead = 1;
			
			if (getDeviceInfo(refChan, &vnaRefDev) == 0)
				refDevRead = 1;

			//getDeviceInfo(trigChan, &vnaTrigDev);
		
			//Let's see which control has been updated.
			//If it's an input control, let's update the input voltage.
			if ((control == inControl) && inDevRead)
			{
				//First let's update our max input voltage voltage.
				updateVNAPanelInputVoltages();
			}
			//Otherwise if it's the out let's update the output voltage.
			//For this to work we need to have read in the output device info.
			else if (control == outControl && outDevRead)
			{
				//Let's get our max voltage.
				updateVNAPanelOutputVoltages();
			}
			
			//Now let's calculate the minimum and maximum frequency the device can produce/capture.
			if (inDevRead && outDevRead)
			{
				//First let's see if the ref chan is enabled and if the in and ref are on the same device.
				if (useRefChan && refDevRead)
					maxFreq = findInputRate(vnaInDev, vnaRefDev);
				else
					maxFreq = vnaInDev.rateIn;

				//Let's compare the sample frequencies of the input and output devices and select the smaller of fthe two.
				maxFreq = Min(maxFreq, vnaOutDev.rateOut);
				
				//Now let's divide our max frequency by 8, since that's realistically what we can read in.
				maxFreq /= MIN_SAMPS_PER_CYCLE;
				
				//Now let's update our frequency bounds control boxes accordingly.
				SetCtrlAttribute(panel, VNA_SIGNAL_START_FREQ, ATTR_MAX_VALUE, maxFreq);
				GetCtrlVal(panel, VNA_SIGNAL_START_FREQ, &temp);
				if (temp > maxFreq)
					SetCtrlVal(panel, VNA_SIGNAL_START_FREQ,  maxFreq);
					
				SetCtrlAttribute(panel, VNA_SIGNAL_STOP_FREQ, ATTR_MAX_VALUE, maxFreq);
				GetCtrlVal(panel, VNA_SIGNAL_STOP_FREQ, &temp);
				if (temp > maxFreq)
					SetCtrlVal(panel, VNA_SIGNAL_STOP_FREQ, maxFreq);
			}
			
			//Finally let's save our panel settings
			saveVNASettings(&settingsStruct, vnaPanelHandle);
			
			break;
	}
	
	return 0;
}

int CVICALLBACK saveVNAPanelSettings (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			saveVNASettings(&settingsStruct, vnaPanelHandle);
			break;
	}
	return 0;
}

int CVICALLBACK adjustVoltageBounds (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			updateVNAPanelOutputVoltages();
			updateVNAPanelInputVoltages();
			saveVNASettings(&settingsStruct, vnaPanelHandle);
			
			break;
	}
	return 0;
}

void updateVNAPanelOutputVoltages()
{
	double gain;
	double minV = 0;
	double maxV = 0;
	double currentV;	
	
	if (vnaOutDev.connected)
	{
		GetCtrlVal(vnaPanelHandle, VNA_GAIN, &gain);
		minV = vnaOutDev.outMinV;
		maxV = vnaOutDev.outMaxV;
		minV *= gain;
		maxV *= gain;
		
		//Now let's update our VNA panel.
		SetCtrlAttribute(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, ATTR_MIN_VALUE, minV);
		SetCtrlAttribute(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, ATTR_MAX_VALUE, maxV);
	}

	//Now let's make sure our panel value is within bounds.
	GetCtrlAttribute(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, ATTR_MAX_VALUE, &maxV);
	GetCtrlAttribute(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, ATTR_MIN_VALUE, &minV);
	GetCtrlVal(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, &currentV);
	currentV = Min(currentV, maxV);
	currentV = Max(minV, currentV);
	SetCtrlVal(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, currentV);
}

void updateVNAPanelInputVoltages()
{
	double ratio;
	double minV = 0;
	double maxV = 0;
	double currentV;	
	
	if (vnaInDev.connected)
	{
		GetCtrlVal(vnaPanelHandle, VNA_RATIO, &ratio);
		minV = vnaInDev.inMinV;
		maxV = vnaInDev.inMaxV;
		minV *= ratio;
		maxV *= ratio;
		
		//Now let's update our VNA panel.
		SetCtrlAttribute(vnaPanelHandle, VNA_INPUT_VOLTAGE, ATTR_MIN_VALUE, minV);
		SetCtrlAttribute(vnaPanelHandle, VNA_INPUT_VOLTAGE, ATTR_MAX_VALUE, maxV);
	}
	
	//Now let's make sure our panel value is within bounds.
	GetCtrlAttribute(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, ATTR_MAX_VALUE, &maxV);
	GetCtrlAttribute(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, ATTR_MIN_VALUE, &minV);
	GetCtrlVal(vnaPanelHandle, VNA_INPUT_VOLTAGE, &currentV);
	currentV = Min(currentV, maxV);
	currentV = Max(minV, currentV);
	SetCtrlVal(vnaPanelHandle, VNA_INPUT_VOLTAGE, currentV);
}
