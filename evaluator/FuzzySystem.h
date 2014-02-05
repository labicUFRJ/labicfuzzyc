//    Laboratorio de Inteligencia Computacional
//    NCE/UFRJ - http://www.labic.nce.ufrj.br/
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __FUZZYSYSTEM_H__
#define __FUZZYSYSTEM_H__

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

// Precisao dos calculos. Lembre-se que os contadores estao em char/uchar
#define INTERVALO 100 
#define CONJ_T 1
#define CONJ_GBELLMF 2
#define ANDMETHOD_MIN 1
#define ANDMETHOD_PROD 2
#define ORMETHOD_MAX 1
#define MAMDANI 1
#define SUGENO 2

#define debug 0
#define fuzzy_min(a, b) ((a) < (b) ? (a) : (b))
#define fuzzy_max(a, b) ((a) > (b) ? (a) : (b))

class Conjunto {
public:
	char tipo;
	float a,b,c,d;
    
	float membership(float x);
};

class LinearOutput {
public:
	float *c;
    
	float evaluate(float *x);
};

typedef struct {
	char numConjuntos;
	float min, max;
	Conjunto *conj;
	LinearOutput *linear_conj;
} Variavel;

typedef struct {
	unsigned char *conjEntrada, *conjSaida;
} Regra;

class FuzzySystem {
public:
	unsigned char tipo, or_method, and_method;
	unsigned char NUMINPUTS, NUMOUTPUTS, NUMRULES;
	Variavel *var;
	Regra *regras;
	float *saidas;
	float *num;
	float *den;
    float **forca_conj_saida;
    float *var_saida;
	
	FuzzySystem(unsigned char t, unsigned char orm, unsigned char andm, unsigned char inputs, unsigned char outputs, unsigned char rules, Variavel *v, Regra *r);
    
    void alocarVariaveis(void);
    void limparVariaveis(void);
	float* rodarSistema(float *x);
    void agregarSaida(unsigned char numsaida);
    void centroid(unsigned char numsaida);
    
};

void rodarMeuFis(float* x, float** y);

#endif /* __FUZZYSYSTEM_H__ */