/** @file
  Plik z implementacją obsługi komend.

  @author Julian Kozłowski <jk417694@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "commands.h"
#include "inputPoly.h"
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include "heap.h"

/**
 * Zwraca wartość bool w zaleznosci od tego czy komenda dobrze się kończy, w przypadku błędu wypisuje error.
 * @param[in] l : linia
 * @param[in] index : indeks w tablicy
 * @param[in] numberOfLines: numer linii
 * @return bool
 */
static bool WrongCommand(line l, size_t index, size_t numberOfLines){
    if(l.letters[index] != EOF && l.letters[index] != '\n'){
        fprintf(stderr, "ERROR %ld WRONG COMMAND\n", numberOfLines);
        return true;
    }
    return false;
}

/**
 * Zwraca wartość bool w zaleznosci od tego czy stos jest pusty, w przypadku błędu wypisuje error.
 * @param[in] h : wskaźnik na stos
 * @param[in] numberOfLine : numer tablicy
 * @return bool
 */
static bool IsEmpty(heap *h, size_t numberOfLine){
    if(HeapIsEmpty(h)){
        fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", numberOfLine);
        return true;
    }
    return false;
}

/**
 * Zwraca wartość bool w zaleznosci od tego czy na stosie są 2 wielomiany, w przypadku błędu wypisuje ze jest
 * niedobor wielomianow.
 * @param[in] h : wskaźnik na stos
 * @param[in] numberOfLine : numer tablicy
 * @return bool
 */
static bool HasTwo(heap *h, size_t numberOfLine){
    if(!HeapHasAtleastTwo(h)){
        fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", numberOfLine);
        return true;
    }
    return false;
}

/**
 * Funkcja wypisująca monomian.
 * @param[in] m : monomian
 */
static void MonoPrint(Mono m){
    PolyPrint(m.p);
    printf(",%d)", m.exp);
}

void PolyPrint(Poly p){
    if(PolyIsCoeff(&p))printf("%ld", p.coeff);
    else{
        printf("(");
        for(size_t i = 0; i < p.size; i++){
            MonoPrint(p.arr[i]);
            if(i < p.size - 1){
                printf("+(");
            }
        }
    }
}

void ReadAndDoCommand(line l, size_t w, heap *h){  

    if(!strncmp(l.letters, "ZERO", 4)){
        if(WrongCommand(l, 4, w) || IsEmpty(h, w)){
            return;
        }
        AddHeap(h, PolyZero());
    }

    else if(!strncmp(l.letters, "IS_EQ", 5)){
        if(WrongCommand(l, 5, w) || HasTwo(h, w))return;
        printf("%d\n", PolyIsEq(&h -> heap[h -> headIndex - 1], &h -> heap[h -> headIndex - 2]));
    }

    else if(!strncmp(l.letters, "IS_COEFF", 8)){
        if(WrongCommand(l, 8, w) || IsEmpty(h, w)){
            return;
        }
        printf("%d\n", PolyIsCoeff(&h -> heap[h -> headIndex - 1]));
    }

    else if(!strncmp(l.letters, "IS_ZERO", 7)){
        if(WrongCommand(l, 7, w) || IsEmpty(h, w)){
            return;
        }
        printf("%d\n", PolyIsZero(&h -> heap[h -> headIndex - 1]));
    }

    else if(!strncmp(l.letters, "CLONE", 5)){
        if(WrongCommand(l, 5, w) || IsEmpty(h, w))return;
        AddHeap(h, PolyClone(&h -> heap[h -> headIndex - 1]));
    }

    else if(!strncmp(l.letters, "ADD", 3)){
        if(WrongCommand(l, 3, w) || HasTwo(h, w))return;
        Poly p = PopHeap(h);
        Poly q = PopHeap(h);
        Poly z = PolyAdd(&p, &q);
        AddHeap(h, z);
        PolyDestroy(&p);
        PolyDestroy(&q);
    }

    else if(!strncmp(l.letters, "MUL", 3)){
        if(WrongCommand(l, 3, w) || HasTwo(h, w))return;
        Poly p = PopHeap(h);
        Poly q = PopHeap(h);
        Poly z = PolyMul(&p, &q);
        AddHeap(h, z);
        PolyDestroy(&p);
        PolyDestroy(&q);
    }

    else if(!strncmp(l.letters, "SUB", 3)){
        if(WrongCommand(l, 3, w) || HasTwo(h, w))return;
        Poly p = PopHeap(h);
        Poly q = PopHeap(h);
        Poly z = PolySub(&p, &q);
        AddHeap(h, z);
        PolyDestroy(&p);
        PolyDestroy(&q);
    }

    else if(!strncmp(l.letters, "DEG_BY", 6)){
        if(isalpha(l.letters[6])){
            fprintf(stderr,"ERROR %ld WRONG COMMAND\n", w);
            return;
        }
        if(l.letters[6] != ' ' || !isdigit(l.letters[7])){
              fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", w);
              return;
        }
        char *end;
        size_t deg = strtoul(l.letters + 6, &end, 10);
        if(errno == ERANGE){
            fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", w);
            return;
        }
        if(IsEmpty(h, w))return;
        if((*end != '\n' && *end != EOF)){
            fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", w);
            return;
        }
        printf("%d\n", PolyDegBy(&h -> heap[h -> headIndex - 1], deg));
    }

    else if(!strncmp(l.letters, "DEG", 3)){
        if(WrongCommand(l, 3, w) || IsEmpty(h, w))return;
        printf("%d\n", PolyDeg(&h -> heap[h -> headIndex - 1]));
    }

    else if(!strncmp(l.letters, "AT", 2)){
        if(isalpha(l.letters[2])){
            fprintf(stderr,"ERROR %ld WRONG COMMAND\n", w);
            return;
        }
        if(l.letters[2] != ' ' || (!isdigit(l.letters[3]) && l.letters[3] != '-')){
              fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", w);
              return;
        }
        char *end;
        long deg = strtoll(l.letters + 2, &end, 10);
        if(errno == ERANGE){
            fprintf(stderr,"ERROR %ld AT WRONG VALUE\n", w);
            return;
        }
        if(IsEmpty(h, w))return;
        if((*end != '\n' && *end != EOF)){
            fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", w);
            return;
        }
        Poly p = PopHeap(h);
        Poly z = PolyAt(&p, deg);
        AddHeap(h, z);
        PolyDestroy(&p);
    }

    else if(!strncmp(l.letters, "PRINT", 5)){
        if(WrongCommand(l, 5, w) || IsEmpty(h, w))return;
        PolyPrint(h -> heap[h -> headIndex - 1]);
        putchar('\n');
    }

    else if(!strncmp(l.letters, "POP", 3)){
        if(WrongCommand(l, 3, w) || IsEmpty(h, w))return;
        Poly p = PopHeap(h);
        PolyDestroy(&p);
    }

    else if(!strncmp(l.letters, "NEG", 3)){
        if(WrongCommand(l, 3, w) || IsEmpty(h, w))return;
        Poly p = PopHeap(h);
        AddHeap(h, PolyNeg(&p));
        PolyDestroy(&p);
    }

    else if(!strncmp(l.letters, "COMPOSE", 7)){
        if(isalpha(l.letters[7])){
            fprintf(stderr,"ERROR %ld WRONG COMMAND\n", w);
            return;
        }
        if(l.letters[7] != ' ' || !isdigit(l.letters[8])){
              fprintf(stderr, "ERROR %ld COMPOSE WRONG PARAMETER\n", w);
              return;
        }
        char *end;
        size_t k = strtoul(l.letters + 7, &end, 10);
        if(errno == ERANGE){
            fprintf(stderr, "ERROR %ld COMPOSE WRONG PARAMETER\n", w);
            return;
        }
        if((*end != '\n' && *end != EOF && *end != '\0')){
            fprintf(stderr, "ERROR %ld COMPOSE WRONG PARAMETER\n", w);
            return;
        }
        if(!HeapHasAtleastKElements(h, k + 1)){
            fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", w);
            return;
        }
        Poly p = PopHeap(h);
        Poly q[k];
        for(size_t i = 0; i < k; i++)q[k - i - 1] = PopHeap(h);
        AddHeap(h, PolyCompose(&p, k, q));
        PolyDestroy(&p);
        for(size_t i = 0; i < k; i++)PolyDestroy(&q[i]);
    }
    else{
        fprintf(stderr,"ERROR %ld WRONG COMMAND\n", w);
    }
}