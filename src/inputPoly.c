#define _GNU_SOURCE
#include "poly.h"
#include "Commands.h"
#include "inputPoly.h"
#include "heap.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define CHECK_PTR(p)  \
  do {                \
    if (p == NULL) {  \
      exit(1);        \
    }                 \
  } while (0)

static bool LineIsPoly(line actualLine){
    assert(actualLine.letters != NULL);
    return isdigit(actualLine.letters[0]) || actualLine.letters[0] == '(' || actualLine.letters[0] == '-';
}

static bool LineIsCommand(line actualLine){
    assert(actualLine.letters != NULL && actualLine.length != 0);
    return isalpha(actualLine.letters[0]);
}

static bool LineIsCommentOrEmpty(line actualLine){
    assert(actualLine.letters != NULL && actualLine.length != 0);
    return actualLine.letters[0] == '#' || actualLine.letters[0] == '\n' || actualLine.letters[0] == EOF;
}

static bool IsAllowed(char a){
    return isdigit(a) || a == '\0' || a == ',' || a == '(' || a == ')' || a == '+' || a == '\0' || a == '\n' || a == '-' || a == EOF;
}

static bool CheckMono(line actualLine, size_t start, size_t end);

static bool CheckPoly(line actualLine, size_t start, size_t end){
    bool result = true;
    if(isdigit(actualLine.letters[start]) || actualLine.letters[start] == '-'){
        if(actualLine.letters[start] == '-')start++;
        while(start <= end && isdigit(actualLine.letters[start])){
            start++;
        }
        return start >= end;
    }

    size_t heap = 0;
    if(actualLine.letters[start] == '('){
        heap++;
        for(size_t i = start + 1; i <= end; ++i){
            result &= IsAllowed(actualLine.letters[i]);
            if(actualLine.letters[i] == '(')heap++;
            if(actualLine.letters[i] == ')')heap--;
            if(heap == 0){
                result &= CheckMono(actualLine, start + 1, i - 1);
                if(actualLine.letters[i+1] == '+'){
                    i++;
                    result &= actualLine.letters[i + 1] == '(';
                }
                start = i + 1;
            }
        }
    }
    result &= heap == 0;
    return result;
}

static bool CheckMono(line actualLine, size_t start, size_t end){
    bool result = true;
    result &= isdigit(actualLine.letters[end]) != 0 ? 1 : 0;
    while(start < end && isdigit(actualLine.letters[end])){
        end--;
    }
    result &= actualLine.letters[end] == ',';
    result &= CheckPoly(actualLine, start, end - 1);
    return result;
}

static void PolyIsWrong(size_t w){
    fprintf(stderr, "ERROR %ld WRONG POLY\n", w);
}

static bool CheckExp(long k){
    return k >= 0 && k <= INT_MAX;
}

static line ReadLine(){
    char *buffor = NULL;
    size_t length = 0;
    int nRead = 0;
    nRead = getline(&buffor, &length, stdin);
    CHECK_PTR(buffor);
    if(nRead == -1) free(buffor);
    return nRead == -1 ? (line) {.length = 0, .letters = NULL} : (line) {.length = nRead, .letters = buffor};
}

static size_t numberOfPolys(line polyToRead, size_t start, size_t end){
    size_t numberOfPolys = 1;
    size_t heap = 0;
    for(int i = start; i < end; i++){
        if(polyToRead.letters[i] == '(')heap++;
        if(polyToRead.letters[i] == ')')heap--;
        if(polyToRead.letters[i] == '+' && !heap)numberOfPolys++;
    }
    return numberOfPolys;
}

// nie alokujemy zawartości tablic, tylko całą tablicę linii(chyba zaleznie od strcpy)
// rozdzielamy sobie na osobne Monos
static Poly readMonos(line polyToRead, bool *isValid, size_t start, size_t end);

size_t findIndex(char *l, size_t start, size_t end){
    size_t heap = 0;
    for(size_t j = start; j < end; j++){
        if(l[j] == '(')heap++;
        if(l[j] == ')')heap--;
        if(l[j] == ',' && heap == 1)return j;
    }
    return 0;
}
static Mono readSingleMono(line monoToRead, bool *isValid, size_t start, size_t end){
    size_t monoLength = monoToRead.length;
    size_t startIndex = findIndex(monoToRead.letters, start, end);
    Mono result;
    char *endC;
    long e = strtol(monoToRead.letters + startIndex + 1, &endC, 10);
    if(*endC != ')' || !CheckExp(e))*isValid = false;
    result.exp = e;
    result.p = readMonos(monoToRead, isValid, start + 1, startIndex - 1);
    //return (Mono) {.exp = (poly_exp_t) strtol(monoToRead.letters + startIndex, &end, 10), .p = readMonos(MakeSubLine(monoToRead, 1, startIndex - 1))};
    return result;
}
// osobny Case dla wielomianów stałych
bool PolyIsConst(line l, size_t start){
    return l.letters[start] != '(';
}

static Poly readMonos(line polyToRead, bool *isValid, size_t start, size_t end){
    if(PolyIsConst(polyToRead, start)){
        char *endC;
        long e = strtol(polyToRead.letters + start, &endC, 10);
        if((*endC != '\n' && *endC != EOF && *endC != '\0' && *endC != ',') || errno == ERANGE)*isValid = false;
        return PolyFromCoeff(e);
    }
    size_t nPol = numberOfPolys(polyToRead, start, end);
    size_t usedMonos = nPol;
    size_t heap = 0;
    size_t indexOfMono = 0;
    Mono* monosToAdd = malloc(sizeof(Mono) * nPol);

    for(size_t i = start; i <= end; ++i){
        if(polyToRead.letters[i] == '(')heap++;
        if(polyToRead.letters[i] == ')')heap--;
        if(heap == 0){
            monosToAdd[indexOfMono] = readSingleMono(polyToRead, isValid, start, i);
            if(polyToRead.letters[i+1] == '+')i++;
            start = i + 1;
            usedMonos--;
            indexOfMono++;
        }
    }

    Poly result = PolyAddMonos(nPol, monosToAdd);
    free(monosToAdd);
    return result;
}

static void LineUp(line *l){
    char *s = l -> letters;
    while(*s){
        *s = toupper((unsigned char) *s); s++;
    }
}

void ReadFile(){
    line l = ReadLine();
    // LineUp(&l);
    size_t numberOfLine = 1;
    Poly p;
    heap *h = NULL;
    bool isValid;
    IniHeap(&h);
    while(l.letters != NULL){
        if(LineIsPoly(l) && CheckPoly(l, 0, l.length - 2)){
            isValid = true;
            p = readMonos(l, &isValid, 0, l.length - 2);
            if(!isValid){
                PolyDestroy(&p);
                PolyIsWrong(numberOfLine);
            }
            else{
                Poly q = SimplifyToCoeff(p);
                AddHeap(h, q);
            }
        }
        else if(LineIsCommand(l)){
            ReadCommand(l, numberOfLine, h);
        }
        else if(!LineIsCommentOrEmpty(l)){
            PolyIsWrong(numberOfLine);
        }
        free(l.letters);
        l = ReadLine();
        //if(l.letters != NULL)LineUp(&l);
        numberOfLine++;
    }
    CleanHeap(h);
    free(h -> heap);
    free(h);
}
