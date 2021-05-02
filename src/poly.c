/** @file
  Interfejs pomocniczych funkcji wykorzystywanych przy implementacji funkcji na wielomianach rzadkich.

  @authors Julian Kozłowski <jk417694@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/


#include "poly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_PTR(p)  \
  do {                \
    if (p == NULL) {  \
      exit(1);        \
    }                 \
  } while (0)

/**
 * Tworzy nową tablicę jednomianow poprzez sklejenie tablicy p z tablicą q.
 * @param[in] sizeP : wielkość tablicy monomianow p
 * @param[in] sizeQ : wielkość tablicy monomianow q
 * @param[in] p : tablice monomianow
 * @param[in] q : tablica monomianow
 * @return tablica monomianow.
 */
static Mono *MergeTwoMonoArrays(size_t sizeP, size_t sizeQ, const Mono *p, const Mono *q) {
    Mono *mergedArray = malloc(sizeof(Mono) * (sizeP + sizeQ));
    CHECK_PTR(mergedArray);

    for (size_t i = 0; i < sizeP; i++) {
        mergedArray[i] = MonoClone(&p[i]);
    }
    for (size_t j = sizeP; j < sizeQ + sizeP; j++) {
        mergedArray[j] = MonoClone(&q[j - sizeP]);
    }
    return mergedArray;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    bool qIsCoeff = PolyIsCoeff(q);
    bool pIsCoeff = PolyIsCoeff(p);


    Poly result;
    Mono coeffPoly;
    Mono *mergedArray;


    if (qIsCoeff && pIsCoeff) {
        return PolyFromCoeff(p -> coeff + q -> coeff);
    } else if (pIsCoeff) {
        coeffPoly = MonoFromPoly(p, 0);
        mergedArray = MergeTwoMonoArrays(1, q->size, &coeffPoly, q->arr);
        result = PolyAddMonos(q->size + 1, mergedArray);
        free(mergedArray);
    } else if (qIsCoeff) {
        coeffPoly = MonoFromPoly(q, 0);
        mergedArray = MergeTwoMonoArrays(p->size, 1, p->arr, &coeffPoly);
        result = PolyAddMonos(p->size + 1, mergedArray);
        free(mergedArray);
    } else {
        mergedArray = MergeTwoMonoArrays(p->size, q->size, p->arr, q->arr);
        result = PolyAddMonos(p->size + q->size, mergedArray);
        free(mergedArray);
    }
    return result;
}


void PolyDestroy(Poly *p) {
    if (!PolyIsCoeff(p)) {
        for(size_t j = 0; j < p -> size; ++j){
            PolyDestroy(&p -> arr[j].p);
        }
        free(p -> arr);
    }
}

static int MonoComparator(const void *a, const void *b) {
    Mono *_a = (Mono *) a;
    Mono *_b = (Mono *) b;
    if (_a->exp < _b->exp) return -1;
    else if (_a->exp == _b->exp) return 0;
    else return 1;
}
/**
 * Sortuje tablicę jednomianow.
 * @param[in] arr : tablica jednomianow
 * @param[in] count : dlugosc tablicy arr
 * @return posortowana tablica monomianow
 */
static void SortMonoArray(Mono *arr, size_t count){
    qsort(arr, count, sizeof(Mono), MonoComparator);
}

/**
 * Sprawdza czy 2 monomiany mozemy do siebie dodac.
 * @param[in] a : monomian
 * @param[in] b : monomian
 * @return wartość boolean.
 */
static bool CanAddTwoMonos(const Mono a, const Mono b) {
    return a.exp == b.exp;
}

/**
 * Modyfikuje tablicę wielomianow w miejscu oraz jej wielkość, przesuwając wszystkie elementy począwszy od indexu w lewo oraz odpowiednio zmniejszając wielkość o jeden.
 * @param[in] size : wskaźnik na wielkość
 * @param[in] array : tablica monomianow
 * @param[in] index : indeks
 */
static void ShiftArray(size_t *size, Mono *array, size_t index){
    for(size_t i = index; i < *size - 1; i++){
        array[i] = array[i+1];
    }
    (*size)--;
}

/**
 * Funkcja upraszczająca tablicę monomianow w miejscu.
 * @param[in] arr : tablica monomianow
 * @param[in] count : wielkosc tablicy monomianow
 * @param[in] realCount : wskaźnik ktory posluzy do zapisu nowej wielkosci
 * return tablica monomianow
 */
static Mono *Simplify(Mono *arr, const size_t count, size_t *realCount) {

    *realCount = count;
    for(size_t j = 1; j < *realCount; ++j){
        if(CanAddTwoMonos(arr[j - 1], arr[j])){
            Mono toBeDestroyed = arr[j - 1];
            arr[j - 1].p = PolyAdd(&arr[j - 1].p, &arr[j].p);
            MonoDestroy(&toBeDestroyed);
            MonoDestroy(&arr[j]);
            ShiftArray(realCount, arr, j);
            j--;
        }
    }
    size_t k = 0;

    while(k < *realCount){
        if(PolyIsZero(&arr[k].p)){
            MonoDestroy(&arr[k]);
            ShiftArray(realCount, arr, k);
        }
        else{
            k++;
        }
    }
    if(*realCount == 0){
        return NULL;
    }
    return arr;
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if(count == 0) return (Poly) {.coeff = 0, .arr = NULL};

    Mono *CopyOfMonos = malloc(count * sizeof(Mono));
    CHECK_PTR(CopyOfMonos);

    memcpy(CopyOfMonos, monos, count * sizeof(Mono));
    SortMonoArray(CopyOfMonos, count);
    size_t realCount = 0;
    Simplify(CopyOfMonos, count, &realCount);

    if(realCount == 0) {
        free(CopyOfMonos);
        return (Poly) {.size = 0, .arr = NULL};
    }
    return (Poly) {.size = realCount, .arr = CopyOfMonos};
}

Poly PolyClone(const Poly *p) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p -> coeff);
    }
    Mono *newArray = malloc(sizeof(Mono) * p->size);
    CHECK_PTR(newArray);

    for(size_t i = 0; i < p -> size; ++i) {
        Poly newPoly = PolyClone(&p->arr[i].p);
        newArray[i] = MonoFromPoly(&newPoly, p->arr[i].exp);
    }

    return (Poly) {.size = p -> size, .arr = newArray};
}
/**
 * Sprawdzanie czy 2 monomianowy są tozsamosciowo sobie rowne
 * @param[in] p : monomian
 * @param[in] q : monomian
 * return boolean
 */
static bool MonoIsEq(const Mono *p, const Mono *q) {
    return (p->exp == q->exp) && PolyIsEq(&p->p, &q->p);
}
/**
 * Sprawdzanie czy wielomian jest tozsamościowy z wielomianem stałym
 * @param[in] p : wielomian
 * return boolean
 */
static bool WholePolyIsCoeff(const Poly *p) {
    if (PolyIsCoeff(p))return true;
    if (p->size == 1 && p->arr[0].exp == 0 && WholePolyIsCoeff(&p->arr[0].p)) {
        return true;
    }
    return false;
}
/**
 * Znajduje wielomian stały tozsamosciowy z wielomianem wejściowym
 * @param[in] p : wielomian
 * return wielomian
 */
static Poly returnRealCoeff(const Poly *p) {
    if (!PolyIsCoeff(p))return returnRealCoeff(&p->arr[0].p);
    return (Poly) {.arr = NULL, .coeff = p -> coeff};
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (WholePolyIsCoeff(p) && WholePolyIsCoeff(q)) {
        return returnRealCoeff(p).coeff == returnRealCoeff(q).coeff;
    }
    if (WholePolyIsCoeff(p) || WholePolyIsCoeff(q)) {
        return false;
    }
    if(p -> size != q -> size) return false;
    else{
        for (size_t i = 0; i < p->size; i++) {
            if (!MonoIsEq(&p->arr[i], &q->arr[i])) {
                return false;
            }
        }
    }
    return true;
}
/**
 * Mnozenie wielomianu przez skalar
 * @param[in] p : wielomian
 * @param[in] x : skalar
 */
static void MultiplyByScalar(Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p)) {
        p->coeff *= x;
    } else {
        for (size_t i = 0; i < p->size; ++i) {
            MultiplyByScalar(&p->arr[i].p, x);
        }
    }
}

Poly PolyNeg(const Poly *p) {
    Poly result = PolyClone(p);
    MultiplyByScalar(&result, -1);
    return result;
}
/**
 * Porownywanie 2 wykładnikow
 * @param[in] a : wykladnik
 * @param[in] b : wykladnik
 * return wykladnik
 */
static poly_exp_t Max(poly_exp_t a, poly_exp_t b) {
    return a > b ? a : b;
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly negative = PolyNeg(q);
    Poly result = PolyAdd(p, &negative);
    PolyDestroy(&negative);
    return result;
}

poly_exp_t PolyDeg(const Poly *p) {
    poly_exp_t result = -1;
    if (PolyIsZero(p)) return result;
    if (PolyIsCoeff(p)) return 0;
    if (p->size == 1) {
        result = p->arr[0].exp;
        return result;
    } else {
        for (size_t i = 0; i < p->size; i++) {
            result = Max(result, p->arr[i].exp * PolyDeg(&p->arr[i].p));
        }
    }
    return result;
}

poly_exp_t PolyDegBy(const Poly *p, size_t varIdX) {
    poly_exp_t result = -1;
    if (PolyIsZero(p))return -1;
    if (WholePolyIsCoeff(p))return 0;
    if (varIdX == 0) {
        return p->arr[p->size - 1].exp;
    } else {
        for (size_t i = 0; i < p->size; ++i) {
            result = Max(PolyDegBy(&p->arr[i].p, varIdX - 1), result);
        }
    }
    return result;
}

/**
 * Mnozenie wielomianu przez wielomian ktory jest tozsamościowo stały
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * return wielomian
 */
static Poly MultiplyPolyByScalar(const Poly *p, const Poly *q){
    Mono* newArray = (Mono*) malloc(p->size * sizeof(Mono));
    CHECK_PTR(newArray);

        for (size_t i = 0; i < p->size; i++) {
            Poly tempPoly = PolyMul(&(p->arr[i].p), q);

            if (PolyIsZero(&tempPoly)) {
                newArray[i] = MonoFromPoly(&tempPoly, 0);
            } else {
                newArray[i] = MonoFromPoly(&tempPoly, p->arr[i].exp);
            }
        }

    Poly res = PolyAddMonos(p->size, newArray);
    free(newArray);
    return res;
}
/**
 * Mnozenie wielomianu przez wielomian, z ktorych zaden nie jest tozsamościowo stały
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * return wielomian
 */
static Poly MultiplyPolyByPoly(const Poly *p, const Poly *q){
    Poly resultPoly = PolyZero();

    for (size_t i = 0; i < p->size; i++) {
        Mono *newArray = (Mono*) malloc(q->size * sizeof(Mono));
        CHECK_PTR(newArray);

        for (size_t j = 0; j < q->size; j++) {
            Poly newPoly = PolyMul(&(p->arr[i].p), &(q->arr[j].p));
            poly_exp_t newExp = MonoGetExp(&(p->arr[i])) + MonoGetExp(&(q->arr[j]));

            if (PolyIsZero(&newPoly)) {
                newExp = 0;
            }

            newArray[j] = MonoFromPoly(&newPoly, newExp);
        }

        Poly addToResult = PolyAddMonos(q->size, newArray);
        Poly finalPoly = PolyAdd(&resultPoly, &addToResult);

        free(newArray);
        PolyDestroy(&resultPoly);
        PolyDestroy(&addToResult);

        resultPoly = finalPoly;
    }
    return resultPoly;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    bool pIsCoeff = PolyIsCoeff(p);
    bool qIsCoeff = PolyIsCoeff(q);

    if (pIsCoeff && qIsCoeff) {
        return PolyFromCoeff(p->coeff * q->coeff);
    }

    if (pIsCoeff) {
        return PolyMul(q, p);
    }

    if (qIsCoeff) {
        return MultiplyPolyByScalar(p, q);
    }

    return MultiplyPolyByPoly(p, q);
}
/**
 * Funkcja potęgowa działająca w czasie O(log(exp)))
 * @param[in] coefficient : wspolczynnik
 * @param[in] exp : wykladnik
 * return wspolczynnik
 */
static poly_coeff_t Power(poly_coeff_t coefficient, poly_exp_t exp) {
    if(exp == 0) return 1;
    poly_coeff_t temporary = Power(coefficient, exp / 2);
    if(exp % 2 == 0){
        return temporary * temporary;
    }
    else{
        return temporary * temporary * coefficient;
    }
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    Poly result = PolyZero();
    if (PolyIsCoeff(p)){
        return *p;
    }
    Poly *polyToAdd = malloc(sizeof(Poly) * p -> size);
    CHECK_PTR(polyToAdd);
    for (size_t i = 0; i < p->size; i++) {
        if (p->arr[i].exp != 0) {
            MultiplyByScalar(&p->arr[i].p, Power(x, p->arr[i].exp));
        }
        result = PolyAdd(&result, &p->arr[i].p);
        polyToAdd[i] = result;
    }
    for(size_t i = 0; i < p -> size - 1; ++i) PolyDestroy(&polyToAdd[i]);
    free(polyToAdd);
    return result;

}

