#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct {
    int k;
    int pnum;
    int mod;
    int* result;
    pthread_mutex_t* mutex;
} FactorialArgs;

// Функция для вычисления факториала
void* Factorial(void* args) {
    FactorialArgs* fa = (FactorialArgs*)args;
    int k = fa->k;
    int mod = fa->mod;
    int pnum = fa->pnum;
    int* result = fa->result;
    pthread_mutex_t* mutex = fa->mutex;

    int local_result = 1;
    for (int i = k; i > 0; i -= pnum) {
        local_result *= i;
        local_result %= mod;
    }

    pthread_mutex_lock(mutex);
    *result *= local_result;
    *result %= mod;
    pthread_mutex_unlock(mutex);

    return NULL;
}

int main(int argc, char** argv) {

    if (argc != 5) {
        printf("Usage: %s -k <number> --pnum=<number of threads> --mod=<modulus>\n", argv[0]);
        return 1;
    }

    int k, pnum, mod;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-k") == 0) {
            k = atoi(argv[i + 1]);
        } else if (strstr(argv[i], "--pnum=") != NULL) {
            pnum = atoi(argv[i] + 7);
        } else if (strstr(argv[i], "--mod=") != NULL) {
            mod = atoi(argv[i] + 6);
        }
    }

    // Создаем массив потоков и структуру для передачи параметров
    pthread_t threads[pnum];
    FactorialArgs args[pnum];
    int result = 1;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    // Запускаем потоки
    for (int i = 0; i < pnum; i++) {
        args[i].k = k - i;
        args[i].pnum = pnum;
        args[i].mod = mod;
        args[i].result = &result;
        args[i].mutex = &mutex;
        pthread_create(&threads[i], NULL, Factorial, (void*)&args[i]);
    }

    // Ожидаем завершения потоков
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("%d! mod %d = %d\n", k, mod, result);

    pthread_mutex_destroy(&mutex);

    return 0;
}