/** @file
  Interfejs odopowiedzialny za odpowienie wczytywania inputu oraz parsowanie wielomianow.

  @authors Julian Kozłowski <jk417694@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLY_INPUTPOLY_H
#define POLY_INPUTPOLY_H
#include <stdio.h>

/**
 * Struktura przetrzymujaca linie.
 */
typedef struct{
    /** dlugosc linii */
    size_t length;
    /** tablica charow, czyli nasz string */
    char *letters;
}line;

/**
  Funkcja wczytująca wszystkie polecenia oraz wielomiany oraz wykonująca owe polecenia.
*/
void ReadFile();
#endif //POLY_INPUTPOLY_H
