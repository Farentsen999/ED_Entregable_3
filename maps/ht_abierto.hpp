#include <iostream>
#include <vector>
#include <list>
#include <string>

using namespace std;

struct Entry {
    int key;
    string value;

    Entry(int k, string v) : key(k), value(v) {}
};

class HashTable {
private:
    vector<list<Entry>> table;
    int size;

    int hashFunction(int key) const {
        return key % size;
    }

public:
    HashTable(int tableSize) : size(tableSize) {
        table.resize(size);
    }

    void insert(int key, string value) {
        int index = hashFunction(key);

        // Si la clave ya existe, actualizamos el valor
        for (auto& entry : table[index]) {
            if (entry.key == key) {
                entry.value = value;
                return;
            }
        }

        // Si no existe, la agregamos al final
        table[index].emplace_back(key, value);
    }

    bool get(int key, string& result) const {
        int index = hashFunction(key);

        for (const auto& entry : table[index]) {
            if (entry.key == key) {
                result = entry.value;
                return true;
            }
        }

        return false;
    }

    void remove(int key) {
        int index = hashFunction(key);

        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->key == key) {
                table[index].erase(it);
                return;
            }
        }
    }
};
