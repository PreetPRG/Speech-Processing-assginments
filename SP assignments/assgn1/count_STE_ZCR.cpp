#include<iostream>
#include<fstream>
using namespace std;

long long int Count_ZCR(fstream &myfile)
{
    long long int temp,zcr=0;
    int count=320;
    int smaple_no=0;
    long long int avg_zcr=0;
    bool pos=true;
    while (myfile>>temp)
    {
        if(temp>=0 and !pos)
        {
            zcr++;
            pos=true;
        }
        else if (temp<0 and pos)
        {
            zcr++;
            pos=false;
        }
        //zcr++;
        count--;
        if(count==0)
        {
            smaple_no++;
            avg_zcr+=zcr;
            zcr=0;
            count=320;
        }
    }
    avg_zcr=avg_zcr/smaple_no;
    //cout<<"ZCR value is: "<<zcr;
    return avg_zcr;
}

int main()
{
    long long int temp,sum=0;
    fstream myfile;
    myfile.open("D:/Materials/IITG_sem1/SP/demo_sounds/vowel_aa.txt");
    
    long long int average=0;
    int count=320;
    int sample_no=0;
    while(myfile>>temp)
    {
        cout<<temp<<"\n";
        // sum=sum+temp*temp;   
        // count--;
        // if(count==0)
        // {
        //     sum=sum/320;
        //     average+=sum;
        //     cout<<"For sample no: "<<sample_no<<" STE for vowel aa is: "<<sum<<"\n";
        //     sum=0;
        //     count=320;
        //     sample_no++;
        // }
    }
    myfile.close();
    // average=average/(sample_no+1);
    // cout<<"average STE for vowel is: "<<average<<endl;

    // myfile.open("D:/Materials/IITG_sem1/SP/demo_sounds/vowel_aa.txt");
    // cout<<"ZCR value is "<<Count_ZCR(myfile);
    // myfile.close();
    return 0;
}