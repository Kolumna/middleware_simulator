#include <stdio.h>
#include <unistd.h>
#include <string.h>

char auth[16] = "";
char redirect[1024] = "__REDIRECT__";

/*
    Program symuluje proces request, ktory posiada
    zmienna auth okreslajaca role uzytkownika. Proces ten oczekuje na przydzielenie trasy przez middleware,
    ktore jest sygnalizowane przez zmiane zawartosci zmiennej redirect na inna wartosc niz "__REDIRECT__".
    Program wypisuje swoj PID, wartosc zmiennej auth oraz adres tej zmiennej w pamieci.
    Po wykryciu zmiany w zmiennej redirect, program wypisuje informacje o przekierowaniu i konczy dzialanie.
*/

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Uzycie: %s <USER | ADMIN>\n", argv[0]);
        return 1;
    }
    if (!strcmp(argv[1], "USER") && !strcmp(argv[1], "ADMIN"))
    {
        printf("Nieprawidlowa rola. Dostepne role: USER, ADMIN\n");
        return 1;
    }
    strncpy(auth, argv[1], sizeof(auth) - 1);

    printf("\033[1;34m======================================================\n");
    printf("Symulator Middleware - request    \n");
    printf("======================================================\033[0m\n\n");

    int i = 0;
    while (1)
    {
        printf("----------------------\n");
        printf("\033[1;32mPID: %d\033[0m\n", getpid());
        printf("Auth = \033[1;31m%s\033[0m | \033[1;34m%p\033[0m\n", auth, (void *)&auth);

        if (redirect[0] != '_')
        {
            printf("\033[1;33m[!] Wykryto przekierowanie na: %s\033[0m\n", redirect);
            break;
        }
        else
        {
            printf("Oczekiwanie na przekierowanie od %d sekund\n", ++i);
        }
        sleep(1);
    }

    return 0;
}
