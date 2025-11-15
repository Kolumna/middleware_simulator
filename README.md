# Middleware Simulator
Ten projekt zawiera dwa programy: `request` oraz `middleware`, które symulują prosty mechanizm przekierowywania żądań na podstawie ról użytkowników. Został tu użyty PTRACE do manipulacji pamięcią procesu w celu zmiany zmiennej `redirect` w procesie `request`.
## request.c
Program `request` symuluje proces żądania, który posiada zmienną `auth` określającą rolę użytkownika. Proces ten oczekuje na przydzielenie trasy przez middleware, które jest sygnalizowane przez zmianę zawartości zmiennej `redirect` na inną wartość niż `"__REDIRECT__"`. Program wypisuje swój PID, wartość zmiennej `auth` oraz adres tej zmiennej w pamięci. Po wykryciu zmiany w zmiennej `redirect`, program wypisuje informację o przekierowaniu i kończy działanie.
## middleware.c
Program `middleware` symuluje middleware, który monitoruje proces `request` i na podstawie roli użytkownika (zmienna `auth`) decyduje o przekierowaniu (zmienna `redirect`). Program przyjmuje jako argument PID procesu `request` oraz docelowy URL. Sprawdza czy rola użytkownika pozwala na dostęp do tego URL i odpowiednio modyfikuje zmienną `redirect` w procesie `request`.
## Kompilacja
Aby skompilować oba programy, użyj następujących poleceń:
```bash
cd src
make
```
## Uruchomienie
Najpierw uruchom program `request` w tle:
```bash
./request <ADMIN | USER>
```
Następnie, użyj PID wyświetlonego przez program `request`, aby uruchomić program `middleware` z odpowiednim URL-em:
```bash
./middleware <PID_request> <URL_docelowy>
```

## Docker
Aby uruchomić oba programy w kontenerze Docker, użyj następujących poleceń:
```bash
docker compose up -d
docker compose exec app bash -c "cd /src && make"
docker compose exec app ./request <ADMIN | USER>
docker compose exec app ./middleware <PID_request> <URL_docelowy>
```
Pamiętaj, aby zastąpić `<PID_request>` rzeczywistym PID-em procesu `request`, `<URL_docelowy>` odpowiednim URL-em oraz `<ADMIN | USER>` odpowiednią rolą użytkownika.