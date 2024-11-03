#include <fstream>
#include <iostream>
#include <cstring>
#include <sstream>
#include <iostream>
#include <vector>
#include "cache.h"
using namespace std;

struct trace
{
	bool MemR; 
	bool MemW; 
	int adr; 
	int data; 
};


int main (int argc, char* argv[]) 
{
	// Input File 
	string filename = argv[1];
	
	ifstream fin;

	// Opening File
	fin.open(filename.c_str());
	if (!fin){ // making sure the file is correctly opened
		cout << "Error opening " << filename << endl;
		exit(1);
	}
	
	// Reading the Text File
	string line;
	vector<trace> myTrace;
	int TraceSize = 0;
	string s1,s2,s3,s4;
	while( getline(fin,line) )
      	{
            stringstream ss(line);
            getline(ss,s1,','); 
            getline(ss,s2,','); 
            getline(ss,s3,','); 
            getline(ss,s4,',');
            myTrace.push_back(trace()); 
            myTrace[TraceSize].MemR = stoi(s1);
            myTrace[TraceSize].MemW = stoi(s2);
            myTrace[TraceSize].adr = stoi(s3);
            myTrace[TraceSize].data = stoi(s4);
            TraceSize+=1;
        }


	// Defining cache and stat
	cache myCache;
	uint8_t main_mem[4096]; 


	int traceCounter = 0;
	bool cur_MemR; 
	bool cur_MemW; 
	uint32_t cur_adr;
	uint32_t cur_data;

	// Main loop of parsing LW/SW instructions and passing to cache controller
	while(traceCounter < TraceSize){
		
		cur_MemR = myTrace[traceCounter].MemR;
		cur_MemW = myTrace[traceCounter].MemW;
		cur_data = static_cast<uint32_t>(myTrace[traceCounter].data);
		cur_adr = static_cast<uint32_t>(myTrace[traceCounter].adr);
		traceCounter += 1;
		myCache.controller (cur_MemR, cur_MemW, cur_data, cur_adr, main_mem); // in your memory controller you need to implement your FSM, LW, SW, and MM. 
	}

	// Computing miss rate of L1/L2 and 
	myCache.compute_stats();

	// Closing the Input File
	fin.close();

	return 0;
}
