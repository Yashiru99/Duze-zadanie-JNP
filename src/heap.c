/** @file
  Plik realizujący funkcje stosowe.

  @author Julian Kozłowski <jk417694@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "poly.h"
#include <stdlib.h>
#include "heap.h"

/** Makro do sprawdzenia czy alokacja się powiodła*/
#define CHECK_PTR(p)  \
  do {                \
    if (p == NULL) {  \
      exit(1);        \
    }                 \
  } while (0)


/**
 * Realokowanie stosu w celu zwiększenia pamięci.
 * @param[in] head : wskaźnik na wskaźnik na stos
 */
static void ResizePlus(heap **head){
    size_t doubledSize = (*head) -> size * 2;
    (*head) -> size = (*head) -> size == 0 ? 2 : doubledSize;
    (*head) -> heap  = realloc((*head) -> heap, sizeof(Poly) * (*head) -> size);
    CHECK_PTR((*head) -> heap);
}


void IniHeap(heap **head){
    if((*head) == NULL){
        (*head) = malloc(sizeof(heap));
        CHECK_PTR(*head);
        (*head) -> heap = NULL;
        (*head) -> headIndex = 0;
        (*head) -> size = 0;
    }
}

void AddHeap(heap *head, Poly p){
    if(head -> headIndex >= head -> size || head -> heap == NULL){
        ResizePlus(&head);
    }
    head -> heap[head -> headIndex] = p;
    head -> headIndex += 1;
}

Poly PopHeap(heap *head){
    Poly result = head -> heap[head -> headIndex - 1];
    head -> headIndex -= 1;
    return result;
}

void CleanHeap(heap *head){
    while(!HeapIsEmpty(head)){
        Poly p = PopHeap(head);
        PolyDestroy(&p);
    }
}

bool HeapIsEmpty(heap *head){
    return head -> headIndex == 0;
}

bool HeapHasAtleastTwo(heap *head){
    return head -> headIndex > 1;
}

bool HeapHasAtleastKElements(heap *head, size_t k){
    return head -> headIndex > k - 1;
}