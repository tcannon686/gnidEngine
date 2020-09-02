/*
 * matrix.h
 *
 * Written by Thomas Cannon 16 January 2017
 *
 * Compile with option -dLUA to compile with Lua support.
 */

typedef float vecc_t;
#ifndef VECC_T_GL_TYPE
#define VECC_T_GL_TYPE				GL_FLOAT
#endif

union matrix;
typedef union matrix matrix_t;

union vector;
typedef union vector vector_t;

typedef union matrix
{
	struct
	{
		vecc_t
			m11, m12, m13, m14,
			m21, m22, m23, m24,
			m31, m32, m33, m34,
			m41, m42, m43, m44;
	};
	
	vecc_t m[16];

#ifdef __cplusplus
	vecc_t operator[](size_t idx);
	matrix_t *operator+(matrix_t *right);
	matrix_t *operator-(matrix_t *right);
	matrix_t *operator*(matrix_t *right);
	vector_t *operator*(vector_t *right);
	matrix_t *operator*(vecc_t right);
	int operator==(matrix_t *right);
	
	vecc_t det();
	vecc_t trace();
	void transpose();
	void invert();
	void negate();
	matrix_t *copy();
	
	matrix();
	matrix(
		vecc_t m11, vecc_t m12, vecc_t m13, vecc_t m14,
		vecc_t m21, vecc_t m22, vecc_t m23, vecc_t m24,
		vecc_t m31, vecc_t m32, vecc_t m33, vecc_t m34,
		vecc_t m41, vecc_t m42, vecc_t m43, vecc_t m44);
#endif
} matrix_t;

typedef union vector
{
	struct
	{
		vecc_t x, y, z, w;
	};
	
	struct
	{
		vecc_t m11, m12, m13, m14;
	};
	
	vecc_t m[4];
	
#ifdef __cplusplus
	vector_t *operator+(vector_t *right);
	vector_t *operator-(vector_t *right);
	vector_t *operator*(vector_t *right);
	vector_t *operator*(matrix_t *right);
	vector_t *operator*(vecc_t right);
	int operator==(vector_t *right);
	vector_t *copy();
	
	void negate();
	void normalize();
	void clamp();
	
	vecc_t magnitude();
	vecc_t dot(vector_t *right);
	vector_t *mean(vector_t *right);
	vector_t *cross(vector_t *right);
	
	vector();
	vector(vecc_t x);
	vector(vecc_t x, vecc_t y);
	vector(vecc_t x, vecc_t y, vecc_t z);
	vector(vecc_t x, vecc_t y, vecc_t z, vecc_t w);
#endif
} vector_t;

/* C matrix functions. */

matrix_t NewMatrix(
	vecc_t m11, vecc_t m12, vecc_t m13, vecc_t m14,
	vecc_t m21, vecc_t m22, vecc_t m23, vecc_t m24,
	vecc_t m31, vecc_t m32, vecc_t m33, vecc_t m34,
	vecc_t m41, vecc_t m42, vecc_t m43, vecc_t m44);

matrix_t IdentityMatrix();
matrix_t EmptyMatrix();

matrix_t MatrixNegate(matrix_t left);

void IdentityMatrixP(matrix_t *dest);
void EmptyMatrixP(matrix_t *dest);
void MatrixNegateP(matrix_t *dest, matrix_t *left);

/* Get the equivilent index of the given element for a matrix. */
#define MatrixIndex(i, j)								(i - 1) * 4 + (j - 1)

matrix_t MatrixPlusMatrix(matrix_t left, matrix_t right);
matrix_t MatrixMinusMatrix(matrix_t left, matrix_t right);

matrix_t MatrixTimesMatrix(matrix_t left, matrix_t right);
vector_t MatrixTimesVector(matrix_t left, vector_t right);
matrix_t MatrixTimesScalar(matrix_t left, vecc_t right);

vecc_t MatrixDeterminant(matrix_t a);
vecc_t MatrixTrace(matrix_t a);
matrix_t MatrixTranspose(matrix_t a);

matrix_t MatrixInverse(matrix_t a);

matrix_t MatrixTo2x2(matrix_t a);
matrix_t MatrixTo3x3(matrix_t a);

int MatrixEqualsMatrix(matrix_t left, matrix_t right);

const char *MatrixToString(matrix_t left);

matrix_t NewTranslateMatrix(vector_t translate);
matrix_t NewScaleMatrix(vector_t scale);
matrix_t NewRotateMatrix(vecc_t angle, vector_t axis);
matrix_t NewOrthographicMatrix(
	vecc_t left, vecc_t right, vecc_t bottom, vecc_t top,
	vecc_t near, vecc_t far);
matrix_t NewFrustrumMatrix(
	vecc_t left, vecc_t right, vecc_t bottom, vecc_t top,
	vecc_t near, vecc_t far);
matrix_t NewPerspectiveMatrix(
	vecc_t fovy,
	vecc_t aspect,
	vecc_t near,
	vecc_t far);
matrix_t NewLookAtMatrix(vector_t eye, vector_t center, vector_t up);

void NewMatrixP(
    matrix_t *dest,
	vecc_t m11, vecc_t m12, vecc_t m13, vecc_t m14,
	vecc_t m21, vecc_t m22, vecc_t m23, vecc_t m24,
	vecc_t m31, vecc_t m32, vecc_t m33, vecc_t m34,
	vecc_t m41, vecc_t m42, vecc_t m43, vecc_t m44);

void NewTranslateMatrixP(matrix_t *dest, vector_t translate);
void NewScaleMatrixP(matrix_t *dest, vector_t scale);
void NewRotateMatrixP(matrix_t *dest, vecc_t angle, vector_t axis);
void NewOrthographicMatrixP(
    matrix_t *dest, 
	vecc_t left, vecc_t right, vecc_t bottom, vecc_t top,
	vecc_t near, vecc_t far);
void NewFrustrumMatrixP(
    matrix_t *dest, 
	vecc_t left, vecc_t right, vecc_t bottom, vecc_t top,
	vecc_t near, vecc_t far);
void NewPerspectiveMatrixP(
    matrix_t *dest, 
	vecc_t fovy,
	vecc_t aspect,
	vecc_t near,
	vecc_t far);
void NewLookAtMatrixP(matrix_t *dest, vector_t eye, vector_t center, vector_t up);

void MatrixPlusMatrixP(matrix_t *dest, matrix_t *left, matrix_t *right);
void MatrixMinusMatrixP(matrix_t *dest, matrix_t *left, matrix_t *right);

void MatrixTimesMatrixP(matrix_t *dest, matrix_t *left, matrix_t *right);
void MatrixTimesVectorP(vector_t *dest, matrix_t *left, vector_t *right);
void MatrixTimesScalarP(matrix_t *dest, matrix_t *left, vecc_t right);

vecc_t MatrixDeterminantP(matrix_t *a);
vecc_t MatrixTraceP(matrix_t *a);
void MatrixTransposeP(matrix_t *dest, matrix_t *a);

void MatrixInverseP(matrix_t *dest, matrix_t *a);
void MatrixTo2x2P(matrix_t *dest, matrix_t *a);
void MatrixTo3x3P(matrix_t *dest, matrix_t *a);

int MatrixEqualsMatrixP(matrix_t *left, matrix_t *right);

vector_t EmptyVector();

vector_t VectorNegate(vector_t left);
vecc_t VectorMagnitude(vector_t left);
vector_t VectorHomo(vector_t left);
vector_t VectorThree(vector_t left);

vector_t VectorMean(vector_t left, vector_t right);
vecc_t VectorDistance(vector_t left, vector_t right);
vector_t VectorPlusVector(vector_t left, vector_t right);
vector_t VectorMinusVector(vector_t left, vector_t right);

vector_t VectorTimesVector(vector_t left, vector_t right);
vecc_t VectorDotVector(vector_t left, vector_t right);
vector_t VectorCrossVector(vector_t left, vector_t right);
vector_t VectorTimesScalar(vector_t left, vecc_t right);

vector_t VectorNormalize(vector_t left);

int VectorEqualsVector(vector_t left, vector_t right);

const char *VectorToString(vector_t left);

vector_t VectorClamp(vector_t left);

vector_t VectorReflect(vector_t i, vector_t n);
vector_t VectorRefract(vector_t i, vector_t n, vecc_t n1, vecc_t n2);

vector_t vec2(vecc_t x, vecc_t y);
vector_t vec3(vecc_t x, vecc_t y, vecc_t z);
vector_t vec4(vecc_t x, vecc_t y, vecc_t z, vecc_t w);

void EmptyVectorP(vector_t *dest);

void VectorNegateP(vector_t *dest, vector_t *left);
vecc_t VectorMagnitudeP(vector_t *left);
void VectorHomoP(vector_t *dest, vector_t *left);
void VectorThreeP(vector_t *dest, vector_t *left);

void VectorMeanP(vector_t *dest, vector_t *left, vector_t *right);
vecc_t VectorDistanceP(vector_t *left, vector_t *right);
void VectorPlusVectorP(vector_t *dest, vector_t *left, vector_t *right);
void VectorMinusVectorP(vector_t *dest, vector_t *left, vector_t *right);

void VectorTimesVectorP(vector_t *dest, vector_t *left, vector_t *right);
vecc_t VectorDotVectorP(vector_t *left, vector_t *right);
void VectorCrossVectorP(vector_t *dest, vector_t *left, vector_t *right);
void VectorTimesScalarP(vector_t *dest, vector_t *left, vecc_t right);

void VectorNormalizeP(vector_t *dest, vector_t *left);

int VectorEqualsVectorP(vector_t *left, vector_t *right);
void VectorClampP(vector_t *dest, vector_t *left);

void VectorReflectP(vector_t *dest, vector_t *i, vector_t *n);
