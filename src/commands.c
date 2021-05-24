/** @file
  Interfejs struktury stosu, zaimplementowanej tablicowo.

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

static bool WrongCommand(line l, size_t index, size_t w){
    if(l.letters[index] != '\0' && l.letters[index] != '\n'){
        fprintf(stderr, "ERROR %ld WRONG COMMAND\n", w);
        return true;
    }
    return false;
}

static bool isEmpty(heap *h, size_t w){
    if(HeapIsEmpty(h)){
        fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", w);
        return true;
    }
    return false;
}

static bool HasTwo(heap *h, size_t w){
    if(!HeapHasAtleastTwo(h)){
        fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", w);
        return true;
    }
    return false;
}

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

void ReadCommand(line l, size_t w, heap *h){
    if(!strncmp(l.letters, "ZERO", 4)){
        if(WrongCommand(l, 4, w) || isEmpty(h, w)){
            return;
        }
        AddHeap(h, PolyZero());
    }
    else if(!strncmp(l.letters, "IS_EQ", 5)){
        if(WrongCommand(l, 5, w) || HasTwo(h, w))return;
        printf("%d\n", PolyIsEq(&h -> heap[h -> headIndex - 1], &h -> heap[h -> headIndex - 2]));
    }
    else if(!strncmp(l.letters, "IS_COEFF", 8)){
        if(WrongCommand(l, 8, w) || isEmpty(h, w)){
            return;
        }
        printf("%d\n", PolyIsCoeff(&h -> heap[h -> headIndex - 1]));
    }
    else if(!strncmp(l.letters, "IS_ZERO", 7)){
        if(WrongCommand(l, 7, w) || isEmpty(h, w)){
            return;
        }
        printf("%d\n", PolyIsZero(&h -> heap[h -> headIndex - 1]));
    }
    else if(!strncmp(l.letters, "CLONE", 5)){
        if(WrongCommand(l, 5, w) || isEmpty(h, w))return;
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
        if(isEmpty(h, w))return;
        if((*end != '\n' && *end != EOF)){
            fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", w);
            return;
        }
        printf("%d\n", PolyDegBy(&h -> heap[h -> headIndex - 1], deg));
    }
    else if(!strncmp(l.letters, "DEG", 3)){
        if(WrongCommand(l, 3, w) || isEmpty(h, w))return;
        printf("%d\n", PolyDeg(&h -> heap[h -> headIndex - 1]));
    }
    else if(!strncmp(l.letters, "AT", 2)){
        if(l.letters[2] != ' ' || (!isdigit(l.letters[3]) && l.letters[3] != '-')){
            fprintf(stderr,"ERROR %ld AT WRONG VALUE\n", w);
            return;
        }
        char *end;
        long deg = strtoll(l.letters + 2, &end, 10);
        if(errno == ERANGE){
            fprintf(stderr,"ERROR %ld AT WRONG VALUE\n", w);
            return;
        }
        if(isEmpty(h, w))return;
        if((*end != '\n' && *end != EOF)){
            fprintf(stderr, "ERROR %ld DEG BY WRONG VALUE\n", w);
            return;
        }
        Poly p = PopHeap(h);
        Poly z = PolyAt(&p, deg);
        AddHeap(h, z);
        PolyDestroy(&p);
    }
    else if(!strncmp(l.letters, "PRINT", 5)){
        if(WrongCommand(l, 5, w) || isEmpty(h, w))return;
        PolyPrint(h -> heap[h -> headIndex - 1]);
        putchar('\n');
    }
    else if(!strncmp(l.letters, "POP", 3)){
        if(WrongCommand(l, 3, w) || isEmpty(h, w))return;
        Poly p = PopHeap(h);
        PolyDestroy(&p);
    }
    else if(!strncmp(l.letters, "NEG", 3)){
        if(WrongCommand(l, 3, w) || isEmpty(h, w))return;
        Poly p = PopHeap(h);
        AddHeap(h, PolyNeg(&p));
        PolyDestroy(&p);
    }
    else{
        fprintf(stderr,"ERROR %ld WRONG COMMAND\n", w);
    }
}