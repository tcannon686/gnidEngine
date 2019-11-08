/*
 * matrix.c
 *
 * Written by Thomas Cannon 16 January 2017
 *
 * This file contains code for both C and Lua matrix math.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef TOLERANCE
#define TOLERANCE                      0.000001
#endif

#define min(a, b)                      ((a < b) ? a : b)
#define max(a, b)                      ((a > b) ? a : b)

#include "matrix.h"

matrix_t NewMatrix(
    vecc_t m11, vecc_t m12, vecc_t m13, vecc_t m14,
    vecc_t m21, vecc_t m22, vecc_t m23, vecc_t m24,
    vecc_t m31, vecc_t m32, vecc_t m33, vecc_t m34,
    vecc_t m41, vecc_t m42, vecc_t m43, vecc_t m44)
{
    matrix_t ret;
    ret.m11 = m11;
    ret.m12 = m12;
    ret.m13 = m13;
    ret.m14 = m14;
    ret.m21 = m21;
    ret.m22 = m22;
    ret.m23 = m23;
    ret.m24 = m24;
    ret.m31 = m31;
    ret.m32 = m32;
    ret.m33 = m33;
    ret.m34 = m34;
    ret.m41 = m41;
    ret.m42 = m42;
    ret.m43 = m43;
    ret.m44 = m44;
    return ret;
}

matrix_t IdentityMatrix()
{
    int i;
    matrix_t ret = EmptyMatrix();
    for(i = 1; i <= 4; i++){
        ret.m[MatrixIndex(i, i)] = 1.0;
    }
    
    return ret;
}

matrix_t EmptyMatrix()
{
    int i;
    matrix_t ret;
    
    for(i = 0; i < 16; i++)
    {
        ret.m[i] = 0;
    }
    
    return ret;
}

matrix_t MatrixNegate(matrix_t left)
{
    int i;
    matrix_t ret;
    
    for(i = 0; i < 16; i++)
    {
        ret.m[i] = -left.m[i];
    }
    
    return ret;
}

matrix_t MatrixPlusMatrix(matrix_t left, matrix_t right)
{
    int i, j;
    matrix_t ret;
    
    for(i = 1; i <= 4; i ++)
    {
        for(j = 1; j <= 4; j ++)
        {
            ret.m[MatrixIndex(i, j)] = left.m[MatrixIndex(i, j)] +
                right.m[MatrixIndex(i, j)];
        }
    }
    
    return ret;
}

matrix_t MatrixMinusMatrix(matrix_t left, matrix_t right)
{
    int i, j;
    matrix_t ret;
    
    for(i = 1; i <= 4; i ++)
    {
        for(j = 1; j <= 4; j ++)
        {
            ret.m[MatrixIndex(i, j)] = left.m[MatrixIndex(i, j)] -
                right.m[MatrixIndex(i, j)];
        }
    }
    
    return ret;
}

matrix_t MatrixTimesMatrix(matrix_t left, matrix_t right)
{
    int i, j, k;
    matrix_t ret;
    
    ret = EmptyMatrix();
    
    for(i = 1; i <= 4; i ++)
    {
        for(j = 1; j <= 4; j ++)
        {
            for(k = 1; k <= 4; k ++)
            {
                ret.m[MatrixIndex(i, j)] += left.m[MatrixIndex(i, k)] *
                    right.m[MatrixIndex(k, j)];
            }
        }
    }
    
    return ret;
}

vector_t MatrixTimesVector(matrix_t left, vector_t right)
{
    int i, j;
    vector_t ret;
    
    ret.x = ret.y = ret.z = ret.w = 0;
    
    for(i = 1; i <= 4; i ++)
    {
        for(j = 1; j <= 4; j ++)
        {
            ret.m[i - 1] += left.m[MatrixIndex(i, j)] * right.m[j - 1];
        }
    }
    
    return ret;
}

matrix_t MatrixTimesScalar(matrix_t left, vecc_t right)
{
    int i;
    matrix_t ret;
    
    for(i = 0; i < 16; i++)
        ret.m[i] = left.m[i] * right;
    
    return ret;
}

vecc_t MatrixDeterminant(matrix_t a)
{
    return a.m11*a.m22*a.m33*a.m44
        -a.m11*a.m22*a.m34*a.m43
        -a.m11*a.m23*a.m32*a.m44
        +a.m11*a.m23*a.m34*a.m42
        +a.m11*a.m24*a.m32*a.m43
        -a.m11*a.m24*a.m33*a.m42
        -a.m12*a.m21*a.m33*a.m44
        +a.m12*a.m21*a.m34*a.m43
        +a.m12*a.m23*a.m31*a.m44
        -a.m12*a.m23*a.m34*a.m41
        -a.m12*a.m24*a.m31*a.m43
        +a.m12*a.m24*a.m33*a.m41
        +a.m13*a.m21*a.m32*a.m44
        -a.m13*a.m21*a.m34*a.m42
        -a.m13*a.m22*a.m31*a.m44
        +a.m13*a.m22*a.m34*a.m41
        +a.m13*a.m24*a.m31*a.m42
        -a.m13*a.m24*a.m32*a.m41
        -a.m14*a.m21*a.m32*a.m43
        +a.m14*a.m21*a.m33*a.m42
        +a.m14*a.m22*a.m31*a.m43
        -a.m14*a.m22*a.m33*a.m41
        -a.m14*a.m23*a.m31*a.m42
        +a.m14*a.m23*a.m32*a.m41;
}

vecc_t MatrixTrace(matrix_t a)
{
    return a.m11 + a.m22 + a.m33 + a.m44;
}

matrix_t MatrixTranspose(matrix_t a)
{
    matrix_t ret;
    ret.m11 = a.m11;
    ret.m12 = a.m21;
    ret.m13 = a.m31;
    ret.m14 = a.m41;
    ret.m21 = a.m12;
    ret.m22 = a.m22;
    ret.m23 = a.m32;
    ret.m24 = a.m42;
    ret.m31 = a.m13;
    ret.m32 = a.m23;
    ret.m33 = a.m33;
    ret.m34 = a.m43;
    ret.m41 = a.m14;
    ret.m42 = a.m24;
    ret.m43 = a.m34;
    ret.m44 = a.m44;
    return ret;
}

matrix_t MatrixInverse(matrix_t a)
{
    matrix_t ret;
    ret.m11 = a.m22*a.m33*a.m44
        +a.m23*a.m34*a.m42
        +a.m24*a.m32*a.m43
        -a.m22*a.m34*a.m43
        -a.m23*a.m32*a.m44
        -a.m24*a.m33*a.m42;
    ret.m12 = a.m12*a.m34*a.m43
        +a.m13*a.m32*a.m44
        +a.m14*a.m33*a.m42
        -a.m12*a.m33*a.m44
        -a.m13*a.m34*a.m42
        -a.m14*a.m32*a.m43;
    ret.m13 = a.m12*a.m23*a.m44
        +a.m13*a.m24*a.m42
        +a.m14*a.m22*a.m43
        -a.m12*a.m24*a.m43
        -a.m13*a.m22*a.m44
        -a.m14*a.m23*a.m42;
    ret.m14 = a.m12*a.m24*a.m33
        +a.m13*a.m22*a.m34
        +a.m14*a.m23*a.m32
        -a.m12*a.m23*a.m34
        -a.m13*a.m24*a.m32
        -a.m14*a.m22*a.m33;
    
    ret.m21 = a.m21*a.m34*a.m43
        +a.m23*a.m31*a.m44
        +a.m24*a.m33*a.m41
        -a.m21*a.m33*a.m44
        -a.m23*a.m34*a.m41
        -a.m24*a.m31*a.m43;
    ret.m22 = a.m11*a.m33*a.m44
        +a.m13*a.m34*a.m41
        +a.m14*a.m31*a.m43
        -a.m11*a.m34*a.m43
        -a.m13*a.m31*a.m44
        -a.m14*a.m33*a.m41;
    ret.m23 = a.m11*a.m24*a.m43
        +a.m13*a.m21*a.m44
        +a.m14*a.m23*a.m41
        -a.m11*a.m23*a.m44
        -a.m13*a.m24*a.m41
        -a.m14*a.m21*a.m43;
    ret.m24 = a.m11*a.m23*a.m34
        +a.m13*a.m24*a.m31
        +a.m14*a.m21*a.m33
        -a.m11*a.m24*a.m33
        -a.m13*a.m21*a.m34
        -a.m14*a.m23*a.m31;
    
    ret.m31 = a.m21*a.m32*a.m44
        +a.m22*a.m34*a.m41
        +a.m24*a.m31*a.m42
        -a.m21*a.m34*a.m42
        -a.m22*a.m31*a.m44
        -a.m24*a.m32*a.m41;
    ret.m32 = a.m11*a.m34*a.m42
        +a.m12*a.m31*a.m44
        +a.m14*a.m32*a.m41
        -a.m11*a.m32*a.m44
        -a.m12*a.m34*a.m41
        -a.m14*a.m31*a.m42;
    ret.m33 = a.m11*a.m22*a.m44
        +a.m12*a.m24*a.m41
        +a.m14*a.m21*a.m42
        -a.m11*a.m24*a.m42
        -a.m12*a.m21*a.m44
        -a.m14*a.m22*a.m41;
    ret.m34 = a.m11*a.m24*a.m32
        +a.m12*a.m21*a.m34
        +a.m14*a.m22*a.m31
        -a.m11*a.m22*a.m34
        -a.m12*a.m24*a.m31
        -a.m14*a.m21*a.m32;
    
    ret.m41 = a.m21*a.m33*a.m42
        +a.m22*a.m31*a.m43
        +a.m23*a.m32*a.m41
        -a.m21*a.m32*a.m43
        -a.m22*a.m33*a.m41
        -a.m23*a.m31*a.m42;
    ret.m42 = a.m11*a.m32*a.m43
        +a.m12*a.m33*a.m41
        +a.m13*a.m31*a.m42
        -a.m11*a.m33*a.m42
        -a.m12*a.m31*a.m43
        -a.m13*a.m32*a.m41;
    ret.m43 = a.m11*a.m23*a.m42
        +a.m12*a.m21*a.m43
        +a.m13*a.m22*a.m41
        -a.m11*a.m22*a.m43
        -a.m12*a.m23*a.m41
        -a.m13*a.m21*a.m42;
    ret.m44 = a.m11*a.m22*a.m33
        +a.m12*a.m23*a.m31
        +a.m13*a.m21*a.m32
        -a.m11*a.m23*a.m32
        -a.m12*a.m21*a.m33
        -a.m13*a.m22*a.m31;
    
    ret = MatrixTimesScalar(ret, 1.0 / MatrixDeterminant(a));
    
    return ret;
}

matrix_t MatrixTo2x2(matrix_t left)
{
    matrix_t ret;
    ret.m11 = left.m11;
    ret.m12 = left.m12;
    ret.m13 = (vecc_t) 0;
    ret.m14 = (vecc_t) 0;

    ret.m21 = left.m21;
    ret.m22 = left.m22;
    ret.m23 = (vecc_t) 0;
    ret.m24 = (vecc_t) 0;

    ret.m31 = (vecc_t) 0;
    ret.m32 = (vecc_t) 0;
    ret.m33 = (vecc_t) 1;
    ret.m34 = (vecc_t) 0;

    ret.m41 = (vecc_t) 0;
    ret.m42 = (vecc_t) 0;
    ret.m43 = (vecc_t) 0;
    ret.m44 = (vecc_t) 1;

    return ret;
}

matrix_t MatrixTo3x3(matrix_t left)
{
    matrix_t ret;
    ret.m11 = left.m11;
    ret.m12 = left.m12;
    ret.m13 = left.m13;
    ret.m14 = (vecc_t) 0;

    ret.m21 = left.m21;
    ret.m22 = left.m22;
    ret.m23 = left.m23;
    ret.m24 = (vecc_t) 0;

    ret.m31 = left.m31;
    ret.m32 = left.m32;
    ret.m33 = left.m33;
    ret.m34 = (vecc_t) 0;
    
    ret.m41 = (vecc_t) 0;
    ret.m42 = (vecc_t) 0;
    ret.m43 = (vecc_t) 0;
    ret.m44 = (vecc_t) 1;

    return ret;
}

int MatrixEqualsMatrix(matrix_t left, matrix_t right)
{
    int i;
    for(i = 0; i < 16; i++)
    {
        if(fabs(left.m[i] - right.m[i]) > TOLERANCE)
            return 0;
    }
    return 1;
}

// Begin matrix pointer

void IdentityMatrixP(matrix_t *dest)
{
    int i;
    EmptyMatrixP(dest);
    for(i = 1; i <= 4; i++){
        dest->m[MatrixIndex(i, i)] = 1.0;
    }
}

void EmptyMatrixP(matrix_t *dest)
{
    int i;
    for(i = 0; i < 16; i++)
    {
        dest->m[i] = 0;
    }
}

void MatrixNegateP(matrix_t *dest, matrix_t *left)
{
    int i;
    
    for(i = 0; i < 16; i++)
    {
        dest->m[i] = -left->m[i];
    }
    
}

void MatrixPlusMatrixP(matrix_t *dest, matrix_t *left, matrix_t *right)
{
    int i, j;
    for(i = 1; i <= 4; i ++)
    {
        for(j = 1; j <= 4; j ++)
        {
            dest->m[MatrixIndex(i, j)] = left->m[MatrixIndex(i, j)] +
                right->m[MatrixIndex(i, j)];
        }
    }
    
}

void MatrixMinusMatrixP(matrix_t *dest, matrix_t *left, matrix_t *right)
{
    int i, j;
    for(i = 1; i <= 4; i ++)
    {
        for(j = 1; j <= 4; j ++)
        {
            dest->m[MatrixIndex(i, j)] = left->m[MatrixIndex(i, j)] -
                right->m[MatrixIndex(i, j)];
        }
    }
}

void MatrixTimesMatrixP(matrix_t *dest, matrix_t *left, matrix_t *right)
{
    int i, j, k;
    EmptyMatrixP(dest);
    
    for(i = 1; i <= 4; i ++)
    {
        for(j = 1; j <= 4; j ++)
        {
            for(k = 1; k <= 4; k ++)
            {
                dest->m[MatrixIndex(i, j)] += left->m[MatrixIndex(i, k)] *
                    right->m[MatrixIndex(k, j)];
            }
        }
    }
}

void MatrixTimesVectorP(vector_t *dest, matrix_t *left, vector_t *right)
{
    int i, j;
    dest->x = dest->y = dest->z = dest->w = 0;
    
    for(i = 1; i <= 4; i ++)
    {
        for(j = 1; j <= 4; j ++)
        {
            dest->m[i - 1] += left->m[MatrixIndex(i, j)] * right->m[j - 1];
        }
    }
}

void MatrixTimesScalarP(matrix_t *dest, matrix_t *left, vecc_t right)
{
    int i;
    for(i = 0; i < 16; i++)
        dest->m[i] = left->m[i] * right;
}

vecc_t MatrixDeterminantP(matrix_t *a)
{
    return a->m11*a->m22*a->m33*a->m44
        -a->m11*a->m22*a->m34*a->m43
        -a->m11*a->m23*a->m32*a->m44
        +a->m11*a->m23*a->m34*a->m42
        +a->m11*a->m24*a->m32*a->m43
        -a->m11*a->m24*a->m33*a->m42
        -a->m12*a->m21*a->m33*a->m44
        +a->m12*a->m21*a->m34*a->m43
        +a->m12*a->m23*a->m31*a->m44
        -a->m12*a->m23*a->m34*a->m41
        -a->m12*a->m24*a->m31*a->m43
        +a->m12*a->m24*a->m33*a->m41
        +a->m13*a->m21*a->m32*a->m44
        -a->m13*a->m21*a->m34*a->m42
        -a->m13*a->m22*a->m31*a->m44
        +a->m13*a->m22*a->m34*a->m41
        +a->m13*a->m24*a->m31*a->m42
        -a->m13*a->m24*a->m32*a->m41
        -a->m14*a->m21*a->m32*a->m43
        +a->m14*a->m21*a->m33*a->m42
        +a->m14*a->m22*a->m31*a->m43
        -a->m14*a->m22*a->m33*a->m41
        -a->m14*a->m23*a->m31*a->m42
        +a->m14*a->m23*a->m32*a->m41;
}

vecc_t MatrixTraceP(matrix_t *a)
{
    return a->m11 + a->m22 + a->m33 + a->m44;
}

void MatrixTransposeP(matrix_t *dest, matrix_t *a)
{
    dest->m11 = a->m11;
    dest->m12 = a->m21;
    dest->m13 = a->m31;
    dest->m14 = a->m41;
    dest->m21 = a->m12;
    dest->m22 = a->m22;
    dest->m23 = a->m32;
    dest->m24 = a->m42;
    dest->m31 = a->m13;
    dest->m32 = a->m23;
    dest->m33 = a->m33;
    dest->m34 = a->m43;
    dest->m41 = a->m14;
    dest->m42 = a->m24;
    dest->m43 = a->m34;
    dest->m44 = a->m44;
}

void MatrixInverseP(matrix_t *dest, matrix_t *a)
{
    dest->m11 = a->m22*a->m33*a->m44
        +a->m23*a->m34*a->m42
        +a->m24*a->m32*a->m43
        -a->m22*a->m34*a->m43
        -a->m23*a->m32*a->m44
        -a->m24*a->m33*a->m42;
    dest->m12 = a->m12*a->m34*a->m43
        +a->m13*a->m32*a->m44
        +a->m14*a->m33*a->m42
        -a->m12*a->m33*a->m44
        -a->m13*a->m34*a->m42
        -a->m14*a->m32*a->m43;
    dest->m13 = a->m12*a->m23*a->m44
        +a->m13*a->m24*a->m42
        +a->m14*a->m22*a->m43
        -a->m12*a->m24*a->m43
        -a->m13*a->m22*a->m44
        -a->m14*a->m23*a->m42;
    dest->m14 = a->m12*a->m24*a->m33
        +a->m13*a->m22*a->m34
        +a->m14*a->m23*a->m32
        -a->m12*a->m23*a->m34
        -a->m13*a->m24*a->m32
        -a->m14*a->m22*a->m33;
    
    dest->m21 = a->m21*a->m34*a->m43
        +a->m23*a->m31*a->m44
        +a->m24*a->m33*a->m41
        -a->m21*a->m33*a->m44
        -a->m23*a->m34*a->m41
        -a->m24*a->m31*a->m43;
    dest->m22 = a->m11*a->m33*a->m44
        +a->m13*a->m34*a->m41
        +a->m14*a->m31*a->m43
        -a->m11*a->m34*a->m43
        -a->m13*a->m31*a->m44
        -a->m14*a->m33*a->m41;
    dest->m23 = a->m11*a->m24*a->m43
        +a->m13*a->m21*a->m44
        +a->m14*a->m23*a->m41
        -a->m11*a->m23*a->m44
        -a->m13*a->m24*a->m41
        -a->m14*a->m21*a->m43;
    dest->m24 = a->m11*a->m23*a->m34
        +a->m13*a->m24*a->m31
        +a->m14*a->m21*a->m33
        -a->m11*a->m24*a->m33
        -a->m13*a->m21*a->m34
        -a->m14*a->m23*a->m31;
    
    dest->m31 = a->m21*a->m32*a->m44
        +a->m22*a->m34*a->m41
        +a->m24*a->m31*a->m42
        -a->m21*a->m34*a->m42
        -a->m22*a->m31*a->m44
        -a->m24*a->m32*a->m41;
    dest->m32 = a->m11*a->m34*a->m42
        +a->m12*a->m31*a->m44
        +a->m14*a->m32*a->m41
        -a->m11*a->m32*a->m44
        -a->m12*a->m34*a->m41
        -a->m14*a->m31*a->m42;
    dest->m33 = a->m11*a->m22*a->m44
        +a->m12*a->m24*a->m41
        +a->m14*a->m21*a->m42
        -a->m11*a->m24*a->m42
        -a->m12*a->m21*a->m44
        -a->m14*a->m22*a->m41;
    dest->m34 = a->m11*a->m24*a->m32
        +a->m12*a->m21*a->m34
        +a->m14*a->m22*a->m31
        -a->m11*a->m22*a->m34
        -a->m12*a->m24*a->m31
        -a->m14*a->m21*a->m32;
    
    dest->m41 = a->m21*a->m33*a->m42
        +a->m22*a->m31*a->m43
        +a->m23*a->m32*a->m41
        -a->m21*a->m32*a->m43
        -a->m22*a->m33*a->m41
        -a->m23*a->m31*a->m42;
    dest->m42 = a->m11*a->m32*a->m43
        +a->m12*a->m33*a->m41
        +a->m13*a->m31*a->m42
        -a->m11*a->m33*a->m42
        -a->m12*a->m31*a->m43
        -a->m13*a->m32*a->m41;
    dest->m43 = a->m11*a->m23*a->m42
        +a->m12*a->m21*a->m43
        +a->m13*a->m22*a->m41
        -a->m11*a->m22*a->m43
        -a->m12*a->m23*a->m41
        -a->m13*a->m21*a->m42;
    dest->m44 = a->m11*a->m22*a->m33
        +a->m12*a->m23*a->m31
        +a->m13*a->m21*a->m32
        -a->m11*a->m23*a->m32
        -a->m12*a->m21*a->m33
        -a->m13*a->m22*a->m31;
    
    MatrixTimesScalarP(dest, dest, 1.0 / MatrixDeterminantP(a));
}

void MatrixTo2x2P(matrix_t *dest, matrix_t *left)
{
    dest->m13 = (vecc_t) 0;
    dest->m14 = (vecc_t) 0;

    dest->m23 = (vecc_t) 0;
    dest->m24 = (vecc_t) 0;

    dest->m31 = (vecc_t) 0;
    dest->m32 = (vecc_t) 0;
    dest->m33 = (vecc_t) 1;
    dest->m34 = (vecc_t) 0;

    dest->m41 = (vecc_t) 0;
    dest->m42 = (vecc_t) 0;
    dest->m43 = (vecc_t) 0;
    dest->m44 = (vecc_t) 1;

    if(dest != left)
    {
        dest->m11 = left->m11;
        dest->m12 = left->m12;
        
        dest->m21 = left->m21;
        dest->m22 = left->m22;
    }
}

void MatrixTo3x3P(matrix_t *dest, matrix_t *left)
{
    dest->m14 = (vecc_t) 0;

    dest->m24 = (vecc_t) 0;

    dest->m34 = (vecc_t) 0;

    dest->m41 = (vecc_t) 0;
    dest->m42 = (vecc_t) 0;
    dest->m43 = (vecc_t) 0;
    dest->m44 = (vecc_t) 1;

    if(dest != left)
    {
        dest->m11 = left->m11;
        dest->m12 = left->m12;
        dest->m13 = left->m13;
        
        dest->m21 = left->m21;
        dest->m22 = left->m22;
        dest->m23 = left->m23;

        dest->m31 = left->m31;
        dest->m32 = left->m32;
        dest->m33 = left->m33;
    }
}

int MatrixEqualsMatrixP(matrix_t *left, matrix_t *right)
{
    int i;
    for(i = 0; i < 16; i++)
    {
        if(fabs(left->m[i] - right->m[i]) > TOLERANCE)
            return 0;
    }
    return 1;
}
// End pointer matrix


const char *MatrixToString(matrix_t left)
{
    char *c;
    c = (char *)malloc(310 * sizeof(char));
    sprintf(c,
"[    %f    %f    %f    %f\n\
    %f    %f    %f    %f\n\
    %f    %f    %f    %f\n\
    %f    %f    %f    %f    ]",
        left.m11, left.m12, left.m13, left.m14,
        left.m21, left.m22, left.m23, left.m24,
        left.m31, left.m32, left.m33, left.m34,
        left.m41, left.m42, left.m43, left.m44);
    return (const char *)c;
}

matrix_t NewTranslateMatrix(vector_t t)
{
    return NewMatrix(
        1, 0, 0, t.x,
        0, 1, 0, t.y,
        0, 0, 1, t.z,
        0, 0, 0, 1);
}
matrix_t NewScaleMatrix(vector_t s)
{
    return NewMatrix(
        s.x,    0,        0,        0,
        0,        s.y,    0,        0,
        0,        0,        s.z,    0,
        0,        0,        0,        1);
}

matrix_t NewRotateMatrix(vecc_t angle, vector_t axis)
{
    vecc_t s = sin(angle);
    vecc_t c = cos(angle);
    vecc_t x = axis.x;
    vecc_t y = axis.y;
    vecc_t z = axis.z;
    return NewMatrix(
        x*x*(1-c)+c,    x*y*(1-c)-z*s,    x*z*(1-c)+y*s,    0,
        y*x*(1-c)+z*s,    y*y*(1-c)+c,    y*z*(1-c)-x*s,    0,
        x*z*(1-c)-y*s,    y*z*(1-c)+x*s,    z*z*(1-c)+c,    0,
        0,                0,                0,                1);    
}

matrix_t NewOrthographicMatrix(
    vecc_t left, vecc_t right, vecc_t bottom, vecc_t top,
    vecc_t near, vecc_t far)
{
    vecc_t tx;
    vecc_t ty;
    vecc_t tz;
    
    tx = -(right + left) / (right - left);
    ty = -(top + bottom) / (top - bottom);
    tz = -(far + near) / (far - near);
    
    return NewMatrix(
        2 / (right - left),    0,                    0,                    tx,
        0,                    2 / (top - bottom),    0,                    ty,
        0,                    0,                    2 / (far - near),    tz,
        0,                    0,                    0,                    1);
}

matrix_t NewFrustrumMatrix(
    vecc_t left, vecc_t right, vecc_t bottom, vecc_t top,
    vecc_t near, vecc_t far)
{

    vecc_t A;
    vecc_t B;
    vecc_t C;
    vecc_t D;

    A = (right + left) / (right - left);
    B = (top + bottom) / (top - bottom);
    C = -(far + near) / (far - near);
    D = -2 * far * near / (far - near);

    return NewMatrix(
        2 * near / (left - right),    0,                            A,                    0,
        0,                            2 * near / (top - bottom),    B,                    0,
        0,                            0,                            C,                    D,
        0,                            0,                            -1,                    0);
}

matrix_t NewPerspectiveMatrix(
    vecc_t fovy,
    vecc_t aspect,
    vecc_t near,
    vecc_t far)
{
    vecc_t f;
    vecc_t A;
    vecc_t B;
    
    f = 1 / tan(fovy / 2);
    A = (far + near) / (near - far);
    B = 2 * far * near / (near - far);
    
    return NewMatrix(
        f / aspect,        0,                0,                0,
        0,                f,                0,                0,
        0,                0,                A,                B,
        0,                0,                -1,                0);
}

matrix_t NewLookAtMatrix(vector_t eye, vector_t center, vector_t up)
{
    vector_t f;
    vector_t UP;
    vector_t s;
    vector_t u;
    f = VectorNormalize(VectorThree(VectorMinusVector(center, eye)));
    UP = VectorNormalize(VectorThree(up));
    s = VectorCrossVector(f, UP);
    u = VectorCrossVector(VectorNormalize(s), f);
    return NewMatrix(
        s.x,    s.y,    s.z,    -eye.x,
        u.x,    u.y,    u.z,    -eye.y,
        -f.x,    -f.y,    -f.z,    -eye.z,
        0,        0,        0,        1);
}

void NewMatrixP(
    matrix_t *dest,
	vecc_t m11, vecc_t m12, vecc_t m13, vecc_t m14,
	vecc_t m21, vecc_t m22, vecc_t m23, vecc_t m24,
	vecc_t m31, vecc_t m32, vecc_t m33, vecc_t m34,
	vecc_t m41, vecc_t m42, vecc_t m43, vecc_t m44)
{
    dest->m11 = m11;
    dest->m12 = m12;
    dest->m13 = m13;
    dest->m14 = m14;
    dest->m21 = m21;
    dest->m22 = m22;
    dest->m23 = m23;
    dest->m24 = m24;
    dest->m31 = m31;
    dest->m32 = m32;
    dest->m33 = m33;
    dest->m34 = m34;
    dest->m41 = m41;
    dest->m42 = m42;
    dest->m43 = m43;
    dest->m44 = m44;
}

void NewTranslateMatrixP(matrix_t *dest, vector_t t)
{
    NewMatrixP(
        dest,
        1, 0, 0, t.x,
        0, 1, 0, t.y,
        0, 0, 1, t.z,
        0, 0, 0, 1);
}
void NewScaleMatrixP(matrix_t *dest, vector_t s)
{
    NewMatrixP(
        dest,
        s.x,    0,        0,        0,
        0,        s.y,    0,        0,
        0,        0,        s.z,    0,
        0,        0,        0,        1);
}

void NewRotateMatrixP(matrix_t *dest, vecc_t angle, vector_t axis)
{
    vecc_t s = sin(angle);
    vecc_t c = cos(angle);
    vecc_t x = axis.x;
    vecc_t y = axis.y;
    vecc_t z = axis.z;
    NewMatrixP(
        dest,
        x*x*(1-c)+c,    x*y*(1-c)-z*s,    x*z*(1-c)+y*s,    0,
        y*x*(1-c)+z*s,    y*y*(1-c)+c,    y*z*(1-c)-x*s,    0,
        x*z*(1-c)-y*s,    y*z*(1-c)+x*s,    z*z*(1-c)+c,    0,
        0,                0,                0,                1);    
}

void NewOrthographicMatrixP(
    matrix_t *dest, 
    vecc_t left, vecc_t right, vecc_t bottom, vecc_t top,
    vecc_t near, vecc_t far)
{
    vecc_t tx;
    vecc_t ty;
    vecc_t tz;
    
    tx = -(right + left) / (right - left);
    ty = -(top + bottom) / (top - bottom);
    tz = -(far + near) / (far - near);
    
    NewMatrixP(
        dest,
        2 / (right - left),    0,                    0,                    tx,
        0,                    2 / (top - bottom),    0,                    ty,
        0,                    0,                    2 / (far - near),    tz,
        0,                    0,                    0,                    1);
}

void NewFrustrumMatrixP(
    matrix_t *dest, 
    vecc_t left, vecc_t right, vecc_t bottom, vecc_t top,
    vecc_t near, vecc_t far)
{

    vecc_t A;
    vecc_t B;
    vecc_t C;
    vecc_t D;

    A = (right + left) / (right - left);
    B = (top + bottom) / (top - bottom);
    C = -(far + near) / (far - near);
    D = -2 * far * near / (far - near);

    NewMatrixP(
        dest,
        2 * near / (left - right),    0,                            A,                    0,
        0,                            2 * near / (top - bottom),    B,                    0,
        0,                            0,                            C,                    D,
        0,                            0,                            -1,                    0);
}

void NewPerspectiveMatrixP(
    matrix_t *dest, 
    vecc_t fovy,
    vecc_t aspect,
    vecc_t near,
    vecc_t far)
{
    vecc_t f;
    vecc_t A;
    vecc_t B;
    
    f = 1 / tan(fovy / 2);
    A = (far + near) / (near - far);
    B = 2 * far * near / (near - far);
    
    NewMatrixP(
        dest,
        f / aspect,        0,                0,                0,
        0,                f,                0,                0,
        0,                0,                A,                B,
        0,                0,                -1,                0);
}

void NewLookAtMatrixP(matrix_t *dest, vector_t eye, vector_t center, vector_t up)
{
    vector_t f;
    vector_t UP;
    vector_t s;
    vector_t u;
    f = VectorNormalize(VectorThree(VectorMinusVector(center, eye)));
    UP = VectorNormalize(VectorThree(up));
    s = VectorCrossVector(f, UP);
    u = VectorCrossVector(VectorNormalize(s), f);
    NewMatrixP(
        dest,
        s.x,    s.y,    s.z,    -eye.x,
        u.x,    u.y,    u.z,    -eye.y,
        -f.x,    -f.y,    -f.z,    -eye.z,
        0,        0,        0,        1);
}

vector_t EmptyVector()
{
    vector_t ret;
    ret.x = 0;
    ret.y = 0;
    ret.z = 0;
    ret.w = 0;
    
    return ret;
}

vector_t VectorNegate(vector_t left)
{
    vector_t ret;
    ret.x = -left.x;
    ret.y = -left.y;
    ret.z = -left.z;
    ret.w = -left.w;
    return ret;
}

vecc_t VectorMagnitude(vector_t left)
{
    return sqrt(
        left.x * left.x
        + left.y * left.y
        + left.z * left.z
        + left.w * left.w);
}

vector_t VectorHomo(vector_t left)
{
    /* Returns the homogenous version of the vector, i.e. sets w = 1. */
    vector_t ret;
    ret.x = left.x;
    ret.y = left.y;
    ret.z = left.z;
    ret.w = 1.0;
    return ret;
}

vector_t VectorThree(vector_t left)
{
    /* Returns the three component version of the vector, i.e. sets w = 0. */
    vector_t ret;
    ret.x = left.x;
    ret.y = left.y;
    ret.z = left.z;
    ret.w = 0.0;
    return ret;
}

vector_t VectorMean(vector_t left, vector_t right)
{
    vector_t ret;
    ret.x = (left.x + right.x) / 2;
    ret.y = (left.y + right.y) / 2;
    ret.z = (left.z + right.z) / 2;
    ret.w = (left.w + right.w) / 2;
    return ret;
}

vecc_t VectorDistance(vector_t left, vector_t right)
{
    float dx = left.x - right.x,
        dy = left.y - right.y,
        dz = left.z - right.z,
        dw = left.w - right.w;
    return sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
}

vector_t VectorPlusVector(vector_t left, vector_t right)
{
    vector_t ret;
    ret.x = left.x + right.x;
    ret.y = left.y + right.y;
    ret.z = left.z + right.z;
    ret.w = left.w + right.w;
    return ret;
}

vector_t VectorMinusVector(vector_t left, vector_t right)
{
    vector_t ret;
    ret.x = left.x - right.x;
    ret.y = left.y - right.y;
    ret.z = left.z - right.z;
    ret.w = left.w - right.w;
    return ret;
}

vector_t VectorTimesVector(vector_t left, vector_t right)
{
    vector_t ret;
    ret.x = left.x * right.x;
    ret.y = left.y * right.y;
    ret.z = left.z * right.z;
    ret.w = left.w * right.w;
    return ret;
}

vecc_t VectorDotVector(vector_t left, vector_t right)
{
    return left.x * right.x
        + left.y * right.y
        + left.z * right.z
        + left.w * right.w;
}

vector_t VectorCrossVector(vector_t left, vector_t right)
{
    vector_t ret;
    ret.x = left.y * right.z - left.z * right.y;
    ret.y = left.z * right.x - left.x * right.z;
    ret.z = left.x * right.y - left.y * right.x;
    ret.w = 0;
    return ret;
}

vector_t VectorTimesScalar(vector_t left, vecc_t right)
{
    vector_t ret;
    ret.x = left.x * right;
    ret.y = left.y * right;
    ret.z = left.z * right;
    ret.w = left.w * right;
    return ret;
}

vector_t VectorNormalize(vector_t left)
{
    return VectorTimesScalar(left, 1.0 / VectorMagnitude(left));
}

int VectorEqualsVector(vector_t left, vector_t right)
{
    return fabs(left.x - right.x) <= TOLERANCE
        && fabs(left.y - right.y) <= TOLERANCE
        && fabs(left.z - right.z) <= TOLERANCE
        && fabs(left.w - right.w) <= TOLERANCE;
}

const char *VectorToString(vector_t left)
{
    char *c;
    c = (char *)malloc(80 * sizeof(char));
    sprintf(c,
"[    %lf    %lf    %lf    %lf    ]",
    left.m11, left.m12, left.m13, left.m14);
    return (const char *)c;
}

vector_t VectorClamp(vector_t left)
{
    left.x = min(max(left.x, 0), 1);
    left.y = min(max(left.y, 0), 1);
    left.z = min(max(left.z, 0), 1);
    left.w = min(max(left.w, 0), 1);
    
    return left;
}

vector_t VectorReflect(vector_t i, vector_t n)
{
    return VectorMinusVector(VectorTimesScalar(n, 2 * VectorDotVector(n, i)), i);
}

vector_t VectorRefract(vector_t i, vector_t normal, vecc_t n1, vecc_t n2)
{
    vecc_t n = n1 / n2;
    vecc_t cosI = -VectorDotVector(normal, i);
    vecc_t sinT2 = n * n * (1 - cosI * cosI);
    if(sinT2 > 1)
        return normal;
    vecc_t cosT = sqrt(1 - sinT2);
    return VectorPlusVector(VectorTimesScalar(i, n), VectorTimesScalar(normal, n * cosI - cosT));
}

// Begin vector pointer
void EmptyVectorP(vector_t *dest)
{
    dest->x = 0;
    dest->y = 0;
    dest->z = 0;
    dest->w = 0;
}

void VectorNegateP(vector_t *dest, vector_t *left)
{
    dest->x = -left->x;
    dest->y = -left->y;
    dest->z = -left->z;
    dest->w = -left->w;
}

vecc_t VectorMagnitudeP(vector_t *left)
{
    return sqrt(
        left->x * left->x
        + left->y * left->y
        + left->z * left->z
        + left->w * left->w);
}

void VectorHomoP(vector_t *dest, vector_t *left)
{
    dest->x = left->x;
    dest->y = left->y;
    dest->z = left->z;
    dest->w = 1.0;
}

void VectorThreeP(vector_t *dest, vector_t *left)
{
    dest->x = left->x;
    dest->y = left->y;
    dest->z = left->z;
    dest->w = 0.0;
}

void VectorMeanP(vector_t *dest, vector_t *left, vector_t *right)
{
    dest->x = (left->x + right->x) / 2;
    dest->y = (left->y + right->y) / 2;
    dest->z = (left->z + right->z) / 2;
    dest->w = (left->w + right->w) / 2;
}

vecc_t VectorDistanceP(vector_t *left, vector_t *right)
{
    vecc_t dx = left->x - right->x,
        dy = left->y - right->y,
        dz = left->z - right->z,
        dw = left->w - right->w;
    return sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
}

void VectorPlusVectorP(vector_t *dest, vector_t *left, vector_t *right)
{
    dest->x = left->x + right->x;
    dest->y = left->y + right->y;
    dest->z = left->z + right->z;
    dest->w = left->w + right->w;
}

void VectorMinusVectorP(vector_t *dest, vector_t *left, vector_t *right)
{
    dest->x = left->x - right->x;
    dest->y = left->y - right->y;
    dest->z = left->z - right->z;
    dest->w = left->w - right->w;
}

void VectorTimesVectorP(vector_t *dest, vector_t *left, vector_t *right)
{
    dest->x = left->x * right->x;
    dest->y = left->y * right->y;
    dest->z = left->z * right->z;
    dest->w = left->w * right->w;
}

vecc_t VectorDotVectorP(vector_t *left, vector_t *right)
{
    return left->x * right->x
        + left->y * right->y
        + left->z * right->z
        + left->w * right->w;
}

void VectorCrossVectorP(vector_t *dest, vector_t *left, vector_t *right)
{
    vecc_t x = left->y * right->z - left->z * right->y,
        y = left->z * right->x - left->x * right->z,
        z = left->x * right->y - left->y * right->x;
    dest->x = x;
    dest->y = y;
    dest->z = z;
    dest->w = 0;
}

void VectorTimesScalarP(vector_t *dest, vector_t *left, vecc_t right)
{
    dest->x = left->x * right;
    dest->y = left->y * right;
    dest->z = left->z * right;
    dest->w = left->w * right;
}

void VectorNormalizeP(vector_t *dest, vector_t *left)
{
    VectorTimesScalarP(dest, left, 1.0 / VectorMagnitudeP(left));
}

int VectorEqualsVectorP(vector_t *left, vector_t *right)
{
    return fabs(left->x - right->x) <= TOLERANCE
        && fabs(left->y - right->y) <= TOLERANCE
        && fabs(left->z - right->z) <= TOLERANCE
        && fabs(left->w - right->w) <= TOLERANCE;
}

void VectorClampP(vector_t *dest, vector_t *left)
{
    dest->x = min(max(left->x, 0), 1);
    dest->y = min(max(left->y, 0), 1);
    dest->z = min(max(left->z, 0), 1);
    dest->w = min(max(left->w, 0), 1);
}

void VectorReflectP(vector_t *dest, vector_t *i, vector_t *n)
{
    VectorTimesScalarP(dest, n, 2 * VectorDotVectorP(n, i));
    VectorMinusVectorP(dest, dest, i);
}

// End vector pointer

vector_t vec2(vecc_t x, vecc_t y)
{
    vector_t ret;
    ret.x = x;
    ret.y = y;
    ret.z = 0;
    ret.w = 0;
    return ret;
}

vector_t vec3(vecc_t x, vecc_t y, vecc_t z)
{
    vector_t ret;
    ret.x = x;
    ret.y = y;
    ret.z = z;
    ret.w = 0;
    return ret;
}

vector_t vec4(vecc_t x, vecc_t y, vecc_t z, vecc_t w)
{
    vector_t ret;
    ret.x = x;
    ret.y = y;
    ret.z = z;
    ret.w = w;
    return ret;
}

