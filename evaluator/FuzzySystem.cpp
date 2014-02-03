// Mario Cecchi - 2012 - LabIC - UFRJ

#include "FuzzySystem.h"
using namespace std;

float Conjunto::membership(float x) {
    
    if (tipo == CONJ_T) {
        if (x < a) {
            return 0;
        }
        else if (x >= a && x < b) {
            return (x - a)/(b - a);
        }
        else if (x >= b && x < c)	{
            return 1;
        }
        else if (x >= c && x < d) {
            return (d - x)/(d - c);
        }
        else {
            return 0;
        }
    } else if (tipo == CONJ_GBELLMF) {
        return 1/(1+pow(fabs((x-c)/a),(2*b)));
    }
    
    return -1;
    
}

float LinearOutput::evaluate(float *x) {
    unsigned int i;
    float answ = 0;
    
    for (i=0; i<(sizeof(x)/sizeof(float)); i++) {
        answ += c[i]*x[i];
    }
    
    answ += c[i];
    
    return answ;
}

FuzzySystem::FuzzySystem(unsigned char t, unsigned char orm, unsigned char andm, unsigned char inputs, unsigned char outputs, unsigned char rules, Variavel *v, Regra *r) {
	saidas = NULL;
	num = NULL;
    den = NULL;
    forca_conj_saida = NULL;
    var_saida = NULL;
    
    // Create new object with system settings
    tipo = t;
    or_method = orm;
    and_method = andm;
    NUMINPUTS = (char) inputs;
    NUMOUTPUTS = (char) outputs;
    NUMRULES = (char) rules;
    
    // Allocate memory to store system data
    var = v;
    regras = r;
    
    saidas = (float*) malloc(NUMOUTPUTS*sizeof(float));
    
    if (tipo == SUGENO) {
        num = (float*) malloc(NUMOUTPUTS*sizeof(float));
        den = (float*) malloc(NUMOUTPUTS*sizeof(float));
        
        if (saidas == NULL || num == NULL || den == NULL) {
            if (debug) cout << "Erro de alocacao" << endl;
        }
    } else if (tipo == MAMDANI) {
        var_saida = (float*) malloc(INTERVALO*sizeof(float));
        
        // Forca_conj_saida sera uma matriz com linhas correspodentes a var. de saida
        // e cada coluna sendo a forca de cada conjunto daquela saida
        forca_conj_saida = (float**) malloc(NUMOUTPUTS*sizeof(float));
        
        if (saidas == NULL || var_saida == NULL || forca_conj_saida == NULL) {
            if (debug) cout << "Erro de alocacao" << endl;
        }
    } else {
        if (debug) cout << "Tipo de sistema nao suportado" << endl;
    }
}

void FuzzySystem::alocarVariaveis() {
    saidas = (float*) malloc(NUMOUTPUTS*sizeof(float));
    
    if (tipo == SUGENO) {
        num = (float*) malloc(NUMOUTPUTS*sizeof(float));
        den = (float*) malloc(NUMOUTPUTS*sizeof(float));
        
        if (saidas == NULL || num == NULL || den == NULL) {
            if (debug) cout << "Erro de alocacao" << endl;
        }
    } else if (tipo == MAMDANI) {
        var_saida = (float*) malloc(INTERVALO*sizeof(float));
        
        // Forca_conj_saida sera uma matriz com linhas correspodentes a var. de saida
        // e cada coluna sendo a forca de cada conjunto daquela saida
        forca_conj_saida = (float**) malloc(NUMOUTPUTS*sizeof(float));
        
        if (saidas == NULL || var_saida == NULL || forca_conj_saida == NULL) {
            if (debug) cout << "Erro de alocacao" << endl;
        }
    } else {
        if (debug) cout << "Tipo de sistema nao suportado" << endl;
    }
}

void FuzzySystem::limparVariaveis() {    
    if (tipo == SUGENO) {
        free(num);
        free(den);
    } else if (tipo == MAMDANI) {
        free(var_saida);
        free(forca_conj_saida);
    }
    
    alocarVariaveis();
}

float* FuzzySystem::rodarSistema(float *x) {
    unsigned char regra, j, conj;
    float forca_regra;
//    
//    if ((sizeof(x)/sizeof(float)) != NUMINPUTS) {
//        if (debug) cout << "Erro: numero de entradas incompativel com sistema." << endl;
//        return NULL;
//    }
    
    if (debug) {
        cout << "Rodando sistema com entradas " << endl;
        for (j=0; j<NUMINPUTS; j++) {
            cout << "x[" << (int)j << "] = ";
            cout << x[j] << endl;
        }
    }
    
    if (tipo == SUGENO) {
        for (j=0; j<NUMOUTPUTS; j++) {
            num[j] = 0;
            den[j] = 0;
        }
    }
    
    for (regra=0; regra<NUMRULES; regra++) {
        if (debug) cout << "Avaliando regra " << (int)regra << endl;
        forca_regra = 1;
        
        // Avaliar entradas para determinar forca que tera a regra
        // j = numero da entrada sendo avaliada em j
        for (j=0; j<NUMINPUTS; j++) {
            conj = (regras[regra].conjEntrada[j] - 1 < 0) ? 255 : regras[regra].conjEntrada[j] - 1; // conjunto que da variavel j que sera avaliado
            
            // Antes de calcular o mi de x, confirma se a regra depende daquela entrada, senao ja passo
            if (conj != 255) {
                if (debug) cout << "u("<< x[j] <<") no conjunto " << (int)conj+1 << " da entrada "<<(int)j<<": " << var[j].conj[conj].membership(x[j]) << endl;
                
                if (and_method == ANDMETHOD_MIN) {
                    forca_regra = fuzzy_min(var[j].conj[conj].membership(x[j]), forca_regra);
                } else if (and_method == ANDMETHOD_PROD) {
                    forca_regra *= var[j].conj[conj].membership(x[j]);
                }
            }
        }
        
        // Avaliar quanto sera cada saida ativada pela regra a partir dos dados atuais
        // Se a regra tiver forca 0, isso eh desnecessario, pois nao vai ativar nenhuma saida.
        if (forca_regra > 0) {
			if (debug) cout << "Regra ativada com forca " << forca_regra << endl;
            for (j=0; j<NUMOUTPUTS; j++) {
                conj = regras[regra].conjSaida[j]-1;
                
                if (tipo == SUGENO) {
                    num[j] += forca_regra * var[NUMINPUTS+j].linear_conj[conj].evaluate(x);
                    den[j] += forca_regra;
                } else if (tipo == MAMDANI) {

                    // Se esta for a primeira vez avaliando essa saida, crio seu vetor
                    // que guardara a forca de cada um dos seus conjuntos
                    if (forca_conj_saida[j] == NULL) {
                        forca_conj_saida[j] = (float*) malloc(var[NUMINPUTS+j].numConjuntos*sizeof(float));
                        if (forca_conj_saida[j] == NULL) {
                            if (1) cout << "Erro de alocacao: forca_conj_saida" << endl;
                            return NULL;
                        }
                    }
                    
                    /*cout << "DEBUG fuzzy_max(forca_conj_saida[" << (int)j << "][" << (int)conj << "]=" << endl;
                    cout << forca_conj_saida[j][conj] << ", " << forca_regra << endl;*/
                    forca_conj_saida[j][conj] = fuzzy_max(forca_conj_saida[j][conj], forca_regra);
                    
                }
                
            }
        }
    }
    
    // Avaliar quanto ficou o final de cada saida
    for (j=0; j<NUMOUTPUTS; j++) {
        if (tipo == SUGENO) {
            saidas[j] = num[j]/den[j];
        } else if (tipo == MAMDANI) {
            agregarSaida(j);
            centroid(j);
        }
    }
    
    //limparVariaveis();
    return saidas;
}

void FuzzySystem::agregarSaida(unsigned char numsaida) {
    Variavel saida = var[NUMINPUTS+numsaida];
    
    float x = saida.min;
    float pedaco = (saida.max - saida.min)/INTERVALO;
    unsigned char i,j;
    
    for (i=0; i<INTERVALO; i++) {
        var_saida[i] = 0;
        
		// Para cada conjunto nesse ponto, verificar qual tem maior forca
		// Roda para cada conjunto
		for (j=0; j<saida.numConjuntos; j++) {
			if (forca_conj_saida[numsaida][j] > 0) {
				var_saida[i] = fuzzy_max(var_saida[i], forca_conj_saida[numsaida][j]*saida.conj[j].membership(x));
			}
        }
        
        if (debug) cout << "Saida " << (int)numsaida << " (x = " << (int)i << ") = " << var_saida[i] << endl;
        
        x += pedaco;
    }
    
}

void FuzzySystem::centroid(unsigned char numsaida) {
    Variavel saida = var[NUMINPUTS+numsaida];

    float x = saida.min;
    float yNom = 0;
    float yDen = 0;
    unsigned char i;
    
    float pedaco = (saida.max - saida.min)/INTERVALO;
    
    for (i=0; i<INTERVALO; i++) {
		yNom += x*var_saida[i];
		yDen += var_saida[i];
		x += pedaco;
	}
	
	saidas[numsaida] = (yDen > 0) ? yNom/yDen : 0;

}