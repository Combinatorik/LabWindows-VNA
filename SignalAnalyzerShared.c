#include "toolbox.h"
#include "SignalAnalyzerShared.h"
#include "CurveFitting.c"

int getTrace(int measurementType, int useCalibration, int trialsToRun, double avgStartFreq, BufferControl *bufferControl, int *panel, int status, int magChart, int phaseChart, int signalsPlot, int freqLabel, char inputChan[CHAN_NAME_LEN], char outputChan[CHAN_NAME_LEN], char refChan[CHAN_NAME_LEN], char trigChan[CHAN_NAME_LEN], uInt8 useRefChan, int useTrigChan, int inputConfigType, double startParam, double stopParam, int stepType, unsigned short stepsPerUnit, double freqOut, double inputVoltage, double outputVoltage, int inputVoltageFollowsOutput, int fitFromEnd, double delayTime, double settleTime, int *vnaRunning, int *requestStop, double **vnaFreqData, double **vnaOutputData, double **vnaMagData, double **vnaPhaseData, int *vnaArraySize, int *vnaDataPoints, int useExternalAmp, double ampGain, int useDivider, double dividerRatio) 
{
	//To do:
	// 0.  Incorporate gain and divider into measurements and panel.
	// 1.  Figure out buffer sizes.  Do we want to use the max FIFO buffer size or is a larger one like the one we're doing now okay?  Do we want only to generate one cycle and repeat that for our measurements?  Yes we do.
	// 2.  Test with different devices.
	// 3.  Incorporate cal code.  Test code with HPF and LPF.  Will the cal work equily well with both circuits?
	// 4.  Incorporate trigger into SR mode.
	// 5.  Incorporate every Nth sample drawing and even processing function
	// 6.  Figure out why phase off @ lower frequencies w/ HPF (probably low signal/noise ratio) and how to fix it.
	//			Maybe incorporate auto voltage adjustment, or averaging, or both?
	// 7.  When two channels are read simulatenously, the channels take turns collecting samples.  Thus there will be a phase shift between the two waves.  Adjust accordinly for more accurate results.
	// 8.  Incorporate external trigger, just digital.
	// 9.  Incorporate every Nth sample update function to plot data.
	// 10. Figure out why phase off @ lower frequencies w/ HPF (probably low signal/noise ratio) and how to fix it.


	TaskHandle inputTaskHandle = 0;
	TaskHandle outputTaskHandle = 0;
	TaskHandle trigTaskHandle = 0;
	int triggerType;
	
	Calibration cal;
	int useCal = 0;
	
	//Initializations are here simply to keep the compiler happy and not produce a bunch of warnings.
	Device inputDevice = {"", 0, -1, "", -1, -1, -1, -1, -1, -1, -1};
	Device outDevice = {"", 0, -1, "", -1, -1, -1, -1, -1, -1, -1};
	Device refDevice = {"", 0, -1, "", -1, -1, -1, -1, -1, -1, -1};
	Device trigDevice = {"", 0, -1, "", -1, -1, -1, -1, -1, -1, -1};
	
	char freqStr[CHAN_NAME_LEN] = "";
	char measurement[CHAN_NAME_LEN] = "";
	char unit[CHAN_NAME_LEN] = "";

	int32 error = 0;
	
	int totalSteps;
	int numTrials = trialsToRun;
	
	double maxRateOut;
	double maxRateIn;
	//int outputType;
	int inputType;
	int plotAttr;

	int samplesToFit;
	unsigned int outputBufferSize;
	unsigned int readInBufferSize;
	unsigned int dataBufferSize;
	int outputBufferMultiplier = 1;
	double avgMultiplier = 1.0 / numTrials;
	
	double samplesPerCycle;
	double adjustedSamplesPerCycle;
	double samplesPerCycleIn;
	double adjustedSamplesPerCycleIn;
	double computedSamplesPerCycle;
	double computedSamplesPerCycleIn;
	unsigned int samplesToWrite;
	unsigned int samplesToRead;
	double rateIn;
	double rateOut;
	unsigned int cyclesOut = getCyclesOut(freqOut);
	int sampleShift = 1;
	double mainDataPhase;
	double refDataPhase;
	int numRead;
	int trialCount;
	int updateSRBuffer;
	
	float64 *data = NULL;
	float64 *averageArray = NULL;
	float64 *dataToFit = NULL;
	float64 *refDataToFit = NULL;
	float64 *wave = NULL;
	float64 *currentOutputBuffer = NULL;
	float64 *triggerBuff = NULL;
	float64 *fittedFunction = NULL;
	float64 *xVals = NULL;
	
	double genPhase = 0;
	double genAmp = 0;
	int freq;
	
	//First things first let's see if everything we need was passed in.
	if(!vnaRunning || !vnaFreqData || !vnaOutputData || !vnaMagData || !vnaPhaseData || !vnaArraySize || !vnaDataPoints)
		return -1;
	
	//Next let's get our connected device info.
	if ((getDeviceInfo(inputChan, &inputDevice) != 0) || (getDeviceInfo(outputChan, &outDevice) != 0) || (useRefChan && (getDeviceInfo(refChan, &refDevice) != 0)) || (useTrigChan && getDeviceInfo(refChan, &refDevice) != 0))
	{
		if (inputDevice.rateIn == -1 || (useRefChan && refDevice.rateIn == -1) || outDevice.rateOut == -1)
		{
			MessagePopup("Error", "Could not obtain device info or selected devices do not support I/O.  Aborting.");
			
			if (canUpdate(panel, status))
				SetCtrlVal(*panel, status, "Status:  Idle");
			
			goto Error;
		}
	}
	
	//Now let's see if we have a valid calibration for this pairing.
	useCal = 0;
	if (useCalibration)
	{
		if (ReadCalFromFile(inputDevice.serial, outDevice.serial, calDir, &cal) == 0)
			useCal = 1;
		else
			MessagePopup("Error", "No calibration found for input/output device pair.\nResults will be left uncorrected.");
	}
	
	//Next let's get the total number of steps we'll need for our runthrough.
	totalSteps = buildStepArray(startParam, stopParam, stepType, stepsPerUnit, 0);

	//Now let's prep our data arrays.
	if (prepVNAData(totalSteps, vnaFreqData, vnaOutputData, vnaMagData, vnaPhaseData, vnaArraySize, vnaDataPoints) != 0)
	{
		MessagePopup("Error", "Could not allocate memory for trace acquisition.  Stopping operation.");
		
		if (canUpdate(panel, status))
				SetCtrlVal(*panel, status, "Status:  Idle");
		
		goto Error;
	}

	//If we're here we succeeded, let's build our parameter array.
	if (measurementType == FREQ_MEASUREMENT)
		buildStepArray(startParam, stopParam, stepType, stepsPerUnit, *vnaFreqData);
	else if (measurementType == VOLT_MEASUREMENT)
		buildStepArray(startParam, stopParam, stepType, stepsPerUnit, *vnaOutputData);
		

	//Next let's get our deivce speeds
	maxRateIn = inputDevice.rateIn;
	maxRateOut = outDevice.rateOut;
	
	//If we're using a ref chan let's see if we need to adjust our input rate.
	//This basically is done when the input and reference channels are on the same device.
	//The channels have to share resources if they are and so we divide our rate by 2.
	if (useRefChan)
		maxRateIn = findInputRate(inputDevice, refDevice);
	
	//We need to do a few things next, and how we do them depends on wether we're in VNA or SR mode.
	if (measurementType == FREQ_MEASUREMENT)
	{
		//Again if we're varying the frequencies let's get our max buffer size by seeing the max buffer size of all frequencies.
		if (getMaxBufferSizes(*vnaFreqData, totalSteps, maxRateIn, maxRateOut, &readInBufferSize, &outputBufferSize))
		{
			MessagePopup("Error", "Could not allocate memory for trace acquisition.  Stopping operation.");
			
			if (canUpdate(panel, status))
				SetCtrlVal(*panel, status, "Status:  Idle");
			
			goto Error;
		}
		
		//Let's add an extra sample for the last 0.
		outputBufferSize++;
		
		//Next let's prep our labels
		strcpy(measurement, "Frequency");
		strcpy(unit, "Hz");
	}
	else
	{
		//Otherwise we're going to have a fixed buffer size since we're only dealing with a single frequency, let's not worry about trying all the different sizes.
		if (getMaxBufferSizes(&freqOut, 1, maxRateIn, maxRateOut, &readInBufferSize, &outputBufferSize))
		{
			MessagePopup("Error", "Could not allocate memory for trace acquisition.  Stopping operation.");
			
			if (canUpdate(panel, status))
				SetCtrlVal(*panel, status, "Status:  Idle");
			
			goto Error;
		}
		
		//Now let's double our output buffer multiplier.  This is so we can have two buffers between which we can switch.
		outputBufferMultiplier *= 2;
		
		//Again let's set up our labels
		strcpy(measurement, "Voltage");
		strcpy(unit, "V");
	}

	//And set our input/output type (finite samples, continuous, etc) variables to finite sample output.
	readInBufferSize += 2;
	dataBufferSize = readInBufferSize;
		
	//Now let's compute our data buffer size, which depends on whether we're using a ref channel or not.
	if (useRefChan)				   
		dataBufferSize *= 2;
	
	//Next let's figure out what kind of trigger type we have.
	//This is the final piece we need for computing our buffer sizes.
	triggerType = getChanType(trigChan);
	if (useTrigChan && (triggerType == ANALOG_CHAN))
		outputBufferMultiplier *= 2;
	
	//Now lets allocate all our buffers.
	wave = (float64*) malloc(outputBufferMultiplier * outputBufferSize * sizeof(float64));
	data = (float64*) malloc(dataBufferSize * sizeof(float64));
	averageArray = (float64*) malloc(dataBufferSize * sizeof(float64));
	fittedFunction = (float64*) malloc(readInBufferSize * sizeof(float64));
	xVals = (float64*) malloc(readInBufferSize * sizeof(float64));

	//Let's make sure the memory has been allocated.
	if (!wave || !data || !averageArray || !fittedFunction || !xVals)
	{
		MessagePopup("Error","Could not allocate memory for trace acquisition.  Stopping operation.");
		
		if(canUpdate(panel, status))
			SetCtrlVal(*panel, status, "Status:  Idle");
		
		goto Error;
	}
	
	//Now if we're using an analog trigger let's prep our square wave.	
	//TO DO:  figure out how to swap this with a PFO trigger buffer rather than an analog out buffer.  This is very necessary for the SR triggering.
/*	if (useTrigChan && triggerType == ANALOG_CHAN)
	{
		genPhase = 0;
		SquareWave(outputBufferSize, TRIGGER_LEVEL, 1.0 / outputBufferSize, &genPhase, 50.0, wave + outputBufferSize);
		
		if (measurementType == VOLT_MEASUREMENT)
		{
			genPhase = 0;
			SquareWave(outputBufferSize, TRIGGER_LEVEL, 1.0 / outputBufferSize, &genPhase, 50.0, wave + 3 * outputBufferSize);
		}
	}*/

	//Next let's perform the initial configuration of our I/O channels and a few more variables and I'm ready to go.
	if (measurementType == FREQ_MEASUREMENT)
	{
		currentOutputBuffer = wave;
		DAQmxErrChk (initVNA(inputChan, outputChan, refChan, trigChan, useRefChan, useTrigChan, freqOut, maxRateOut, maxRateIn, inputVoltage, outputVoltage, dataBufferSize, outputBufferSize, inputConfigType, &inputTaskHandle, &outputTaskHandle, &trigTaskHandle));
	}
	else
	{
		//First let's configure out input voltage.
		outputVoltage = stopParam;
		
		//Next set up our output buffer control.  This tells the thread running the output code when to switch buffers.
		currentOutputBuffer = bufferControl->outputBuffer = wave;
		bufferControl->outputBufferSize = &outputBufferSize;
		
		//And then configure out IO device for SR mode.
		DAQmxErrChk (initSR(inputChan, outputChan, refChan, trigChan, useRefChan, useTrigChan, bufferControl, readInBufferSize, freqOut, maxRateOut, maxRateIn, inputVoltage, outputVoltage, inputConfigType, &inputTaskHandle, &outputTaskHandle, &trigTaskHandle));
	}

	//Now let's prep the plots.
	//First the input voltage plot
	if (canUpdate(panel, magChart))
	{
		//SetAxisRange(panel, magChart, VAL_NO_CHANGE, 0, 1, VAL_MANUAL, 0, inputVoltage);
		DeleteGraphPlot(*panel, magChart, -1, VAL_IMMEDIATE_DRAW);
	}
	
	//Then the phase plot.
	if (canUpdate(panel, phaseChart))
	{
		//SetAxisRange(panel, signalsPlot, VAL_NO_CHANGE, 0, 1, VAL_MANUAL, -1.0 * inputVoltage, inputVoltage);
		DeleteGraphPlot(*panel, phaseChart, -1, VAL_IMMEDIATE_DRAW);
	}
	
	//Let's set our mag/phase data plot axis type.
	if (stepType == 0)
	{
		//If we're in steps per decade mode set the x axis to logrithmic.
		if (canUpdate(panel, magChart))
			SetCtrlAttribute(*panel, magChart, ATTR_XMAP_MODE, VAL_LOG);
	
		if (canUpdate(panel, phaseChart))
			SetCtrlAttribute(*panel, phaseChart, ATTR_XMAP_MODE, VAL_LOG);
	}
	else
	{
		//Otherwise let's set our plot to linear mode.
		if (canUpdate(panel, magChart))
			SetCtrlAttribute(*panel, magChart, ATTR_XMAP_MODE, VAL_LINEAR);
		
		if (canUpdate(panel, magChart))
			SetCtrlAttribute(*panel, phaseChart, ATTR_XMAP_MODE, VAL_LINEAR);
	}

	//Finally let's process draw events and we're ready to go.
	if (canUpdate(panel, status))
		SetCtrlVal(*panel, status, "Status:  Running");
	
	ProcessDrawEvents();
	freq = 0;
	*vnaRunning = 1;

	do
	{
		//Before we begin let's update our status message.
		if (canUpdate(panel, freqLabel))
		{
			if (measurementType == FREQ_MEASUREMENT)
				sprintf(freqStr, "%s:  %0.2f %s", measurement, (*vnaFreqData)[freq], unit);
			else
				sprintf(freqStr, "%s:  %0.2f %s", measurement, (*vnaOutputData)[freq], unit);
			
			SetCtrlVal(*panel, freqLabel, freqStr);
		}
		
		//Now let's see if we're at the point where we want to start averaging.
		//Basically we can turn it on only past a certain frequency if need be.
		if (!(*requestStop))
		{
			if ((*vnaFreqData)[freq] >= avgStartFreq)
				numTrials = trialsToRun;
			else
				numTrials = 1;
		}

		trialCount = 0;

		//Now let's make sure we set our SR output buffer flag.
		//This is so if we're in SR mode we won't be writing a new buffer when we average more than one cycle together.
		updateSRBuffer = 1;
				
		//Here begins the loop for averaging when we're averaging read-in voltages together.
		//This is for the SR code.  Since we start reading at arbitrary points there we have no
		//choice but to have this approach.  Maybe if the SR code will be rewritten to be triggered
		//this will change.
		do
		{
			if (!(*requestStop))
			{
				//Next let's zero our averages array.
				for (int i = 0; i < dataBufferSize; i++)
					averageArray[i] = 0.0;

				//Reset our input array.  This eliminates potential issues.
				//This code can probably be removed for optimization since we're not reading past the read-in sample anyway.
				/*for (i = 0; i < dataBufferSize; i++)
					data[i]= 0;*/

				//First things first, let's figure out our experiment parameters, the frequency, output voltage, etc.
				//This will of course depend on our measurement type.
				if (measurementType == FREQ_MEASUREMENT)
				{
					freqOut = (*vnaFreqData)[freq];
					(*vnaOutputData)[freq] = outputVoltage;
				}
				else if (measurementType == VOLT_MEASUREMENT)
				{
					(*vnaFreqData)[freq] = freqOut;
				}

				//Now let's calculate the number of cycles we'll be inputting/outputting.
				cyclesOut = getCyclesOut(freqOut);

				//Next let's compute the current number of output and input samples for the given frequency.
				//Recomputing this stuff is unnecessary for the SR code, but it doesn't hurt either.
				getSamplesPerCycle(freqOut, maxRateOut, &samplesPerCycle, &adjustedSamplesPerCycle);
				getSamplesPerCycle(freqOut, maxRateIn, &samplesPerCycleIn, &adjustedSamplesPerCycleIn);
				samplesToWrite = getBufferSize(cyclesOut, adjustedSamplesPerCycle) + 1;
				samplesToRead = getBufferSize(cyclesOut, adjustedSamplesPerCycleIn) + 1;

				//Next let's figure out our input and output rates.
				//Basically if we had to adjust the input or output buffers we have to change the clocks to that when the hardware produces the wave
				//it is at the desired frequency.
				rateIn = getAdjustedDeviceRate (freqOut, maxRateIn, samplesPerCycleIn, adjustedSamplesPerCycleIn);
				rateOut = getAdjustedDeviceRate (freqOut, maxRateOut, samplesPerCycle, adjustedSamplesPerCycle);

				//Next let's generate our data
				genPhase = 0;
				genAmp = (*vnaOutputData)[freq];
				SineWave (outputBufferSize, (*vnaOutputData)[freq], 1.0 / adjustedSamplesPerCycle, &genPhase, currentOutputBuffer);

				//And set up our data plot and x array for graphing.
				//SetAxisRange(*panel, signalsPlot, VAL_NO_CHANGE, 0, 0, VAL_MANUAL, -3.0 * (*vnaMagData)[freq], 3.0 * (*vnaMagData)[freq]);
				buildLinearArray((1.0 / rateIn), 0, xVals, readInBufferSize);

				//Now let's get our data.
				do
				{
					if (measurementType == FREQ_MEASUREMENT)
					{
						//If we're doing a frequency sweep let' act accordingly
						DAQmxErrChk(getVNAPoint((*vnaFreqData)[freq], rateIn, rateOut, readInBufferSize, dataBufferSize, outputBufferSize, inputVoltage, outputVoltage, inputTaskHandle, outputTaskHandle, useRefChan, trigTaskHandle, useTrigChan, triggerType, fitFromEnd, samplesToRead, samplesToWrite, &numRead, currentOutputBuffer, data));
					}
					else
					{
						//Otherwise if we're doing a voltage sweep let's act accordinly
						DAQmxErrChk(getSRPoint(bufferControl, freq + trialCount, updateSRBuffer, freqOut, dataBufferSize, samplesToRead, samplesToWrite, inputTaskHandle, outputTaskHandle, useRefChan, trigTaskHandle, useTrigChan, triggerType, settleTime, currentOutputBuffer, data, &numRead, requestStop));
						currentOutputBuffer = findNewSRWaveAddr(bufferControl, useTrigChan);
						updateSRBuffer = 0;
					}

					//Now let's add our latest results to the averages array.
					for (int j = 0; j < dataBufferSize; j++)
						averageArray[j] += (((measurementType == FREQ_MEASUREMENT) ? avgMultiplier : 1) * data[j]);

					//Finally let's process our system events.  Collecting data can be time consuming and we need to see if the user asked the program to do something else.
					ProcessSystemEvents();

					//Let's see if the user asked to exit the loop.
					if (*requestStop)
						break;

					//This would be a good place to increment the trial count varibale.
					trialCount++;
				}
				while (measurementType == FREQ_MEASUREMENT && trialCount < numTrials);		  
			}
			
			if (!(*requestStop))
			{
				double ampVar;
				double phaseVar;
				//Next let's fit our current averages array and build our fitted function.
				computeWaveParameters(numRead, useRefChan, cyclesOut, freqOut, rateIn, adjustedSamplesPerCycleIn, fitFromEnd, &computedSamplesPerCycleIn, averageArray, &dataToFit, &refDataToFit, &ampVar, &phaseVar, &samplesToFit, &mainDataPhase, &refDataPhase);
				(*vnaMagData)[freq] += ampVar;
				(*vnaPhaseData)[freq] += phaseVar;
			}

			//Now that we have our final waveform, let's perform some processing on it to get a better measurement.
			/*		if (!(*requestStop))
					{
						double min;
						double max;
						double offset;
						ssize_t minpos;
						ssize_t maxpos;

						//First let's adjust for constant DC offset.
						MaxMin1D(data, numRead, &max, &maxpos, &min, &minpos);
						offset = (max + min) / 2.0;

						for (int i = 0; i < dataBufferSize; i++)
							averageArray[i] -= offset;

						//Next let's fit our current averages array and build our fitted function.
						computeWaveParameters(numRead, useRefChan, cyclesOut, freqOut, rateIn, adjustedSamplesPerCycleIn, fitFromEnd, &computedSamplesPerCycleIn, averageArray, &dataToFit, &refDataToFit, &((*vnaMagData)[freq]), &((*vnaPhaseData)[freq]), &samplesToFit, &mainDataPhase, &refDataPhase);
						SineWave(readInBufferSize, (*vnaMagData)[freq], 1.0 / computedSamplesPerCycleIn, &mainDataPhase, fittedFunction);

						//And then plot our intermediate results.
						if (canUpdate(panel, signalsPlot))
						{
							DeleteGraphPlot(*panel, signalsPlot, -1, VAL_IMMEDIATE_DRAW);
							plotAttr = PlotXY (*panel, signalsPlot, xVals, dataToFit, samplesToFit, VAL_DOUBLE, VAL_DOUBLE,VAL_FAT_LINE ,VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
							SetPlotAttribute(*panel, signalsPlot, plotAttr,ATTR_PLOT_LG_TEXT, "Data");
							plotAttr = PlotXY (*panel, signalsPlot, xVals, fittedFunction, samplesToFit, VAL_DOUBLE, VAL_DOUBLE,VAL_THIN_LINE ,VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
							SetPlotAttribute(*panel, signalsPlot, plotAttr,ATTR_PLOT_LG_TEXT, "Fitted");
						}
					} */
			
			if ((*requestStop))
				break;
		
		} while (measurementType == VOLT_MEASUREMENT && trialCount < numTrials);
				
		if (measurementType == VOLT_MEASUREMENT)
		{
			(*vnaMagData)[freq] = (*vnaMagData)[freq] / numTrials;
			(*vnaPhaseData)[freq] = (*vnaPhaseData)[freq] / numTrials;
		}

		if (!(*requestStop))
		{
			SineWave(readInBufferSize, (*vnaMagData)[freq], 1.0 / computedSamplesPerCycleIn, &mainDataPhase, fittedFunction);

			//And then plot our intermediate results.
			if (canUpdate(panel, signalsPlot))
			{
				DeleteGraphPlot(*panel, signalsPlot, -1, VAL_IMMEDIATE_DRAW);
				plotAttr = PlotXY (*panel, signalsPlot, xVals, dataToFit, samplesToFit, VAL_DOUBLE, VAL_DOUBLE,VAL_FAT_LINE ,VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
				SetPlotAttribute(*panel, signalsPlot, plotAttr,ATTR_PLOT_LG_TEXT, "Data");
				plotAttr = PlotXY (*panel, signalsPlot, xVals, fittedFunction, samplesToFit, VAL_DOUBLE, VAL_DOUBLE,VAL_THIN_LINE ,VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
				SetPlotAttribute(*panel, signalsPlot, plotAttr,ATTR_PLOT_LG_TEXT, "Fitted");
			}
		}

		//Next let's find our true input voltage given our amplifier and divier settings.
		//Let's only run all this stuff though if a stop hasn't been requested.
		if (!(*requestStop))
		{
			(*vnaOutputData)[freq] = findTrueOutputVoltage((*vnaOutputData)[freq], useExternalAmp, ampGain);
			
			if (measurementType == FREQ_MEASUREMENT)
			{
				(*vnaMagData)[freq] = findTrueInputVoltage((*vnaMagData)[freq], (*vnaFreqData)[freq], useDivider, dividerRatio,&cal);
				(*vnaPhaseData)[freq] = findTrueInputPhase((*vnaPhaseData)[freq], (*vnaFreqData)[freq], &cal);
			}
			else
			{
				(*vnaMagData)[freq] = findTrueInputVoltage((*vnaMagData)[freq], freqOut, useDivider, dividerRatio,&cal);
				(*vnaPhaseData)[freq] = findTrueInputPhase((*vnaPhaseData)[freq], freqOut, &cal);
			}

			//Now let's plot what we have of the bode plots so far.
			if (canUpdate(panel, magChart))
			{
				if (measurementType == FREQ_MEASUREMENT)
				{
					//SetAxisRange(panel, magChart,
					PlotXY (*panel, magChart, *vnaFreqData, *vnaMagData, freq + 1, VAL_DOUBLE, VAL_DOUBLE, VAL_FAT_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
				}
				else
					PlotXY (*panel, magChart, *vnaOutputData, *vnaMagData, freq + 1, VAL_DOUBLE, VAL_DOUBLE, VAL_FAT_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
			}

			if (canUpdate(panel, phaseChart))
			{
				if (measurementType == FREQ_MEASUREMENT)
					PlotXY (*panel, phaseChart, *vnaFreqData, *vnaPhaseData, freq + 1, VAL_DOUBLE, VAL_DOUBLE, VAL_FAT_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
				else
					PlotXY (*panel, phaseChart, *vnaOutputData, *vnaPhaseData, freq + 1, VAL_DOUBLE, VAL_DOUBLE, VAL_FAT_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_WHITE);
			}

			//Lets wait for a few ms to let the user look at the plot.
			Delay(delayTime);
		}

		//Finally let's see if we're done.
		if (*requestStop || (freq >= (totalSteps - 1)))
		{
			//If we are we set our running flag.
			*vnaRunning = 0;
		}
		else
		{
			ProcessSystemEvents();
			freq++;
		}
	} while (*vnaRunning);
	
	//Let's reset the chan to 0 now that we're done running.
	DAQmxErrChk(zeroOutputChan(outputTaskHandle, outputBufferMultiplier, inputTaskHandle, useRefChan, trigTaskHandle, useTrigChan, triggerType, maxRateIn, maxRateOut));
	
	//Now let's simply update our status and we're done.
	if (canUpdate(panel, status))
		SetCtrlVal(*panel, status, "Status:  Idle");
	
Error:
	if( inputTaskHandle!= 0 )
		StopAndClear_MultiFunctionSynchAIAO(inputTaskHandle);
	
	if( outputTaskHandle!= 0 )
		StopAndClear_MultiFunctionSynchAIAO(outputTaskHandle);
	
	if( trigTaskHandle != 0 )
		StopAndClear_MultiFunctionSynchAIAO(trigTaskHandle);

	if( wave )
		free(wave);
	
	if( data )
		free(data);
	
	if( averageArray )
		free(averageArray);
	
	if( fittedFunction )
		free(fittedFunction);
	
	if( xVals )
		free(xVals);
	
	return error;
}

int canUpdate(int *panel, element)
{
	if (panel && *panel >= 0 && element > 0)
		return 1;
	else
		return 0;
}

void determineElementUpdateStatus (int panel, int element, int *canUpdate)
{
	if (panel && canUpdate)
	{
		if (panel >= 0 && element > 0)
			*canUpdate = 1;
		else
			*canUpdate = 0;
	}
}

int prepVNAData(int numDataPoints, double **vnaFreqData, double **vnaOutputData, double **vnaMagData, double **vnaPhaseData, int *vnaArraySize, int *vnaDataPoints)
{
	//First let's make sure we got everything.
	if(!vnaFreqData || !vnaMagData || !vnaPhaseData || !vnaArraySize || !vnaDataPoints)
		return -1;
	
	//In order to save time we're going to reallocate the data array only if the number of new points is greater than the current number of data points.
	if(numDataPoints > *vnaArraySize)
	{
		//If it is let's delete the current arrays
		if (*vnaFreqData)
		{
			free(*vnaFreqData);
			*vnaFreqData = NULL;
		}
		
		if (*vnaOutputData)
			free(*vnaOutputData);
		
		if (*vnaMagData)
			free(*vnaMagData);
		
		if (*vnaPhaseData)
			free(*vnaPhaseData);
		
		//Next let's reallocate our arrays.
		*vnaFreqData = (double*) malloc(numDataPoints *  sizeof(double));
		*vnaOutputData = (double*) malloc(numDataPoints * sizeof(double));
		*vnaMagData = (double*) malloc(numDataPoints * sizeof(double));
		*vnaPhaseData = (double*) malloc(numDataPoints * sizeof(double));

		//And then allocate a new one.
		if (!(*vnaFreqData) || !(*vnaOutputData) || !(*vnaMagData) || !(*vnaPhaseData))
		{
			//If it failed let's update the size variables.
			*vnaDataPoints = *vnaArraySize = 0;
			
			//And let the user know.
			return -1;
		}

		//If it succeeded let's update the number of points variable.
		*vnaDataPoints = *vnaArraySize = numDataPoints;
	}
	else
	{
		//Otherwise all we need to do is just set the new number of data points variable.
		*vnaDataPoints = numDataPoints;
	}
	
	//Let's reset the array.
	for (int i = 0; i < *vnaArraySize; i++)
	{
		(*vnaFreqData)[i] = 0;
		(*vnaOutputData)[i] = 0;
		(*vnaMagData)[i] = 0;
		(*vnaPhaseData)[i] = 0;
	}
	
	return 0;
}

unsigned int buildStepArray (double startFreq, double stopFreq, int stepType, unsigned short stepsPerUnit, double *stepArray)
{
	//Declarations
	unsigned int totalSteps = 0;
	double stepSize;
	double logVal;
	double temp;
	int i;
	uInt8 genArray;
	
	//First let's set up our environment variables.
	//Let's see if we need to generate our array or if we're just counting the number of array entry points we'll need.
	if (stepArray)
		genArray = 1;
	else
		genArray = 0;
	
	//Next let's make sure startFreq <= stopFreq
	if (startFreq > stopFreq)
	{
		temp = startFreq;
		startFreq = stopFreq;
		stopFreq = temp;
	}
	
	//Now let's figure out the number of data points we're going to take.
	if (startFreq == stopFreq)
	{
		//If they're the same number let's just do one.
		totalSteps = 1;
		
		if (genArray)
			stepArray[0] = startFreq;
	}
	else if (stepType == 0)
	{
		//If we're in steps per decade mode.
		//We need to figure out the number of complete decades and multiply it by the steps per decade.
		logVal = log10(stopFreq / startFreq);
		totalSteps += stepsPerUnit * ((int) logVal);

		//Next we need to do something a bit bizzare.
		//We need to see if the stop frequency is not the start frequency times some power of ten.
		//This is important because we may need to take additional steps to get to the end frequency.
		//We do this by seeing if the power and the truncated power are the same number.
		if (logVal != floor(logVal))
		{
			//If it's not, let's figure out the step size for this decade and compute how many steps it takes to get from the start of the ocate
			//to the stop frequency.
			temp = startFreq * pow(10.0, floor(logVal));
			stepSize = (startFreq * pow(10.0, floor(logVal) + 1.0) - temp) / stepsPerUnit;

			//We need to divide the range of the by the step size to get the last few steps.
			//We need to take the ceiling of this division because any fraction means we have an additional step, the end frequency.
			temp = (stopFreq - temp) / stepSize;
			totalSteps += (int) ceil(temp);
		}

		//This is for the upper bound.
		totalSteps++;
		
		//Next let's generate the array if we were passed one.
		if(genArray)
		{
			double minLog = log10(startFreq);
			double maxLog = log10(stopFreq);
			
			stepSize = (maxLog - minLog) / (totalSteps - 1);

			for (i = 0; i < (totalSteps - 1); i++)
			{
				stepArray[i] = pow(10.0, minLog + (i * stepSize));
			}

			stepArray[totalSteps - 1] = stopFreq;
		}
	}
	else if (stepType == 1)
	{
		//Else if we're in steps per octave mode.  The plan is largely the same here, only with powers of 2.
		logVal = log(stopFreq / startFreq) / log(2.0);
		totalSteps += stepsPerUnit * ((int) logVal);
		
		if (logVal != floor(logVal))
		{
			temp = startFreq * pow(2.0, floor(logVal));
			stepSize = (startFreq * pow(2.0, floor(logVal) + 1.0) - temp) / stepsPerUnit;
			temp = (stopFreq - temp) / stepSize;
			totalSteps += (int)ceil(temp);
		}

		totalSteps++;
		
		if (genArray)
		{
			double minLog = log10(startFreq) / log10(2.0);
			double maxLog = log10(stopFreq) / log10(2.0);
			
			stepSize = (maxLog - minLog) / (totalSteps - 1);

			for (i = 0; i < totalSteps - 1; i++)
			{
				stepArray[i] = pow(2.0, minLog + (i * stepSize));
				//stepArray[i] = (startFreq + stepSize * (i % stepsPerUnit)) * pow(2.0, floor(i / stepsPerUnit));
			}

			stepArray[totalSteps - 1] = stopFreq;
		}
	}
	else
	{
		//Otherwise we're doing evenly spaced steps. Let's set our plot to linear mode.
		totalSteps = stepsPerUnit;
		
		if (genArray)
		{
			stepSize = (stopFreq - startFreq) / (stepsPerUnit - 1);

			for (i = 0; i < stepsPerUnit; i++)
			{
				stepArray[i] = startFreq + (i * stepSize);
			}
		}
	}

	return totalSteps;
}

int initVNA(char *inputChan, char *outputChan, char *refChan, char *trigChan, int useRefChan, int useTrigChan, double freqOut, double maxRateOut, double maxRateIn, double inputVoltage, double outputVoltage, unsigned int dataBufferSize, unsigned int outputBufferSize, unsigned int inputConfigType, TaskHandle *inputTaskHandle, TaskHandle *outputTaskHandle, TaskHandle *trigTaskHandle)
{
	int inputType;
	int outputType;
	
	outputType = inputType = DAQmx_Val_FiniteSamps;
	
	return (ConfigIO(inputChan, refChan, outputChan, trigChan, useRefChan, 1, useTrigChan, 1, 1, -inputVoltage, -outputVoltage, inputVoltage, outputVoltage, maxRateIn, maxRateOut, inputTaskHandle, outputTaskHandle, trigTaskHandle, inputType, outputType, dataBufferSize, outputBufferSize, inputConfigType));
}

int getVNAPoint(double freqOut, double rateIn, double rateOut, unsigned int readInBufferSize, unsigned int dataBufferSize, unsigned int outputBufferSize, double inputVoltage, double outputVoltage, TaskHandle inputTaskHandle, TaskHandle outputTaskHandle, uInt8 useRefChan, TaskHandle triggerTaskHandle, uInt8 useTrigger, int triggerType, int fitFromEnd, unsigned int samplesToRead, unsigned int samplesToWrite, unsigned int *numRead, float64 *wave, float64 *data)
{
	//Declarations
	int32 error = 0;
	
	int inputType;
	int outputType;

	double zeroOutBuffer[2] = {0.0, 0.0};
	double zeroInBuffer[4];
	int numZerosRead;
	
	//Let's make sure we have pointers for the data we'll be return.
	if (!numRead || !wave || !data)
		return -1;

	//Next let's set our in/out types.
	outputType = inputType = DAQmx_Val_FiniteSamps;
	
	//Now let's configure the clock on our channels
	DAQmxErrChk (ReconfigureChannelClock(rateIn, inputTaskHandle, inputType, samplesToRead));
	DAQmxErrChk (ReconfigureChannelClock(rateOut, outputTaskHandle, outputType, samplesToWrite));
	//DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(inputTaskHandle, DAQmx_Val_Acquired_Into_Buffer, (uInt32) rateIn / 4, 0, updateSignalPlot, NULL));

	//And prime the output
	DAQmxErrChk (Write_MultiFunctionSynchAIAO(outputTaskHandle, wave, samplesToWrite));
	DAQmxErrChk (Start_MultiFunctionSynchAIAO(outputTaskHandle));

	//Now let's read in the data.
	DAQmxErrChk (Start_MultiFunctionSynchAIAO(inputTaskHandle));
	
	//Next we need to see if we're using an external channel as a trigger.
	if (useTrigger)
		DAQmxErrChk (setTrigger(triggerTaskHandle, triggerType));
	
	DAQmxErrChk (Read_MultiFunctionSynchAIAO(inputTaskHandle, samplesToRead, data, dataBufferSize, numRead, DAQmx_Val_GroupByChannel));

	//Now let's stop reading.
	DAQmxErrChk (Stop_MultiFunctionSynchAIAO(inputTaskHandle));
	DAQmxErrChk (Stop_MultiFunctionSynchAIAO(outputTaskHandle));

	//And reset our trigger.
	if (useTrigger)
		DAQmxErrChk (resetTrigger(triggerTaskHandle, triggerType));
	
	//Next let's reset the output, force the voltage to be 0.
	//This will reduce problems on the next cycle.
	DAQmxErrChk (ReconfigureChannelClock(rateIn, inputTaskHandle, inputType, 2));
	DAQmxErrChk (ReconfigureChannelClock(rateOut, outputTaskHandle, outputType, 2));
	DAQmxErrChk (Write_MultiFunctionSynchAIAO(outputTaskHandle, zeroOutBuffer, 2));
	DAQmxErrChk (Start_MultiFunctionSynchAIAO(outputTaskHandle));
	DAQmxErrChk (Start_MultiFunctionSynchAIAO(inputTaskHandle));

	if (useTrigger)
		DAQmxErrChk (setTrigger(triggerTaskHandle, triggerType));

	DAQmxErrChk (Read_MultiFunctionSynchAIAO(inputTaskHandle, 2, zeroInBuffer, 4, &numZerosRead, DAQmx_Val_GroupByChannel));
	DAQmxErrChk (Stop_MultiFunctionSynchAIAO(inputTaskHandle));
	DAQmxErrChk (Stop_MultiFunctionSynchAIAO(outputTaskHandle));
	//DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(inputTaskHandle, DAQmx_Val_Acquired_Into_Buffer, (uInt32) rateIn / 4, 0, NULL, NULL));

	if (useTrigger)
		DAQmxErrChk (resetTrigger(triggerTaskHandle, triggerType));

Error:
	return error;
}

int initSR(char *inputChan, char *outputChan, char *refChan, char *trigChan, int useRefChan, int useTrigChan, BufferControl *bufferControl, unsigned int dataBufferSize, double freqOut, double maxRateOut, double maxRateIn, double inputVoltage, double outputVoltage, int inputConfigType, TaskHandle *inputTaskHandle, TaskHandle *outputTaskHandle, TaskHandle *trigTaskHandle)
{
	//Declarations
	int32 error = 0;
	double samplesPerCycle;
	double adjustedSamplesPerCycle;
	double samplesPerCycleIn;
	double adjustedSamplesPerCycleIn;
	double rateIn;
	double rateOut;
	unsigned int samplesToWrite;
	unsigned int samplesToRead;
	unsigned int cyclesOut;
	double genPhase;
	int inputType = DAQmx_Val_FiniteSamps;
	int outputType = DAQmx_Val_ContSamps;
	
	//First let's make sure we got everything.
	if (!bufferControl || !inputTaskHandle || !outputTaskHandle || (useTrigChan && !trigTaskHandle))
		return -1;
	
	cyclesOut = getCyclesOut(freqOut);
	
	//It's important that we get the details right immidiately, we're not going to be reconfiguring the output channel once it's started.
	//So let's do a few calculations right now and configure out channels accordingly.
	getSamplesPerCycle(freqOut, maxRateOut, &samplesPerCycle, &adjustedSamplesPerCycle);
	getSamplesPerCycle(freqOut, maxRateIn, &samplesPerCycleIn, &adjustedSamplesPerCycleIn);
	samplesToWrite = getBufferSize(cyclesOut, adjustedSamplesPerCycle) + 1;
	samplesToRead = getBufferSize(cyclesOut, adjustedSamplesPerCycleIn) + 1;
	rateIn = getAdjustedDeviceRate (freqOut, maxRateIn, samplesPerCycleIn, adjustedSamplesPerCycleIn);
	rateOut = getAdjustedDeviceRate (freqOut, maxRateOut, samplesPerCycle, adjustedSamplesPerCycle);

	//First let's set up our global flags.
	bufferControl->currentBuffer = 0;
	bufferControl->switchBufferAfterLastSample = 0;
	
	//Now let's set up the IO channels
	DAQmxErrChk (ConfigIO(inputChan, refChan, outputChan, trigChan, useRefChan, useTrigChan, useTrigChan, 1, 0, -inputVoltage, -outputVoltage, inputVoltage, outputVoltage, rateIn, rateOut, inputTaskHandle, outputTaskHandle, trigTaskHandle, inputType, outputType, dataBufferSize, *(bufferControl->outputBufferSize), inputConfigType));

	//Now let's prep our buffer switching function.
	DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(*outputTaskHandle, DAQmx_Val_Transferred_From_Buffer, *(bufferControl->outputBufferSize) + 1, 0, updateOutputBuffer, bufferControl));
		
Error:
	return error;
}

float64* findNewSRWaveAddr(BufferControl *bufferControl, int useTrigger)
{
	//First we need to know if the output is using the first buffer or the second buffer.
	int bufferShift = !(bufferControl->currentBuffer);

	//Next we need to know if we're using a trigger channel, which is an additional buffer of data.
	//If we are, let's adjust our output position accordingly.
	if (useTrigger)
		bufferShift *= 2;

	//Our wave address is nothing more than our buffer start size plus our buffer size.
	return ((bufferControl->outputBuffer)) + bufferShift * (*(bufferControl->outputBufferSize));
}

int getSRPoint(BufferControl *bufferControl, int run, int updateBuffer, double freqOut, unsigned int dataBufferSize, unsigned int samplesToRead, unsigned int samplesToWrite, TaskHandle inputTaskHandle, TaskHandle outputTaskHandle, uInt8 useRefChan, TaskHandle triggerTaskHandle, uInt8 useTrigger, int triggerType, double settleTime, float64 *wave, float64 *data, int *numRead, int *requestStop)
{
	//To do:
	// 8.  Incorporate external digital trigger.

	//Declarations
	int32 error = 0;
	
	int inputType;
	int outputType;
	double rateIn;
	double rateOut;
	double mainDataPhase;
	double refDataPhase;
	double endTime;
	
	//Let's make sure we have pointers for the data we'll be return.
	if (!bufferControl || !wave || !data)
		return -1;
	
	if (run == 0)
	{
		//Let's write the data to our output channel.
		DAQmxErrChk (Write_MultiFunctionSynchAIAO(outputTaskHandle, wave, samplesToWrite));

		//Let's just start the output
		DAQmxErrChk (Start_MultiFunctionSynchAIAO(outputTaskHandle));
		
		//Now that our output has started let's give our system a chance to settle.
		waitSettleTime(settleTime, requestStop);
	}
	else if (updateBuffer)
	{
		
		//Otherwise let's just set up our buffer and raise the flag to switch buffers.
		bufferControl->newDataAddress = wave;

		//Now let's make sure our other thread didn't encounter any errors thus far.
		DAQmxErrChk(bufferControl->error);

		//We're ready to go.  Let's raise the flag for the switch.
		bufferControl->switchBufferAfterLastSample = 1;

		//Let's wait until our buffers are done switching.
		//We'll know it's done when the switch buffer flag is reset.
		while (bufferControl->switchBufferAfterLastSample == 1)
		{
			Delay(0.1);
		}

		//Now that our buffers have switched let's give our output a chance to settle.
		waitSettleTime(settleTime, requestStop);
	}
	
	//Now let's start the input task to read in the data.
	DAQmxErrChk (Start_MultiFunctionSynchAIAO(inputTaskHandle));
	
	//Next we need to see if we're using an external channel as a trigger to trigger the input.
	if (useTrigger)
		DAQmxErrChk (setTrigger(triggerTaskHandle, triggerType));
	
	//Now let's read in the data.
	DAQmxErrChk (Read_MultiFunctionSynchAIAO(inputTaskHandle, samplesToRead, data, dataBufferSize, numRead, DAQmx_Val_GroupByChannel));

	//Now let's stop reading.
	DAQmxErrChk (Stop_MultiFunctionSynchAIAO(inputTaskHandle));
	
	//And reset our trigger.
	if (useTrigger)
		DAQmxErrChk (resetTrigger(triggerTaskHandle, triggerType));
	
Error:
	return error;
}

int waitSettleTime(double settleTime, int *requestStop)
{
	double startTime = Timer();
	double currentTime = Timer();
	
	while ((currentTime - startTime) < settleTime)
	{
		Delay(0.1);
		ProcessSystemEvents();
		currentTime = Timer();
		
		if (*requestStop)
			return 0;
	}
	
	return 1;
}
	

int32 CVICALLBACK updateOutputBuffer (TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	int32 error = 0;
	BufferControl *control = NULL;
	double phase = 0;
	char errBuff[2048];
	
	if (!callbackData)
		return -1;
	
	control = (BufferControl*) callbackData;
	
	if (taskHandle != 0 && control->switchBufferAfterLastSample == 1)
	{
		//Let's update our buffer first.
		DAQmxErrChk(Write_MultiFunctionSynchAIAO(taskHandle, control->newDataAddress, *(control->outputBufferSize)));
		control->currentBuffer = 1 - control->currentBuffer;
		Delay(0.8);
		control->switchBufferAfterLastSample = 0;
	}
	
Error:
	if( DAQmxFailed(error) )
	{
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		MessagePopup("Output Thread Error", errBuff);
		control->error = error; 
	}
	
	return error;
}

int computeWaveParameters(int numRead, int useRefChan, int cyclesOut, double freqOut, double rateIn, double adjustedSamplesPerCycleIn, int fitFromEnd, double *computedSamplesPerCycleIn, float64 *data, float64 **dataToFit, float64 **refDataToFit, double *magDataPoint, double *phaseDataPoint, int *pointsToFit, double *mainDataPhase, double *refDataPhase)
{
	//Definitions
	unsigned int dataAddrShift;
	unsigned int samplesToFit;
	int sampleShift = 1;
	
	double frequency = freqOut;
	double amp;
	double phase;
	double refFrequency;
	double refAmp;
	double refPhase;
	
	int cyclesToFit;
	
	SearchType search;
	SearchType refSearch;
	
	int returnVal;
	
	//First things first let's make sure the user passed in all the necesary paramters.
	if (!data || !dataToFit || !refDataToFit || !computedSamplesPerCycleIn)
		return -1;
	
	//Next let's figure out the number of cycles we need to fit.
	//This is done simply by seeing how many cycles we have to work with, and if it's less than the number we need we set it to 1.
	cyclesToFit = getCyclesToFit(cyclesOut);

	//Now let's fit and find our frequency.
	//The point of this is to find a more accurate number of samples to fit when we're doing our final fit.
	//Unfortunately however this can fail and produce a huge number of samples to fit, much larger than the buffer, so we have to be careful.
	prepFitVars(freqOut, adjustedSamplesPerCycleIn, cyclesOut, cyclesToFit, fitFromEnd, sampleShift, data, &dataAddrShift, dataToFit, &samplesToFit, &search);
	
	if (SingleToneInfo (data, numRead, 1.0 / rateIn, &search, &frequency, &amp, &phase) < 0)
		frequency = freqOut;
		
	//Next let's recompute our samples per cycle and adjust our pointers.
	*computedSamplesPerCycleIn = rateIn / frequency;
	prepFitVars(frequency, *computedSamplesPerCycleIn, cyclesOut, cyclesToFit, fitFromEnd, sampleShift, data, &dataAddrShift, dataToFit, &samplesToFit, &search);
	
	//Now let's make sure we're not going to be going over our buffer size or we'll get an exception.
	//Most of the time it won't make any difference, but sometimes it may, like when garbage was read in.
	//First let's make sure our start address isn't greater than the buffer size.
	if ((sampleShift + dataAddrShift) >  numRead)
	{
		//If it is, let's adjust by using the adjusted samples per cycle in value found earlier.
		//The computed value may have messed up, this is likely due to bad data. 
		dataAddrShift = fitFromEnd * getBufferSize(cyclesOut - cyclesToFit, adjustedSamplesPerCycleIn);
	}

	//At this point we know that the point at which we're starting our fit (sampleShift + dataAddrShift) is less than the end of our array, let's make sure the end of the fit is as well.
	//Let's quickly make sure the sample to which we're fitting isn't greater than the data read in.
	if (sampleShift + dataAddrShift + samplesToFit > numRead)
	{
		//If it is, we can can adjust the samplesToFit variable to be the minimum of either the difference between the fit start and the data end or just a raw sample count to fit based on the adjustedSammplesPerCycleIn variable.
		samplesToFit = Min(numRead - (sampleShift + dataAddrShift), getBufferSize(cyclesToFit, adjustedSamplesPerCycleIn));
	}
	
	//Finally we need to make sure that our samplesToFit is positive.  We read in an extra sample just for this.
	if (samplesToFit <= 0)
		samplesToFit = 1;

	//Now let's simply recompute our fit start address.
	*dataToFit = data + sampleShift + dataAddrShift;
	
	//Now let's compute our reference fit parameters.  We have everything we need at this point.
	//Let's see if we're using a reference channel.
	if (useRefChan)
	{
		//If we are let's set our reference fit address to the reference data.
		//This is simply our data buffer we have plus the number of samples read in per channel.
		*refDataToFit = *dataToFit + numRead;
		refSearch.centerFrequency = frequency;
		refSearch.frequencyWidth = 10.0;
	}
	
	//Next let's extract our amplitude and phase information.
	if ((returnVal = SingleToneInfo (*dataToFit, samplesToFit, 1.0 / rateIn, &search, &frequency, &amp, &phase) >= 0))
	{
		//Now let's record the result
		*magDataPoint = amp;
	}
	else
	{
		//If we've failed let's just return a -1 to let the user know we've failed.
		*magDataPoint = *phaseDataPoint = -1;
		return returnVal;
	}
	
	//Next let's fit
	if (useRefChan)
	{
		//If we're using a reference channel let's compare the phase  of the main signal channel to the phase of the reference channel.
		if ((returnVal = SingleToneInfo (*refDataToFit, samplesToFit, 1.0 / rateIn, &search, &refFrequency, &refAmp, &refPhase)) < 0)
		{
			//And if we fail let's just set it to 0 and return the error.  
			*phaseDataPoint = -1;
			return returnVal;
		}
	}
	else
	{
		refPhase = 360.0 * WAVEFORMCYCLESHIFT;
	}
	
	if (mainDataPhase)
		*mainDataPhase = phase;
	
	if (refDataPhase)
		*refDataPhase = refPhase;
	
	if (pointsToFit)
	{
		if (cyclesToFit > 1)
			cyclesToFit = 5;
		else
			cyclesToFit = 1;
		
		*pointsToFit = getBufferSize(cyclesToFit, *computedSamplesPerCycleIn);
		
		if (*pointsToFit > numRead)
			*pointsToFit = getBufferSize(cyclesToFit, adjustedSamplesPerCycleIn);
	}
	
	phase = normalizePhase(phase - refPhase);
	*phaseDataPoint = phase;
	
	return 0;
}

void adjustWaveDCOffset(double *data, size_t length)
{
	double min;
	double max;
	double offset;
	size_t minIndex;
	size_t maxIndex;
	
	MaxMin1D(data, length, &max, &maxIndex, &min, &minIndex);
	
	offset = (max - min) * 0.5;
	
	for (size_t i = 0; i < length; i++)
		data[i] -= offset;
};

int buildLinearArray(double m, double b, float64 *yVals, int arrayLength)
{
	//Declarations
	int i;
	
	//First let's make sure our params are in order.
	if (!yVals)
		return -1;
	
	//Next let's build our xVals array.
	for(i = 0; i < arrayLength; i++)
	{
		yVals[i] = i * m + b;
	}
	
	//That's it, we're done.
	return 0;
}

int32 CVICALLBACK updateSignalPlot (TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	/*int *plotHandle;

	if (*plotHandle >= 0)
		;//Update plot with current data.

	*///Now let's process user events and get out of here.
	ProcessSystemEvents();
	
	return 0;
}

int prepFitVars(double frequency, double samplesPerCycle, unsigned int cyclesOut, unsigned int cyclesToFit, unsigned int fitFromEnd, unsigned int sampleShift, float64 *data, unsigned int *dataAddrShift, float64 **dataToFit, unsigned int *samplesToFit, SearchType *search)
{
	if (!data || !dataAddrShift || !dataToFit || !samplesToFit || !search)
		return -1;
	
	*dataAddrShift = fitFromEnd * getBufferSize(cyclesOut - cyclesToFit, samplesPerCycle);
	*dataAddrShift += !fitFromEnd * getBufferSize(WAVEFORMCYCLESHIFT, samplesPerCycle);
	*dataToFit = data + sampleShift + *dataAddrShift;
	*samplesToFit = getBufferSize(cyclesToFit, samplesPerCycle);
	search->centerFrequency = frequency;
	search->frequencyWidth = 10.0;

	return 0;
}

 double getNearestFreq(double frequency, double deviceOutRate)
{
	double refreshRate = deviceOutRate / (10.0 * frequency);
	double nearestFreq = refreshRate - floor(refreshRate);
	
	if (nearestFreq < 0.5)
		nearestFreq = floor(refreshRate);
	else
		nearestFreq = ceil(refreshRate);
	
	nearestFreq = deviceOutRate / (nearestFreq * 10.0);
	
	return nearestFreq;
}

unsigned int getCyclesOut(double frequency)
{
	double cyclesOoM;
	unsigned int cyclesOut;
	
	//First let's check our boundary cases.
	if (frequency == 0)
		//If we have a DC voltage let's do one "cycle" out, just in case there's some ripple there.
		return 1;
	else if (frequency < 0)
		//Otherwise if it's less than 0 let's just take its negative and move on.  All negative means is the wave is out of phase, we still need the same
		//number of points to sample it properly.
		frequency *= -1;
	
	//Now we get the order of magnitude of the current frequency.
	cyclesOoM = floor(log10(frequency));

	//Then we'll set it to 0 if it's negative, or just decrement if it's greater than 1.
	if (cyclesOoM < 0)
		cyclesOoM = 0;
	else if (cyclesOoM > 1)
		cyclesOoM--;

	//Now let's raise ten by that number to get the full number of cycles we'll collect.
	cyclesOut = pow(10, cyclesOoM);

	//If our cycle count is 1, let's increment it to 2.  This should increase phase measurement accuracy for low frequencies.
	if (cyclesOut == 1)
		cyclesOut++;

	return cyclesOut;
}

int getMaxBufferSizes (double *frequencyArray, int totalSteps, double rateIn, double rateOut, unsigned int *readInBufferSize, unsigned int *readOutBufferSize)
{							 
	unsigned int currentInputSize;
	unsigned int currentOutputSize;
	double samplesPerCycle;
	double adjustedSamplesPerCycle;
	double samplesPerCycleIn;
	double adjustedSamplesPerCycleIn;
	unsigned int cyclesOut;
	
	if (!frequencyArray || !readInBufferSize || !readOutBufferSize)
		return -1;
	
	*readInBufferSize = 0;
	*readOutBufferSize = 0;
	
	for (int i = 0; i < totalSteps; i++)
	{
		//First let's get our number of samples per cycle in and out.
		getSamplesPerCycle (frequencyArray[i], rateOut, &samplesPerCycle, &adjustedSamplesPerCycle);
		getSamplesPerCycle (frequencyArray[i], rateIn, &samplesPerCycleIn, &adjustedSamplesPerCycleIn);
		cyclesOut = getCyclesOut(frequencyArray[i]);

		//Next let's compute our buffer sizes and round to the nearest integer.
		currentInputSize = getBufferSize(cyclesOut, adjustedSamplesPerCycleIn);
		currentOutputSize = getBufferSize(cyclesOut, adjustedSamplesPerCycle);
		
		if (currentInputSize > *readInBufferSize)
			*readInBufferSize = currentInputSize;
		
		if (currentOutputSize > *readOutBufferSize)
			*readOutBufferSize = currentOutputSize;
	}
	
	return 0;
}

unsigned int getBufferSize(double cyclesOut, double adjustedSamplesPerCycle)
{
	double bufferSize = cyclesOut * adjustedSamplesPerCycle;

	return ((unsigned int) (ceil(bufferSize)));
}

int getSamplesPerCycle (double signalFrequency, double deviceRate, double *samplesPerCycle, double *adjustedSamplesPerCycle)
{
	double maxSamples = 25000;
	
	if(!samplesPerCycle || !adjustedSamplesPerCycle)
		return -1;
	
	*samplesPerCycle = (deviceRate / signalFrequency);

	if (*samplesPerCycle < maxSamples)
		*adjustedSamplesPerCycle = *samplesPerCycle;
	else
		*adjustedSamplesPerCycle = maxSamples;

	return 0;
}

double getAdjustedDeviceRate(double signalFrequency, double deviceMaxRate, double samplesPerCycle, double adjustedSamplesPerCycle)
{
	double rate;
	
	if (samplesPerCycle == adjustedSamplesPerCycle)
		rate = deviceMaxRate;
	else
		rate = adjustedSamplesPerCycle * signalFrequency;
	
	return rate;
}

unsigned int getCyclesToFit(unsigned int cyclesOut)
{
	unsigned int cyclesToFit = 15;
	
	if(cyclesToFit > cyclesOut)
		cyclesToFit = cyclesOut;
	
	//If we're at very low frequencies, let's only fit one cycle.
	//This will prevent buffer overflow issues since we're adjusting the fit
	//to remove that initial bend.
	if (cyclesToFit == 2)
		cyclesToFit--;
	
	return cyclesToFit;
}
		
	
unsigned int getNormalizedArraySize (unsigned int bufferSize)
{
	unsigned int maxSamples = 25000;
	
	if (bufferSize <= maxSamples)
		return bufferSize;
	else
		return maxSamples;
}

double normalizePhase(double phase)
{
	while (phase > 180.0)
		phase -= 360.0;
	
	while (phase < -180.0)
		phase += 360.0;
	
	return phase;
}

double findTrueOutputVoltage(double voltage, int useExternalAmp, double gain)
{
	if (useExternalAmp)
		voltage *= gain;
	
	return voltage;
}

double findTrueInputVoltage(double voltage, double frequency, int useDivider, double dividerRatio, Calibration *cal)
{
	if (cal->completed)
	{
		double offset = (cal->am * pow(frequency, 3.0) + cal->bm * pow(frequency, 2.0) + cal->cm * frequency + cal->dm);
		offset = 1.0 / offset;
		voltage *= offset;
	}
	
	if (useDivider)
		voltage *= dividerRatio;
	
	return voltage;
}

double findTrueInputPhase(double phase, double frequency, Calibration *cal)
{
	if (cal->completed)
	{
		double offset = cal->ap * pow(frequency, 3.0) + cal->bp * pow(frequency, 2.0) + cal->cp * frequency + cal->dp;
		phase -= offset;
	}
	
	return phase;
}


Range* getNewRange(double start, double stop, double min, double max, unsigned short steps)
{
	Range* newRange;
	double temp;
	
	start = Max(start, min);
	start = Min(start, max);
	stop = Max(stop, min);
	stop = Min(stop, max);
	
	if (start > stop)
	{
		temp = start;
		start = stop;
		stop = temp;
	}
	
	if (steps < 1)
		steps = 1;
	
	if ((newRange = (Range*) malloc(sizeof(Range))))
	{
		newRange->start = start;
		newRange->stop = stop;
		newRange->min = min;
		newRange->max = max;
		newRange->steps = steps;
		newRange->nextRange = NULL;
	}
	
	return newRange;
}
