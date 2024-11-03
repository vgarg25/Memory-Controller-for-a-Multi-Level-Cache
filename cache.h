#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

const int DBG_MODE = 1;

struct Stat
{
	float hitL1; 
	float hitL2; 
	float accL1;
	float accL2;
	float hitVL1; 
	float accVL1;

	int totalOps;
	int totalCycles;
	// Final Stats to be printed
	float L1_miss_rate; 
	float L2_miss_rate; 
	float AAT; 
};

struct L1CacheBlock
{
	bool valid;         		// Valid bit (true or false)
	uint32_t tag : 26;  		// 26 tag bits (using bit fields)
	uint8_t data[4];    		// Array of four 1-byte data elements
	L1CacheBlock* next;
};

struct L1VictimCacheBlock
{
	bool valid;         		// Valid bit (true or false)
	uint32_t tag : 30;  		// 30 tag bits (using bit fields)
	uint8_t data[4];    		// Array of four 1-byte data elements
	L1VictimCacheBlock* next;	// Pointer to the next element in the linked list
};

struct L2CacheBlock
{
	bool valid;         		// Valid bit (true or false)
	uint32_t tag : 29;  		// 29 tag bits (using bit fields)
	uint8_t data[4];    		// Array of four 1-byte data elements
	L2CacheBlock* next;		// Pointer to the next element in the linked list
};

struct adr_data
{
	bool hit_or_eviction;
	uint32_t adr;
	uint8_t data[4];
};

class cache 
{
	private:

		L1CacheBlock* L1_cache[16];			// LinkedList would be of one element only - 16 Entry Direct Mapped cache - Searching in this list would be using the 4 index bits
		L1VictimCacheBlock* L1_Victim_Cache[1];		// Full way associative cache with 4 entries
		L2CacheBlock* L2_cache[2];			// For 2 lines of L2 cache each of 8 entries for 8 - way Associativity - Searching in this list would be using 1 index bit

		Stat myStat;
	public:
		// Constructor
		cache();

		// Row Linked List Initialization Functions for Caches
		L1CacheBlock* l1_row_initialize();
		L1VictimCacheBlock* l1_victim_row_initialize();
		L2CacheBlock* l2_row_initialize();

		// Controller/Operation Functions
		void controller(bool MemR, bool MemW, uint32_t data, uint32_t adr, uint8_t* main_mem);
		void read_operation(uint32_t adr, uint8_t* main_mem);
		void write_operation(uint32_t adr,uint32_t data);

		// Main Memory functions
		adr_data main_mem_search(uint32_t adr, uint8_t* main_mem);
		void main_mem_write(uint32_t adr, uint32_t data, uint8_t* main_mem);

		// Victim L1 Search and L2 Search - If they hit they would remove the hit entry return hit True along with data and addr and pass to hierarchy update
		adr_data l1_search(uint32_t adr);
		adr_data l1_victim_search(uint32_t adr);
		adr_data l2_search(uint32_t adr);
		
		// Update memory hierarchy flow
		adr_data update_l1(adr_data update_entry);
		adr_data update_l1_victim(adr_data update_entry);
		void update_l2(adr_data update_entry);
		int update_hierarchy(adr_data update_entry);

		// Stats Function
		void compute_stats();
};


