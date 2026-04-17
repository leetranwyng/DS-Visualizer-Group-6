#pragma once
#include <vector>

struct LinkedListNode {
    int data;
    LinkedListNode* prev;
    LinkedListNode* next;

    LinkedListNode(int value);
};

class LinkedList {
private:
    LinkedListNode* head;
    LinkedListNode* tail;

public:
    LinkedList();
    ~LinkedList();

    void insertFront(int value);
    void insertBack(int value);
    bool deleteValue(int value);
    LinkedListNode* search(int value);
    bool updateValue(int oldValue, int newValue);
    void clear();

    LinkedListNode* getHead() const;

    std::vector<int> toVector() const;
    void loadFromVector(const std::vector<int>& values);
};