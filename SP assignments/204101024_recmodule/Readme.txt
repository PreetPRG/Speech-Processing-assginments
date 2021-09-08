========================================================================
    CONSOLE APPLICATION : 204101024_yes_no Project Overview
========================================================================

1)Procedure to compile and run program:
	-Build Solutions.
	-Ignore Warnings signed-unsigned / long int to int warnings.
	-Start without Debugging.

2)Inputs: Real time Inputs are given using command line recording, 1st u need to record silence than record series of YES or NO.  

3)Outputs: Series of YES or NO with each on new line. For default file: 
YES
NO
YES
 
4)Logic:
	*)Sliding Window teachniques and calculation of STE and ZCR
	-Use of sliding window technique with slide factor 80 and window size 320.
	-STE and ZCR of sliding window is calculated by prefix sum logic where program stores prefix sum at each point,
	and caluclates STE and ZCR for each frame.
	
	*)Boundary Marking
	-To mark starting of YES/NO input the value of STE should be greater than threshold of silence. 
	-Threshold of silence is dynamic and determined by precomputing silence file and equalizing it to maximum STE of file.
	 Maximum STE is taken as it would allow to differentiate between actual voice and background noise.
	-End mark of YES/NO is done when STE becomes less than threshold of silence.
	-To disallow abrupt thrust, i have added a parameter which will discard input if interval size(end-start) is less than 20. 	

	*)Evaluating YES/NO
	-zcr is used to calculate whether YES/NO is spoken.
	-for a particular interval last 40% of data is considered and zcr of it is calculated.
	-If, zcr counts greater than 35(threshold) than YES else NO.
	-zcr at later part is used as YES has fricative 'S' and NO has 'O' and zcr for fricative > vowel.

Thank you,
Preet R Gandhi
204101024
gandhi_preet@iitg.ac.in
	    