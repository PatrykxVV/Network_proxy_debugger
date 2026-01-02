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

Dodatkowy graf ilustrujacy działanie

[ nc -l 8080 ]        = backend
        ▲
        │ TCP
        │
[ tcp_proxy :9000 ]   = proxy
        ▲
        │ TCP
        │
[ nc 127.0.0.1 9000 ] = klient


## Testowanie (ręcznie)
Proxy bylo manualnie testowane z uzyciem netcat(nc) w celu zweryfikowania end-to-end TCP forwarding i polączenia pomiędzy 'lifecycle behavior'

### Start backend server
'''bash
nc -l 8080

### Start proxy
./tcp_proxy

### Start client
nc 127.0.0.1 9000

W katalogu python/ znajduje się prosty framework scenariuszy testowych, który symuluje rzeczywiste problemy sieciowe.

Przykładowe scenariusze:

wolny klient (wysyłanie danych bajt po bajcie),

brak odpowiedzi backendu (timeout),

nagłe zerwanie połączenia w trakcie transmisji.

Uruchamianie scenariuszy:

python scenarios.py backend-timeout
python scenarios.py slow-client
python scenarios.py disconnect

## Budowanie

```bash
cd cpp/proxy
mkdir build && cd build
cmake ..
make

