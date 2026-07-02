#ifndef HT_CERRADO_HPP
#define HT_CERRADO_HPP

#include <vector>
#include <string>
#include <cstddef>
#include "hash_functions.hpp"


enum class ProbeStrategy { LINEAR, QUADRATIC, DOUBLE }; // Evita que las palabras queden globalmente reservadas.

template <typename K> // Permite que la clave de la entry sea de cualquier tipo (en este caso long long y string).

///@brief Struct que representa un entry para hashing cerrado.
struct EntryCerrado {
    K key;
    int value;
    enum State { EMPTY, OCCUPIED, DELETED } state;

    EntryCerrado() : key(), value(0), state(EMPTY) {}
};

template <typename K> // Permite que la clave de la entry sea de cualquier tipo (en este caso long long y string).

///@class HashTablaCerrado
///@brief Clase que representa una Tabla Hash con Hashing Cerrrado
class HashTableCerrado {
private:
    std::vector<EntryCerrado<K>> table;
    long long capacity;
    long long count;     
    ProbeStrategy strategy;
    double maxLoadFactor;  // Umbral para redimensionar el arreglo y evitar la degradación del rendimiento.

    /// @brief Metodo que calcula el índice en la tabla para el intento i segun la estrategia establecida.
    /// @param key Clave de la entry.
    /// @param i Numero del intento.
    /// @param n Capacidad actual de la tabla.
    /// @return Indice valido dentro del arreglo.
    long long probe(const K& key, long long i, long long n) const {
        long long base = HashTraits<K>::h1(key, n);
        switch (strategy) {
            case ProbeStrategy::LINEAR:
                return (base + i) % n;
            case ProbeStrategy::QUADRATIC:
                return (base + i + 2 * i * i) % n;
            case ProbeStrategy::DOUBLE: {
                long long step = HashTraits<K>::h2(key, n);
                return (base + i * step) % n;
            }
        }
        return base;
    }

    /// @brief Método que lleva a cabo el rehashing
    /// @param newCapacity Nueva capacidad para la tabla (conviene usar un número primo).
    void resize(long long newCapacity) {
        std::vector<EntryCerrado<K>> old = table;
        table.clear();
        table.resize(newCapacity);
        capacity = newCapacity;
        count = 0;

        for (auto& e : old) {
            if (e.state == EntryCerrado<K>::OCCUPIED) {
                insert(e.key, e.value); // reinserta usando el conteo ya acumulado
            }
        }
    }

public:
    ///@brief Constructor explícito de la tabla hash con hashing cerrado.
    /// Inicializa la tabla con una capacidad, un loadFactor y una ProbeStrategy especificos.
    ///@param initialCapacity Capacidad inicial de la tabla (mayor que 0).
    ///@param strat Estrategia de sondeo (LINEAR, QUADRATIC o DOUBLE). Por defecto LINEAR.
    ///@param maxLoad Factor de carga límite antes de un resize. Por defecto menor a 0.5 como se vio en el PPT8.
    explicit HashTableCerrado(long long initialCapacity,
                            ProbeStrategy strat = ProbeStrategy::LINEAR,
                            double maxLoad = 0.49)
        : capacity(initialCapacity), count(0), strategy(strat), maxLoadFactor(maxLoad) {
        table.resize(capacity);
    }

    ///@brief Inserta una entry con un valor específico en la tabla o actualiza su valor si ya existe.
    ///@param key Clave a buscar o insertar.
    ///@param value Valor entero asociado que se le asignará a la entry.
    void insert(const K& key, int value) {
        if (static_cast<double>(count + 1) / capacity > maxLoadFactor) {
            resize(capacity * 2);
        }

        for (long long i = 0; i < capacity; i++) {
            long long index = probe(key, i, capacity);

            if (table[index].state != EntryCerrado<K>::OCCUPIED) {
                table[index].key = key;
                table[index].value = value;
                table[index].state = EntryCerrado<K>::OCCUPIED;
                count++;
                return;
            }

            if (table[index].state == EntryCerrado<K>::OCCUPIED && table[index].key == key) {
                table[index].value = value;
                return;
            }
        }
        // No debería llegar aquí gracias al resize, pero por seguridad:
        resize(capacity * 2);
        insert(key, value);
    }

    ///@brief Incrementa el contador de ocurrencias de una clave (Esquema pedido en el enunciado).
    ///Implementa el flujo: if (k está en H) H[k] = H[k] + 1; else H[k] = 1;
    ///@param key Clave del usuario que publico el tweet.
    void increment(const K& key) {
        if (static_cast<double>(count + 1) / capacity > maxLoadFactor) {
            resize(capacity * 2);
        }

        for (long long i = 0; i < capacity; i++) {
            long long index = probe(key, i, capacity);

            if (table[index].state == EntryCerrado<K>::OCCUPIED && table[index].key == key) {
                table[index].value += 1;
                return;
            }

            if (table[index].state != EntryCerrado<K>::OCCUPIED) {
                table[index].key = key;
                table[index].value = 1;
                table[index].state = EntryCerrado<K>::OCCUPIED;
                count++;
                return;
            }
        }
        resize(capacity * 2);
        increment(key);
    }

    ///@brief Busca una clave en la tabla y recupera su valor asociado por referencia (por si la clave no existe).
    ///@param key Clave a buscar.
    ///@param result Variable pasada por referencia donde se guardará el valor si la clave existe.
    ///@return true Si la clave fue encontrada con éxito.
    ///@return false Si la clave no fue encontrada.
    bool get(const K& key, int& result) const {
        for (long long i = 0; i < capacity; i++) {
            long long index = probe(key, i, capacity);
            if (table[index].state == EntryCerrado<K>::EMPTY) break; // ya no va a aparecer

            if (table[index].state == EntryCerrado<K>::OCCUPIED && table[index].key == key) {
                result = table[index].value;
                return true;
            }
        }
        return false;
    }

    ///@brief Realiza un borrado lógico de una clave en la tabla.
    ///@param key Clave del elemento que se desea eliminar.
    void remove(const K& key) {
        for (long long i = 0; i < capacity; i++) {
            long long index = probe(key, i, capacity);
            if (table[index].state == EntryCerrado<K>::EMPTY) break;

            if (table[index].state == EntryCerrado<K>::OCCUPIED && table[index].key == key) {
                table[index].state = EntryCerrado<K>::DELETED;
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
        // Parte 1: Memoria base reservada por el vector (capacidad de celdas * tamaño de cada celda)
        std::size_t base = capacity * sizeof(EntryCerrado<K>);
        std::size_t extra = 0;

        // Parte 2: Buscar en las celdas ocupadas si hay consumo dinámico en el Heap
        for (const auto& e : table) {
            if (e.state == EntryCerrado<K>::OCCUPIED) {
                extra += approxExtraSize(e.key);
            }
        }
        return base + extra;
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