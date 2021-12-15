int getError(double *wave, int cycleSize, int maxSamplesToCompute, double amp, double omega, double dt, double phase, double *ssError);
int curveFit(double *wave, unsigned int waveSize, double samplesPerCycle, double rateIn, double amp, double freq, int initGuesses, int searchDepth, double *closestPhase, double *sumOfSquaresError);

int getError(double *wave, int cycleSize, int maxSamplesToCompute, double amp, double omega, double dt, double phase, double *ssError)
{
	//Declarations
	int i;
	double error = 0;
	double currentError = 0;
	int stepSize;

	//First let's make sure we have everything.
	if (!wave || !ssError)
		return -1;
	
	//Next let's look at our cycle size.  If it's more than 250 points let's reduce it's size, we don't want this alg to take too much time.
	if (cycleSize <= maxSamplesToCompute)
		stepSize = 1;
	else
		stepSize = cycleSize / maxSamplesToCompute;
	
	for (i = 0; i < cycleSize; i += stepSize)
	{
		error = (wave[i] - amp * sin(omega * i * dt + phase));
		error *= error;
		currentError += error;
	}
	
	*ssError = currentError;
	return 0;
}

//Here we're going to curve fit a sine wave to get a more accurate phase.
//This is a standard hill climbing algorithm with systematic spacing for our initial guesses.
//For best results pass in an odd number
int curveFit(double *wave, unsigned int waveSize, double samplesPerCycle, double rateIn, double amp, double freq, int initGuesses, int searchDepth, double *closestPhase, double *sumOfSquaresError)
{
	//First our declarations
	double *errors;
	double error;
	double currentError;
	double smallestError;
	int smallestErrorPos;
	double dt = 1.0 / rateIn;
	double dp;
	double phase = -Pi();
	double omega = 2.0 * Pi() * freq;
	double lowerLimit;
	double upperLimit;
	double lowerError;
	double upperError;
	int i;
	int bin;
	int cycleSize = (int)floor(samplesPerCycle);
	int maxSamps = 250;
	
	//First we need to check if we have a buffer and we can work with it.
	if (initGuesses <= 1)
		initGuesses = 25;
	
	//Let's make sure we store the upper value as well.
	dp = (2.0 * Pi()) / initGuesses;
	initGuesses++;
	
	//First let's allocate our error bin array.
	errors = (double*) malloc(initGuesses * sizeof(double));
	
	//If we don't have everything we needs let's bail.
	if (!errors || !wave || !closestPhase || (waveSize == 0) || samplesPerCycle <= 0)
	{
		if (errors)
			free(errors);
		
		return -1;
	}
	
	//First let's get our initial guesses.
	//This consists of trying a bunch of different phases and seeing which has the smallest mean-squared error
	for (bin = 0; bin < initGuesses; bin++)
	{
		getError(wave, cycleSize, maxSamps, amp, omega, dt, phase, &currentError);
		errors[bin] = currentError;
		phase += dp;
	}
	
	//Now let's search for the smallest error.
	smallestError = errors[0];
	smallestErrorPos = 0;
	
	for (bin = 1; bin < initGuesses; bin++)
	{
		if (errors[bin] < smallestError)
		{
			smallestError = errors[bin];
			smallestErrorPos = bin;
		}
	}
	
	//Next let's get the lower/upper limit.
	if (smallestErrorPos == 0)
	{
		lowerError = smallestError;
		upperError = errors[1];
		lowerLimit = -Pi();
		upperLimit = -Pi() + dp;
	}
	else if (smallestErrorPos == (initGuesses - 1))
	{
		lowerError = errors[initGuesses - 1];
		upperError = smallestError;
		lowerLimit = Pi() - dp;
		upperLimit = Pi();
	}
	else if (errors[smallestErrorPos - 1] < errors[smallestErrorPos + 1])
	{
		lowerError = errors[smallestErrorPos - 1];
		upperError = smallestError;
		lowerLimit = -Pi() + (smallestErrorPos - 1) * dp;
		upperLimit = -Pi() + (smallestErrorPos) * dp;
	}		
	else
	{
		lowerError = smallestError;
		upperError = errors[smallestErrorPos + 1];
		lowerLimit = -Pi() + smallestErrorPos * dp;
		upperLimit = -Pi() + (smallestErrorPos + 1) * dp;
	}
	
	//We don't need our errors array anymore, so let's free it.
	free(errors);
	
	//Now that we have our bounds let's start the searching game.
	for (i = 0; i < searchDepth; i++)
	{
		//First let's compute our phase.
		phase = (upperLimit + lowerLimit) / 2;
		
		//Next let's get our error for the computed phase.
		getError(wave, cycleSize, maxSamps, amp, omega, dt, phase, &currentError);
		
		//Now let's compare errors and set new limits.
		if (lowerError < upperError)
		{
			upperError = currentError;
			upperLimit = phase;
		}
		else
		{
			lowerError = currentError;
			lowerLimit = phase;
		}
	}
	
	//Now let's compare errors and return our results.
	if (lowerError < upperError)
	{
		error = lowerError;
		phase = lowerLimit;
	}
	else
	{
		error = upperError;
		phase = upperLimit;
	}
	
	//Now let's convert to degrees.
	phase *= (180.0/Pi());
	
	if (sumOfSquaresError)
		*sumOfSquaresError = error;
	
	*closestPhase = phase;
	
	return 0;
}
