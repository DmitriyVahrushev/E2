//
// Created by Dmitriy on 01.07.2019.
//

#ifndef E2_DIGITALSIGNATURE_H
#define E2_DIGITALSIGNATURE_H

#include "BigInteger.h"

void create_signature(BigInteger p, BigInteger q, BigInteger theta, BigInteger* signature, const uint8_t* hash, BigInteger &y);
void check_signature(BigInteger r, BigInteger s, BigInteger p, BigInteger q, BigInteger &y, const uint8_t* hash, BigInteger theta);

#endif //E2_DIGITALSIGNATURE_H
