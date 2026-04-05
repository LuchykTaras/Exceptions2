#include <iostream>
#include <string>
using namespace std;

//        КЛАСИ ВИНЯТКІВ

class ListException {
protected:
    string message;
public:
    explicit ListException(const string& msg) : message(msg) {}
    virtual ~ListException() = default;
    virtual string what() const { return "[ListException] " + message; }
};

class EmptyListException : public ListException {
public:
    explicit EmptyListException(const string& operation)
        : ListException("Операція '" + operation + "' неможлива — список порожній.") {
    }
    string what() const override { return "[EmptyListException] " + message; }
};

class IndexOutOfRangeException : public ListException {
    int index, size;
public:
    IndexOutOfRangeException(int idx, int sz)
        : ListException("Індекс " + to_string(idx) +
            " виходить за межі списку розміром " + to_string(sz) + "."),
        index(idx), size(sz) {
    }
    string what() const override { return "[IndexOutOfRangeException] " + message; }
    int getIndex() const { return index; }
    int getSize()  const { return size; }
};

class MemoryAllocationException : public ListException {
public:
    explicit MemoryAllocationException(const string& context = "")
        : ListException("Не вдалося виділити пам'ять" +
            (context.empty() ? "." : " (" + context + ").")) {
    }
    string what() const override { return "[MemoryAllocationException] " + message; }
};

class InvalidValueException : public ListException {
public:
    explicit InvalidValueException(const string& details = "")
        : ListException("Недійсне значення" +
            (details.empty() ? "." : ": " + details + ".")) {
    }
    string what() const override { return "[InvalidValueException] " + message; }
};

// ═══════════════════════════════════════
//        ДВОЗВ'ЯЗНИЙ СПИСОК
// ═══════════════════════════════════════

struct Node {
    int data;
    Node* prev;
    Node* next;
    explicit Node(int val) : data(val), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList {
    Node* head;
    Node* tail;
    int   size;

    Node* allocateNode(int value) {
        try {
            return new Node(value);
        }
        catch (const bad_alloc&) {
            throw MemoryAllocationException("вузол зі значенням " + to_string(value));
        }
    }

    void assertNotEmpty(const string& op) const {
        if (size == 0) throw EmptyListException(op);
    }

    void assertValidIndex(int index) const {
        if (index < 0 || index >= size)
            throw IndexOutOfRangeException(index, size);
    }

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}
    ~DoublyLinkedList() { clear(); }

    void pushBack(int value) {
        Node* node = allocateNode(value);
        if (!tail) { head = tail = node; }
        else { node->prev = tail; tail->next = node; tail = node; }
        ++size;
    }

    void pushFront(int value) {
        Node* node = allocateNode(value);
        if (!head) { head = tail = node; }
        else { node->next = head; head->prev = node; head = node; }
        ++size;
    }

    void insertAt(int index, int value) {
        if (index < 0 || index > size)
            throw IndexOutOfRangeException(index, size);
        if (index == 0) { pushFront(value); return; }
        if (index == size) { pushBack(value);  return; }

        Node* node = allocateNode(value);
        Node* cur = head;
        for (int i = 0; i < index; ++i) cur = cur->next;
        node->next = cur;
        node->prev = cur->prev;
        cur->prev->next = node;
        cur->prev = node;
        ++size;
    }

    int popBack() {
        assertNotEmpty("popBack");
        int value = tail->data;
        Node* del = tail;
        if (head == tail) { head = tail = nullptr; }
        else { tail = tail->prev; tail->next = nullptr; }
        delete del;
        --size;
        return value;
    }

    int popFront() {
        assertNotEmpty("popFront");
        int value = head->data;
        Node* del = head;
        if (head == tail) { head = tail = nullptr; }
        else { head = head->next; head->prev = nullptr; }
        delete del;
        --size;
        return value;
    }

    int removeAt(int index) {
        assertNotEmpty("removeAt");
        assertValidIndex(index);
        if (index == 0)        return popFront();
        if (index == size - 1) return popBack();

        Node* cur = head;
        for (int i = 0; i < index; ++i) cur = cur->next;
        int value = cur->data;
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        delete cur;
        --size;
        return value;
    }

    int& operator[](int index) {
        assertNotEmpty("operator[]");
        assertValidIndex(index);
        Node* cur = head;
        for (int i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }

    void clear() {
        Node* cur = head;
        while (cur) { Node* next = cur->next; delete cur; cur = next; }
        head = tail = nullptr;
        size = 0;
    }

    int  getSize()  const { return size; }
    bool isEmpty()  const { return size == 0; }

    void print() const {
        if (isEmpty()) { cout << "[порожній список]\n"; return; }
        cout << "HEAD <-> ";
        for (Node* cur = head; cur; cur = cur->next)
            cout << cur->data << (cur->next ? " <-> " : "");
        cout << " <-> TAIL  (розмір: " << size << ")\n";
    }
};

// ═══════════════════════════════════════
//                 MAIN
// ═══════════════════════════════════════

int main() {
    DoublyLinkedList list;

    cout << "=== 1. Операції на порожньому списку ===\n";

    try { list.popBack(); }
    catch (const EmptyListException& e) { cout << e.what() << "\n"; }

    try { list.popFront(); }
    catch (const EmptyListException& e) { cout << e.what() << "\n"; }

    try { list.removeAt(0); }
    catch (const EmptyListException& e) { cout << e.what() << "\n"; }

    try { cout << list[0]; }
    catch (const EmptyListException& e) { cout << e.what() << "\n"; }

    cout << "\n=== 2. Заповнення списку ===\n";
    list.pushBack(10);
    list.pushBack(20);
    list.pushBack(30);
    list.pushBack(40);
    list.pushBack(50);
    list.pushFront(5);
    list.print();

    cout << "\n=== 3. insertAt ===\n";
    list.insertAt(3, 99);
    list.print();

    try { list.insertAt(-1, 7); }
    catch (const IndexOutOfRangeException& e) { cout << e.what() << "\n"; }

    try { list.insertAt(1000, 7); }
    catch (const IndexOutOfRangeException& e) { cout << e.what() << "\n"; }

    cout << "\n=== 4. removeAt ===\n";
    cout << "Видалено: " << list.removeAt(2) << "\n";
    list.print();

    try { list.removeAt(100); }
    catch (const IndexOutOfRangeException& e) { cout << e.what() << "\n"; }

    cout << "\n=== 5. operator[] ===\n";
    cout << "list[0] = " << list[0] << "\n";
    list[1] = 777;
    cout << "list[1] = " << list[1] << " (після зміни)\n";
    list.print();

    try { cout << list[-5]; }
    catch (const IndexOutOfRangeException& e) { cout << e.what() << "\n"; }

    cout << "\n=== 6. popFront / popBack ===\n";
    cout << "popFront = " << list.popFront() << "\n";
    cout << "popBack  = " << list.popBack() << "\n";
    list.print();

    cout << "\n=== 7. MemoryAllocationException (симуляція) ===\n";
    try { throw MemoryAllocationException("вузол для значення 42"); }
    catch (const MemoryAllocationException& e) { cout << e.what() << "\n"; }

    cout << "\n=== 8. InvalidValueException ===\n";
    try { throw InvalidValueException("значення виходить за допустимий діапазон"); }
    catch (const InvalidValueException& e) { cout << e.what() << "\n"; }

    cout << "\n=== 9. Поліморфна обробка через базовий клас ===\n";
    list.clear();
    try { list.popBack(); }
    catch (const ListException& e) { cout << "Базовий catch: " << e.what() << "\n"; }

    return 0;
}