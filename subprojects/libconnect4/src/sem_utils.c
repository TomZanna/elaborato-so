#include <errno.h>
#include <sys/sem.h>

int sem_lib_wait(int sem_id, int sem_num, int sem_op) {
  static struct sembuf sops = {0};

  sops.sem_num = sem_num;
  sops.sem_op = sem_op;

  // se vengo risvegliato da un interrupt, torno in attesa
  int ret;
  do {
    ret = semop(sem_id, &sops, 1);
  } while (ret == -1 && errno == EINTR);

  return ret;
}

int sem_lib_signal(int sem_id, int sem_num, int sem_op) {
  static struct sembuf sops = {0};

  sops.sem_num = sem_num;
  sops.sem_op = sem_op;

  return semop(sem_id, &sops, 1);
}
