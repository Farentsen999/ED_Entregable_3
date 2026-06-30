#ifndef HASH_FUNCTIONS_HPP
#define HASH_FUNCTIONS_HPP

#include <string>
#include <cmath>
#include <cstdint>

// Constante para hashing multiplicativo
float A = (std::sqrt(5.0) - 1.0) / 2.0;

//Claves enteras (long long, usado para user_id)
inline long long h1_int(long long key, long long n) {
    return key % n;
}

inline long long h2_int(long long key, long long n) {
    double a = static_cast<double>(key) * A;
    a -= static_cast<long long>(a);
    return n * a;
}


//Claves de texto (std::string, usado para user_screen_name) Algoritmo FNV-1a (64 bits)
inline std::uint64_t stringHashBase(const std::string& s) {
    std::uint64_t hash = 1469598103934665603ULL; // FNV offset basis
    const std::uint64_t prime = 1099511628211ULL; // FNV prime
    for (unsigned char c : s) {
        hash ^= c;
        hash *= prime;
    }
    return hash;
}

inline long long h1_str(const std::string& key, long long n) {
    std::uint64_t hv = stringHashBase(key);
    return static_cast<long long>(hv % static_cast<std::uint64_t>(n));
}

inline long long h2_str(const std::string& key, long long n) {
    // Segunda función independiente: se usa un "seed" distinto para
    // evitar correlación directa con h1_str.
    std::uint64_t hash = 14695981039346656037ULL;
    const std::uint64_t prime = 1099511628211ULL;
    for (unsigned char c : key) {
        hash ^= (c + 7); // desplazamiento simple para diferenciar de h1_str
        hash *= prime;
    }
    long long h = static_cast<long long>(hash % static_cast<std::uint64_t>(n - 1));
    return h + 1; // rango [1, n-1]
}


// Template de traits para que las tablas hash usen las funcionese h1/h2 segun el tipo de K.
template <typename K>
struct HashTraits;

template <>
struct HashTraits<long long> {
    static long long h1(const long long& key, long long n) { return h1_int(key, n); }
    static long long h2(const long long& key, long long n) { return h2_int(key, n); }
};

template <>
struct HashTraits<std::string> {
    static long long h1(const std::string& key, long long n) { return h1_str(key, n); }
    static long long h2(const std::string& key, long long n) { return h2_str(key, n); }
};

#endif