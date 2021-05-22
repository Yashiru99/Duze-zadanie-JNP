#ifndef POLY_HEAP_H
#define POLY_HEAP_H
#include "poly.h"
typedef struct{
    Poly *heap;
    size_t size;
    size_t headIndex;
}heap;

void AddHeap(heap *head, Poly p);
Poly PopHeap(heap *head);
void IniHeap(heap **head);
bool HeapIsEmpty(heap *head);
bool HeapHasAtleastTwo(heap *head);
void CleanHeap(heap *head);
#endif //POLY_HEAP_H
