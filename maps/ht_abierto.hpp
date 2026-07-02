#ifndef HT_ABIERTO_HPP
#define HT_ABIERTO_HPP

#include <vector>
#include <list>
#include <string>
#include <cstddef>
#include "hash_functions.hpp"

template <typename K> // Permite que la clave de la entry sea de cualquier tipo (en este caso long long y string).

///@brief Struct que representa un entry para hashing abierto.
struct EntryAbierto {
    K key;
    int value;
    EntryAbierto(const K& k, int v) : key(k), value(v) {}
};

template <typename K> // Permite que la clave de la entry sea de cualquier tipo (en este caso long long y string).

///@class HashTablaAbierto
///@brief Clase que representa una Tabla Hash con Hashing Abierto.
class HashTableAbierto {
private:
    std::vector<std::list<EntryAbierto<K>>> table;
    long long capacity;
    long long count;   
    double maxLoadFactor; // Umbral para redimensionar el arreglo y evitar la degradación del rendimiento.

    long long hashIndex(const K& key, long long n) const {
        return HashTraits<K>::h1(key, n);
    }

    /// @brief Método que lleva a cabo el rehashing
    /// @param newCapacity Nueva capacidad para la tabla (conviene usar un número primo).
    void resize(long long newCapacity) {
        std::vector<std::list<EntryAbierto<K>>> old = table;
        table.clear();
        table.resize(newCapacity);
        capacity = newCapacity;

        for (auto& bucket : old) {
            for (auto& e : bucket) {
                long long idx = hashIndex(e.key, capacity);
                table[idx].push_back(e);
            }
        }
    }

public:
    ///@brief Constructor explícito de la tabla hash con hashing cerrado.
    /// Inicializa la tabla con una capacidad y un loadFactor especificos.
    ///@param initialCapacity Capacidad inicial de la tabla (mayor que 0).
    ///@param maxLoad Factor de carga límite antes de un resize. Por defecto menor a 0.9 como se vio en el PPT8.
    explicit HashTableAbierto(long long initialCapacity, double maxLoad = 0.89)
        : capacity(initialCapacity), count(0), maxLoadFactor(maxLoad) {
        table.resize(capacity);
    }

    ///@brief Inserta una entry con un valor específico en la tabla o actualiza su valor si ya existe.
    ///@param key Clave a buscar o insertar.
    ///@param value Valor entero asociado que se le asignará a la entry.
    void insert(const K& key, int value) {
        long long index = hashIndex(key, capacity);

        for (auto& entry : table[index]) {
            if (entry.key == key) {
                entry.value = value;
                return;
            }
        }

        if (static_cast<double>(count + 1) / capacity > maxLoadFactor) {
            resize(capacity * 2);
            index = hashIndex(key, capacity);
        }

        table[index].emplace_back(key, value);
        count++;
    }

    ///@brief Incrementa el contador de ocurrencias de una clave (Esquema pedido en el enunciado).
    ///Implementa el flujo: if (k está en H) H[k] = H[k] + 1; else H[k] = 1;
    ///@param key Clave del usuario que publico el tweet.
    void increment(const K& key) {
        long long index = hashIndex(key, capacity);

        for (auto& entry : table[index]) {
            if (entry.key == key) {
                entry.value += 1;
                return;
            }
        }

        if (static_cast<double>(count + 1) / capacity > maxLoadFactor) {
            resize(capacity * 2);
            index = hashIndex(key, capacity);
        }

        table[index].emplace_back(key, 1);
        count++;
    }

    ///@brief Busca una clave en la tabla y recupera su valor asociado por referencia (por si la clave no existe).
    ///@param key Clave a buscar.
    ///@param result Variable pasada por referencia donde se guardará el valor si la clave existe.
    ///@return true Si la clave fue encontrada con éxito.
    ///@return false Si la clave no fue encontrada.
    bool get(const K& key, int& result) const {
        long long index = hashIndex(key, capacity);
        for (const auto& entry : table[index]) {
            if (entry.key == key) {
                result = entry.value;
                return true;
            }
        }
        return false;
    }

    ///@brief Remueve el nodo que contiene la entry con la clave correspondinete.
    ///@param key Clave del elemento que se desea eliminar.
    void remove(const K& key) {
        long long index = hashIndex(key, capacity);
        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->key == key) {
                table[index].erase(it);
                count--;
                return;
            }
        }
    }

    ///@brief Metodos que retornan la capacidad, el tamaño y el loadFactor de la Tabla.
    long long size() const { return count; }
    long long bucketCount() const { return capacity; }
    double loadFactor() const { return static_cast<double>(count) / capacity; }

    
    ///@brief Calcula una estimación del tamaño total de memoria RAM que consume la tabla.
    ///Esto es porque sizeof(std::string) retorna un tamaño fijo, el texto real del string
    ///se guarda dinámicamente en el heap.
    ///@return std::size_t Cantidad total aproximada de bytes utilizados en memoria.
    std::size_t estimatedMemoryBytes() const {
        // Parte 1: Espacio reservado por el vector para las cabeceras de cada std::list
        std::size_t base = capacity * sizeof(std::list<EntryAbierto<K>>);

        // Parte 2: Overhead por nodo. Una std::list en C++ es doblemente enlazada, luego cada nodo
        // contiene un puntero al anterior, uno al siguiente y uno al valor.
        std::size_t perNodeOverhead = 3 * sizeof(void*);

        // Parte 3: recorrer cada lista y sumar el espacio ocupado por cada nodo 
        std::size_t total = base;
        for (const auto& bucket : table) {
            for (const auto& e : bucket) {
                total += sizeof(EntryAbierto<K>) + perNodeOverhead + approxExtraSize(e.key);
            }
        }
        return total;
    }

private:
    ///@brief Sobrecarga para claves de tipo numérico (long long).
    ///@return size_t Siempre retorna 0 bytes extras pues no generan consumo extra en memoria dinamica.
    static std::size_t approxExtraSize(const long long&) { return 0; }

    ///@brief Sobrecarga para claves de tipo cadena de texto (std::string).
    ///@param s Cadena de texto a evaluar.
    ///@return size_t Capacidad real en bytes del buffer dinámico en el Heap.
    static std::size_t approxExtraSize(const std::string& s) { return s.capacity(); }
};

#endif 
