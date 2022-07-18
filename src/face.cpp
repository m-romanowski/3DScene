#include "face.h"

Face::Face()
{
    A = B = C = 0;
}

Face::Face(int A, int B, int C)
{
    this->A = A;
    this->B = B;
    this->C = C;
}
