/* Nome: Gustavo Mendes Maciel
 * NUSP: 9298062
 *
 * Nome: Marcelo Baiano Pastorino Trylesinski
 * NUSP: 9297996
 *
 * MAC0219 - MiniEP2
 */

#define COUNT_LIMIT 10000
#define MODE_COUNT 'c'
#define MODE_MANAGER 'm'

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Struct com os dados necessarios para uma thread */
typedef struct str_thdata
{
    char type;
    int id;
    int position;
    int move;
} thdata;

/* Vetor global que representa as pedras */
thdata *rocks;
thdata empty_data = {0, -1, -1, 0};
int n;

/* Variaveis globais */
int count;
int frogs_created;
int deadlock;
char mode;

/* Mutex */
pthread_mutex_t mutex;

/* Variavel cond */
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/* Funcao da thread manager */
void *manager_func(void *arg)
{
    pthread_mutex_lock(&mutex);
    while (frogs_created < 2 * n)
        pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    while (!deadlock)
    {
        pthread_mutex_lock(&mutex);
        int i;
        for (i = 0; i < 2 * n + 1; i++)
        {
            if (rocks[i].id != empty_data.id)
                if (rocks[i + rocks[i].move].id == empty_data.id ||
                    rocks[i + rocks[i].move * 2].id == empty_data.id)
                break;
        }
        if (i == 2 * n + 1) deadlock = 1;
        pthread_mutex_unlock(&mutex);
    }
}

/* Funcao da thread sapo. Recebe um struct thdata */
void *frog_action(void *arg)
{
    thdata *data = (thdata *) arg;

    pthread_mutex_lock(&mutex);
    frogs_created++;
    if (frogs_created >= 2 * n) pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    while (!deadlock)
    {
        pthread_mutex_lock(&mutex);
        if (mode == MODE_COUNT && count >= COUNT_LIMIT) deadlock = 1;
        if (rocks[data->position + data->move].id == empty_data.id)
        {
            rocks[data->position + data->move] = rocks[data->position];
            rocks[data->position] = empty_data;
            data->position += data->move;
            count = 0;
            printf("%c%d pulou 1 casa\n", data->type, data->id);
        }
        else if (rocks[data->position + 2 * data->move].id == empty_data.id)
        {
            rocks[data->position + 2 * data->move] = rocks[data->position];
            rocks[data->position] = empty_data;
            data->position += 2 * data->move;
            count = 0;
            printf("%c%d pulou 2 casas\n", data->type, data->id);
        }
        else
        {
            count++;
        }
        pthread_mutex_unlock(&mutex);
    }
}

/* Funcao main */
int main(int numargs, char *arg[])
{
    int result;
    pthread_t *frogs;
    pthread_t manager;
    thdata *thread_args;

    n = strtol(arg[1], NULL, 10);
    mode = arg[2][0];
    frogs = malloc((2 * n) * sizeof (pthread_t));
    thread_args = malloc((2 * n) * sizeof (thdata));
    rocks = malloc((2 * n + 1) * sizeof (thdata));
    count = 0;
    deadlock = 0;
    frogs_created = 0;
    pthread_mutex_init(&mutex, NULL);

    printf("\nNúmero de sapos/rãs: %d\n", n);

    if (mode == MODE_MANAGER)
    {
        printf("Criando thread manager\n");
        result = pthread_create(&manager, NULL, manager_func, NULL);
        assert(!result);
    }

    /* Cria as threads de ras */
    for (int i = 0; i < n; i++)
    {
        thread_args[i].type = 'R';
        thread_args[i].id = i;
        thread_args[i].position = i;
        thread_args[i].move = 1;
        printf("Criando thread rã %c%d\n", thread_args[i].type, thread_args[i].id);
        rocks[i] = thread_args[i];
        result = pthread_create(&frogs[i], NULL, frog_action, (void *) &thread_args[i]);
        assert(!result);
    }

    /* Cria as threads de sapos */
    for (int i = n; i < 2 * n; i++)
    {
        thread_args[i].type = 'S';
        thread_args[i].id = i;
        thread_args[i].position = i + 1;
        thread_args[i].move = -1;
        printf("Criando thread sapo %c%d\n", thread_args[i].type, thread_args[i].id);
        rocks[i+1] = thread_args[i];
        result = pthread_create(&frogs[i], NULL, frog_action, (void *) &thread_args[i]);
        assert(!result);
    }

    rocks[n] = empty_data;

    /* Espera as threads terminarem */
    for (int i = 0; i < 2 * n; i++)
    {
        result = pthread_join(frogs[i], NULL);
        assert(!result);
        printf("Thread %c%d finalizada\n", thread_args[i].type, thread_args[i].id);
    }

    if (mode == MODE_MANAGER)
    {
        result = pthread_join(manager, NULL);
        assert(!result);
        printf("Thread manager finalizada\n");
    }

    printf("\nEstado final da lagoa:\n");
    for (int i = 0; i < 2 * n + 1; i++)
        printf("[%c%d]  ", rocks[i].type, rocks[i].id);
    printf("\n\n");

    /* Desaloca o que foi alocado na memoria */
    free(frogs);
    free(thread_args);
    free(rocks);
    pthread_mutex_destroy(&mutex);

    exit(EXIT_SUCCESS);
}
