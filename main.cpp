#include <stdio.h>
#include <stdlib.h>
#include "MYFIFO.h"
#include "main.h"
#include <iostream>
int main(int argc, char* argv[]) {
	int S = atoi(argv[1]);
	int N = atoi(argv[2]);
	char* fname = argv[3];
	FILE* fp;
	fp = fopen(fname, "r");
	if (fp == NULL)
	{
		printf("Invalid File");
		return 0;
	}
	int tag_number = 0;
	int address, operationtype, destination, source1, source2;
	while (fscanf(fp, "%x %d %d %d %d ", &address, &operationtype, &destination, &source1, &source2) != EOF) {
		instructions.AddInstruct(address, operationtype, destination, source1, source2);
		RFList[destination].reg_ready = RFList[source1].reg_ready = RFList[source2].reg_ready = REGISTER_READY;
	}
	current_tick_number = 0;
	do
	{

		FakeRetire();
		Execute();
		Issue(N);
		Dispatch(S);
		Fetch(N);
		current_tick_number++;
		
	} while (AdvanceCycle());
	int tt_timer = current_tick_number - 1;
	printf("number of instructions = %d\n", instructions.len());
	printf("number of cycles       = %d\n", tt_timer);
	std::cout << "IPC                    = " << ((float)instructions.len()) / ((float)(tt_timer));	
	return 0;
}