// LPC_analysis.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<vector>
#include<cmath>
#include<cstring>
#include <sstream>
using namespace std;

#define PI 3.141

//hamming window
vector<long double> hamming_window;

//liftering window
vector<long double> liftering_window;

//cepstral coeffecients all for a vowel;
vector<vector<long double>> cepstral_coeffecients;

//find Ri value for given single steady frame.
void find_Ri(vector<long double> data,long double *Ri,long long int start, long long int end,int p)
{
	//Apply hamming window on data;
	int count=0;
	for(long long int i=start;i<end;i++)
	{
		data[i]=data[i]*hamming_window[count];
		count++;
	}
	for(int i=0;i<p+1;i++)
	{
		for(long long int j=start;j<end-i;j++)
		{
			Ri[i]+=data[j]*data[j+i];
		}
		//cout<<"Frame from "<<start<<"  "<<end<<"  "<<Ri[i]<<"\n";
	}
	//cout<<"\n";
	return;
}

//Does LPC analysis on given data and finds ais and cis subsequently for each frame.
void LPC_analysis(vector<long double> data,int p)
{
	long long int window_size=320;
	long long int end=window_size;
	long long int start=0;
	int frame_count=0;
	while(end<=data.size())
	{
		long double *R= new long double[p+1];
		for(int i=0;i<=p;i++)
			R[i]=0;
		find_Ri(data,R,start,end,p);
		long double *E=new long double [p+1];
		long double *ai = new long double[p+1];
		long double *ci = new long double[p+1];
		E[0]=R[0];
		long double *K= new long double[p+1];
		long double** alpha = new long double*[p+1];
		for(int i = 0; i < p+1; ++i)
			alpha[i] = new long double[p+1];
		for(int i=1;i<=p;i++)
		{
			long double sum_k=0;
			for(int j=1;j<=i-1;j++)
			{
				sum_k+=alpha[i-1][j]*R[i-j];
			}
			K[i]=(R[i]-sum_k)/E[i-1];
			alpha[i][i]=K[i];
			for(int j=1;j<=i-1;j++)
			{
				alpha[i][j]=alpha[i-1][j] - K[i]*alpha[i-1][i-j];
			}
			E[i]=(1-K[i]*K[i])*E[i-1];
		}
		//cout<<"frame no: "<<frame_count<<"\n";
		for(int j=1;j<=p;j++)
		{
			ai[j]=alpha[p][j];
			//cout<<ai[j]<<" ";
		}
		//cout<<"\n";
		
		//counting ci values;
		ci[0]=log(R[0]*R[0]);
		for(int i=1;i<=p;i++)
		{
			long double temp_sum=0;
			for(int j=1;j<i;j++)
			{
				long double j_i = (long double)j/(long double)i;
				temp_sum += (ci[j]*ai[i-j])*j_i;
			}
			temp_sum+=ai[i];
			ci[i]=temp_sum;
		}
		vector<long double> cepstral_coeffecient_for_frame;
		for(int j=1;j<=p;j++)
		{
			ci[j]=ci[j]*liftering_window[j];
			//cout<<ci[j]<<"  ";
			cepstral_coeffecient_for_frame.push_back(ci[j]);
		}
		//cout<<"\n";
		cepstral_coeffecients.push_back(cepstral_coeffecient_for_frame);
		frame_count++;
		start+=window_size;
		end+=window_size;
		delete[] E;
		delete[] R;
		delete[] ci;
		delete[] ai;
		delete[] K;
		for(int i = 0; i < p+1; ++i)
			delete[] alpha[i];
		delete[] alpha;
	}
}


//From frame which has max_ste we consider some frames from left and some from right of it and complete total needed range of frames.
void calculate_steady_frame(vector<long double> data,vector<long double> &steady_data, long long int middle, int range)
{
	long long int start;
	long long int end;
	if(range%2!=0)
	{
		start=middle-((range/2)+1)*320;
		end=middle+(range/2)*320;
	}
	else{
		start=middle-(range/2)*320;
		end=middle+(range/2)*320;
	}
	//cout<<"Start Frame is "<<start<<" end frame is "<<end<<"\n";
	for(long long int i=start;i<end;i++)
	{
		steady_data.push_back(data[i]);
	}
	return;
}

//calculates average ceps for each frames of the vowel and appends it to the file. 
void calculate_average_cepstrals(int no_frames,int no_cepstral_coeffecients)
{
	vector<vector<long double>> avg_cepstral_coeff;
	int count=0;
	
	//*frame wise averaging of cepstrals
	while(count<no_frames)
	{
		vector<long double> sum_coeff;
		for(int j=0;j<no_cepstral_coeffecients;j++)
		{
			sum_coeff.push_back(0);			
		}

		for(int i=0;i<cepstral_coeffecients.size();i++)
		{
			if(i%no_frames==count)
			{
				//cout<<i<<"th frame in complete construction \n";
				for(int j=0;j<no_cepstral_coeffecients;j++)
				{
					sum_coeff[j]+=cepstral_coeffecients[i][j];
					cout<<cepstral_coeffecients[i][j]<<" ";
				}
				cout<<"\n";
			}
		}
		for(int j=0;j<no_cepstral_coeffecients;j++)
		{
			sum_coeff[j]=sum_coeff[j]/(long double)(cepstral_coeffecients.size()/no_frames);			
		}
		avg_cepstral_coeff.push_back(sum_coeff);
		count++;
	}
	fstream cepstrals_file;
	cepstrals_file.open("cepstral_coeffecient_file.txt",std::ios_base::app);
	if(!cepstrals_file) 
	{ 
       cout<<"Error in opening file!!!"; 
       return; 
	} 
	cout<<"After average: \n\n";
	for(int i=0;i<avg_cepstral_coeff.size();i++)
	{
		for(int j=0;j<no_cepstral_coeffecients;j++)
		{
			cout<<avg_cepstral_coeff[i][j]<<"  ";
			cepstrals_file<<avg_cepstral_coeff[i][j]<<"\n";
		}
		cout<<"\n";
	}
	cepstrals_file.close();

}

int _tmain(int argc, _TCHAR* argv[])
{
	int no_cepstral_coeffecients=12;
	int no_steady_frames=5;
	
	//intializing hamming window
	for(int i=0;i<320;i++)
	{
		long double tempp =(long double)( 0.54-(0.46*cos((2*PI*i)/319)));
		hamming_window.push_back(tempp);
	}

	//intializing liftering window
	liftering_window.push_back(-1);
	for(int i=1;i<=no_cepstral_coeffecients;i++)
	{
		long double tempp=(long double)(1+(no_cepstral_coeffecients/2)*sin((PI*i)/no_cepstral_coeffecients));
		liftering_window.push_back(tempp);
	}
	
	fstream data_file;
	int refrence_files=10;
	
	//finding cepstral coeffecient for 5 steady frames of each 10 files of a single vowel.
	for(int m=1;m<=refrence_files;m++)
	{
		vector<long double> data;
		string s1 ="204101024_u_";				// here we need to manually change path, like x=vowel and 204101024_x_
		stringstream ss;
		ss << m;
		string str = ss.str();
		s1+=str;
		string s3=".txt";
		s1+=s3;
		cout<<s1<<"\n";
		data_file.open(s1);

		//DC_shift calculation considering initial 1 sec of silence in file.
		long double temp;
		long long int counter_data = 16000;		//total sample size for 1 sec of silence in consideration.
		long long int temp_counter=counter_data;
		long double dc_shift=0.0;
		//long double silence_STE=0;
		//long double max_silence_STE=INT_MIN;
		int frame_size_counter=320;
		while(temp_counter--)
		{
			data_file>>temp;
			dc_shift+=temp;
		}

		dc_shift=dc_shift/counter_data;
		
		long double max_amplitude = -std::numeric_limits<double>::max();	
	
		//applying dc shift to whole recorded signal.
		int counter=320;
		long double max_ste=0.0;
		long double STE=0.0;
		long long int max_ste_index=0;
		while(data_file>>temp)
		{
			data.push_back(temp-dc_shift);
			STE+=(temp-dc_shift)*(temp-dc_shift);
			counter--;
			if(counter==0)
			{
				if(max_ste<STE)
				{
					max_ste=STE;								//For finding stable frame index.
					max_ste_index=data.size()-1;				//Calculating STE for each frame and finding which is max.
				}
				counter=320;
				STE=0;
			}
			long double amplitude=abs(temp-dc_shift);
			if(max_amplitude < amplitude)
			{
				max_amplitude=amplitude;							//Finding max_amp for normalization purpose
			}
		}
		data_file.close();
		//cout<<"Max amplitude obtained is: "<<max_amplitude<<"\n";
		//cout<<"max ste is "<<max_ste<<"\n";
		//cout<<"Max STE Index: "<<max_ste_index<<"\n";
	
		//Normalization
		long double max_amplitude_needed=10000;
		long double ratio_amplitude=max_amplitude_needed/max_amplitude;
		for(unsigned long long int i=0;i<data.size();i++)
		{
			data[i]=data[i]*ratio_amplitude;
		}
		
		//find steady frames
		vector<long double> steady_data;
		calculate_steady_frame(data,steady_data,max_ste_index,no_steady_frames);
		//cout<<"Steady data size = "<<steady_data.size()<<"\n";
		
		//Applying hamming window and then LPC analysis on disjoint steady frames of data.
		LPC_analysis(steady_data,no_cepstral_coeffecients);
	}
	//calculating frame wise average of all 50*12 ceps.
	calculate_average_cepstrals(no_steady_frames,no_cepstral_coeffecients);
	return 0;
}

