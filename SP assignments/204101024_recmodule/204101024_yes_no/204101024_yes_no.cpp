// 204101024_yes_no.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<vector>
using namespace std;

//funtions counts STE and ZCR per each frame and stores in corressponding vectors, return no of frames, it takes O(n) time complexity. Uses sliding window.

long long int Count_STE_ZCR(const vector<long long int> data,vector<long long int> &ste_per_frame,vector<long long int> &zcr_per_frame)
{
	long long int sum=0,zcr=0;											
	vector<long long int> prefix_sum_energy;							//vector to store prefix sum of "square of amplitude" at each point in complete wave.
	vector<long long int> prefix_sum_zero_cross;						//vector to store prefix sum of "zcr" at each point in complete wave.
   
    bool pos=true;														//flag to check if wave is having positive or negative amplitude, it is used to count zcr.	
	  
	for(unsigned long long int i=0;i<data.size();i++)
    {
		if(data[i]>=0 && !pos)
        {
            zcr++;
            pos=true;
        }																//itereate through every data point and counts zcr and energy and sums with prefix of it
        else if (data[i]<0 && pos)										// and stores in corresponding vectors.
        {
            zcr++;
            pos=false;
        }
        sum+=data[i]*data[i];
		prefix_sum_energy.push_back(sum);
		prefix_sum_zero_cross.push_back(zcr);
    }
	
	//Now, counts STE and ZCR for each frame.
	int window_size=320,slide_factor=80;										//window of each frame and how much it should slide for next frame.
	long long int no_frames = (data.size()-window_size)/slide_factor + 1;		// counting total number of frames present in data.
	long long int start=0;
	
	ste_per_frame.push_back(prefix_sum_energy[start+window_size-1]/window_size);
	zcr_per_frame.push_back(prefix_sum_zero_cross[start+window_size-1]);
	start+=slide_factor;
	for(unsigned long long int i=1;i<no_frames;i++)
	{
		ste_per_frame.push_back((prefix_sum_energy[start+window_size-1]-prefix_sum_energy[start-1])/window_size);		//storing STE and ZCR values for each frame
		zcr_per_frame.push_back(prefix_sum_zero_cross[start+window_size-1]-prefix_sum_zero_cross[start-1]);			//by gradually sliding by sliding factor.
		start+=slide_factor;
	}
	return no_frames;
}

//function checks whether YES/NO is spoken, uses zcr to determine it. Takes last 40% to count zcr.

void check_for_yes_no(const vector<long long int> zcr_per_frame,long long int start,long long int end)
{
	int thr_to_remove_abrupt_thrust_size=20;										// threshold to remove abrupt thrust in wave during recording.
	if( (end-start) < thr_to_remove_abrupt_thrust_size )								
		return;

	long long int mid = start + (end-start)*0.6;										
	long long int average_zcr=0;
	for(long long int i=mid;i<end;i++)
	{
		average_zcr+=zcr_per_frame[i];												// measuring zcr from middle to end and counting it's average.
	}
	average_zcr=average_zcr/(end-mid);
	
	int threshold_zcr=35;
	if(average_zcr>threshold_zcr)													//threshold_zcr is used to determine whether yes or no is spoken.
		cout<<"YES\n";
	else
		cout<<"NO\n";
	return;
}

int _tmain(int argc, _TCHAR* argv[])
{
	vector<long long int> zcr_per_frame;
	vector<long long int> ste_per_frame;											//3 vectors are used to store:- data , zcr for each frame and ste for each frame respectively. 
	vector<long long int> data;
	long long int sum=0,max_noise=0;												//max_noise is used to measure maximum amount of noise present in silence recording.
	long long int threshold_silence=1500,temp;										//threshold_silence is parameter below which there is a silence and above something is spoken.
	cout<<"################################################\n";
	cout<<"FIRST RECORD SILENCE TO MEASURE ROOM ACOUSTICS!\n";
	cout<<"################################################\n\n";
	system("Recording_Module.exe 1 silence_file.wav yes_no_utility_silence.txt");
	fstream silence_file;
	silence_file.open("yes_no_utility_silence.txt");										// opens silence file to measures the background noise.
				
	long long int average_silence=0;
	int count_frame=0,window_size=320;												
	int counter=window_size;
	long long int count_data=0;
	while(silence_file>>temp)														//use of disjoint window technique to find max STE in silence file.
	{	
		count_data++;
		average_silence+=temp;														//for a single frame we calculate STE and store the maximum STE obtained.	
		sum+=temp*temp;
		counter--;
		if(counter==0)
		{
			if(max_noise < (sum)/window_size)										// suming squares of each point and dividing whole by window_size 													
				max_noise=(sum)/window_size;										// to find STE for each frame and finds the maximum STE from every frame.
			count_frame++;
			counter=window_size;
			sum=0;
		}
	}
	threshold_silence = max_noise;													// making threshold silence equals to maximum amount of STE in silence file,
	silence_file.close();															//it is helpful to differentiate between actual audio and noise.
	
	cout<<"################################################\n";
	cout<<"Record actual data to be analysed!\n";
	cout<<"################################################\n\n";
	system("Recording_Module.exe 3 data_file.wav yes_no_utility.txt");
	fstream yes_no_file;
    yes_no_file.open("yes_no_utility.txt");												//opens actual audio file and reads and stores the value in data vector.
	while(yes_no_file>>temp)
	{
		data.push_back(temp);
	}
	yes_no_file.close();

	long long int no_frames=Count_STE_ZCR(data,ste_per_frame,zcr_per_frame);       //count STE and ZCR per each frame.
    //cout<<"\n"<<no_frames;
	
	bool start_marker=true;															//start_marker=true means marking has not started.
	long long int start,end;															
	for(long long int i=0;i<no_frames;i++)											//iterates through every frame and if STE of a frame becomes
	{																				//greater than threshold silence it marks it as starting of YES/NO input.
		if(ste_per_frame[i]>threshold_silence && start_marker)						
		{
			start_marker=false;										
			start=i;
		}
		else if(ste_per_frame[i]<threshold_silence && !start_marker)				//and when STE of a frame becomes less than threshold silence it marks it
		{																			// as end and tries to evaluate whether YES is spoken or NO in that interval.	
			start_marker=true;
			end=i;
			//cout<<"Start and End: "<<start<< "  "<<end<<"\n";
			check_for_yes_no(zcr_per_frame,start,end);
		}
	}
	return 0;
}

