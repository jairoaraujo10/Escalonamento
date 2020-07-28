#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Estrutura de uma tarefa
typedef struct {
    int id;             // Identificador da tarefa
    int P;              // Período
    int C;              // Tempo de execução
    int D;              // Deadline
} Tarefa;

// Estrutura de um conjunto de tarefas
typedef struct {
    int n;              // Quantidade de tarefas
    Tarefa *tarefas;    // Array de tarefas
} Tarefas;

// Estrutura do ciclo de execução
typedef struct {
    int length;         // Tamanho do ciclo de execução das tarefa
    int *tarefas;       // Fila de execução de tarefas
} Ciclo;

Tarefas *carregaArquivo(char *argv[]) {
    FILE *file = fopen(argv[1], "r");
    size_t len = 100;
    char *line = malloc(len);
    int i;

    Tarefas *t = (Tarefas *) malloc(sizeof(Tarefas));
    t->n = -1;
    while (getline(&line, &len, file) > 0) t->n++;
    t->tarefas = (Tarefa *) calloc(t->n, sizeof(Tarefa));
    fclose(file);

    file = fopen(argv[1], "r");
    getline(&line, &len, file);
    for (i = 0; getline(&line, &len, file) > 0; i++) {
        char *ptr;
        t->tarefas[i].id = i;
        t->tarefas[i].P = (int) strtol(strtok(line, "\t"), &ptr, 10);
        t->tarefas[i].C = (int) strtol(strtok(NULL, "\t\t"), &ptr, 10);
        t->tarefas[i].D = (int) strtol(strtok(NULL, "\t\t"), &ptr, 10);
    }
    free(line);
    fclose(file);
    return t;
}

int mdc(int a, int b) {
    if (b == 0) return a;
    return mdc(b, a % b);
}

// Calcula o período de cada hiperciclo pelo MMC dos períodos
int cicloLength(Tarefas *t) {
    int res = 1, i;
    for (i = 0; i < t->n; i++) {
        res = res * t->tarefas[i].P / mdc(res, t->tarefas[i].P);
    }
    return res;
}

// Cria um ciclo de execução e preenche com o valor -1
Ciclo *criarHiperciclo(Tarefas *t) {
    int i;
    Ciclo *c = (Ciclo *) malloc(sizeof(Ciclo));
    c->length = cicloLength(t);
    c->tarefas = (int *) calloc(c->length, sizeof(int));
    for (i = 0; i < c->length; i++) c->tarefas[i] = -1;
    return c;
}

void imprimirTarefas(Tarefas *t) {
    int i;
    printf("\tP\tC\tD\n");
    for (i = 0; i < t->n; i++) {
        printf("T%d\t%d\t%d\t%d\n", t->tarefas[i].id, t->tarefas[i].P, t->tarefas[i].C, t->tarefas[i].D);
    }
    printf("\n");
}

void imprimirCiclo(Tarefas *t, Ciclo *c) {
    int i, j;
    printf("---- Escala de Tempo ----\n");
    for (i = 0; i < t->n; i++) {
        printf("T%d\t", t->tarefas[i].id);
        for (j = 0; j < c->length; j++) {
            if (t->tarefas[i].id == c->tarefas[j]) printf("0 ");
            else printf("- ");
        }
        printf("\n");
    }
    printf("---- Escala de Tempo ----\n\n");
}

// Realiza o teste de escalonabilidade para o algoritmo Rate Monotonic
int testRM(Tarefas *t) {
    int i;
    float uti = 0, limiar = (float) t->n * ((float) pow(2, 1 / (float) t->n) - 1);
    for (i = 0; i < t->n; i++) {
        uti += (float) t->tarefas[i].C / (float) t->tarefas[i].P;
    }
    if (uti <= limiar) {
        printf("Utilizacao total %.2f <= Limiar: %.2f - O sistema eh escalonavel.\n\n", uti, limiar);
        return 1;
    }
    printf("Utilizacao total %.2f > Limiar: %.2f - O sistema pode nao ser escalonavel.\n\n", uti, limiar);
    return 0;
}

// PENDENTE - Realiza o teste de escalonabilidade para o algoritmo Deadline Monotonic
int testDM(Tarefas *t) {
    printf("Test Deadline Monotonic\n\n");
    return 0;
}

// PENDENTE - Realiza o teste de escalonabilidade para o algoritmo Earliest Deadline First
int testEDF(Tarefas *t) {
    printf("Test Earliest Deadline First\n\n");
    return 0;
}

// Insertion Sort - Ordena por (0) id, (1) P ou (2) D.
Tarefa *ordenaTarefas(Tarefas *t, int ordenaPor) {
    Tarefa element;
    int i, j;
    for (i = 1; i < t->n; i++) {
        element = t->tarefas[i];
        j = i - 1;

        while (j >= 0 && ((ordenaPor == 0 && t->tarefas[j].id > element.id) ||
                          (ordenaPor == 1 && t->tarefas[j].P > element.P) ||
                          (ordenaPor == 2 && t->tarefas[j].D > element.D))) {
            t->tarefas[j + 1] = t->tarefas[j];
            j = j - 1;
        }
        t->tarefas[j + 1] = element;
    }
    return t->tarefas;
}

Ciclo *rateMonotonic(Tarefas *t) {
    int i, j, k, l;
    printf("Rate Monotonic - ");
    testRM(t);

    Ciclo *c = criarHiperciclo(t);

    ordenaTarefas(t, 1);

    for (i = 0; i < t->n; i++) {
        for (j = 0; j < c->length; j += t->tarefas[i].P) {
            k = j;
            for (l = t->tarefas[i].C; l > 0; l--) {
                while (c->tarefas[k] != -1) {
                    k++;
                }
                c->tarefas[k] = t->tarefas[i].id;
            }
        }
    }

    ordenaTarefas(t, 0);
    return c;
}

Ciclo *deadlineMonotonic(Tarefas *t) {
    int i, j, k, l;
    printf("Deadline Monotonic - ");
    testDM(t);

    Ciclo *c = criarHiperciclo(t);

    ordenaTarefas(t, 2);

    for (i = 0; i < t->n; i++) {
        for (j = 0; j < c->length; j += t->tarefas[i].P) {
            k = j;
            for (l = t->tarefas[i].C; l > 0; l--) {
                while (c->tarefas[k] != -1) {
                    k++;
                }
                c->tarefas[k] = t->tarefas[i].id;
            }
        }
    }

    ordenaTarefas(t, 0);
    return c;
}

// Estrutura auxiliar usada para representar tarefas no algoritmo de EDF
typedef struct {
    int id;
    int C;
    int PAnt;
    int DAbs;
    int PProx;
} aux;

// Insertion Sort - Ordena por deadline absoluto
aux *ordenaAux(aux *tfs, int n) {
    aux element;
    int i, j;
    for (i = 1; i < n; i++) {
        element = tfs[i];
        j = i - 1;
        while (j >= 0 && tfs[j].DAbs > element.DAbs) {
            tfs[j + 1] = tfs[j];
            j = j - 1;
        }
        tfs[j + 1] = element;
    }
    return tfs;
}

Ciclo *earliestDeadlineFirst(Tarefas *t) {
    int i, j;
    printf("Earliest Deadline First - ");
    testEDF(t);

    Ciclo *c = criarHiperciclo(t);

    aux *tfs = (aux *) calloc(t->n, sizeof(aux));
    for (i = 0; i < t->n; i++) {
        tfs[i].id = t->tarefas[i].id;
        tfs[i].C = 0;
        tfs[i].PAnt = 0;
        tfs[i].PProx = t->tarefas[i].P;
        tfs[i].DAbs = tfs[i].PAnt + t->tarefas[i].D;
    }

    for (i = 0; i < c->length; i++) {
        for (j = 0; j < t->n; j++) {
            if (tfs[j].PAnt == i) tfs[j].C = t->tarefas[tfs[j].id].C;
        }
        ordenaAux(tfs, t->n);
        for (j = 0; j < t->n; j++) {
            if (tfs[j].C > 0) {
                c->tarefas[i] = tfs[j].id;
                tfs[j].C--;
                if (tfs[j].C == 0) {
                    tfs[j].PAnt = tfs[j].PProx;
                    tfs[j].PProx += t->tarefas[tfs[j].id].P;
                    tfs[j].DAbs = tfs[j].PAnt + t->tarefas[tfs[j].id].D;
                }
                break;
            }
        }
    }
    free(tfs);
    return c;
}

void liberaTarefas(Tarefas *t) {
    free(t->tarefas);
    free(t);
}

void liberaCiclo(Ciclo *c) {
    free(c->tarefas);
    free(c);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        Tarefas *t = carregaArquivo(argv);
        imprimirTarefas(t);

        Ciclo *c1 = rateMonotonic(t);
        imprimirCiclo(t, c1);
        liberaCiclo(c1);

        Ciclo *c2 = deadlineMonotonic(t);
        imprimirCiclo(t, c2);
        liberaCiclo(c2);

        Ciclo *c3 = earliestDeadlineFirst(t);
        imprimirCiclo(t, c3);
        liberaCiclo(c3);

        liberaTarefas(t);
    }
    return 0;
}
