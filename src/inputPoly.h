/** @file
  Interfejs odopowiedzialny za odpowienie wczytywania inputu oraz parsowanie wielomianow.

  @authors Julian Kozłowski <jk417694@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLY_INPUTPOLY_H
#define POLY_INPUTPOLY_H
#include <stdio.h>

typedef struct{
    size_t length;
    char *letters;
}line;

/**
  Funkcja wczytująca wszystkie polecenia oraz wielomiany oraz wykonująca owe polecenia.
*/
void ReadFile();
#endif //POLY_INPUTPOLY_H
