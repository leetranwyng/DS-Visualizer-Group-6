#include "LinkedList.h"

LinkedListNode::LinkedListNode(int value) {
    data = value;
    prev = nullptr;
    next = nullptr;
}

LinkedList::LinkedList() {
    head = nullptr;
    tail = nullptr;
}

LinkedList::~LinkedList() {
    clear();
}

void LinkedList::clear() {
    LinkedListNode* cur = head;
    while (cur != nullptr) {
        LinkedListNode* temp = cur;
        cur = cur->next;
        delete temp;
    }
    head = nullptr;
    tail = nullptr;
}

void LinkedList::insertFront(int value) {
    LinkedListNode* newNode = new LinkedListNode(value);

    if (head == nullptr) {
        head = tail = newNode;
        return;
    }

    newNode->next = head;
    head->prev = newNode;
    head = newNode;
}

void LinkedList::insertBack(int value) {
    LinkedListNode* newNode = new LinkedListNode(value);

    if (tail == nullptr) {
        head = tail = newNode;
        return;
    }

    tail->next = newNode;
    newNode->prev = tail;
    tail = newNode;
}

bool LinkedList::deleteValue(int value) {
    LinkedListNode* cur = head;

    while (cur != nullptr && cur->data != value) {
        cur = cur->next;
    }

    if (cur == nullptr) return false;

    if (cur == head) head = cur->next;
    if (cur == tail) tail = cur->prev;

    if (cur->prev != nullptr) cur->prev->next = cur->next;
    if (cur->next != nullptr) cur->next->prev = cur->prev;

    if (head != nullptr) head->prev = nullptr;
    if (tail != nullptr) tail->next = nullptr;

    delete cur;
    return true;
}

LinkedListNode* LinkedList::search(int value) {
    LinkedListNode* cur = head;
    while (cur != nullptr) {
        if (cur->data == value) return cur;
        cur = cur->next;
    }
    return nullptr;
}

bool LinkedList::updateValue(int oldValue, int newValue) {
    LinkedListNode* found = search(oldValue);
    if (found == nullptr) return false;
    found->data = newValue;
    return true;
}

LinkedListNode* LinkedList::getHead() const {
    return head;
}

std::vector<int> LinkedList::toVector() const {
    std::vector<int> values;
    LinkedListNode* cur = head;
    while (cur != nullptr) {
        values.push_back(cur->data);
        cur = cur->next;
    }
    return values;
}

void LinkedList::loadFromVector(const std::vector<int>& values) {
    clear();
    for (int x : values) {
        insertBack(x);
    }
}