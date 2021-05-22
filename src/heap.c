#include "poly.h"
#include <stdlib.h>
#include "heap.h"

static void ResizePlus(heap **head){
    size_t doubledSize = (*head) -> size * 2;
    (*head) -> size = (*head) -> size == 0 ? 2 : doubledSize;
    (*head) -> heap  = realloc((*head) -> heap, sizeof(Poly) * (*head) -> size);
}

static void ResizeMinus(heap **head){
    (*head) -> size /= 2;
    (*head) = realloc(*head, sizeof(Poly) * (*head) -> size);
}

void IniHeap(heap **head){
    if((*head) == NULL){
        (*head) = malloc(sizeof(heap));
        (*head) -> heap = NULL;
        (*head) -> headIndex = 0;
        (*head) -> size = 0;
    }
}
// headIndex wskazuje na pierwsze wolne miejsce
void AddHeap(heap *head, Poly p){
    if(head -> headIndex >= head -> size || head -> heap == NULL){
        ResizePlus(&head);
    }
    head -> heap[head -> headIndex] = p;
    head -> headIndex += 1;
}

Poly PopHeap(heap *head){
    if(head -> headIndex < (1/4 * head -> size)){
        ResizeMinus(&head);
    }
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