//To do:
// 4. We dont' have to generate teh full buffer.  Let's see what we can do with only one cycle.

//Include files
#include <utility.h>
#include <ansi_c.h>
#include <analysis.h>
#include "daqmxioctrl.h"
#include <cvirte.h>		
#include <userint.h>
#include "inifile.h"
#include "NIDAQmx.h"
#include "Multi-Function-Synch AI-AO.h"
#include "Multi-Function-Synch AI-AO_Fn.h"
#include "SignalAnalyzerShared.h"
#include "FileIO.h"
#include "GlobalDefs.h"

//Global Variable Definitions
static int mainPanelHandle = -1;
static int useExternalAmp = 0;
static double ampGain = 1.0;
static int useVDivider = 0;
static double dividerRatio = 1.0;

//Individual instrument C files
#include "VNA.c"
#include "SR.c"
#include "Cal.c"

int InitDirectories();
int MakeDirectory(char *dir);

int InitDirectories()
{
	//There's only one to init for now, the cal directory.
	GetDir(baseDir);
	sprintf(calDir, "%s\\cal", baseDir);
	return MakeDirectory(calDir);
}

int MakeDirectory(char *dir)
{
	int ret = MakeDir(dir);
	
	if (ret == 0 || ret == -9)
		return 0;
	else
		return -1;
}
	

int main (int argc, char *argv[])
{
	//Declarations
	int panelHandle;
	
	//First let's load the engine and set up the panel.
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel(0, "Multi-Function-Synch AI-AO.uir", MAIN)) < 0)
		return -1;
	
	//Init directories.
	if (InitDirectories())
	{
		MessagePopup("Error", "Could not initialize directory structure.  Exiting.");
		return -1;
	}

	//Let's open our ini file
	if(openIniFile(&settingsStruct) != 0)
		MessagePopup("Error", "Could not open/create INI settings file");
	
	//Now let's set the main panel handle global variable.
	mainPanelHandle = panelHandle;
	
	//Finally let's run the user interface.
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	
	//Now let's get rid of of the ini file.
	if (settingsStruct.fileOpened)
		Ini_Dispose((settingsStruct.settingsHandle));

	return 0;
}


	


///////////////////////////
// Main Panel Callbacks  //
///////////////////////////

int  CVICALLBACK launchVNA(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int panelHandle;
	char chan[CHAN_NAME_LEN];
	
	switch (event)
	{
		case EVENT_COMMIT:
			//Let's load the vna panel if we don't have one open already.
			//This is because we don't want multiple panels accessing the magnitude and phase arrays simultaneously.
			if (vnaPanelHandle != -1)
			{
				MessagePopup("Error", "Only one network analyzer panel allowed at a time.");
				return -1;
			}
			
			if ((panelHandle = LoadPanel(0,"Multi-Function-Synch AI-AO.uir", VNA)) < 0)
				return -1;
			
			vnaPanelHandle = panelHandle;
			vnaRequestQuit = 0;
			
			//Next let's set up the input channels.
			NIDAQmx_NewPhysChanAOCtrl (panelHandle, VNA_OUT_PHYS_CHANNEL, 0);
			NIDAQmx_NewPhysChanDOLineCtrl (panelHandle, VNA_TRIG_PHYS_CHANNEL, 0);
			NIDAQmx_NewPhysChanAICtrl (panelHandle, VNA_INPUT_PHYS_CHANNEL, 0);
			NIDAQmx_NewPhysChanAICtrl (panelHandle, VNA_R_INPUT_PHYS_CHANNEL, 0);
	
			//Next let's load the previously used settings.
			readVNASettings(&settingsStruct, vnaPanelHandle);
			
			//Next let's load our device info variables with the current entries in our DAQ channel controls.
			updateVNAControllsCallback(vnaPanelHandle, 0, EVENT_COMMIT, 0, 0, 0);
			updateVNAPanelInputVoltages();
			updateVNAPanelOutputVoltages();
			
			//Finally let's run the user interface.
			DisplayPanel (panelHandle);
									   
			//Finally let's hide the main panel.
			HidePanel(mainPanelHandle);
			
			break;
	}
	
	return 0;
}

int CVICALLBACK launchStaticResponse (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{   
	char chan[CHAN_NAME_LEN];
	int panelHandle;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//Let's load the vna panel if we don't have one open already.
			//This is because we don't want multiple panels accessing the magnitude and phase arrays simultaneously.
			if (sRespPanelHandle != -1)
			{
				MessagePopup("Error", "Only one static response panel allowed at a time.");
				return -1;
			}
			
			if ((panelHandle = LoadPanel(0,"Multi-Function-Synch AI-AO.uir", STATIC_RES)) < 0)
				return -1;
			
			sRespPanelHandle = panelHandle;
			srRequestQuit = 0;
		
			//Next let's set up the input channels.
			NIDAQmx_NewPhysChanAOCtrl (panelHandle, STATIC_RES_OUT_PHYS_CHANNEL, 0);
			NIDAQmx_NewPhysChanDOCtrl (panelHandle, STATIC_RES_TRIG_PHYS_CHANNEL, 0);
			NIDAQmx_NewPhysChanAICtrl (panelHandle, STATIC_RES_INPUT_PHYS_CHANNEL, 0);
			NIDAQmx_NewPhysChanAICtrl (panelHandle, STATIC_RES_R_INPUT_PHYS_CHANNEL, 0);
			
			//Next let's load the previously used settings.
			readSRSettings(&settingsStruct, sRespPanelHandle);
			
			//Next let's load our device info variables with the current entries in our DAQ channel controls.
			updateSRDevices(sRespPanelHandle, 0, EVENT_COMMIT, 0, 0, 0);
			updateSRPanelInputVoltages();
			updateSRPanelOutputVoltages();

			//Finally let's run the user interface.
			DisplayPanel (panelHandle);
			
			//Finally let's hide the main panel.
			HidePanel(mainPanelHandle);
			
			break;
	}
	return 0;
}

int CVICALLBACK launchCalPanel (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	int panelHandle; 

	switch (event)
	{
		case EVENT_COMMIT:
 			//Let's make sure no other panels are open.
			//This is because we don't want multiple panels accessing the hardware simultaneously.
			if (calPanelHandle != -1 || vnaPanelHandle != -1 || sRespPanelHandle != -1)
			{
				MessagePopup("Error", "Calibration not allowed while a test panel is open.");
				return -1;
			}

			if ((vnaPanelHandle = LoadPanel(0,"Multi-Function-Synch AI-AO.uir", VNA)) < 0)
				return -1;
			
			if ((panelHandle = LoadPanel(0,"Multi-Function-Synch AI-AO.uir", CAL)) < 0)
			{
				DiscardPanel(vnaPanelHandle);
				return -1;
			}
			
			calPanelHandle = panelHandle;
			vnaRequestQuit = 0;
			
			//Next let's set up the cal panel channels controls.
			NIDAQmx_NewPhysChanAOCtrl (panelHandle, CAL_OUT_PHYS_CHANNEL, 0);
			NIDAQmx_NewPhysChanAICtrl (panelHandle, CAL_INPUT_PHYS_CHANNEL, 0);
			//NIDAQmx_NewPhysChanAICtrl (panelHandle, CAL_R_INPUT_PHYS_CHANNEL, 0);
	
			//Next let's configure the VNA for a calibration run.
			SetCtrlVal(vnaPanelHandle, VNA_DELAY_TIME, 0.0);
			SetCtrlVal(vnaPanelHandle, VNA_USE_TRIG_CHAN, 0);
			SetCtrlVal(vnaPanelHandle, VNA_AVERAGES, 1);
			SetCtrlVal(vnaPanelHandle, VNA_INPUT_VOLTAGE, 1.0);
			SetCtrlVal(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, 1.0);
			SetCtrlVal(vnaPanelHandle, VNA_FIT_POS, 0);
			SetCtrlVal(vnaPanelHandle, VNA_GAIN, 1.0);
			SetCtrlVal(vnaPanelHandle, VNA_RATIO, 1.0);
			SetCtrlVal(vnaPanelHandle, VNA_STEPS, 25);
			SetCtrlVal(vnaPanelHandle, VNA_SIG_TYPE, 2);
			SetCtrlVal(vnaPanelHandle, VNA_INPUT_CONFIG, 0);
			
			//Finally let's run the user interface.
			DisplayPanel (panelHandle);
									   
			//Finally let's hide the main panel.
			HidePanel(mainPanelHandle);
			
			break;
	}
	return 0;
}

int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_COMMIT)
	{
			vnaRunning = 0;
			srRunning = 0;
			QuitUserInterface(0);
	}

		
	return 0;
}

int closeDevicePanel(int *panelHandle, int *runningVar)
{
	int returnVal = -1;
	
	if (panelHandle)
	{
		//First let's stop whatever test is going on, if there is one going on.
		if (runningVar)
			*runningVar = 0;
		
		//Next let's close our panel.
		returnVal = DiscardPanel(*panelHandle);
		*panelHandle = -1;
	}
	
	return returnVal;
}
