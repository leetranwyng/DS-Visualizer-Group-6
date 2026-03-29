#pragma once
#include <iostream>
using namespace std;

struct Node1 {
    int data;
    Node1* prev;
    Node1* next;

    Node1(int value) {
        data = value;
        prev = NULL;
        next = NULL;
    }
};

class DoublyLinkedList {
private:
    Node1* head;

public:
    DoublyLinkedList();

    void insertFront(int value);
    void insertBack(int value);
    void deleteValue(int value);
    void displayForward();
};