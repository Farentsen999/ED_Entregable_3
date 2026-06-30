#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "ht_cerrado.hpp"

using namespace std;

void printHeader(const string& title) {
    cout << "\n=== " << title << " ===\n";
}

int main() {
    // Datos sintéticos: simula tweets de usuarios repetidos
    vector<long long> userIds = {10, 20, 10, 15, 7, 10, 20, 999999999991LL, 7, 15, 15};
    vector<string> screenNames = {"ana", "beto", "ana", "caro", "deni", "ana",
                                   "beto", "zoe_99", "deni", "caro", "caro"};

    // ---------------------------------------------------------------
    // 1) Consistencia entre las tres estrategias, clave user_id
    // ---------------------------------------------------------------
    printHeader("user_id (long long) - Linear vs Quadratic vs Double");
    HashTableCerrado<long long> linear(7, ProbeStrategy::LINEAR);
    HashTableCerrado<long long> quad(7, ProbeStrategy::QUADRATIC);
    HashTableCerrado<long long> dbl(7, ProbeStrategy::DOUBLE);

    for (auto id : userIds) {
        linear.increment(id);
        quad.increment(id);
        dbl.increment(id);
    }

    for (auto id : userIds) {
        int rl, rq, rd;
        bool ok = linear.get(id, rl) && quad.get(id, rq) && dbl.get(id, rd);
        assert(ok);
        assert(rl == rq && rq == rd);
        cout << "user_id=" << id << " -> count=" << rl << " (las 3 estrategias OK)\n";
    }
    assert(linear.size() == quad.size() && quad.size() == dbl.size());
    cout << "Claves distintas: " << linear.size() << "\n";

    // ---------------------------------------------------------------
    // 2) Consistencia entre las tres estrategias, clave screen_name
    // ---------------------------------------------------------------
    printHeader("user_screen_name (string) - Linear vs Quadratic vs Double");
    HashTableCerrado<string> linearS(7, ProbeStrategy::LINEAR);
    HashTableCerrado<string> quadS(7, ProbeStrategy::QUADRATIC);
    HashTableCerrado<string> dblS(7, ProbeStrategy::DOUBLE);

    for (auto& name : screenNames) {
        linearS.increment(name);
        quadS.increment(name);
        dblS.increment(name);
    }

    for (auto& name : screenNames) {
        int rl, rq, rd;
        bool ok = linearS.get(name, rl) && quadS.get(name, rq) && dblS.get(name, rd);
        assert(ok);
        assert(rl == rq && rq == rd);
        cout << "screen_name=" << name << " -> count=" << rl << " (las 3 estrategias OK)\n";
    }
    assert(linearS.size() == quadS.size() && quadS.size() == dblS.size());
    cout << "Claves distintas: " << linearS.size() << "\n";

    // ---------------------------------------------------------------
    // 3) insert() vs increment(): insert sobreescribe, increment acumula
    // ---------------------------------------------------------------
    printHeader("insert() sobreescribe valor, increment() acumula");
    HashTableCerrado<long long> mix(7, ProbeStrategy::LINEAR);
    mix.insert(5, 100);
    int r;
    assert(mix.get(5, r) && r == 100);
    mix.insert(5, 200); // mismo key -> sobreescribe
    assert(mix.get(5, r) && r == 200);
    mix.increment(5);   // ahora suma 1
    assert(mix.get(5, r) && r == 201);
    cout << "insert/increment OK (valor final = " << r << ")\n";

    // ---------------------------------------------------------------
    // 4) remove() + reinserción tras DELETED
    // ---------------------------------------------------------------
    printHeader("remove() y reinsercion sobre slot DELETED");
    HashTableCerrado<long long> delTest(7, ProbeStrategy::LINEAR);
    delTest.increment(1);
    delTest.increment(8);  // 8 % 7 == 1 % 7 -> misma posición base, fuerza colisión
    delTest.increment(15); // 15 % 7 == 1 -> también colisiona
    assert(delTest.size() == 3);

    delTest.remove(8);
    assert(delTest.size() == 2);
    int rGone;
    assert(!delTest.get(8, rGone)); // ya no debe encontrarse

    // 1 y 15 deben seguir siendo accesibles pese a que 8 quedó DELETED en medio
    int r1, r15;
    assert(delTest.get(1, r1) && r1 == 1);
    assert(delTest.get(15, r15) && r15 == 1);
    cout << "remove() OK: claves restantes siguen accesibles tras un DELETED intermedio\n";

    // Reinsertar una nueva clave debería poder reusar el slot DELETED
    delTest.increment(22); // 22 % 7 == 1 -> compite por la misma cadena de sondeo
    assert(delTest.size() == 3);
    int r22;
    assert(delTest.get(22, r22) && r22 == 1);
    cout << "reinsercion en slot DELETED OK\n";

    // ---------------------------------------------------------------
    // 5) Resize automático bajo carga alta
    // ---------------------------------------------------------------
    printHeader("Resize automatico (forzar muchas inserciones)");
    HashTableCerrado<long long> small(4, ProbeStrategy::LINEAR);
    long long initialBuckets = small.bucketCount();
    for (long long i = 0; i < 1000; i++) {
        small.increment(i % 300);
    }
    assert(small.size() == 300);
    assert(small.bucketCount() > initialBuckets);
    assert(small.loadFactor() <= 0.7 + 1e-9);
    cout << "Resize OK. bucketCount inicial=" << initialBuckets
         << " -> final=" << small.bucketCount()
         << " (loadFactor=" << small.loadFactor() << ")\n";

    // Verifica que los conteos sobrevivieron el resize correctamente
    for (long long k = 0; k < 300; k++) {
        int cnt;
        assert(small.get(k, cnt));
        // i va de 0 a 999 con i % 300, cada residuo aparece 4 o 3 veces
        assert(cnt == 3 || cnt == 4);
    }
    cout << "Conteos correctos tras resize\n";

    // ---------------------------------------------------------------
    // 6) Comparación de estrategias con misma estructura, distintos objetos
    //    (test cruzado: que QUADRATIC y DOUBLE también sobrevivan resize)
    // ---------------------------------------------------------------
    printHeader("Resize en QUADRATIC y DOUBLE");
    HashTableCerrado<long long> smallQ(4, ProbeStrategy::QUADRATIC);
    HashTableCerrado<long long> smallD(4, ProbeStrategy::DOUBLE);
    for (long long i = 0; i < 500; i++) {
        smallQ.increment(i % 150);
        smallD.increment(i % 150);
    }
    assert(smallQ.size() == 150);
    assert(smallD.size() == 150);
    cout << "QUADRATIC: bucketCount=" << smallQ.bucketCount()
         << " size=" << smallQ.size() << "\n";
    cout << "DOUBLE:    bucketCount=" << smallD.bucketCount()
         << " size=" << smallD.size() << "\n";

    // ---------------------------------------------------------------
    // 7) Estimación de memoria (solo demostrativo, no assert numérico)
    // ---------------------------------------------------------------
    printHeader("Estimacion de memoria");
    cout << "Cerrada LINEAR (long long): " << linear.estimatedMemoryBytes() << " bytes\n";
    cout << "Cerrada LINEAR (string):    " << linearS.estimatedMemoryBytes() << " bytes\n";

    cout << "\nTODOS LOS TESTS DE HT_CERRADO PASARON\n";
    return 0;
}