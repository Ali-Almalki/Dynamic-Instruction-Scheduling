#pragma once
#ifndef MYFIFO_H
#define MYFIFO_H
#include <stdio.h>
struct Node {
	int value;
	Node* next;
};

class MYFIFO
{
private:
	Node* head;
	Node* tail;
	int cnt;
public:
	MYFIFO() {
		this->head = tail = NULL;
		cnt = 0;
	}
	~MYFIFO() {
		Node* tmp = head;
		while (tmp != NULL) {
			Node* tmp1 = tmp;
			tmp = tmp->next;
			delete tmp1;
		}
		this->head = tail = NULL;
		cnt = 0;
	}
	void Add(int value) {
		Node* t = new Node;
		if (tail == NULL)
			tail = t;
		else
			tail->next = t;
		t->next = NULL;
		t->value = value;
		tail = t;
		cnt++;
		if (head == NULL)
			head = t;
	}
	void RemoveAtValue(int value) {
		Node* tmp=head;
		while (tmp != NULL) {
			if (tmp->value == value)
			{
				Remove(tmp);
				return;
			}
			tmp = tmp->next;
		}
	}
	void Print(){
		Node* tmp=head;
		while (tmp != NULL) {
			printf("%d ", tmp->value);
			tmp = tmp->next;
		}
		printf("\n");
	}
	void Remove(Node* t) {
		if (t == NULL)
			return;

		Node* tmp = head;
		if (t == head) {
			head = head->next;
			if (head == NULL)
				tail = NULL;
			delete t;
			cnt--;
			return;
		}
		while (tmp != tail) {
			Node* tmp1 = tmp->next;			
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
			tmp = tmp->next;
		}

	}
	void Sort() {
		int i = 0, j = 0;
		Node* tmp = this->First();
		Node* tmp2;
		while (tmp != NULL) {
			tmp2 = tmp;
			while (tmp2 != NULL) {
				if (tmp2->value < tmp->value) {
					int swap_value = tmp->value;
					tmp->value = tmp2->value;
					tmp2->value = swap_value;
				}
				tmp2 = tmp2->next;
			}
			tmp = tmp->next;
		}
	}
	int len() { return cnt; }
	Node* First() { return head; }
	Node* Last() { return tail; }
};
#endif // MYFIFO

