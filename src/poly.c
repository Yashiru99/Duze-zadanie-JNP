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
#define C PolyFromCoeff


Mono *MergeTwoMonoArrays(size_t sizeP, size_t sizeQ, const Mono *p, const Mono *q) {
    // tworzymy głęboką kopię skopiowanej tablicy
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
    Mono *mergedArray; // przygotowanie głębokiej kopii


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
    if (PolyIsCoeff(p)) {
        return;
    }
    else{
        for(size_t j = 0; j < p -> size; ++j){
            PolyDestroy(&p -> arr[j].p);
        }
    }
    free(p -> arr);
}

int MonoComparator(const void *a, const void *b) {
    Mono *_a = (Mono *) a;
    Mono *_b = (Mono *) b;
    if (_a->exp < _b->exp) return -1;
    else if (_a->exp == _b->exp) return 0;
    else return 1;
}

void SortMonoArray(Mono *arr, size_t count){
    qsort(arr, count, sizeof(Mono), MonoComparator);
}
bool CanAddTwoMonos(const Mono a, const Mono b) {
    return a.exp == b.exp;
}


void shiftArray(size_t *size, Mono *array, size_t k){
    for(size_t i = k; i < *size - 1; i++){
        array[i] = array[i+1];
    }
    (*size)--;
}

Mono *simplify(Mono *arr, const size_t count, size_t *realCount) {



    *realCount = count;
    // finalnie nic nie zmienamy w arr
    // upraszczanie przez dodawanie
    for(size_t j = 0; j < *realCount - 1; ++j){
        if(CanAddTwoMonos(arr[j], arr[j+1])){
            Mono toBeDestroyed = arr[j];
            arr[j].p = PolyAdd(&arr[j].p, &arr[j + 1].p);
            MonoDestroy(&toBeDestroyed);
            MonoDestroy(&arr[j + 1]);
            shiftArray(realCount, arr, j + 1);
            j--;
        }
    }
    size_t k = 0;

    while(k < *realCount){
        if(PolyIsZero(&arr[k].p)){
            MonoDestroy(&arr[k]);
            shiftArray(realCount, arr, k);
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
//robimy płytka kopie naszego monos i go zwracamy
Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if(count == 0) return (Poly) {.coeff = 0, .arr = NULL};

    Mono *CopyOfMonos = calloc(count, sizeof(Mono));
    CHECK_PTR(CopyOfMonos);

    memcpy(CopyOfMonos, monos, count * sizeof(Mono));
    SortMonoArray(CopyOfMonos, count);
    size_t realCount = 0;
    simplify(CopyOfMonos, count, &realCount);

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
    Mono *new_array = malloc(sizeof(Mono) * p->size);
    for(size_t i = 0; i < p -> size; ++i) {
        Poly new_poly = PolyClone(&p->arr[i].p);
        new_array[i] = MonoFromPoly(&new_poly, p->arr[i].exp);
    }

    return (Poly) {.size = p -> size, .arr = new_array};
}

bool MonoIsEq(const Mono *p, const Mono *q) {
    return (p->exp == q->exp) && PolyIsEq(&p->p, &q->p);
}

bool WholePolyIsCoeff(const Poly *p) {
    if (PolyIsCoeff(p))return true;
    if (p->size == 1 && p->arr[0].exp == 0 && WholePolyIsCoeff(&p->arr[0].p)) {
        return true;
    }
    return false;
}

Poly returnRealCoeff(const Poly *p) {
    if (!PolyIsCoeff(p))return returnRealCoeff(&p->arr[0].p);
    Poly result;
    result.arr = NULL;
    result.coeff = p->coeff;
    return result;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (WholePolyIsCoeff(p) && WholePolyIsCoeff(q)) {
        return returnRealCoeff(p).coeff == returnRealCoeff(q).coeff;
    }
    if (WholePolyIsCoeff(p) || WholePolyIsCoeff(q)) {
        return false;
    }
    if (!WholePolyIsCoeff(p) && !WholePolyIsCoeff(q) && p->size == q->size) {
        for (size_t i = 0; i < p->size; i++) {
            if (!MonoIsEq(&p->arr[i], &q->arr[i])) {
                return false;
            }
        }
    }
    return true;
}


Poly PolyNeg(const Poly *p) {
    Poly result = PolyClone(p);
    MultiplyByScalar(&result, -1);
    return result;
}

poly_exp_t max(poly_exp_t a, poly_exp_t b) {
    return a > b ? a : b;
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly negative = PolyNeg(q);
    Poly result = PolyAdd(p, &negative);
    PolyDestroy(&negative);
    return result;
}

// Wielomiany mam co do zasady posortowane, gdzie najwiekszy jest na koncu
poly_exp_t PolyDeg(const Poly *p) {
    poly_exp_t result = -1;
    if (PolyIsZero(p)) return result;
    if (PolyIsCoeff(p))return 0;
    if (p->size == 1) {
        result = p->arr[0].exp;
        return result;
    } else {
        for (size_t i = 0; i < p->size; i++) {
            result = max(result, p->arr[i].exp * PolyDeg(&p->arr[i].p));
        }
    }
    return result;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    poly_exp_t result = -1;
    if (PolyIsZero(p))return -1;
    if (WholePolyIsCoeff(p))return 0;
    if (var_idx == 0) {
        return p->arr[p->size - 1].exp;
    } else {
        for (size_t i = 0; i < p->size; ++i) {
            result = max(PolyDegBy(&p->arr[i].p, var_idx - 1), result);
        }
    }
    return result;
}


void MultiplyByScalar(Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p)) {
        p->coeff *= x;
    } else {
        for (size_t i = 0; i < p->size; ++i) {
            MultiplyByScalar(&p->arr[i].p, x);
        }
    }
}

Poly PolyMul(const Poly *p, const Poly *q) {
    bool p_is_coeff = PolyIsCoeff(p);
    bool q_is_coeff = PolyIsCoeff(q);

    if (p_is_coeff && q_is_coeff) {
        return PolyFromCoeff(p->coeff * q->coeff);
    }

    if (p_is_coeff) {
        return PolyMul(q, p);
    }

    if (q_is_coeff) {
        // jeśli mnożymy wielomian przez stałą, to mnożymy współczynniki przy wszystkich monomianach

        Mono* new_array = (Mono*) calloc(p->size, sizeof(Mono));

        for (size_t i = 0; i < p->size; i++) {
            Poly tmp_poly = PolyMul(&(p->arr[i].p), q);

            if (PolyIsZero(&tmp_poly)) {
                new_array[i] = MonoFromPoly(&tmp_poly, 0);
            } else {
                new_array[i] = MonoFromPoly(&tmp_poly, p->arr[i].exp);
            }
        }

        Poly res = PolyAddMonos(p->size, new_array);
        free(new_array);
        return res;
    }

    // ostateczny przypadek: mnożymy przez siebie dwa wielomiany stopnia > 0

    Poly result_poly = PolyZero();

    for (size_t i = 0; i < p->size; i++) {
        Mono *new_array = (Mono*) malloc(q->size * sizeof(Mono));

        for (size_t j = 0; j < q->size; j++) {
            Poly new_poly = PolyMul(&(p->arr[i].p), &(q->arr[j].p));
            poly_exp_t new_exp = MonoGetExp(&(p->arr[i])) + MonoGetExp(&(q->arr[j]));

            if (PolyIsZero(&new_poly)) {
                new_exp = 0;
            }

            new_array[j] = MonoFromPoly(&new_poly, new_exp);
        }

        Poly add_to_result_poly = PolyAddMonos(q->size, new_array);
        Poly tmp_res_poly = PolyAdd(&result_poly, &add_to_result_poly);

        free(new_array);
        PolyDestroy(&result_poly);
        PolyDestroy(&add_to_result_poly);

        result_poly = tmp_res_poly;
    }

    return result_poly;
}

poly_coeff_t power(poly_coeff_t coefficient, poly_exp_t exp) {
    if(exp == 0) return 1;
    poly_coeff_t temporary = power(coefficient, exp / 2);
    if(exp % 2 == 0){
        return temporary * temporary;
    }
    else{
        return temporary * temporary * coefficient;
    }
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    Poly result = C(0);
    Poly *PolyToAdd = malloc(sizeof(Poly) * p -> size);
    if (PolyIsCoeff(p)){
        free(PolyToAdd);
        return *p;
    }
    for (size_t i = 0; i < p->size; i++) {
        if (p->arr[i].exp != 0) {
            MultiplyByScalar(&p->arr[i].p, power(x, p->arr[i].exp));
        }
        result = PolyAdd(&result, &p->arr[i].p);
        PolyToAdd[i] = result;
    }
    for(size_t i = 0; i < p -> size - 1; ++i) PolyDestroy(&PolyToAdd[i]);
    free(PolyToAdd);
    return result;

}

