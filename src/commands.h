/** @file
  Interfejs obsługujący wypisywanie wielomianow, wczytywanie i wykonywanie komend.

  @author Julian Kozłowski <jk417694@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLY_COMMANDS_H
#define POLY_COMMANDS_H
#include "inputPoly.h"
#include "heap.h"
/**
 * Funkcja wczytująca i wykonująca komendę.
 * @param[in] l : linia
 * @param[in] w : numer linii
 * @param[in] h : wskaźnik na stos
 */
void ReadAndDoCommand(line l, size_t w, heap *h);

/**
 * Funkcja wypisująca wielomian.
 * @param[in] p : wielomian
 */
void PolyPrint(Poly p);

#endif //POLY_COMMANDS_H
