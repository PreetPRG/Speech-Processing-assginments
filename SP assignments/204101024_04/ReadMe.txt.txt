##############################################################################
Assignment 4: Kmeans and LBG Algorithm
Roll No: 204101024
##############################################################################

There are 2 mini projects in it, 1)K_means project(folder: 204101024_K_means) and 2)LBG_algorithm(folder: 204101024_LBG_algorithm)

*)K_means algorithm is implemented in first project with consideration of Universe data.
*)LBG algorithm is implemented in second project with consideration of Universe data.

Open Both project in Visual Studio 2010 by opening VC++ project file present in respective project folder.

Compile and Run Procedure: 

1)K_means:
Compile and Run: compile in Visual studio, please ignore the sign conversion warnings(unsigned to sign). after compilation you can directly run the program without debugging.

Input: Project takes Universe data from universe.txt file so make sure there is universe.txt file in internal project folder(by default already present). No manual input required.
Output: Overall distortion at each iteration of algorithm, and after final convergence output of codebook.

Tunable parameters: 
			-codebooksize(default=8) it tells number of codevector in codebook,
			-delta(default=0.00001) tells after what absolute difference in old distortion and new distortion algorithm should terminate.
			-M(default=100) max number of iterations allowed for k means algorithm.
Flow of Code:
		-Read universe data,
		-Apply Kmeans with default parameters
			-Intialize codevectors by random vectors present in universe.
			-Find nearest neighbour of each vector present in universe with compared to codevectors. and add vector into bucket of its nearest neighbour codevector.
			-Calculate overall distortion and check for termination criteria.
			-If there is empty cell problem program will terminate.
			-else for each bucket find new centroid and repeat step2.

2)LBG_algorithm:
Compile and Run: compile in visual studio, please ignore the sifn conversion warnings(unsigned to sign). after compilation you can directly run the program without debugging.

Input: Project takes Universe data from universe.txt file so make sure there is universe.txt file in internal project folder(by default already present). No manual input required.
Output: For every codebook size prints the codebook and after binary split prints the ouput of Kmeans algorithm.

Tunable parameters:
			-codebooksize(default=8) it tells number of codevector in codebook,
			-delta(default=0.00001) tells after what absolute difference in old distortion and new distortion algorithm should terminate.
			-no_iteration(default=30) max number of iterations allowed for k means algorithm.
			-epsilon(default=0.03) binary split parameter.
Flow of Code:
		-Read universe data
		-Apply LBG algorithm with default parameters
			-Intially create codebook of size=1, it has codevector=centroid of whole universe.
			-Binarysplit codebook using epsilon,
			-Apply K_means on new codebook.
			-Repeat step 2 untill size of codebook becomes required codebook size.
Thank you,
Preet R Gandhi
204101024