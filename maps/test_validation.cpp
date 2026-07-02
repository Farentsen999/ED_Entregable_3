#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "ht_cerrado.hpp"
#include "ht_abierto.hpp"
#include "ht_stl.hpp"

using namespace std;

int main() {
    // Datos sintéticos: simula tweets de usuarios repetidos
    vector<long long> userIds = {10, 20, 10, 15, 7, 10, 20, 999999999991LL, 7, 15, 15};
    vector<string> screenNames = {"ana", "beto", "ana", "caro", "deni", "ana",
                                "beto", "zoe_99", "deni", "caro", "caro"};

    cout << "=== Test con user_id (long long) ===\n";
    HashTableCerrado<long long> linear(7, ProbeStrategy::LINEAR);
    HashTableCerrado<long long> quad(7, ProbeStrategy::QUADRATIC);
    HashTableCerrado<long long> dbl(7, ProbeStrategy::DOUBLE);
    HashTableAbierto<long long> chained(7);
    HashTableSTL<long long> stl(7);

    for (auto id : userIds) {
        linear.increment(id);
        quad.increment(id);
        dbl.increment(id);
        chained.increment(id);
        stl.increment(id);
    }

    for (auto id : userIds) {
        int rl, rq, rd, rc, rs;
        bool ok = linear.get(id, rl) && quad.get(id, rq) && dbl.get(id, rd)
                && chained.get(id, rc) && stl.get(id, rs);
        assert(ok);
        assert(rl == rq && rq == rd && rd == rc && rc == rs);
        cout << "user_id=" << id << " -> count=" << rl
            << " (linear=quad=double=chained=stl OK)\n";
    }
    assert(linear.size() == quad.size());
    assert(linear.size() == chained.size());
    assert(linear.size() == (long long)stl.size());
    cout << "Claves distintas: " << linear.size() << "\n";

    cout << "\n=== Test con user_screen_name (string) ===\n";
    HashTableCerrado<string> linearS(7, ProbeStrategy::LINEAR);
    HashTableCerrado<string> quadS(7, ProbeStrategy::QUADRATIC);
    HashTableCerrado<string> dblS(7, ProbeStrategy::DOUBLE);
    HashTableAbierto<string> chainedS(7);
    HashTableSTL<string> stlS(7);

    for (auto& name : screenNames) {
        linearS.increment(name);
        quadS.increment(name);
        dblS.increment(name);
        chainedS.increment(name);
        stlS.increment(name);
    }

    for (auto& name : screenNames) {
        int rl, rq, rd, rc, rs;
        bool ok = linearS.get(name, rl) && quadS.get(name, rq) && dblS.get(name, rd)
                && chainedS.get(name, rc) && stlS.get(name, rs);
        assert(ok);
        assert(rl == rq && rq == rd && rd == rc && rc == rs);
        cout << "screen_name=" << name << " -> count=" << rl
            << " (linear=quad=double=chained=stl OK)\n";
    }
    assert(linearS.size() == chainedS.size());
    cout << "Claves distintas: " << linearS.size() << "\n";

    cout << "\n=== Test de resize (forzar muchas inserciones) ===\n";
    HashTableCerrado<long long> small(4, ProbeStrategy::LINEAR);
    HashTableAbierto<long long> smallChained(4);
    for (long long i = 0; i < 1000; i++) {
        small.increment(i % 300);
        smallChained.increment(i % 300);
    }
    assert(small.size() == 300);
    assert(smallChained.size() == 300);
    cout << "Resize OK. bucketCount cerrada=" << small.bucketCount()
        << " bucketCount abierta=" << smallChained.bucketCount() << "\n";

    cout << "\n=== Estimacion de memoria (solo demostrativo) ===\n";
    cout << "Cerrada (long long): " << linear.estimatedMemoryBytes() << " bytes\n";
    cout << "Abierta (long long): " << chained.estimatedMemoryBytes() << " bytes\n";
    cout << "STL (long long): " << stl.estimatedMemoryBytes() << " bytes\n";
    cout << "Cerrada (string): " << linearS.estimatedMemoryBytes() << " bytes\n";

    cout << "\nTODOS LOS TESTS PASARON\n";
    return 0;
}
