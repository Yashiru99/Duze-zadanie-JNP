/** @file
  Plik dopowiedzialny za odpowienie wczytywania inputu oraz parsowanie wielomianow.

  @authors Julian Kozłowski <jk417694@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

/** Makro zdefiniowane w celu uzyskania dostępu do funkcji getline() */
#define _GNU_SOURCE

#include "poly.h"
#include "commands.h"
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

/** Makro do sprawdzenia czy alokacja się powiodła*/
#define CHECK_PTR(p)  \
  do {                \
    if (p == NULL) {  \
      exit(1);        \
    }                 \
  } while (0)

/**
 * Sprawdzenie czy linia jest potencjalnym wielomianem.
 * @param[in] actualLine : linia
 * @return czy linia moze być wielomianem?
 */
static bool LineIsPoly(line actualLine){
    assert(actualLine.letters != NULL);
    return isdigit(actualLine.letters[0]) || actualLine.letters[0] == '(' || actualLine.letters[0] == '-';
}

/**
 * Sprawdzenie czy linia jest potencjalną komendą.
 * @param[in] actualLine : linia
 * @return czy linia moze być komendą?
 */
static bool LineIsCommand(line actualLine){
    assert(actualLine.letters != NULL && actualLine.length != 0);
    return isalpha(actualLine.letters[0]);
}

/**
 * Sprawdzenie czy linia jest pusta, albo jest komentarzem.
 * @param[in] actualLine : linia
 * @return czy linia jest pusta lub jest komentarzem?
 */
static bool LineIsCommentOrEmpty(line actualLine){
    assert(actualLine.letters != NULL && actualLine.length != 0);
    return actualLine.letters[0] == '#' || actualLine.letters[0] == '\n' || actualLine.letters[0] == EOF;
}

/**
 * Sprawdzenie czy litera jest poprawna.
 * @param[in] a : litera do sprawdzenia
 * @return czy char moze wystąpić w komendzie lub wielomianie?
 */
static bool IsAllowed(char a){
    return isdigit(a) || a == ',' || a == '(' || a == ')' || a == '+' || a == '\n' || a == '-' || a == EOF;
}

static bool CheckMono(line actualLine, size_t start, size_t end);

/**
 * Sprawdza czy wykladnik jest w poprawnym zakresie.
 * @param[in] k : wykladnik
 * @return czy wykladnik jest w zakresie [0, INT_MAX]?
 */
static bool CheckExp(long k){
    return k >= 0 && k <= INT_MAX;
}

/**
 * Sprawdza czy wielomian jest poprawny.
 * @param[in] actualLine : linia
 * @param[in] start : start
 * @param[in] end : koniec
 * @return czy wielomian w linii zaczynający się od start i kończącej na koniec jest poprawny?
 */
static bool CheckPoly(line actualLine, size_t start, size_t end){
    if(!IsAllowed(actualLine.letters[0]))return false;
    bool result = true;
    if(isdigit(actualLine.letters[start]) || actualLine.letters[start] == '-'){
        char *e = NULL;
        strtol(actualLine.letters + start, &e, 10);
        if(errno == ERANGE)return false;
        if(actualLine.letters[start] == '-')start++;
        while(start <= end && isdigit(actualLine.letters[start])){
            start++;
        }
        return start >= end;
    }

    size_t heap = 0;
    if(actualLine.letters[start] == '(' && (actualLine.letters[start + 1] == '(' || actualLine.letters[start + 1] == '-' || isdigit(actualLine.letters[start + 1]))){
        heap++;
        for(size_t i = start + 1; i <= end; ++i){
            result &= IsAllowed(actualLine.letters[i]);
            if(actualLine.letters[i] == '(')heap++;
            if(actualLine.letters[i] == ')')heap--;
            if(heap == 0){
                result &= (actualLine.letters[i+1] == '+' || actualLine.letters[i+1] == '\n' || actualLine.letters[i+1] == EOF || actualLine.letters[i+1] == ',');
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

/**
 * Sprawdza czy monomian jest poprawny.
 * @param[in] actualLine : linia
 * @param[in] start : start
 * @param[in] end : koniec
 * @return czy monomian w linii zaczynający się od start i kończącej na end jest poprawny?
 */
static bool CheckMono(line actualLine, size_t start, size_t end){
    bool result = true;
    result &= isdigit(actualLine.letters[end]) != 0 ? 1 : 0;
    while(start < end && isdigit(actualLine.letters[end])){
        end--;
    }
    long e = strtol(actualLine.letters, NULL, 10);
    if(!CheckExp(e))return false;
    result &= actualLine.letters[end] == ',';
    result &= CheckPoly(actualLine, start, end - 1);
    return result;
}

/**
 * Wypisuje linię w ktorej zauwazylismy niepoprawny wielomian.
 * @param[in] w : numer linii.
 */
static void PolyIsWrong(size_t w){
    fprintf(stderr, "ERROR %ld WRONG POLY\n", w);
}

/**
 * Funkcja wczytująca linie, alokuje go w podany bufor i aktualizuje długość.
 * @param[in] buffor : wskaźnik na wskaźnik na bufor
 * @param[in] len : wskaźnik na długość.
 * @return Wczytana linia.
 */
static line ReadLine(char **buffor, size_t *len){
    int nRead = 0;
    nRead = getline(buffor, len, stdin);
    *len = nRead + 1;
    CHECK_PTR(*buffor);
    return nRead == -1 ? (line) {.length = 0, .letters = NULL} : (line) {.length = nRead, .letters = *buffor};
}

/**
 * Określa ile monomianow zostanie wczytanych.
 * @param[in] polyToRead : linia do wczytania
 * @param[in] start : start
 * @param[in] end : koniec
 * @return ile monomianow musimy wczytac.
 */
static size_t numberOfPolys(line polyToRead, size_t start, size_t end){
    size_t numberOfPolys = 1;
    size_t heap = 0;
    for(size_t i = start; i < end; i++){
        if(polyToRead.letters[i] == '(')heap++;
        if(polyToRead.letters[i] == ')')heap--;
        if(polyToRead.letters[i] == '+' && !heap)numberOfPolys++;
    }
    return numberOfPolys;
}

static Poly readMonos(line polyToRead, bool *isValid, size_t start, size_t end);
/**
 * Znajduje indeks na "," ktory rozdziela wielomian w monomianie od wykladnika
 * @param[in] l : tablica liter do wczytania
 * @param[in] start : start
 * @param[in] end : koniec
 * @return czy monomian w linii zaczynający się od start i kończącej na end jest poprawny?
 */
static size_t findIndex(char *l, size_t start, size_t end){
    size_t heap = 0;
    for(size_t j = start; j < end; j++){
        if(l[j] == '(')heap++;
        if(l[j] == ')')heap--;
        if(l[j] == ',' && heap == 1)return j;
    }
    return 0;
}

/**
 * Funkcja wczytujaca pojedynczy monomian.
 * @param[in] monoToRead : linia z ktorej odczytujemy monomian
 * @param[in] start : start linii
 * @param[in] isValid : wskaźnik na zmienną ktora ulega zmianie przy wczytaniu zlego wielomianu
 * @param[in] end : koniec linii
 * @return odczytany monomian
 */
static Mono readSingleMono(line monoToRead, bool *isValid, size_t start, size_t end){
    size_t startIndex = findIndex(monoToRead.letters, start, end);
    Mono result;
    char *endC;
    long e = strtol(monoToRead.letters + startIndex + 1, &endC, 10);
    if(*endC != ')' || !CheckExp(e))*isValid = false;
    result.exp = e;
    result.p = readMonos(monoToRead, isValid, start + 1, startIndex - 1);
    return result;
}

/**
 * Określa czy potencjalny czytany wielomian będzie wielomianem stałym.
 * @param[in] l : tablica liter do wczytania
 * @param[in] start : start linii
 * @return czy wielomian będzie stały?
 */
static bool PolyIsConst(line l, size_t start){
    return l.letters[start] != '(';
}

/**
 * Odczytuje wielomian i określa czy został poprawnie wczytany
 * @param[in] polyToRead : tablica liter do wczytania
 * @param[in] isValid : wskaźnik na zmienną ktora ulega zmianie przy wczytaniu zlego wielomianu
 * @param[in] start : start linii
 * @param[in] end : koniec linii
 * @return wielomian
 */
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
    CHECK_PTR(monosToAdd);

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
/**
  Funkcja wczytująca wszystkie polecenia oraz wielomiany oraz wykonująca owe polecenia.
*/
void ReadFile(){
    char *buffor = NULL;
    size_t len = 0;
    line l = ReadLine(&buffor, &len);
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
                AddHeap(h, p);
            }
        }
        else if(LineIsCommand(l)){
            ReadAndDoCommand(l, numberOfLine, h);
        }
        else if(!LineIsCommentOrEmpty(l)){
            PolyIsWrong(numberOfLine);
        }
        l = ReadLine(&buffor, &len);
        numberOfLine++;
    }
    CleanHeap(h);
    free(h -> heap);
    free(h);
    free(buffor);
}
