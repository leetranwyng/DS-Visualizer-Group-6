#include "LinkedListRenderer.h"

void RenderLinkedList() {
    DoublyLinkedList list;

    list.insertFront(10);
    list.insertFront(5);
    list.insertBack(20);
    list.insertBack(30);

    cout << "List: ";
    list.displayForward();

    list.deleteValue(20);

    cout << "After delete 20: ";
    list.displayForward();
}