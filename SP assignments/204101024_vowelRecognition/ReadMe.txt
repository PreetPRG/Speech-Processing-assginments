##############################################################################
Assignment 3: Vowel Recognition
Roll No: 204101024
##############################################################################

This project is intended to recognize a vowel based on LPC coeffeients and using tokura's distance for comparission purpose.
It contains 2 projects namely LPC_analysis_ref for generating refrence cepstral coeffecients and LPC_analysis_test for testing the generated file and recognizing vowels. 

Compile and Run Procedure details:

For LPC_analysis_test Project: 
1)There should be "cepstral_coeffecient_file.txt" file in internal project folder. It contains refrence cepstrals computed in refrence project. By default it is already present.
2)For live testing recording will be done for 4 sec, intial 1 sec is for analysis of dc_shift so keep 1st sec as silence and than speak up the vowel.
2)Please ignore sign conversion warnings while build.
3)Run directly without debugging.
4)In output, prints name of file and vowel predicted according to LPC analysis and tokura's distance. Accuracy will be printed on pre-recorded files, getting 100% for all vowels on mydata.
5)Testing is done on file no 11 to 20 for a particular vowel.


For LPC_analysis_refrence Project:
1)Make sure to delete cepstral_coeffecient_file in internal project folder. As by default they are computed, rerunning project will append same coeffecients.
2)For a single vowel there is no need for path of vowel file to be changed but for generating refrence ceps for new vowel, path of file is needed to be changed on line no: 232.
3)The order of generating refrence matters alot as testing project depends on position of refrence ceps. So please follow this order for generating ceps: a,e,i,o,u.  
4)Please ignore sign conversion warnings while build.
5)Run directly without debugging
6)Prints Cepstrals for each 5 frames of a file and also frame wise average of them too.
6)After generating all ceps, we need to copy "cepstral_coeffecient_file.txt" file in LPC_analysis_test Project internal folder.




Project Implementation details:

There are 2 projects, one for generating refrence coeffecients and another for testing vowels.

LPC_analysis_ref project:
	
	-It generates cepstral coeffecients for 5 frames of each of 10 files for a single vowel and than takes frame wise average and at last we have 5*12 cepstrals for each vowel,
	we do same for every vowel and generate a refrence file which has all these ceps.
	-Procedure to generate: -calculates dc_shift value by analysing starting 1 sec of recording and apply dc_shift to whole single.
				-calculates normalization and also index of the frame with max_ste.
				-calculate steady frames, 5 frames around max_ste frame are considered.
				-Apply LPC analysis on steady frames.
				-store ceps of all the 10 files for a vowel.
				-Take frame wise average of all and appends them in a file.

LPC_analysis_test project:
	
	-It generates cepstral coeffecients for input data file calcuates tokura distance of it with respect to refrence cepstrals of each vowel and minimum tokura distance vowel
	will be printed.
	-Procedure to generate: -5 frames are considered for LPC analysis and their cepstrals are computed.
				-now we find tokura distance of 1st frame with 1st frame of a particular vowel in refrence file. similarly, do for 2nd frame with 2nd frame from refrence,
				sum them up and find average of, similarly do for all the vowels, vowel which has minimum of all is predicted.

Some of debugging comments are not deleted as they might help in future for analysis.
Thank you.
Preet R Gandhi