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
// Struktura do trzyamania linii, ma długość oraz litery

static bool LineIsPoly(line actualLine){
    assert(actualLine.letters != NULL);
    return isdigit(actualLine.letters[0]) || actualLine.letters[0] == '(' || actualLine.letters[0] == '-';
}
static bool LineIsCommand(line actualLine){
    assert(actualLine.letters != NULL && actualLine.length != 0);
    return isalpha(actualLine.letters[0]);
}


static bool IsAllowed(char a){
    return isdigit(a) || a == '\0' || a == ',' || a == '(' || a == ')' || a == '+' || a == '\0' || a == '\n' || a == '-' || a == EOF;
}
static bool CheckMono(line actualLine, size_t start, size_t end);
static bool CheckPoly(line actualLine, size_t start, size_t end){
    bool result = true;
    // jeżeli jest stały to tak go wczytujemy
    if(isdigit(actualLine.letters[start]) || actualLine.letters[start] == '-'){
        if(actualLine.letters[start] == '-')start++;
        while(start <= end && isdigit(actualLine.letters[start])){
            start++;
        }
        return start >= end;
    }
    size_t heap = 0;
    // wpp wczytujemy go jako pojedyncze monomiany
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
// zwracam zaalokowane miejsce w pamięci(nasza linia)
static line ReadLine(){
    char *buffor = NULL;
    size_t length = 0;
    int nRead = 0;
    nRead = getline(&buffor, &length, stdin);
    if(nRead == -1) free(buffor);
    return nRead == -1 ? (line) {.length = 0, .letters = NULL} : (line) {.length = nRead, .letters = buffor};
}
//zwraca zaalokowany buffor
static size_t numberOfPolys(line polyToRead){
    size_t numberOfPolys = 1;
    size_t heap = 0;
    for(int i = 0; i < polyToRead.length; i++){
        if(polyToRead.letters[i] == '(')heap++;
        if(polyToRead.letters[i] == ')')heap--;
        if(polyToRead.letters[i] == '+' && !heap)numberOfPolys++;
    }
    return numberOfPolys;
}
// tworzymy podlinię linii, zaczynamy od sIndex, końcymy na eIndex
// zwraca zaalokowaną podlinię
static line MakeSubLine(line l, size_t startingIndex, size_t endingIndex){
    size_t length = endingIndex - startingIndex + 1;
    line result;
    result.length = length;
    result.letters = calloc(length, sizeof(char));
    memcpy(result.letters, (l.letters + startingIndex), (length - 1) * sizeof(char));
    return result;
}
// nie alokujemy zawartości tablic, tylko całą tablicę linii(chyba zaleznie od strcpy)
// rozdzielamy sobie na osobne Monos
static lines MakeSinglePolys(line polyToRead){
    size_t numPolys = numberOfPolys(polyToRead);
    lines result;
    line *arrayOfPolys = malloc(numPolys * sizeof(line));
    size_t numberOfPolysLeft = numPolys;
    size_t length = 1;
    size_t startingIndex = 0;
    size_t index = 0;
    size_t numberOfBrackets = 1;
    for(size_t j = 1; j < polyToRead.length; j++){
        if(!numberOfBrackets && (polyToRead.letters[j] == '+' || polyToRead.letters[j] == '\n' || polyToRead.letters[j] == EOF)){
            arrayOfPolys[index] = MakeSubLine(polyToRead, startingIndex, length);
            index++;
            startingIndex = j + 1;
            numberOfPolysLeft--;
        }
        if(polyToRead.letters[j] == '(')numberOfBrackets++;
        if(polyToRead.letters[j] == ')')numberOfBrackets--;
        length++;
    }
    result.length = numPolys;
    if(numberOfPolysLeft){
        arrayOfPolys[index] = MakeSubLine(polyToRead, startingIndex, length);
    }
    result.Polys = arrayOfPolys;
    return result;
}
static Poly readMonos(line polyToRead, bool *isValid);

size_t findIndex(char *l, size_t length){
    size_t heap = 0;
    for(size_t j = 0; j < length; j++){
        if(l[j] == '(')heap++;
        if(l[j] == ')')heap--;
        if(l[j] == ',' && heap == 1)return j+1;
    }
    return 0;
}
static Mono readSingleMono(line monoToRead, bool *isValid){
    size_t monoLength = monoToRead.length;
    size_t startIndex = findIndex(monoToRead.letters, monoLength);
    char *end;
    Mono result;
    long e = strtol(monoToRead.letters + startIndex, &end, 10);
    if(*end != ')' || !CheckExp(e))*isValid = false;
    result.exp = e;
    // Tutaj wczytuje wykładnik, ma być z przedziału od 0 do 2147483647
    line l = MakeSubLine(monoToRead, 1, startIndex - 1);
    result.p = readMonos(l, isValid);
    free(l.letters);
    //return (Mono) {.exp = (poly_exp_t) strtol(monoToRead.letters + startIndex, &end, 10), .p = readMonos(MakeSubLine(monoToRead, 1, startIndex - 1))};
    return result;
}
// osobny Case dla wielomianów stałych
bool PolyIsConst(line l){
    return l.letters[0] != '(';
}

static Poly readMonos(line polyToRead, bool *isValid){
    if(PolyIsConst(polyToRead)){
        char *end;
        long e = strtol(polyToRead.letters, &end, 10);
        if((*end != '\n' && *end != EOF && *end != '\0') || errno == ERANGE)*isValid = false;
        // printf("%d", *isValid);
        return PolyFromCoeff(e);
    }
    lines arrayOfPolys = MakeSinglePolys(polyToRead);
    Mono *arrayOfMonos = malloc(sizeof(Mono) * arrayOfPolys.length);
    for(size_t i = 0; i < arrayOfPolys.length; i++){
        arrayOfMonos[i] = readSingleMono(arrayOfPolys.Polys[i], isValid);
    }
    Poly result = PolyAddMonos(arrayOfPolys.length, arrayOfMonos);
    free(arrayOfMonos);
    for(size_t i = 0; i < arrayOfPolys.length; i++){
        free(arrayOfPolys.Polys[i].letters);
    }
    free(arrayOfPolys.Polys);
    return result;
}

void ReadFile(){
    line l = ReadLine();
    for(int i = 0; i < l.length; i++){
        l.letters[i] = toupper(l.letters[i]);
    }
    printf("%s", l.letters);
    size_t numberOfLine = 1;
    Poly p;
    heap *h = NULL;
    bool isValid;
    IniHeap(&h);
    while(l.letters != NULL){
        if(LineIsPoly(l) && CheckPoly(l, 0, l.length - 2)){
            isValid = true;
            p = readMonos(l, &isValid);
            if(!isValid){
                PolyDestroy(&p);
                PolyIsWrong(numberOfLine);
            }
            else if(CanBeSimflified(p)){
                Poly q = PolyFromCoeff(SimplifyToCoeff(p));
                PolyDestroy(&p);
                AddHeap(h, q);
            }
            else{
                AddHeap(h, p);
            }
        }
        else if(LineIsCommand(l)){
            ReadCommand(l, numberOfLine, h);
        }
        else{
            PolyIsWrong(numberOfLine);
        }
        free(l.letters);
        l = ReadLine();
        for(int i = 0; i < l.length; i++){
            l.letters[i] = toupper(l.letters[i]);
        }
        numberOfLine++;
    }
    CleanHeap(h);
    free(h -> heap);
    free(h);
}
