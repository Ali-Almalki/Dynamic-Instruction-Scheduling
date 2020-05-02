#pragma once
#ifndef MAIN_H
#include "MYFIFO.h"
#include "InstructionQueue.h"
#define UNKNOWN -1
#define IF 0
#define ID 1
#define IS 2
#define EX 3
#define WB 4

#define REGISTER_NOT_READY 0
#define REGISTER_READY 1


struct register_file_info_t {
	int reg_ready;
	int tag_number;
};
MYFIFO FAKEROB, dispatch_list, issue_list, exec_list;
InstructionQueue instructions;
int current_tick_number = 0;
register_file_info_t RFList[256];
void FakeRetire() {
	// Remove instructions from the head of     
	// the fake-ROB until an instruction is       
	// reached that is not in the WB state	
	Node* t = FAKEROB.First();
	while (t != NULL)
	{
		instruct_node* tmp = instructions.At(t->value);
		instruct_info instruct = tmp->value;
		if (instruct.STAGE != WB)
			break;
		printf("%d fu{%d} src{%d,%d} dst{%d} IF{%d,%d} ID{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,%d}\n",
			instruct.tag_number, instruct.operation_type, instruct.source1.reg_number, instruct.source2.reg_number, instruct.destination.reg_number, instruct.cycle_info.fetch.start_timer,
			instruct.cycle_info.fetch.duration, instruct.cycle_info.dispatch.start_timer, instruct.cycle_info.dispatch.duration, instruct.cycle_info.issue.start_timer,
			instruct.cycle_info.issue.duration, instruct.cycle_info.execute.start_timer, instruct.cycle_info.execute.duration,
			instruct.cycle_info.writeback.start_timer,instruct.cycle_info.writeback.duration
		);
		Node* t1 = t->next;
		FAKEROB.Remove(t);
		t = t1;
	}
}
void Execute() {
	Node* t = exec_list.First();
	while (t != NULL) {		
		instruct_node* instr_node = instructions.At(t->value);
		instruct_info* instruct = &(instr_node->value);
		Node* t1 = t;
		t = t->next;
		if (instruct->cycle_info.execute.start_timer == current_tick_number - instruct->operation_time) {
			instruct->STAGE = WB;
			instruct->cycle_info.writeback.start_timer = current_tick_number;
			instruct->cycle_info.writeback.duration = 1;
			if (instruct->destination.reg_number != -1) {
				int dst = instruct->destination.reg_number;
				Node* issue_tmp = issue_list.First();
				while (issue_tmp != NULL) {
					int issue_tag = issue_tmp->value;
					instruct_node* instr_node1 = instructions.At(issue_tag);
					instruct_info* tmp_inst = &(instr_node1->value);
					if (tmp_inst->source1.tag_number == instruct->tag_number)
						tmp_inst->source1.reg_ready = REGISTER_READY;
					if (tmp_inst->source2.tag_number == instruct->tag_number)
						tmp_inst->source2.reg_ready = REGISTER_READY;
					issue_tmp = issue_tmp->next;
				}
				if (RFList[dst].tag_number == instruct->tag_number) {
					RFList[dst].reg_ready = REGISTER_READY;
				}
			}
			exec_list.Remove(t1);
		}
	}
}
void Issue(int N) {
	Node* issue_tmp = issue_list.First();
	int numberOfAddedNode = N+1;
	while (issue_tmp != NULL) {
		if (numberOfAddedNode == 0) return;
		int tag_number = issue_tmp->value;
		instruct_node* instr_node = instructions.At(tag_number);
		instruct_info* instruct = &(instr_node->value);
		Node* tmp = issue_tmp;
		issue_tmp = issue_tmp->next;
		if (instruct->source1.reg_ready == REGISTER_READY && instruct->source2.reg_ready == REGISTER_READY) {
			exec_list.Add(tag_number);			
			issue_list.Remove(tmp);
			instruct->cycle_info.issue.duration = current_tick_number - instruct->cycle_info.issue.start_timer;
			instruct->cycle_info.execute.start_timer = current_tick_number;
			instruct->STAGE = EX;
			numberOfAddedNode = numberOfAddedNode - 1;
		}		
	}
}
void Dispatch(int S) {
	
	Node* disptach_tmp = dispatch_list.First();
	MYFIFO tmp_list;

	while (disptach_tmp != NULL) {
		int tag_number = disptach_tmp->value;
		instruct_node* instr_node = instructions.At(tag_number);
		instruct_info* instruct = &(instr_node->value);
		Node* tmp = disptach_tmp;
		disptach_tmp = disptach_tmp->next;
		if (instruct->STAGE == ID) {
			tmp_list.Add(tag_number);			
		}
		else {
			instruct->STAGE = ID;
			instruct->cycle_info.dispatch.start_timer = current_tick_number;
			instruct->cycle_info.fetch.duration = current_tick_number - instruct->cycle_info.fetch.start_timer;
		}
	}
	if (issue_list.len() >= S) return;
	tmp_list.Sort();
	Node* tmp = tmp_list.First();
	while (issue_list.len() < S && tmp!=NULL) {
		int tag_number = tmp->value;
		instruct_node* instr_node = instructions.At(tag_number);
		instruct_info* instruct = &(instr_node->value);
		
		if (instruct->source1.reg_number != -1) {
			int tt_number = instruct->source1.reg_number;
			if (RFList[tt_number].reg_ready == REGISTER_READY)
				instruct->source1.reg_ready = REGISTER_READY;
			else
				instruct->source1.reg_ready = REGISTER_NOT_READY;
			instruct->source1.tag_number = RFList[tt_number].tag_number;
		}else
			instruct->source1.reg_ready = REGISTER_READY;

		if (instruct->source2.reg_number != -1) {
			int tt_number = instruct->source2.reg_number;
			if (RFList[tt_number].reg_ready == REGISTER_READY)
				instruct->source2.reg_ready = REGISTER_READY;
			else
				instruct->source2.reg_ready = REGISTER_NOT_READY;
			instruct->source2.tag_number = RFList[tt_number].tag_number;
		}
		else
			instruct->source2.reg_ready = REGISTER_READY;
		if (instruct->destination.reg_number != -1) {
			RFList[instruct->destination.reg_number].reg_ready = REGISTER_NOT_READY;
			RFList[instruct->destination.reg_number].tag_number = tag_number;
		}
		instruct->STAGE = IS;
		instruct->cycle_info.issue.start_timer = current_tick_number;
		instruct->cycle_info.dispatch.duration = current_tick_number - instruct->cycle_info.dispatch.start_timer;
		issue_list.Add(tmp->value);
		dispatch_list.RemoveAtValue(tmp->value);
		tmp = tmp->next;
	}
}
void Fetch(int N) {
	int counter = N;
	
	while (dispatch_list.len()<2*N && counter > 0) {
		instruct_node* fetched = instructions.Fetch();

		if (fetched == NULL)
			return;
		counter--;
		instruct_info* instruct = &(fetched->value);
		if (instruct->operation_type == 0)
			instruct->operation_time = instruct->cycle_info.execute.duration = 1;
		int tag_number = instruct->tag_number;
		instruct->STAGE = IF;
		instruct->cycle_info.fetch.start_timer = current_tick_number;
		instruct->cycle_info.fetch.duration = 1;
		if (instruct->operation_type == 1)
			instruct->operation_time = instruct->cycle_info.execute.duration = 2;
		if (instruct->operation_type == 2)
			instruct->operation_time = instruct->cycle_info.execute.duration = 5;
		FAKEROB.Add(tag_number);
		dispatch_list.Add(tag_number);
	}

}
int AdvanceCycle() {
	if(current_tick_number == 9999)
		return !(FAKEROB.len() == 0 && instructions.IsEnd());
	return !(FAKEROB.len() == 0 && instructions.IsEnd());
}
#endif // !MAIN_H
