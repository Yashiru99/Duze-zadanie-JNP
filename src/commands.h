/** @file
  Interfejs struktury stosu, zaimplementowanej tablicowo.

  @author Julian Kozłowski <jk417694@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLY_COMMANDS_H
#define POLY_COMMANDS_H
#include "inputPoly.h"
#include "heap.h"
void ReadCommand(line l, size_t w, heap *h);
void PolyPrint(Poly p);
#endif //POLY_COMMANDS_H
