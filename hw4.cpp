#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <bitset>

using namespace std;

struct TableEntry{
	int valid;
	int permission;
	int ppn;
	int use;
};

struct Page{
	int vaddr_size;
	int paddr_size;
	int page_size;
	vector<TableEntry> entries;
};


int main(int argc, char *argv[]){
	int input;  // file input from page table file
	int count = 0;  // position in page table file
	struct TableEntry *tableEntry;
	struct Page page;
	
	ifstream pageTableFile (argv[1], ifstream::in);
	
	// Read in page table file
	while(pageTableFile >> input){
		switch(count){
			case 0:
				page.vaddr_size = input;
				break;
			case 1:
				page.paddr_size = input;
				break;
			case 2:
				page.page_size = input;
				break;
			case 3:
				// start of line
				tableEntry = new TableEntry();
				tableEntry->valid = input;
				break;
			case 4:
				tableEntry->permission = input;
				break;
			case 5:
				tableEntry->ppn = input;
				break;
			case 6:
				// last
				tableEntry->use = input;
				count = 2;  // reset line
				page.entries.push_back(*tableEntry);
				break;
		}
		
		count++;
	}
	
	
	// Read in vaddr and print paddr
	int value;
	string sinput;  // input as string
	int clockPos = 0;
	
	while(cin >> sinput){
		istringstream converter(sinput);  // for converting to hex/dec
		if(sinput.length() >= 2 && sinput[0] == '0' && sinput[1] == 'x'){ // check if hex
			converter >> hex >> value;
		} else {
			converter >> dec >> value;
		}
		
		// convert to binary
		bitset<64> binnum (value);
		string addr = binnum.to_string().substr(64-page.vaddr_size); // cut extra bits + convert to str
		
		// calculate vpn, ppn, and offset
		int vpnBitSize = (page.entries.size() / 2) + (page.entries.size() % 2); // number of bits in vpn
		string vpnstr = addr.substr(0,vpnBitSize); // select vpn bits
		string offsetstr = addr.substr(vpnBitSize); // select offset bits
		bitset<64> vpnbin (vpnstr); // convert vpn to binary
		bitset<64> offsetbin (offsetstr); // convert offset to binary
		int vpn = vpnbin.to_ulong(); // convert vpn to int
		int ppn = page.entries[vpn].ppn; // select the correct table entry
		int offset = offsetbin.to_ulong(); // convert offset to int
		int pbase = ppn * page.page_size; // calculate page base
		
		
		// print correct output		
		if(page.entries[vpn].valid == 1 && page.entries[vpn].permission == 1)
			cout << pbase + offset << endl;
		else if(page.entries[vpn].permission == 1){
			#ifdef PROB1
			cout << "DISK" << endl;
			#else
			cout << "PAGE FAULT" << endl;
			while(page.entries[clockPos].use == 1){
				page.entries[clockPos].use = 0;
				clockPos = (clockPos + 1) % page.entries.size();
			}
			ppn = page.entries[clockPos].ppn;
			pbase = ppn * page.page_size;
			cout << pbase + offset << endl;
			#endif
		}
		else
			cout << "SEGFAULT" << endl;
		
	}
	
	return 0;
}