#ifndef HT_STL_HPP
#define HT_STL_HPP

#include <unordered_map>
#include <string>
#include <cstddef>


template <typename K> // Permite que la clave de la entry sea de cualquier tipo (en este caso long long y string).

///@class HashTableSTL
///@brief Wrapper de la tabla hash de la STL (std::unordered_map).
class HashTableSTL {
private:
    std::unordered_map<K, int> table;

public:
    ///@brief Constructor explícito del Wrapper.
    /// @param bucket_count_hint cantidad de buckets inicial.
    explicit HashTableSTL(long long bucket_count_hint = 17) {
        table.reserve(static_cast<std::size_t>(bucket_count_hint));
    }

    ///@brief Inserta una par clave valor con un valor específico en la tabla o actualiza su valor si ya existe.
    ///@param key Clave a buscar o insertar.
    ///@param value Valor entero asociado que se le asignará al par.
    void insert(const K& key, int value) {
        table[key] = value;
    }

    ///@brief Incrementa el contador de ocurrencias de una clave (Esquema pedido en el enunciado).
    ///Implementa el flujo: if (k está en H) H[k] = H[k] + 1; else H[k] = 1;
    ///@param key Clave del usuario que publico el tweet.
    void increment(const K& key) {
        auto it = table.find(key);
        if (it != table.end()) {
            it->second += 1;
        } else {
            table.emplace(key, 1);
        }
    }

    ///@brief Busca una clave en la tabla y recupera su valor asociado por referencia (por si la clave no existe).
    ///@param key Clave a buscar.
    ///@param result Variable pasada por referencia donde se guardará el valor si la clave existe.
    ///@return true Si la clave fue encontrada con éxito.
    ///@return false Si la clave no fue encontrada.
    bool get(const K& key, int& result) const {
        auto it = table.find(key);
        if (it != table.end()) {
            result = it->second;
            return true;
        }
        return false;
    }

    ///@brief Realiza un par clave-valor de la tabla.
    ///@param key Clave del elemento que se desea eliminar.
    void remove(const K& key) {
        table.erase(key);
    }

    ///@brief Metodos que retornan la capacidad, el tamaño y el loadFactor de la Tabla.
    long long size() const { return static_cast<long long>(table.size()); }
    long long bucketCount() const { return static_cast<long long>(table.bucket_count()); }
    double loadFactor() const { return table.load_factor(); }

    ///@brief Estima el consumo de memoria en bytes (aproximado) de la tabla hash de la STL.
    /// Consiera el tamaño del arreglo dinámico, el overhead de cada nodo (C++ usa listas simples) y
    ///el tamaño de los std::pair almacenados y el consumo extra de memoria dinámica en el heap por std::string.
    ///@return std::size_t Cantidad total aproximada de bytes utilizados en memoria.
    std::size_t estimatedMemoryBytes() const {
        // Parte 1: Espacio ocupado por los buckets
        std::size_t bucketsMem = table.bucket_count() * sizeof(void*);

        // Parte 2: Overhead por nodo (puntero next)
        std::size_t perNodeOverhead = sizeof(void*);

        // Parte 3: Suma el espacio ocupado por cada par clave-valor y su overhead
        std::size_t total = bucketsMem;
        for (const auto& kv : table) {
            total += sizeof(std::pair<const K, int>) + perNodeOverhead + approxExtraSize(kv.first);
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