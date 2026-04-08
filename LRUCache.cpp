// Design and implement a data structure for a Least Recently Used (LRU) cache.
// It should support the following operations:
// get(key): Get the value of the key if it exists, otherwise return -1.
// put(key, value): Update the value of the key if it exists, otherwise add the key-value pair to
// the cache. If the cache is full, remove the least recently used item before adding the new one.
// The operations should have an average time complexity of O(1).

#include <iostream>
#include <unordered_map>

using namespace std;

class Node {
public:
    int key;
    int value;
    Node *next;
    Node *prev;

    Node(int v, int k) : value(v), key(k), next(nullptr), prev(nullptr) {}
};

class DLL {
public:
    Node *head;
    Node *tail;

    DLL() {
        head = new Node(-1, -1);
        tail = new Node(-1, -1);
        head->next = tail;
        tail->prev = head;
    }
};

class LRUCache {
private:
    DLL list;
    unordered_map<int, Node *> cache;
    int capacity;

public:
    LRUCache(int capacity) { this->capacity = capacity; }

    int get(int key) {
        if (cache.find(key) != cache.end()) {
            Node *node = cache[key];

            node->prev->next = node->next;
            node->next->prev = node->prev;

            node->next = list.head->next;
            node->next->prev = node;
            list.head->next = node;
            node->prev = list.head;

            return node->value;
        }
        return -1;
    }

    void put(int key, int value) {
        if (cache.find(key) != cache.end()) {
            Node *node = cache[key];
            node->prev->next = node->next;
            node->next->prev = node->prev;
            delete node;
        } else if (cache.size() == capacity) {
            Node *node = list.tail->prev;
            cache.erase(node->key);
            node->prev->next = list.tail;
            list.tail->prev = node->prev;
            delete node;
        }
        Node *newNode = new Node(value, key);
        list.head->next->prev = newNode;
        newNode->next = list.head->next;
        list.head->next = newNode;
        newNode->prev = list.head;
        cache[key] = newNode;
    }
};

int main() {
    LRUCache cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    cout << cache.get(1) << endl;
    cache.put(3, 3);
    cout << cache.get(1) << endl;
    cache.put(4, 4);
    cout << cache.get(2) << endl;
    cout << cache.get(3) << endl;
    cout << cache.get(4) << endl;
    return 0;
}
