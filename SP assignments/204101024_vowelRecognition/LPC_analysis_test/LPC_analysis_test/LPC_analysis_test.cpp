// LPC_analysis_test.cpp : Defines the entry point for the console application.


#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<vector>
#include<cmath>
#include<cstring>
#include <sstream>

using namespace std;

#define PI 3.14159265

//hamming window
vector<long double> hamming_window;

//liftering window
vector<long double> liftering_window;


//cepstral coeffecients all for a single vowel.
vector<vector<long double>> cepstral_coeffecients;

//To store cepstral coeffecients from refrence file.
vector< vector<long double>> cepstral_coeff_ref;

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
			//cout<<ci[i]<<" ";
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


//calculates tokura distance of a ceps of each frame with refrence ceps and predicts the vowel which has minimum tokura distance. 
char calculate_min_tokura(int no_steady_frames,int no_cepstral_coeff)
{
	// we have refrence ceps, we have generated ceps, now for each vowel we need to compute tokura for each 5 frames and average them and compare with minimum.
	long double total_error=0;
	long double min_error=std::numeric_limits<double>::max();
	int store_vowel_index;
	int counter=0;
	long double tokura_weights[]={1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};
	for(int i=0;i<cepstral_coeff_ref.size();i++)
	{
		long double error_sum=0;
		for(int j=0;j<no_cepstral_coeff;j++)
		{
			error_sum+=tokura_weights[j]*(pow(cepstral_coeffecients[counter][j]-cepstral_coeff_ref[i][j],2));
		}
		total_error+=error_sum;
		error_sum=0;
		counter++;
		if(counter==no_steady_frames)
		{
			total_error/=no_steady_frames;
			//cout<<"Total error obtained in vowel "<<i/no_steady_frames<<"  "<<total_error<<"\n";
			if(min_error>total_error)
			{
				min_error=total_error;
				store_vowel_index=i;
				//cout<<"Stored Vowel Index changed\n";
			}
			total_error=0;
			counter=0;
		}
	}
	store_vowel_index/=no_steady_frames;
	char predicted;
	//cout<<"Minimum error obtained: "<<min_error;
	//cout<<"store vowel index: "<<store_vowel_index<<"\n";
	switch(store_vowel_index)
	{
	case 0: predicted='a';
			break;
	case 1: predicted='e';
			break;
	case 2: predicted='i';
			break;
	case 3: predicted='o';
			break;
	case 4: predicted='u';
			break;
	default: predicted='z';
			cout<<"Nothing predicted\n";
	}
	return predicted;
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
	int flag=0;

	//handling condition if there is no frame on left or right
	while(start<0)
	{
		start+=320;
		end+=320;
		flag=1;
	}
	while(end>=data.size())
	{
			if(flag==1)
			{
				cout<<"Improper recording\n";
				return;
			}
			end-=320;
			start-=320;
	}

	for(long long int i=start;i<end;i++)
	{
		steady_data.push_back(data[i]);
	}
	return;
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

	//initializing liftering window
	liftering_window.push_back(-1);
	for(int i=1;i<=no_cepstral_coeffecients;i++)
	{
		long double tempp=(long double)(1+(no_cepstral_coeffecients/2)*sin((PI*i)/no_cepstral_coeffecients));
		liftering_window.push_back(tempp);
	}

	//copying all the reference ceps in a matrix, which help in easy computation of tokura's distance.
	fstream cepstrals_file;
	cepstrals_file.open("cepstral_coeffecient_file.txt");
	int counterr=0;
	vector<long double> cepstral_coeff_ref_frame;
	//cout<<"Cepstral coeffecients from refrence files are: \n\n";
	long double tempp;
	while(cepstrals_file>>tempp)
	{
		cepstral_coeff_ref_frame.push_back(tempp);
		//cout<<tempp<<"  ";
		counterr++;
		if(counterr%no_cepstral_coeffecients==0)
		{
			//cout<<"\n";
			cepstral_coeff_ref.push_back(cepstral_coeff_ref_frame);
			cepstral_coeff_ref_frame.clear();
			counterr=0;
		}
	}
	cepstrals_file.close();

	fstream data_file;
	
	while(1)
	{
		int test_files=20;
		long double accuracy=0;
		long double no_test_files=10;
		
		cout<<"\nWhich vowel do you want to test?\n";
		cout<<"Press 1 for a,\nPress 2 for e,\nPress 3 for i, \nPress 4 for o, \nPress 5 for u, \nPress 6 for manual testing, \nPress 0 for Quit!\n";
		int ch;
		cin>>ch;
		char vowel;
		switch(ch)
		{
		case 1: vowel='a';
				break;
		case 2: vowel='e';
				break;
		case 3: vowel='i';
				break;
		case 4: vowel='o';
				break;
		case 5: vowel='u';
				break;
		case 6: vowel='m';
				test_files=11;
				no_test_files=1;
				break;
		case 0: return 0;
			
		default: return 0;
		}
		for(int m=11;m<=test_files;m++)
		{
			vector<long double> data;
			//does correct naming of file 
			string s1 ="204101024_";
			string s4(1,vowel);
			string s2 ="_";
			s1+=s4;
			s1+=s2;
			stringstream ss;
			ss << m;
			string str = ss.str();
			s1+=str;
			string s3=".txt";
			s1+=s3;
			cout<<s1<<"\n";
			
			//if live recording is chosen option
			if(vowel=='m')
				system("Recording_Module.exe 4 204101024_m_11.wav 204101024_m_11.txt");
			data_file.open(s1);

			//Calculating DC Shift
			long double temp;
			long long int counter_data = 16000;		//1 sec of silence in consideration
			long long int temp_counter=counter_data;
			long double dc_shift=0.0;
			int frame_size_counter=320;
			while(temp_counter--)
			{
				data_file>>temp;
				dc_shift+=temp;
			}
			dc_shift=dc_shift/counter_data;
			
			
			//Applying dc_shift and finding maximum amplitude and max_ste disjoint frame index.
			long double max_amplitude=-std::numeric_limits<double>::max();	
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
						max_ste=STE;
						max_ste_index=data.size()-1;
					}
					counter=320;
					STE=0;
				}
			
				long double amplitude=abs(temp-dc_shift);
				if(max_amplitude < amplitude)
				{
					max_amplitude=amplitude;
				}
			}
			data_file.close();

			//cout<<"Max amplitude obtained is: "<<max_amplitude<<"\n";
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
			if(steady_data.size()!=0)
			{
				//Do LPC analysis and calculate ceps on steady data frames.
				LPC_analysis(steady_data,no_cepstral_coeffecients);
				
				//calculate tokura weights with respect to refrence files and one with minimum is returned.
				char predicted=calculate_min_tokura(no_steady_frames,no_cepstral_coeffecients);
				
				cout<<predicted<<" is predicted\n";
				if(predicted==vowel)
					accuracy+=1;
				cepstral_coeffecients.clear();
			}
		}
		if(vowel!='m')
			cout<<"Accuracy is :"<<(accuracy/no_test_files)*100<<endl;
	}

	return 0;
}
