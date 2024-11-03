#include "cache.h"

// L1 Initilization function
L1CacheBlock* cache::l1_row_initialize()
{
	L1CacheBlock* head = nullptr;
	L1CacheBlock* current = nullptr;

	for(int i=0; i<1; ++i)
	{
		L1CacheBlock *node = new L1CacheBlock();
		node->valid = false;
		node->tag = 0;
		for(int j=0; j<4; ++j)
		{
			node->data[j] = 0;
		}
		node->next = nullptr;

		if (head == nullptr)
		{
			head = node;
			current = node;
		}
		else
		{
			current->next = node;
			current = node;
		}
	}
	
	return head;
}

// L1 Victim Initialization function
L1VictimCacheBlock* cache::l1_victim_row_initialize()
{
	L1VictimCacheBlock* head = nullptr;
	L1VictimCacheBlock* current = nullptr;

	for(int i=0; i<4; ++i)
	{
		L1VictimCacheBlock *node = new L1VictimCacheBlock();
		node->valid = false;
		node->tag = 0;
		for(int j=0; j<4; ++j)
		{
			node->data[j] = 0;
		}
		node->next = nullptr;

		if (head == nullptr)
		{
			head = node;
			current = node;
		}
		else
		{
			current->next = node;
			current = node;
		}
	}
	
	return head;
}


// L2 Initialization function
L2CacheBlock* cache::l2_row_initialize()
{
	// Initialize the first entry in Linked List as head
	L2CacheBlock* head = nullptr;

	// Referencing current
	L2CacheBlock* current = nullptr;

	// For 8-Way Associative Cache
	for(int i=0; i<8; ++i)
	{	
		// Adding new element to Linked List
		L2CacheBlock* node = new L2CacheBlock();
		
		// Initializing the variables in the struct
		node->valid = false;
		node->tag = 0;
		for(int j=0; j<4; ++j)
		{
			node->data[j] = 0;
		}
		node->next = nullptr;

		// If first element denoted by head = null, then set head and current as the node we just created, else current would point to node we just created and overwrite current with node to move LinkedList forward
		if (head == nullptr)
		{
			head = node;
			current = node;
		}
		else
		{
			current->next = node;
			current = node;
		}
	}

	return head;
}

// Cache clas constructor initializing the caches and stats variables
cache::cache()
{

	// Initializing the caches 
	// L1 Cache
	for(int i=0; i<16; ++i)
	{
		L1_cache[i] = l1_row_initialize();
	}
	// L1Victim Cache
	for(int i=0; i<1; ++i)
	{
		L1_Victim_Cache[i] = l1_victim_row_initialize();
	}
	// L2 Cache
	for(int i=0; i<2; ++i)
	{
		L2_cache[i] = l2_row_initialize();
	}

	// Initializing the cache variables
	this->myStat.hitL1 = 0;
	this->myStat.hitL2 = 0;
	this->myStat.hitVL1 = 0;
	this->myStat.accL1 = 0;
	this->myStat.accL2 = 0;
	this->myStat.accVL1 = 0;
	this->myStat.totalOps = 0;
	this->myStat.totalCycles = 0;
}

// Main controller block
void cache::controller(bool MemR, bool MemW, uint32_t data, uint32_t adr, uint8_t* main_mem)
{
	// Read Condition
	if(MemR == 1 && MemW == 0)
		{
			if (DBG_MODE == 1) { cout << endl << "Read Operation, Address - " << adr << endl; }
			read_operation(adr, main_mem);

		}
	// Write Condition
	else if(MemR == 0 && MemW == 1)
		{	
			if (DBG_MODE == 1) { cout << endl << "Write Operation, Address - " << adr << ", Data - " << data << endl; }
			write_operation(adr, data);
			main_mem_write(adr, data, main_mem);

		}
	else
		{
			cout << "Invalid Input" << endl;
		}
}


// Read operation through the memory hierarchy
void cache::read_operation(uint32_t adr, uint8_t* main_mem)
{	
	// L1 CACHE
	myStat.accL1++;
	adr_data l1_hit = l1_search(adr);
	if (l1_hit.hit_or_eviction == true) 
	{
		if(DBG_MODE == 1) { cout << " Entry found in L1 " << l1_hit.adr << endl;}
		myStat.hitL1++;
		update_hierarchy(l1_hit);
		return;
	}	
	
	// Victim L1 CACHE
	myStat.accVL1++;
	adr_data l1_victim_hit = l1_victim_search(adr);
	if (l1_victim_hit.hit_or_eviction == true)
	{
		if(DBG_MODE == 1) { cout << " Entry found in L1 Victim " << l1_victim_hit.adr <<endl;}
		myStat.hitVL1++;
		update_hierarchy(l1_victim_hit);
		return;
	}

	// L2 CACHE
	myStat.accL2++;
	adr_data l2_hit = l2_search(adr);
	if (l2_hit.hit_or_eviction == true)
	{
		if(DBG_MODE == 1) { cout << " Entry found in L2 " << l2_hit.adr << endl;}
		myStat.hitL2++;
		update_hierarchy(l2_hit);
		return;
	}

	// MAIN MEMORY
	adr_data main_mem_hit = main_mem_search(adr, main_mem);
	if(DBG_MODE == 1) { cout << " Entry found in Main Mem " << main_mem_hit.adr << endl;}
	update_hierarchy(main_mem_hit);
}

// Write operation
void cache::write_operation(uint32_t adr, uint32_t data)
{
	uint32_t temp_data[4];
	temp_data[0] = (data & 0xFF);	
	temp_data[1] = (data & 0xFF00) >> 8;
	temp_data[2] = (data & 0xFF0000) >> 16;
	temp_data[3] = (data & 0xFF000000) >> 24;

	// L1 Cache
	adr_data l1_hit = l1_search(adr);
	if (l1_hit.hit_or_eviction == true)
	{	
		// Writing new Data
		for (int i=0; i<4; ++i)
		{
			l1_hit.data[i] = temp_data[i];
		}
		update_hierarchy(l1_hit);
		return;
	}

	// Victim L1 Cache
	adr_data l1_victim_hit = l1_victim_search(adr);
	if (l1_victim_hit.hit_or_eviction == true)
	{	
		// Writing new Data
		for (int i=0; i<4; ++i)
		{
			l1_victim_hit.data[i] = temp_data[i];
		}
		
		// Update Victim L1
		adr_data victim_l1_evicted_entry = update_l1_victim(l1_victim_hit);
		if (victim_l1_evicted_entry.hit_or_eviction == false) 
		{ return; } 

		// Update L2 - Added for safety - Unlikely to reach here
		cout << "Debug - Write - Victim Hit - Update L2" << endl;
		update_l2(victim_l1_evicted_entry);
		return;
	}

	// L2 Cache
	adr_data l2_hit = l2_search(adr);
	if (l2_hit.hit_or_eviction == true)
	{	
		// Writing new Data
		for (int i=0; i<4; ++i)
		{
			l2_hit.data[i] = temp_data[i];
		}

		// Update L2
		update_l2(l2_hit);
	}
}

// Searching for address in the L1 cache
adr_data cache::l1_search(uint32_t adr)
{
	uint32_t index = ((adr>>2) & 0x0000000F);
	uint32_t tag = ((adr>>6) & 0x03FFFFFF);

	adr_data l1_hit = {false, adr, {0}};

	L1CacheBlock* current = L1_cache[index];
	L1CacheBlock* prev = nullptr;
	while (current != nullptr)
	{
		if ((current->valid == true) && (current->tag == tag))
		{
			// Found entry in L1 Cache
			l1_hit.hit_or_eviction = true;
			for(int i = 0; i < 4; ++i)
			{
				l1_hit.data[i] = current->data[i];
				// cout << current->data[i] << endl;
			}

			// Removing the element from the Linked List
			if (prev == nullptr) { L1_cache[index] = current->next; }
			else { prev->next = current->next; }


			return l1_hit;
		}
		prev = current;
		current = current->next;
	}

	return l1_hit;
}

// Searching for address in L1 Victim Cache
adr_data cache::l1_victim_search(uint32_t adr)
{
	uint32_t tag = ((adr>>2) & 0x3FFFFFFF);
	uint32_t index = 0;
	adr_data l1_victim_hit = {false, adr, {0}};

	L1VictimCacheBlock* current = L1_Victim_Cache[index];
	L1VictimCacheBlock* prev = nullptr;
	while (current != nullptr)
	{
		if ((current->valid == true) && (current->tag == tag))
		{
			// Found entry in L1 Victim Cache 
			l1_victim_hit.hit_or_eviction = true;
			for(int i = 0; i < 4; ++i)
			{	
				l1_victim_hit.data[i] = current->data[i];
				// cout << current->data[i] << endl;
			}
			
			// Removing the element from the Linked List
			if (prev == nullptr) { L1_Victim_Cache[index] = current->next; }
			else { prev->next = current->next; }
			
			// Returning Hit Status, adr and data
			return l1_victim_hit;
		}
		prev = current;
		current = current->next;
	}
	return l1_victim_hit;
}

// Searching address in L2 Cache
adr_data cache::l2_search(uint32_t adr)
{
	uint32_t tag = ((adr>>3) & 0x1FFFFFFF);
	uint32_t index = ((adr>>2) & 0x00000001);

	adr_data l2_hit = {false, adr, {0}};

	L2CacheBlock* current = L2_cache[index];
	L2CacheBlock* prev = nullptr;
	while (current != nullptr)
	{
		if ((current->valid == true) && (current->tag == tag))
		{
			// Found entry in L2 Cache 
			l2_hit.hit_or_eviction = true;
			for(int i = 0; i < 4; ++i)
			{	
				l2_hit.data[i] = current->data[i];
				// cout << current->data[i] << endl;
			}

			// Removing the element from the Linked List
			if (prev == nullptr) { L2_cache[index] = current->next; }
			else { prev->next = current->next; }

			// Returning Hit Status, adr and data
			return l2_hit;
		}
		prev = current;
		current = current->next;
	}
	return l2_hit;
}

// Final read search in main mem
adr_data cache::main_mem_search(uint32_t adr, uint8_t* main_mem)
{
	adr = adr & 0xFFFFFFFC;
	adr_data main_mem_hit = {true, adr, {0}};
	for (int i = 0; i < 4; ++i)
	{
		main_mem_hit.data[i] = main_mem[adr + i];
	}

	return main_mem_hit;
}

// Writing in main mem
void cache::main_mem_write(uint32_t adr, uint32_t data, uint8_t* main_mem)
{
	adr = adr & 0xFFFFFFFC;
	
	uint32_t temp_data[4];
	temp_data[0] = (data & 0xFF);	
	temp_data[1] = (data & 0xFF00) >> 8;
	temp_data[2] = (data & 0xFF0000) >> 16;
	temp_data[3] = (data & 0xFF000000) >> 24;

	for (int i = 0; i < 4; ++i)
	{
		main_mem[adr + i] = temp_data[i];
	}
}

adr_data cache::update_l1(adr_data update_entry)
{

	if (DBG_MODE == 1) { cout << " Update L1 Entry, Hit or Eviction " << update_entry.hit_or_eviction << " Address " << update_entry.adr << endl; }

	uint32_t tag = ((update_entry.adr>>6) & 0x03FFFFFF); 
	uint32_t index = ((update_entry.adr>>2) & 0x0000000F); 
	
	// New Entry
	L1CacheBlock* node = new L1CacheBlock();
	node->valid = true;
	node->tag = tag;
	for (int i = 0; i < 4; ++i)
	{
		node->data[i] = update_entry.data[i];
	}
	
	// Add the entry into the LRU row
	node->next = L1_cache[index];
	L1_cache[index] = node;

	// Remove LRU entry if num elements grater than 1(In L1)
	int num_entries = 1;
	adr_data l1_evicted_entry = {false, 0, {0}};
	L1CacheBlock* current = L1_cache[index];
	L1CacheBlock* prev = nullptr;
	while (current != nullptr)
	{
		if (num_entries > 1)
		{
			prev->next = nullptr;
			l1_evicted_entry.hit_or_eviction = current->valid;
			l1_evicted_entry.adr = (current->tag)<<6 | index<<2 ;
			for (int i = 0; i < 4; ++i)
			{
				l1_evicted_entry.data[i] = current->data[i];
			}
			break;
		}
		
		num_entries++;
		prev = current;	
		current = current->next;
	}

	return l1_evicted_entry;

}

adr_data cache::update_l1_victim(adr_data update_entry)
{
	if (DBG_MODE == 1) { cout << " Update L1 Victim Entry, Hit or Eviction " << update_entry.hit_or_eviction << " Address " << update_entry.adr << endl; }

	uint32_t tag = ((update_entry.adr>>2) & 0x3FFFFFFF);
	uint32_t index = 0;
	// New Entry
	L1VictimCacheBlock* node = new L1VictimCacheBlock();
	node->valid = true;
	node->tag = tag;
	for (int i = 0; i < 4; ++i)
	{
		node->data[i] = update_entry.data[i];
	}

	// Add the entry into the LRU row
	node->next = L1_Victim_Cache[index];
	L1_Victim_Cache[index] = node;

	// Remove LRU entry if num elements greater than 4
	int num_entries = 1;
	adr_data victim_l1_evicted_entry = {false, 0, {0}};
	L1VictimCacheBlock* current = L1_Victim_Cache[index];
	L1VictimCacheBlock* prev = nullptr;
	while (current != nullptr)
	{
		if (num_entries > 4)
		{
			prev->next = nullptr;
			victim_l1_evicted_entry.hit_or_eviction = current->valid;
			victim_l1_evicted_entry.adr = (current->tag)<<2;
			for (int i = 0; i < 4; ++i)
			{
				victim_l1_evicted_entry.data[i] = current->data[i];
			}
			break;
		}

		num_entries++;
		prev = current;
		current = current->next;
	}

	return victim_l1_evicted_entry;
}

void cache::update_l2(adr_data update_entry)
{
	if (DBG_MODE == 1) { cout << " Update L2 Entry, Hit or Eviction " << update_entry.hit_or_eviction << " Address " << update_entry.adr << endl; }

	uint32_t tag = ((update_entry.adr>>3) & 0x1FFFFFFF);
	uint32_t index = ((update_entry.adr>>2) & 0x00000001);

	// New Entry
	L2CacheBlock* node = new L2CacheBlock();
	node->valid = true;
	node->tag = tag;
	for (int i = 0; i < 4; ++i)
	{
		node->data[i] = update_entry.data[i];
	}

	// Add the entry into the LRU Row
	node->next = L2_cache[index];
	L2_cache[index] = node;
	
	// Remove LRU entry if num elements is greater than 8
	int num_entries = 1;
	L2CacheBlock* current = L2_cache[index];
	L2CacheBlock* prev = nullptr;
	while (current != nullptr)
	{
		if (num_entries > 8)
		{
			prev->next = nullptr;
			// L2 Evicted entry is discarded
			break;
		}

		num_entries++;
		prev = current;
		current = current->next;
	}
}

// Updating full hierarchy from L1 with domino to L2
int cache::update_hierarchy(adr_data update_entry)
{
	// Update L1
	adr_data l1_evicted_entry = update_l1(update_entry);
	if (DBG_MODE == 1) { cout << " L1 Evicted Entry (" << l1_evicted_entry.hit_or_eviction << "," << l1_evicted_entry.adr << ")" << endl; }
	if (l1_evicted_entry.hit_or_eviction == false) { return 0; }

	// Update Victim L1
	adr_data victim_l1_evicted_entry = update_l1_victim(l1_evicted_entry);
	if (DBG_MODE == 1) { cout << " L1 Victim Evicted Entry (" << victim_l1_evicted_entry.hit_or_eviction << "," << victim_l1_evicted_entry.adr << ")" << endl; }
	if (victim_l1_evicted_entry.hit_or_eviction == false) { return 0; } 

	// Update L2
	update_l2(victim_l1_evicted_entry);
	return 0;

}

void cache::compute_stats()
{	
	myStat.L1_miss_rate = (myStat.accL1 - myStat.hitL1)/(myStat.accL1);
	myStat.L2_miss_rate = (myStat.accL2 - myStat.hitL2)/(myStat.accL2);
	
	cout<<myStat.accL1<<endl<<myStat.hitL1<<endl;
	cout<<myStat.accVL1<<endl<<myStat.hitVL1<<endl;
	cout<<myStat.accL2<<endl<<myStat.hitL2<<endl;
	cout<< "(" << myStat.L1_miss_rate<<","<<myStat.L2_miss_rate<<","<<myStat.AAT<<")"<<endl;
}