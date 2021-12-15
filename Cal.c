//Parameters
//#include "SignalAnalyzerShared.h"
//#include "Multi-Function-Synch AI-AO.h"
//#include "FileIO.h"
#include "FileIO.h"

static int calPanelHandle = -1;
static Calibration calRun;

int CVICALLBACK startCal (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	//Declarations
	Device inDev;
	Device outDev;
	int refCal = 0;

	double startFreq = 1.0;
	double stopFreq;
	double inputVoltage = 1.0;
	double outputVoltage = 1.0;
	double gain = 1.0;
	double ratio = 1.0;
	int stepType = 2;
	unsigned short stepsPerUnit = 25;
	int totalSteps = 0;
	char inputChan[CHAN_NAME_LEN];
	char outputChan[CHAN_NAME_LEN];
	char refChan[CHAN_NAME_LEN];
	char trigChan[CHAN_NAME_LEN] = "";
	int useRefChan;
	int useTrigChan = 0;
	int32 error = 0;
	int inputConfigType;
	char errBuff[2048]= {'\0'};
	int fitStartPos = 0;
	double delayTime = 0.0;
	unsigned short numTrials = 1;
	double coeff[4];
	double mserror;
	double *fittedArray;

	switch (event)
	{
		case EVENT_COMMIT:
		//First let's copy all variables from the panel.
			GetCtrlVal(calPanelHandle, CAL_INPUT_PHYS_CHANNEL, inputChan);
			GetCtrlVal(calPanelHandle, CAL_OUT_PHYS_CHANNEL, outputChan);
			//GetCtrlVal(calPanelHandle, CAL_R_INPUT_PHYS_CHANNEL, refChan);
			//GetCtrlVal(calPanelHandle, CAL_USE_REF_CHAN, &refCal);
			GetCtrlVal(calPanelHandle, CAL_INPUT_CONFIG, &inputConfigType);
			
			//Next let's inform the user of the procedure.
			GenericMessagePopup("Instructions", "Please connect the selected output device directly to the selected input devce and push OK to continue.", "OK", "Cancle", 0, 0, 0, 0, 0, 0, 0);

			//Next let's get the device info and populate our cal info with it.
			getDeviceInfo(inputChan, &inDev);
			getDeviceInfo(outputChan, &outDev);
			calRun.serial = inDev.serial;
			calRun.outSerial = outDev.serial;
			stopFreq = Min(inDev.rateIn, outDev.rateOut);
			stopFreq /= MIN_SAMPS_PER_CYCLE;

			//Now let's run the VNA.
			calRun.completed = 0;
			SetCtrlAttribute(calPanelHandle, CAL_SAVE, ATTR_DIMMED, TRUE);
			vnaRequestStop = 0;
			error = getTrace(FREQ_MEASUREMENT, 1, numTrials, 0, 0, &calPanelHandle, CAL_STATUS_MSG, CAL_MAG_CHART, CAL_PHASE_CHART, 0, CAL_FREQ_MSG, inputChan, outputChan, refChan, trigChan, refCal, useTrigChan, inputConfigType, startFreq, stopFreq, stepType, stepsPerUnit, startFreq, inputVoltage, outputVoltage, 0, fitStartPos, delayTime, 0, &vnaRunning, &vnaRequestStop, &vnaFreqData, &vnaOutputData, &vnaMagData, &vnaPhaseData, &vnaArraySize, &vnaDataPoints, 1, gain, 1, ratio);
			if (error < 0)
			{
				MessagePopup("Error", "VNA failed.\nCanceling operation.");
				return -1;
			}
			
			//Finally, if we didn't exit, let's compute the cal parameters.
			if (vnaRequestStop == 0)
			{
				//Next let's allocate our fitted array.
				if((fittedArray = calloc(vnaDataPoints, sizeof(double))) == 0)
				{
					MessagePopup("Error", "Calibration Process Failed.\nCould not allocate memory for operation.");
					return -1;
				}
				
				//Next let's curve fit the magnitude response to compute the calibration parameters.
				if (PolyFitEx(vnaFreqData, vnaMagData, vnaDataPoints, 3, NULL, NULL, 0, ALGORITHM_POLYFIT_SVD, fittedArray, coeff, &mserror) >= 0) 
				{
					calRun.am = coeff[3];
					calRun.bm = coeff[2];
					calRun.cm = coeff[1];
					calRun.dm = coeff[0];
				}
				else
				{
					MessagePopup("Error", "Calibration process failed.\nCould not fit magnitude data");
					free(fittedArray);
					return -1;
				}
					
				//And now the phase data.
				if (PolyFitEx(vnaFreqData, vnaPhaseData, vnaDataPoints, 3, NULL, NULL, 0, ALGORITHM_POLYFIT_SVD, fittedArray, coeff, &mserror) >= 0)
				{
					calRun.ap = coeff[3];
					calRun.bp = coeff[2];
					calRun.cp = coeff[1];
					calRun.dp = coeff[0];
				}
				else
				{
					MessagePopup("Error", "Calibration process failed.\nCould not fit phase data.");
					free(fittedArray);
					return -1;
				}
				
				//Now let's finish up a few last pieces and we're done.
				free(fittedArray);
				calRun.refCal = refCal;
				calRun.completed = 1;
				
				SetCtrlVal(calPanelHandle, CAL_A, calRun.am);
				SetCtrlVal(calPanelHandle, CAL_B, calRun.bm);
				SetCtrlVal(calPanelHandle, CAL_C, calRun.cm);
				SetCtrlVal(calPanelHandle, CAL_D, calRun.dm);
				SetCtrlVal(calPanelHandle, CAL_A_2, calRun.ap);
				SetCtrlVal(calPanelHandle, CAL_B_2, calRun.bp);
				SetCtrlVal(calPanelHandle, CAL_C_2, calRun.cp);
				SetCtrlVal(calPanelHandle, CAL_D_2, calRun.dp);				
				SetCtrlAttribute(calPanelHandle, CAL_SAVE, ATTR_DIMMED, FALSE);
			}
			break;
	}
	return 0;
}

int CVICALLBACK stopCal (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//Before we begin let's update our status message.
			if(vnaRunning)
				SetCtrlVal(panel, CAL_STATUS_MSG, "Status:  Stopping");
			
			vnaRequestStop = 1;
			break;
	}
	return 0;
}

int CVICALLBACK saveCal (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(WriteCalToFile(calDir, &calRun))
				MessagePopup("Error", "Could not save calibration to file.");
			else
				SetCtrlAttribute(calPanelHandle, CAL_SAVE, ATTR_DIMMED, TRUE);
			break;
	}
	return 0;
}

int CVICALLBACK quitCalPanel (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			exitVNA();
			DiscardPanel(calPanelHandle);
			calPanelHandle = -1;
			DisplayPanel(mainPanelHandle);
			break;
	}
	return 0;
}
