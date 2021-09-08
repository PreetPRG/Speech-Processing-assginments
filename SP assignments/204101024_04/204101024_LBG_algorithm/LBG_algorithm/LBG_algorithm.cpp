// LBG_algorithm.cpp : Defines the entry point for the console application.
// Implemented by: 204101024

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<time.h>
#include<stdlib.h>
#include<vector>
#include<cmath>

using namespace std;

#define codebooksize 8		//Codebook size

long double tokura_weights[]={1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};  //Tokura weights
long double delta=0.00001;	//Convergence criteria for Kmeans
int no_iteration=30;	//Maximum number of iteration allowed for Kmeans.
long double epsilon=0.03; //Parameter for splitting codebook.

vector<vector<long double> > universe_data;				//Vector of vector stores whole universe data.

vector<vector<long double>> cluster_centroids;			//Stores centroid of each cluster, or we can say it stores codebook.


void K_means(long long int data_size)
{
	
	long double old_D=std::numeric_limits<double>::max();
	
	for(int m=0;m<no_iteration;m++)
	{
		
		//For each vector find nearest neighbour and allocate to a cluster and calcuate overall distortion.
		vector<vector<long long int>> buckets;
		for(int i=0;i<cluster_centroids.size();i++)
		{
			vector<long long int> temp;
			buckets.push_back(temp);
		}

		long double overall_distortion=0.0;
		
		//iterate through all vectors in universe and for each vector find nearest neighbour and vector to its representative bucket.
		for(int i=0;i<data_size;i++)
		{
			long double min_distance = std::numeric_limits<double>::max();
			int min_index=-1;

			for(int j=0;j<cluster_centroids.size();j++)
			{
				long double error_sum=0.0;
				for(int k=0;k<12;k++)
				{
					error_sum+=(long double)tokura_weights[k]*(pow(cluster_centroids[j][k]-universe_data[i][k],2));		
				}
				if(min_distance > error_sum)
				{
					min_distance=error_sum;									//Updating nearest neighbour.
					min_index=j;
				}
			}
			overall_distortion += min_distance;
			buckets[min_index].push_back(i);			//Storing index of a particular vector in bucket.
		}

		overall_distortion/=(long double)data_size;
		cout<<"Overall Distortion at m = "<<m<<" is "<<overall_distortion<<"\n";
		
		if(old_D - overall_distortion <= delta)
		{
			return;
		}
		old_D=overall_distortion;
		
		//Find new centroids.
		for(int i=0;i<cluster_centroids.size();i++)
		{
			vector<long double> tempp;
			for(int k=0;k<12;k++)
			{
				tempp.push_back(0);
			}

			if(buckets[i].size()<=0)
			{
				cout<<"Empty Cell problem occured!\n";
				exit(0);
			}
			else
			{
				for(int j=0;j<buckets[i].size();j++)
				{
					vector<long double> temp=universe_data[buckets[i][j]];
					for(int k=0;k<12;k++)
					{
						tempp[k]+=temp[k];							//Summing up all the vectors belong to that cluster.
					}
				}
				for(int k=0;k<12;k++)
				{
					cluster_centroids[i][k]=tempp[k];
					cluster_centroids[i][k] /= (long double)buckets[i].size();
				}
			}
		}
		for(int i=0;i<buckets.size();i++)
		{
			buckets[i].clear();
		}
		buckets.clear();
	}
}


void LBG_algorithm(long long int data_size)
{
	//Make intial codebook with size=1 and consider centroid of whole data as its entry.	
	cluster_centroids.clear();

	vector<long double> centroid;
	for(int i=0;i<12;i++)
	{
		centroid.push_back(0.0);
	}
	for(int i=0;i<data_size;i++)
	{
		for(int j=0;j<12;j++)
		{
			centroid[j]+=universe_data[i][j];	
		}
	}
	//average
	for(int i=0;i<12;i++)
	{
		centroid[i]/=data_size;
	}

	cluster_centroids.push_back(centroid);
	int m=1;
	while(m*2<=codebooksize)
	{
		cout<<"Codebook of size = "<<m<<" is :\n";
		for(int i=0;i<cluster_centroids.size();i++)
		{
			for(int j=0;j<12;j++)
			{
				cout<<cluster_centroids[i][j]<<" ";
			}
			cout<<"\n";
		}
		
		//creating new set of codebook by splitting codebook in 2 parts based on epsilon parameter.
		for(int i=0;i<m;i++)
		{
			vector<long double> temp1;
			vector<long double> temp2;

			//split in two parts.
			for(int j=0;j<12;j++)
			{
				temp1.push_back((long double)cluster_centroids[0][j]*(long double)(1.0 + epsilon));
				temp2.push_back((long double)cluster_centroids[0][j]*(long double)(1.0 - epsilon));
			}

			cluster_centroids.push_back(temp1);
			cluster_centroids.push_back(temp2);
			
			temp1.clear();
			temp2.clear();
			cluster_centroids.erase(cluster_centroids.begin());
		}
		m=m*2;
		cout<<"Applying k means at codebook size = "<<m<<"\n";
		K_means(data_size);
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	//Read the universe file and store it in the Vector of vectors(Matrix).
	
	fstream universe_file;
	universe_file.open("universe.txt");
	int count=12;
	long double temp;
	vector<long double> cepstrals;
	long long int no_files=0;
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
	LBG_algorithm(no_files);
	cout<<"Final Codebook is: \n";
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

