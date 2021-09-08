// K_means.cpp : Defines the entry point for the console application.
// Implemented by: 204101024

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<time.h>
#include<stdlib.h>
#include<vector>
#include<cmath>
#include<cstring>
#include <sstream>
using namespace std;

#define codebooksize 8

long double tokura_weights[]={1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};     //Tokuras weights
long double delta=0.00001; //Threshold value for convergence.
int M=100; //Maximum allowed iterations for K means algorithm.

vector<vector<long double> > universe_data;				//stores whole universe vectors.
vector<vector< long double> > cluster_centroids;		//stores centroid of each cluster(8*12 matrix), we can also say it as codebook.

void K_means(int data_size)
{

	//randomly select eight vectors from codebook and make it as centroid of codebook.
	srand(time(0));				//Each time random value should come.
	
	//Generate 8 random numbers and consider it to centroids.
	for(long long int i=0;i<codebooksize;i++)
	{
		long long int no = rand()%data_size;
		cluster_centroids.push_back(universe_data[no]);
	}

	long double old_D=std::numeric_limits<double>::max();
	
	for(int m=0;m<M;m++)
	{
		
		//For each vector find nearest neighbour and allocate to a cluster and calcuate overall distortion.
		vector<vector<long long int>> buckets;					//buckets vector store the index of the vector present in universe which is closest to the respected codebook vector.
		
		for(long long int i=0;i<cluster_centroids.size();i++)
		{
			vector<long long int> temp;
			buckets.push_back(temp);
		}

		long double overall_distortion=0.0;

		//iterate through all vectors in universe and for each vector find nearest neighbour and vector to its representative bucket.
		for(long long int i=0;i<data_size;i++)
		{
			long double min_distance = std::numeric_limits<double>::max();
			long long int min_index = -1;

			for(long long int j=0;j<cluster_centroids.size();j++)
			{
				long double error_sum=0.0;
				for(int k=0;k<12;k++)
				{
					error_sum+=tokura_weights[k]*(pow(cluster_centroids[j][k]-universe_data[i][k],2));		
				}
				if(min_distance>error_sum)
				{
					min_distance=error_sum;									//Updating nearest neighbour.
					min_index=j;
				}
			}
			overall_distortion+=min_distance;
			buckets[min_index].push_back(i);			//Storing index of a particular vector in bucket.
		}

		overall_distortion/=(long double)data_size;
		
		cout<<"Overall Distortion at m = "<<m<<" is "<<overall_distortion<<"\n";

		//convergence
		if(old_D - overall_distortion <= delta)
		{
			cout<<"Successfully Algorithm has converged!\n";
			return;
		}
		old_D=overall_distortion;
		
		//Find new centroids.
		//for each bucket we find centroid by taking arithmetic mean of the all the vectors in the bucket.
		for(long long int i=0;i<cluster_centroids.size();i++)
		{
			vector<long double> tempp;
			for(int k=0;k<12;k++)
			{
				tempp.push_back(0.0);
			}
			if(buckets[i].size()<=0)
			{
				cout<<"Empty Cell problem occured!\n";
				exit(0);
			}
			else
			{
				for(long long int j=0;j<buckets[i].size();j++)
				{
					vector<long double> temp=universe_data[buckets[i][j]];
					for(int k=0;k<12;k++)
					{	
						//Summing up all the vectors belong to that cluster.
						tempp[k]+=temp[k];
					}
				}
				for(long long int k=0;k<12;k++)
				{
					cluster_centroids[i][k]=tempp[k];
					cluster_centroids[i][k] /= (long double)buckets[i].size();
				}
			}
		}
		for(long long int i=0;i<buckets.size();i++)
		{
			buckets[i].clear();
		}
		buckets.clear();
	}
	return;
}


int _tmain(int argc, _TCHAR* argv[])
{
	//Read the universe file and store it in the Vector of vectors(Matrix).
	
	fstream universe_file;
	universe_file.open("universe.txt");
	int count=12;
	long double temp;
	vector<long double> cepstrals;
	int no_files=0;
	while(universe_file>>temp)
	{
		cepstrals.push_back(temp);
		count--;
		if(count==0)
		{
			universe_data.push_back(cepstrals);
			cepstrals.clear();
			count=12;
			no_files++;
		}
	}
	universe_file.close();
	
	K_means(no_files);

	cout<<"\nCodebook obtained is following: \n";
	for(long long int i=0;i<cluster_centroids.size();i++)
	{
		for(long long int j=0;j<12;j++)
		{
			cout<<cluster_centroids[i][j]<<" ";
		}
		cout<<"\n";
	}

	return 0;
}

