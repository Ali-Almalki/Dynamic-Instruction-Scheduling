#pragma once
#ifndef INSTRUCTION_QUEUE_H
#define INSTRUCTION_QUEUE_H
#include <stdio.h>
struct reg_info_t {
	int reg_number;
	int reg_ready;
	int tag_number;
};

struct stage_info_t {
	int start_timer;
	int duration;
};

struct cycle_info_t {
	int tag_number;
	stage_info_t fetch;
	stage_info_t dispatch;
	stage_info_t issue;
	stage_info_t execute;
	stage_info_t writeback;
};

struct instruct_info {
	int tag_number;
	cycle_info_t cycle_info;
	reg_info_t source1;
	reg_info_t source2;
	reg_info_t destination;
	int STAGE; // IF, ID, IS ...
	int operation_type;  // 0,1,2
	int operation_time;
};
struct instruct_node {
	instruct_info value;
	instruct_node* next;
};
class InstructionQueue
{
private:
	instruct_node* head;
	instruct_node* tail;
	instruct_node* current_instruct;  // start of unfetched instrucion;
	int cnt;
public:
	InstructionQueue() {
		this->head = tail = NULL;
		cnt = 0;
	}
	~InstructionQueue() {
		instruct_node* tmp = head;
		while (tmp != NULL) {
			instruct_node* tmp1 = tmp;
			tmp = tmp->next;
			delete tmp1;
		}
		this->head = tail = NULL;
		cnt = 0;
	}
	void Add(instruct_info value) {
		instruct_node* t = new instruct_node;
		if (tail == NULL)
			tail = t;
		else
			tail->next = t;
		t->next = NULL;
		t->value = value;
		tail = t;
		cnt++;
		if (head == NULL)
		{
			head = t;
			current_instruct = head;
		}
			
	}
	void AddInstruct(int address, int operationtype, int  destination, int  source1, int  source2) {
		instruct_info info;
		info.operation_type = operationtype;
		info.destination.reg_number = destination;
		info.source1.reg_number = source1;
		info.source2.reg_number = source2;
		info.tag_number = cnt;
		Add(info);
	}
	void Remove(instruct_node* t) {
		if (t == NULL)
			return;

		instruct_node* tmp = head;
		if (t == head) {
			head = head->next;
			if (head == NULL)
				tail = NULL;
			delete t;
			cnt--;
			return;
		}
		while (tmp != tail) {
			instruct_node* tmp1 = tmp->next;
			if (tmp1 == t) {
				if (tmp1 == tail) {
					tail = tmp;
					tail->next = NULL;
				}
				else {
					tmp->next = tmp1->next;
				}
				delete tmp1;
				cnt--;
				break;
			}
		}

	}
	int len() { return cnt; }
	instruct_node* First() { return head; }
	instruct_node* Last() { return tail; }

	instruct_node* At(int index) {
		instruct_node* tmp = head;
		while (tmp != NULL) {
			if (index == 0) {
				return tmp;
			}
			tmp = tmp->next;
			index--;
		}
		return NULL;
	}
	bool IsEnd() {
		if (current_instruct == NULL)
			return true;
		return false;
	}
	instruct_node* Fetch() {
		if (current_instruct == NULL)
			return NULL;
		instruct_node* res = current_instruct;
		current_instruct = current_instruct->next;
		return res;
	}
};
#endif