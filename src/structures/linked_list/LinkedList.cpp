#include "LinkedList.h"

DoublyLinkedList::DoublyLinkedList() {
    head = NULL;
}

void DoublyLinkedList::insertFront(int value) {
    Node1* newNode = new Node1(value);

    if (head == NULL) {
        head = newNode;
        return;
    }

    newNode->next = head;
    head->prev = newNode;
    head = newNode;
}

void DoublyLinkedList::insertBack(int value) {
    Node1* newNode = new Node1(value);

    if (head == NULL) {
        head = newNode;
        return;
    }

    Node1* temp = head;

    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = newNode;
    newNode->prev = temp;
}

void DoublyLinkedList::deleteValue(int value) {
    Node1* temp = head;

    while (temp != NULL && temp->data != value) {
        temp = temp->next;
    }

    if (temp == NULL) {
        cout << "Value not found\n";
        return;
    }

    if (temp->prev != NULL) {
        temp->prev->next = temp->next;
    }
    else {
        head = temp->next;
    }

    if (temp->next != NULL) {
        temp->next->prev = temp->prev;
    }

    delete temp;
}

void DoublyLinkedList::displayForward() {
    Node1* temp = head;

    while (temp != NULL) {
        cout << temp->data << " <-> ";
        temp = temp->next;
    }

    cout << "NULL\n";
}