#include "FileIO.h"

int openIniFile(Settings *settings)
{
	//First the dclarations
	IniText *iniSettings;
	char *iniFileDir;
	int *fileSuccessfullyOpened;
	
	char exeDir[MAX_PATHNAME_LEN] = "";
	int readFromFileFailed;
	FILE *iniFileLocation;
	
	//First let's make sure we have all parameters passed in.
	if (!settings)
		return -1;
	
	iniSettings = &(settings->settingsHandle);
	iniFileDir = settings->iniFile;
	fileSuccessfullyOpened = &(settings->fileOpened);
	
	//Next let's build the ini file directory and try to open the file.
	GetProjectDir(exeDir);
	MakePathname(exeDir, "config.ini", iniFileDir);
	*iniSettings = Ini_New(0);
	readFromFileFailed = Ini_ReadFromFile(*iniSettings, iniFileDir);
	
	//Let's see if it failed and it's because the file doesn't exist, let's create it.
	if (readFromFileFailed == -94 || readFromFileFailed == -5001)
	{
		//We'll use the standard method.
		iniFileLocation = fopen(iniFileDir, "w");

		//If it worked, let's close the file and try the op again.
		if (iniFileLocation)
		{
			fclose(iniFileLocation);
			readFromFileFailed = Ini_ReadFromFile(*iniSettings, iniFileDir);
		}
	}
	
	if (!readFromFileFailed)
		*fileSuccessfullyOpened = 1;
	else
		*fileSuccessfullyOpened = 0;
	
	return readFromFileFailed;
}

int saveVNASettings(Settings *settings, int vnaPanelHandle)
{
	IniText *settingsStorage;
	char channel[256];
	char key[19] = "Network Analyzer";
	double value;
	int ivalue;
	unsigned short usvalue;
	
	if ((vnaPanelHandle < 0) || (settings == NULL) || (settings->fileOpened == 0))
		return -1;
	
	settingsStorage = &(settings->settingsHandle);
	
	GetCtrlVal(vnaPanelHandle, VNA_INPUT_PHYS_CHANNEL, channel);
	Ini_PutString(*settingsStorage, key, "Input Channel", channel);

	GetCtrlVal(vnaPanelHandle, VNA_INPUT_CONFIG, &ivalue);
	Ini_PutDouble(*settingsStorage, key, "Input Terminal Configuration", ivalue);
																   	
	GetCtrlVal(vnaPanelHandle, VNA_R_INPUT_PHYS_CHANNEL, channel);
	Ini_PutString(*settingsStorage, key, "Reference Channel", channel);
		
	GetCtrlVal(vnaPanelHandle, VNA_USE_REF_CHAN, &ivalue);
	Ini_PutDouble(*settingsStorage, key, "Use Reference Channel", (double) ivalue);

	GetCtrlVal(vnaPanelHandle, VNA_OUT_PHYS_CHANNEL, channel);
	Ini_PutString(*settingsStorage, key, "Output Channel", channel);
	
	GetCtrlVal(vnaPanelHandle, VNA_TRIG_PHYS_CHANNEL, channel);
	Ini_PutString(*settingsStorage, key, "Trigger Channel", channel);

	GetCtrlVal(vnaPanelHandle, VNA_USE_TRIG_CHAN, &ivalue);
	Ini_PutDouble(*settingsStorage, key, "Use Trigger Channel", (double) ivalue);
	
	GetCtrlVal(vnaPanelHandle, VNA_AVERAGES, &usvalue);
	Ini_PutDouble(*settingsStorage, key, "Num Traces to Average", (double) usvalue);

	GetCtrlVal(vnaPanelHandle, VNA_FIT_POS, &ivalue);
	Ini_PutDouble(*settingsStorage, key, "Fit Start Pos", (double) ivalue);
	
	GetCtrlVal(vnaPanelHandle, VNA_DELAY_TIME, &value);
	Ini_PutDouble(*settingsStorage, key, "Delay Between Runs", value);
	
	GetCtrlVal(vnaPanelHandle, VNA_GAIN, &value);
	Ini_PutDouble(*settingsStorage, key, "Amp Gain", value);
	
	GetCtrlVal(vnaPanelHandle, VNA_RATIO, &value);
	Ini_PutDouble(*settingsStorage, key, "Divider Ratio", value);
	
	GetCtrlVal(vnaPanelHandle, VNA_INPUT_VOLTAGE, &value);
	Ini_PutDouble(*settingsStorage, key, "Input Voltage", value);
	
	GetCtrlAttribute(vnaPanelHandle, VNA_INPUT_VOLTAGE, ATTR_MAX_VALUE, &value);
	Ini_PutDouble(*settingsStorage, key, "Input Voltage Max", value);
	
	GetCtrlVal(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, &value);
	Ini_PutDouble(*settingsStorage, key, "Output Voltage", value);
	
	GetCtrlAttribute(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, ATTR_MAX_VALUE, &value);
	Ini_PutDouble(*settingsStorage, key, "Output Voltage Max", value);

	GetCtrlVal(vnaPanelHandle, VNA_SIGNAL_START_FREQ, &value);
	Ini_PutDouble(*settingsStorage, key, "Start Freq", value);
	
	GetCtrlAttribute(vnaPanelHandle, VNA_SIGNAL_START_FREQ, ATTR_MAX_VALUE, &value);
	Ini_PutDouble(*settingsStorage, key, "Start Freq Max", value);
	
	GetCtrlVal(vnaPanelHandle, VNA_SIGNAL_STOP_FREQ, &value);
	Ini_PutDouble(*settingsStorage, key, "Stop Freq", value);
	
	GetCtrlAttribute(vnaPanelHandle, VNA_SIGNAL_STOP_FREQ, ATTR_MAX_VALUE, &value);
	Ini_PutDouble(*settingsStorage, key, "Stop Freq Max", value);
	
	GetCtrlVal(vnaPanelHandle, VNA_STEPS, &usvalue);
	Ini_PutDouble(*settingsStorage, key, "Steps", usvalue);
	
	GetCtrlVal(vnaPanelHandle, VNA_SIG_TYPE, &ivalue);
	Ini_PutDouble(*settingsStorage, key, "Steps type", ivalue);
	
	Ini_WriteToFile(*settingsStorage, settings->iniFile);
	
	return 0;
}

int readVNASettings(Settings *settings, int vnaPanelHandle)
{
	IniText *settingsStorage;
	char *channel;
	char key[19] = "Network Analyzer";
	double value;
	
	if ((vnaPanelHandle < 0) || (settings == NULL) || (settings->fileOpened == 0))
		return -1;
	
	settingsStorage = &(settings->settingsHandle);
	
	if(Ini_GetPointerToString(*settingsStorage, key, "Input Channel", &channel) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_INPUT_PHYS_CHANNEL, channel);
		
	if(Ini_GetDouble(*settingsStorage, key, "Input Terminal Configuration", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_INPUT_CONFIG, (int) value);

	if(Ini_GetPointerToString(*settingsStorage, key, "Reference Channel", &channel) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_R_INPUT_PHYS_CHANNEL, channel);
	
	if(Ini_GetDouble(*settingsStorage, key, "Use Reference Channel", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_USE_REF_CHAN, (int) value);
		
	if(Ini_GetPointerToString(*settingsStorage, key, "Output Channel", &channel) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_OUT_PHYS_CHANNEL, channel);
			
	if(Ini_GetPointerToString(*settingsStorage, key, "Trigger Channel", &channel) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_TRIG_PHYS_CHANNEL, channel);
	
	if(Ini_GetDouble(*settingsStorage, key, "Use Trigger Channel", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_USE_TRIG_CHAN, (int) value);

	if(Ini_GetDouble(*settingsStorage, key, "Num Traces to Average", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_AVERAGES, (int) value);
	
	if(Ini_GetDouble(*settingsStorage, key, "Fit Start Pos", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_FIT_POS, (int) value);

	if(Ini_GetDouble(*settingsStorage, key, "Delay Between Runs", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_DELAY_TIME, value);
		
	if(Ini_GetDouble(*settingsStorage, key, "Amp Gain", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_GAIN, value);
	
	if(Ini_GetDouble(*settingsStorage, key, "Divider Ratio", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_RATIO, value);

	if(Ini_GetDouble(*settingsStorage, key, "Input Voltage", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_INPUT_VOLTAGE, value);

	if(Ini_GetDouble(*settingsStorage, key, "Input Voltage Max", &value) > 0)
		SetCtrlAttribute(vnaPanelHandle, VNA_INPUT_VOLTAGE, ATTR_MAX_VALUE, value);

	if(Ini_GetDouble(*settingsStorage, key, "Output Voltage", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, value);

	if(Ini_GetDouble(*settingsStorage, key, "Output Voltage Max", &value) > 0)
		SetCtrlAttribute(vnaPanelHandle, VNA_OUTPUT_VOLTAGE, ATTR_MAX_VALUE, value);
	
	if(Ini_GetDouble(*settingsStorage, key, "Start Freq", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_SIGNAL_START_FREQ, value);

	if(Ini_GetDouble(*settingsStorage, key, "Start Freq Max", &value) > 0)
		SetCtrlAttribute(vnaPanelHandle, VNA_SIGNAL_START_FREQ, ATTR_MAX_VALUE, value);

	if(Ini_GetDouble(*settingsStorage, key, "Stop Freq", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_SIGNAL_STOP_FREQ, value);

	if(Ini_GetDouble(*settingsStorage, key, "Stop Freq Max", &value) > 0)
		SetCtrlAttribute(vnaPanelHandle, VNA_SIGNAL_STOP_FREQ, ATTR_MAX_VALUE, value);

	if(Ini_GetDouble(*settingsStorage, key, "Steps", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_STEPS, (unsigned short) value);

	if(Ini_GetDouble(*settingsStorage, key, "Steps type", &value) > 0)
		SetCtrlVal(vnaPanelHandle, VNA_SIG_TYPE, (int) value);
	
	return 0;
}

int saveSRSettings(Settings *settings, int vnaPanelHandle)
{
	IniText *settingsStorage;
	char channel[256];
	char key[25] = "Static Response Analyzer";
	double value;
	int ivalue;
	unsigned short usvalue;
	
	if ((vnaPanelHandle < 0) || (settings == NULL) || (settings->fileOpened == 0))
		return -1;
	
	settingsStorage = &(settings->settingsHandle);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_INPUT_PHYS_CHANNEL, channel);
	Ini_PutString(*settingsStorage, key, "Input Channel", channel);

	GetCtrlVal(vnaPanelHandle, STATIC_RES_INPUT_CONFIG, &ivalue);
	Ini_PutDouble(*settingsStorage, key, "Input Terminal Configuration", ivalue);

	GetCtrlVal(vnaPanelHandle, STATIC_RES_R_INPUT_PHYS_CHANNEL, channel);
	Ini_PutString(*settingsStorage, key, "Reference Channel", channel);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_OUT_PHYS_CHANNEL, channel);
	Ini_PutString(*settingsStorage, key, "Output Channel", channel);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_SETTLE_TIME, &value);
	Ini_PutDouble(*settingsStorage, key, "Settle Time", value);

	GetCtrlVal(vnaPanelHandle, STATIC_RES_TRIG_PHYS_CHANNEL, channel);
	Ini_PutString(*settingsStorage, key, "Trigger Channel", channel);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_USE_TRIG_CHAN, &ivalue);
	Ini_PutDouble(*settingsStorage, key, "Use Trigger Channel", (double) ivalue);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_FIT_POS, &ivalue);
	Ini_PutDouble(*settingsStorage, key, "Fit Start Pos", (double) ivalue);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_DELAY_TIME, &value);
	Ini_PutDouble(*settingsStorage, key, "Delay Between Runs", value);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_INPUT_VOLTAGE, &value);
	Ini_PutDouble(*settingsStorage, key, "Input Voltage", value);
	
	GetCtrlAttribute(vnaPanelHandle, STATIC_RES_INPUT_VOLTAGE, ATTR_MAX_VALUE, &value);
	Ini_PutDouble(*settingsStorage, key, "Input Voltage Max", value);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_GAIN, &value);
	Ini_PutDouble(*settingsStorage, key, "Amp Gain", value);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_RATIO, &value);
	Ini_PutDouble(*settingsStorage, key, "Divider Ratio", value);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_SIGNAL_START_V, &value);
	Ini_PutDouble(*settingsStorage, key, "Start Voltage", value);
	
	GetCtrlAttribute(vnaPanelHandle, STATIC_RES_SIGNAL_START_V, ATTR_MAX_VALUE, &value);
	Ini_PutDouble(*settingsStorage, key, "Start Voltage Max", value);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_SIGNAL_STOP_V, &value);
	Ini_PutDouble(*settingsStorage, key, "Stop Voltage", value);
	
	GetCtrlAttribute(vnaPanelHandle, STATIC_RES_SIGNAL_STOP_V, ATTR_MAX_VALUE, &value);
	Ini_PutDouble(*settingsStorage, key, "Stop Voltage Max", value);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_STEPS, &usvalue);
	Ini_PutDouble(*settingsStorage, key, "Steps", usvalue);
	
	GetCtrlVal(vnaPanelHandle, STATIC_RES_OUTPUT_FREQ, &value);
	Ini_PutDouble(*settingsStorage, key, "Signal Frequency", value);
	
	GetCtrlAttribute(vnaPanelHandle, STATIC_RES_OUTPUT_FREQ, ATTR_MAX_VALUE, &value);
	Ini_PutDouble(*settingsStorage, key, "Signal Frequency Max", value);

	Ini_WriteToFile(*settingsStorage, settings->iniFile);
	
	return 0;
}

int readSRSettings(Settings *settings, int vnaPanelHandle)
{
	IniText *settingsStorage;
	char *channel;
	char key[25] = "Static Response Analyzer";
	double value;
	
	if ((vnaPanelHandle < 0) || (settings == NULL) || (settings->fileOpened == 0))
		return -1;
	
	settingsStorage = &(settings->settingsHandle);
	
	if(Ini_GetPointerToString(*settingsStorage, key, "Input Channel", &channel) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_INPUT_PHYS_CHANNEL, channel);
		
	if(Ini_GetDouble(*settingsStorage, key, "Input Terminal Configuration", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_INPUT_CONFIG, (int) value);

	if(Ini_GetPointerToString(*settingsStorage, key, "Reference Channel", &channel) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_R_INPUT_PHYS_CHANNEL, channel);
			
	if(Ini_GetPointerToString(*settingsStorage, key, "Output Channel", &channel) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_OUT_PHYS_CHANNEL, channel);

	if(Ini_GetDouble(*settingsStorage, key, "Settle Time", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_SETTLE_TIME, value);
			
	if(Ini_GetPointerToString(*settingsStorage, key, "Trigger Channel", &channel) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_TRIG_PHYS_CHANNEL, channel);
	
	if(Ini_GetDouble(*settingsStorage, key, "Use Trigger Channel", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_USE_TRIG_CHAN, (int) value);

	if(Ini_GetDouble(*settingsStorage, key, "Fit Start Pos", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_FIT_POS, (int) value);

	if(Ini_GetDouble(*settingsStorage, key, "Delay Between Runs", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_DELAY_TIME, value);
		
	if(Ini_GetDouble(*settingsStorage, key, "Amp Gain", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_GAIN, value);
	
	if(Ini_GetDouble(*settingsStorage, key, "Divider Ratio", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_RATIO, value);

	if(Ini_GetDouble(*settingsStorage, key, "Input Voltage", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_INPUT_VOLTAGE, value);

	if(Ini_GetDouble(*settingsStorage, key, "Input Voltage Max", &value) > 0)
		SetCtrlAttribute(vnaPanelHandle, STATIC_RES_INPUT_VOLTAGE, ATTR_MAX_VALUE, value);

	if(Ini_GetDouble(*settingsStorage, key, "Start Voltage", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_SIGNAL_START_V, value);

	if(Ini_GetDouble(*settingsStorage, key, "Start Voltage Max", &value) > 0)
		SetCtrlAttribute(vnaPanelHandle, STATIC_RES_SIGNAL_START_V, ATTR_MAX_VALUE, value);

	if(Ini_GetDouble(*settingsStorage, key, "Stop Voltage", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_SIGNAL_STOP_V, value);

	if(Ini_GetDouble(*settingsStorage, key, "Stop Voltage Max", &value) > 0)
		SetCtrlAttribute(vnaPanelHandle, STATIC_RES_SIGNAL_STOP_V, ATTR_MAX_VALUE, value);

	if(Ini_GetDouble(*settingsStorage, key, "Steps", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_STEPS, (unsigned short) value);

	if(Ini_GetDouble(*settingsStorage, key, "Signal Frequency", &value) > 0)
		SetCtrlVal(vnaPanelHandle, STATIC_RES_OUTPUT_FREQ, value);

	if(Ini_GetDouble(*settingsStorage, key, "Signal Frequency Max", &value) > 0)
		SetCtrlAttribute(vnaPanelHandle, STATIC_RES_OUTPUT_FREQ, ATTR_MAX_VALUE, value);

	return 0;
}

void saveInstrumentData(int isRunning, int dataPoints, double *freqData, double *voutData, double *magData, double *phaseData)
{
	FILE* file;
	char filepath[MAX_PATHNAME_LEN] = "";
	int dialogReturn;
	int i;

	//First we need to make sure that the VNA isn't running.  If it is let's get out of here.
	if (!isRunning)
	{
		//Next  let's make sure there is data to save.
		if (dataPoints > 0)
		{
			//Now let's select a file and make sure no errors occur.
			if((dialogReturn = FileSelectPopup("", "*.dat", "*.dat; *.txt; *.csv", "Save VNA Data", VAL_SAVE_BUTTON, 0, 0, 1, 1, filepath)) > 0)
			{
				//Now let's open the file,,
				file = fopen(filepath, "w");

				//Make sure it was actually opened
				if (file != NULL)
				{
					//And write the headers and data to the file
					fprintf(file, "Frequency\tVoltage Out\tVoltage In\tPhase\n");

					for (i = 0; i < dataPoints; i++)
						fprintf(file, "%f\t%f\t%f\t%f\n", freqData[i], voutData[i], magData[i], phaseData[i]);

					//And then close it.
					fclose(file);
				}
				else
				{
					MessagePopup("Error", "Could not open file to write data.  Aborting.");
				}
			}
		}
		else
		{
			MessagePopup("Error", "No data to save!");
		}
	}
	else
	{
		MessagePopup("Error", "Cannot save data while instrument is running.\nPlease cancel current operation or wait for it to complete.");
	}
}

int ReadCalFromFile (uint64_t inSerial, uint64_t outDevSerial, char calDir[MAX_PATHNAME_LEN], Calibration *cal)
{
	char filename[MAX_PATHNAME_LEN];
	char file[MAX_PATHNAME_LEN];
	char outSerial[50];
	IniText ini;
	
	//First let's init everything.
	if ((ini = Ini_New(0)) == 0)
		return -1;

	//If so let's format our file names.
	sprintf(outSerial, "%d", (int)outDevSerial);
	sprintf(filename, "%d.ini", (int)inSerial);
	sprintf(file, "%s\\%s", calDir, filename);
	
	if (Ini_ReadFromFile(ini, file) < 0)
	{
		Ini_Dispose(ini);
		return -1;
	}
	
	if(Ini_GetDouble(ini, outSerial, "am", &cal->am) <= 0) return -1;
	if(Ini_GetDouble(ini, outSerial, "bm", &cal->bm) <= 0) return -1;
	if(Ini_GetDouble(ini, outSerial, "cm", &cal->cm) <= 0) return -1;
	if(Ini_GetDouble(ini, outSerial, "dm", &cal->dm) <= 0) return -1;
	if(Ini_GetDouble(ini, outSerial, "ap", &cal->ap) <= 0) return -1;
	if(Ini_GetDouble(ini, outSerial, "bp", &cal->bp) <= 0) return -1;
	if(Ini_GetDouble(ini, outSerial, "cp", &cal->cp) <= 0) return -1;
	if(Ini_GetDouble(ini, outSerial, "dp", &cal->dp) <= 0) return -1;
	cal->serial = inSerial;
	cal->outSerial = outDevSerial;
	cal->completed = 1;
	cal->refCal = 0;
	
	Ini_Dispose(ini);
	return 0;
}
	
	

int WriteCalToFile (char calDir[MAX_PATHNAME_LEN], Calibration *calRun)
{
	//Declarations
	char filename[MAX_PATHNAME_LEN];
	char file[MAX_PATHNAME_LEN];
	char outSerial[50];
	IniText ini;
	
	//first things first let's set it up
	ini = Ini_New(0);
	
	//If we couldn't get the ini container let's exit.
	if (ini == 0)
		return -1;

	//Let's see if we've been given a complete calibration
	if (calRun->completed)
	{
		//If so let's format our file names.
		sprintf(outSerial, "%d", calRun->outSerial);
		sprintf(filename, "%d.ini", calRun->serial);
		sprintf(file, "%s\\%s", calDir, filename);
		
		//And then store our variables 
		if(Ini_PutDouble(ini, outSerial, "am", calRun->am) < 0) return -1;
		if(Ini_PutDouble(ini, outSerial, "bm", calRun->bm) < 0) return -1;
		if(Ini_PutDouble(ini, outSerial, "cm", calRun->cm) < 0) return -1;
		if(Ini_PutDouble(ini, outSerial, "dm", calRun->dm) < 0) return -1;
		if(Ini_PutDouble(ini, outSerial, "ap", calRun->ap) < 0) return -1;
		if(Ini_PutDouble(ini, outSerial, "bp", calRun->bp) < 0) return -1;
		if(Ini_PutDouble(ini, outSerial, "cp", calRun->cp) < 0) return -1;
		if(Ini_PutDouble(ini, outSerial, "dp", calRun->dp) < 0) return -1;
		if(Ini_WriteToFile(ini, file) < 0) return -1; 
	}
	else
		return -1;
	
	Ini_Dispose(ini);
	
	return 0;
}

