/** @file
  Interfejs struktury stosu, zaimplementowanej tablicowo.

  @author Julian Kozłowski <jk417694@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLY_HEAP_H
#define POLY_HEAP_H
#include "poly.h"


/**
 * Funkcja dodaje wielomian na wierzchołek stosu.
 * @param[in] head : wkaźnik na stos
 * @param[in] p: wielomian
 */
void AddHeap(heap *head, Poly p);

/**
 * Funkcja zdejmuje wielmian ze szczytu stosu oraz go zwraca.
 * @param[in] head : wkaźnik na stos
 * @return wielomian
 */
Poly PopHeap(heap *head);

/**
 * Funkcja inicjalizuje stos.
 * @param[in] head : wkaźnik na wskaźnik na stos
 */
void IniHeap(heap **head);

/**
 * Funcja określająca czy stos jest pusty.
 * @param[in] head : wkaźnik na wskaźnik na stos
 * @return bool
 */
bool HeapIsEmpty(heap *head);

/**
 * Funcja określająca czy na stosie jest więcej niz jeden wielomian.
 * @param[in] head : wkaźnik na wskaźnik na stos
 * @return bool
 */
bool HeapHasAtleastTwo(heap *head);

/**
 * Funcja określająca czy na stosie jest więcej niz k wielomianow.
 * @param[in] head : wkaźnik na wskaźnik na stos
 * @param[in] k : ilosc wielomianow
 * @return czy na stosie jest conajmniej k wielomianow?
 */
bool HeapHasAtleastKElements(heap *head, size_t k);

/**
 * Funkcja czyszcząca zawartosc stosu.
 * @param[in] head : wkaźnik na wskaźnik na stos
 */
void CleanHeap(heap *head);

#endif //POLY_HEAP_H
