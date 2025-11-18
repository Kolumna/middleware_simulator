#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

/*
    Program symuluje middleware, ktory monitoruje proces request
    i na podstawie roli uzytkownika (zmienna auth) decyduje o przekierowaniu
    (zmienna redirect). Program przyjmuje jako argument PID procesu request
    oraz docelowy URL. Sprawdza czy rola uzytkownika pozwala na dostep do tego URL
    i odpowiednio modyfikuje zmienna redirect w procesie request.
*/

// Funkcja znajduje adres ciagu znakow w pamieci procesu o danym PID
long get_address_of_string_in_process(pid_t pid, const char *target)
{
  FILE *maps;
  char filename[256], line[256];

  snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
  maps = fopen(filename, "r");
  if (!maps)
  {
    perror("fopen");
    return -1;
  }

  int t_len = strlen(target);

  while (fgets(line, sizeof(line), maps))
  {

    unsigned long start, end;
    char perms[5];

    if (sscanf(line, "%lx-%lx %4s", &start, &end, perms) != 3)
      continue;

    if (perms[0] != 'r')
      continue;

    unsigned long addr;
    for (addr = start; addr < end; addr++)
    {

      errno = 0;
      long data = ptrace(PTRACE_PEEKDATA, pid, (void *)addr, NULL);

      if (errno != 0)
        continue;

      unsigned char b0 = (unsigned char)(data & 0xFF);

      if (b0 == (unsigned char)target[0])
      {
        int match = 1;

        int i;
        for (i = 1; i < t_len; i++)
        {
          errno = 0;
          long d = ptrace(PTRACE_PEEKDATA, pid, (void *)(addr + i), NULL);
          if (errno != 0 || (unsigned char)(d & 0xFF) != (unsigned char)target[i])
          {
            match = 0;
            break;
          }
        }

        if (match)
        {
          fclose(maps);
          return addr;
        }
      }
    }
  }

  fclose(maps);
  return -1;
}

// Funkcja zapisuje nowy ciag znakow do pamieci procesu o danym PID pod wskazanym adresem
int write_string(pid_t pid, long addr, const char *newstr)
{
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  strncpy(buffer, newstr, sizeof(buffer) - 1);

  size_t word = sizeof(long);

  int i;
  for (i = 0; i < sizeof(buffer); i += word)
  {
    long chunk = 0;
    memcpy(&chunk, buffer + i, word);

    if (ptrace(PTRACE_POKEDATA, pid, (void *)(addr + i), (void *)chunk) == -1)
    {
      perror("POKEDATA");
      return -1;
    }
  }

  return 0;
}

int main(int argc, char **argv)
{
  if (argc != 3)
  {
    printf("Uzycie: %s <pid> <badany_url>\n", argv[0]);
    return 1;
  }

  printf("\033[1;34m======================================================\n");
  printf("Symulator Middleware - server     \n");
  printf("======================================================\033[0m\n");

  pid_t pid = atoi(argv[1]);
  char *url = argv[2];

  printf("Skanowanie podanego URL %d: \"%s\"\n", pid, url);

  if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1)
  {
    perror("PTRACE_ATTACH");
    return 1;
  }
  waitpid(pid, NULL, 0);

  // adres ciagu przekierowania
  long redirect_addr = get_address_of_string_in_process(pid, "__REDIRECT__");
  if (redirect_addr == -1)
  {
    printf("Nie znaleziono ciagu %s w procesie %d\n", url, pid);
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    return 1;
  }
  printf("\033[1;33m[!] Znaleziono adres przekierowania w procesie %d: %p\033[0m\n", pid, (void *)redirect_addr);

  // adres ciagu roli
  long role_addr = get_address_of_string_in_process(pid, "ADMIN");
  if (role_addr == -1)
  {
    printf("Nie znaleziono ciagu ADMIN w procesie %d. Przekierowanie na strone logowania.\n", pid);
    role_addr = get_address_of_string_in_process(pid, "USER");

    if (role_addr == -1)
    {
      printf("Nie znaleziono ciagu USER w procesie %d\n. Blad", pid);
      ptrace(PTRACE_DETACH, pid, NULL, NULL);
      return 1;
    }

    // przekierowanie na strone logowania
    if (write_string(pid, redirect_addr, "/login") == -1)
    {
      ptrace(PTRACE_DETACH, pid, NULL, NULL);
      return 1;
    }

    return 0;
  }

  // przekierowanie na docelowy URL
  if (write_string(pid, redirect_addr, url) == -1)
  {
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    return 1;
  }

  ptrace(PTRACE_DETACH, pid, NULL, NULL);

  printf("Podany proces ma odpowiednia role do przekierowania na ten URL! → \"%s\"\n", url);
  return 0;
}
