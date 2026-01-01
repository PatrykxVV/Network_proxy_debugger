# Network Proxy Debugger

Minimalny TCP proxy w C++ do nauki i debugowania przepływu ruchu
sieciowego (client → proxy → backend), szczególnie w sytuacjach,
gdy odpowiedź nie wraca lub jest opóźniona.

---

## Cel

- nauka niskopoziomowej komunikacji TCP w C++
- zrozumienie wzorca proxy
- debugowanie problemów sieciowych
- budowa projektu krok po kroku

---

## Architektura

Klient ---> Proxy ---> Backend

Klient <--- Proxy <--- Backend

## Budowanie

```bash
cd cpp/proxy
mkdir build && cd build
cmake ..
make
