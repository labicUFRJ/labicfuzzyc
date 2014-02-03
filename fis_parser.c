// FIS Parser
//
// COMPILANDO / COMPILING
// gcc fis_parser.c iniparser/dictionary.c iniparser/iniparser.c iniparser/strlib.c -o fis_parser
//
// EXECUTANDO / RUNNING
// ./fis_parser input.fis output.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "iniparser/iniparser.h"

#define MAMDANI 1
#define SUGENO 2

int arduino = 0;

int main(int argc, char* argv[]) {
    FILE *fsaidapde, *ffis;
    char nomefis[50];
    char nomeoutput[50];
    char arqsaida[100];
    //char nomefis[] = "caminhao_anfis.fis";
    char* s = NULL;
    int ninputs, noutputs, nrules, type;
    int i, j;
    int inp = 1; //eh input ou output
    int nmfs;
    char vname[128];
    char mfname[128];
    char mfbuf[256];
    char *ormethod = NULL;
    char *andmethod = NULL;
    char* nomevariavel = NULL;
    float min, max;
    int nvar = 0;
    
    //printf("\033[2J\033[1;1H");
    puts("===============================================");
    puts("           LabicFuzzyC: FIS Parser");
    puts("   Laboratório de Inteligência Computacional");
    puts("       UFRJ - Rio de Janeiro - Brasil");
    puts("       http://www.labic.nce.ufrj.br/");
    puts("===============================================");
    puts("");
    
    if (argc < 3) {
        printf("-> Faltando argumentos de entrada. Modo de uso:\n%s INPUT.fis OUTPUT.cpp\n", argv[0]);
        return 1;
    } else {
        strcpy(nomefis, argv[1]);
        printf("-> Abrindo arquivo '%s'...\n", nomefis);
        strcpy(arqsaida, argv[2]);
    }
    
    ffis = fopen(nomefis, "r");
    
    if (ffis == NULL) {
        printf("ERRO: Não pode carregar o arquivo fis especificado.\n");
        return 666;
    }
    
    fsaidapde = fopen(arqsaida, "w");
    
    if (fsaidapde == NULL) {
        printf("Nao foi possivel abrir o arquivo para gravacao.\n");
        return 10;
    }
    
    //system
    dictionary* fis = iniparser_load(nomefis);
    
    puts("1. Arquivo FIS carregado!");
    
    // Lendo e definindo configuracoes do sistema
    puts("2. Definindo constantes e configuracoes do sistema...");
    
    // Verifico qual o tipo do sistema, mamdani ou sugeno
    s = iniparser_getstr(fis, "System:Type");
    if (!strcmp(s, "mamdani")) {
        type = MAMDANI;
    } else if (!strcmp(s, "sugeno")) {
        type = SUGENO;
    } else {
        printf("ERRO: Tipo de sistema fuzzy nao suportado.\n");
        return 666;
    }
    
    ninputs  = iniparser_getint(fis, "System:NumInputs",  -1); if(ninputs  == -1) printf("Invalid system nInputs.\n");
    noutputs = iniparser_getint(fis, "System:NumOutputs", -1); if(noutputs == -1) printf("Invalid system nOutputs.\n");
    nrules    = iniparser_getint(fis, "System:NumRules",    -1); if(nrules    == -1) printf("Invalid system numRules.\n");
    
    // Verifica se os metodos sao suportados
    andmethod = iniparser_getstr(fis, "System:AndMethod");
    if(strcmp(andmethod, "min") && strcmp(andmethod, "prod")) {
        printf("ERRO: Essa versao do sistema so funciona com AndMethod do tipo min ou prod.\n");
    }
    
    ormethod = iniparser_getstr(fis, "System:OrMethod");
    if(strcmp(ormethod, "max")) printf("ERRO: Essa versao do sistema so funciona com OrMethod do tipo max.\n");
    
    if (type == MAMDANI) {
        s = iniparser_getstr(fis, "System:ImpMethod");
        if(strcmp(s, "min")) printf("ERRO: Essa versao do sistema mamdani so funciona com ImpMethod do tipo min.\n");
        
        s = iniparser_getstr(fis, "System:AggMethod");
        if(strcmp(s, "max")) printf("ERRO: Essa versao do sistema mamdani so funciona com AggMethod do tipo max.\n");
    }
    
    s = iniparser_getstr(fis, "System:DefuzzMethod");
    if(strcmp(s, "centroid") && strcmp(s, "mamdani")) {
        printf("ERRO: Essa versao do sistema so funciona com DefuzzMethod do tipo centroid ou wtaver.\n");
    }
    
    
    // Le dados das entradas e saidas
    
    puts("3. Lendo variaveis e conjuntos...");
    
    fprintf(fsaidapde, "// A funcao rodarMeuFis abrigara os dados gerados\n// pelo parser e recebera as entradas e a variavel\n// que guardara as saidas. Eh apenas um atalho para\n// a funcao rodarSistema para guardar as regras do\n// FIS, sem ter que adiciona-las a funcao principal\n// (main/setup/loop...)\n\n");
    fprintf(fsaidapde, "// Nao deve ser alterado!\n\n// Exemplo de uso:\n// float entrada[%d];\n", ninputs);
    
    for (i=0; i<ninputs; i++) {
        fprintf(fsaidapde, "// entrada[%d] = %d;\n", i, i*10);
    }
    
    fprintf(fsaidapde, "// float *saida = rodarMeuFis(entrada);\n\n#include \"FuzzySystem.h\"\n\nfloat* rodarMeuFis(float *x) {\n\n");
    
    fprintf(fsaidapde, "    Variavel var[%d];\n    Regra regras[%d];\n    unsigned char i;\n\n", ninputs+noutputs, nrules);
    fprintf(fsaidapde, "    for (i=0;i<%d;i++) {\n    regras[i].conjEntrada = (unsigned char*) malloc(%d*sizeof(unsigned char));\n    regras[i].conjSaida = (unsigned char*)   malloc(%d*sizeof(unsigned char));\n    }\n\n", nrules, ninputs, noutputs);
    
    // Para cada variavel...
    for (i=0; ; i++) {
        if(inp && i >= ninputs) { //finished reading inputs, read outputs now
            i = 0;
            inp = 0;
        }
        else if(!inp && i >= noutputs) break;
        
        // Le nome da variavel e salva em 'nomevariavel'
        sprintf(vname, (inp ? "Input%d:Name" : "Output%d:Name"), i + 1);
        nomevariavel = iniparser_getstr(fis, vname);
        fprintf(fsaidapde, "\n    // Variavel %s\n", nomevariavel);
        // Le numero de cojuntos
        sprintf(vname, (inp ? "Input%d:NumMFs" : "Output%d:NumMFs"), i + 1);
        nmfs = iniparser_getint(fis, vname,  -1); if(nmfs == -1) printf("ERRO: Numero invalido de MFs de variavel.\n");
        // Le o range da variavel
        sprintf(vname, (inp ? "Input%d:Range" : "Output%d:Range"), i + 1);
        s = iniparser_getstr(fis, vname);
        sscanf(s, " [ %f %f ]", &min, &max);
        
        /*
         fprintf(fsaidapde, "    sistema.var[%d].nome = \"%s\";\n", nvar, nomevariavel);
         fprintf(fsaidapde, "    sistema.var[%d].min = %f;\n", nvar, min);
         fprintf(fsaidapde, "    sistema.var[%d].max = %f;\n", nvar, max);
         fprintf(fsaidapde, "    sistema.var[%d].numConjuntos = %d;\n", nvar, nmfs);
         */
        fprintf(fsaidapde, "    var[%d].min = %f;\n", nvar, min);
        fprintf(fsaidapde, "    var[%d].max = %f;\n", nvar, max);
        fprintf(fsaidapde, "    var[%d].numConjuntos = %d;\n", nvar, nmfs);
        
        if (!inp && type == SUGENO) {
            fprintf(fsaidapde, "    var[%d].linear_conj = (LinearOutput *) malloc(%d*sizeof(LinearOutput));\n", nvar, nmfs);
            fprintf(fsaidapde, "    for (i=0;i<%d;i++) { var[%d].linear_conj[i].c = (float *) malloc((%d)*sizeof(float)); }\n", nmfs, nvar, ninputs+1);
        } else {
            fprintf(fsaidapde, "    var[%d].conj = (Conjunto *) malloc(%d*sizeof(Conjunto));\n", nvar, nmfs);
        }
        
        // Para cada conjunto da variavel sendo lida...
        for(j = 0; j < nmfs; ++j) {
            
            sprintf(vname, (inp ? "Input%d:MF%d" : "Output%d:MF%d"), i + 1, j + 1);
            s = iniparser_getstr(fis, vname);
            // Le o tipo de conjunto
            if(sscanf(s, " '%*[^\']' : '%[^\']'", mfbuf) != 1) printf("ERRO: Tipo invalido de MF ou nao pode ser lido.\n");
            // Le o nome do conjunto
            sscanf(s, " '%[^\']'", mfname);
            
            // Se for um triangulo
            if(!strcmp(mfbuf, "trimf")) {
                double a, b, c;
                
                if(sscanf(s, " '%*[^\']' : '%*[^\']' , [ %lf %lf %lf ]", &a, &b, &c) != 3) printf("Nao pode ler todos os pontos do triangulo.\n");
                
                fprintf(fsaidapde, "    // Conjunto triangulo %s\n", mfname);
                fprintf(fsaidapde, "    var[%d].conj[%d].tipo = CONJ_T;\n", nvar, j);
                fprintf(fsaidapde, "    var[%d].conj[%d].a = %f;\n", nvar, j, a);
                fprintf(fsaidapde, "    var[%d].conj[%d].b = %f;\n", nvar, j, b);
                fprintf(fsaidapde, "    var[%d].conj[%d].c = %f;\n", nvar, j, b);
                fprintf(fsaidapde, "    var[%d].conj[%d].d = %f;\n", nvar, j, c);
                
            }
            // Se for um trapezio
            else if(!strcmp(mfbuf, "trapmf")) {
                double a, b, c, d;
                
                if(sscanf(s, " '%*[^\']' : '%*[^\']' , [ %lf %lf %lf %lf ]", &a, &b, &c, &d) != 4) printf("Nao pode ler todos os pontos do trapezio\n");
                
                fprintf(fsaidapde, "    // Conjunto trapezio %s\n", mfname);
                fprintf(fsaidapde, "    var[%d].conj[%d].tipo = CONJ_T;\n", nvar, j);
                fprintf(fsaidapde, "    var[%d].conj[%d].a = %f;\n", nvar, j, a);
                fprintf(fsaidapde, "    var[%d].conj[%d].b = %f;\n", nvar, j, b);
                fprintf(fsaidapde, "    var[%d].conj[%d].c = %f;\n", nvar, j, c);
                fprintf(fsaidapde, "    var[%d].conj[%d].d = %f;\n", nvar, j, d);
            }
            // Se for algum tipo do sugeno
            else if(!strcmp(mfbuf, "gbellmf")) {
                double a, b, c;
                
                if(sscanf(s, " '%*[^\']' : '%*[^\']' , [ %lf %lf %lf ]", &a, &b, &c) != 3) printf("Nao pode ler todos os pontos do conjunto gbellmf\n");
                
                fprintf(fsaidapde, "    // Conjunto gbellmf %s\n", mfname);
                fprintf(fsaidapde, "    var[%d].conj[%d].tipo = CONJ_GBELLMF;\n", nvar, j);
                fprintf(fsaidapde, "    var[%d].conj[%d].a = %f;\n", nvar, j, a);
                fprintf(fsaidapde, "    var[%d].conj[%d].b = %f;\n", nvar, j, b);
                fprintf(fsaidapde, "    var[%d].conj[%d].c = %f;\n", nvar, j, c);
                
            }
            else if(!strcmp(mfbuf, "linear")) {
                double a, b, c;
                
                if(sscanf(s, " '%*[^\']' : '%*[^\']' , [ %lf %lf %lf ]", &a, &b, &c) != 3) printf("Nao pode ler todos os pontos do conjunto linear\n");
                fprintf(fsaidapde, "    // Conjunto linear %s\n", mfname);
                fprintf(fsaidapde, "    var[%d].linear_conj[%d].c[0] = %f;\n", nvar, j, a);
                fprintf(fsaidapde, "    var[%d].linear_conj[%d].c[1] = %f;\n", nvar, j, b);
                fprintf(fsaidapde, "    var[%d].linear_conj[%d].c[2] = %f;\n", nvar, j, c);
            }
            else printf("ERRO: Tipo invalido de MF. Esperado: trimf/trapmf/gbellmf/linear\n");
        }
        nvar++;
    }
    
    // Fechar dicionario iniParser
    iniparser_freedict(fis);
    
    // Ler regras
    // TODO cada regra pode ter um numero diferente de condicoes!!
    puts("4. Lendo regras...");
    fprintf(fsaidapde, "\n    // Regras\n");
    char* tk;
    int mf;
    char buf[128];
    //int type;
    
    // Procura no arquivo a parte de regras...
    while(1)
    {
        if(fgets(buf, sizeof(buf), ffis) == NULL) printf("Nao foi possivel encontrar a secao de regras no FIS.");
        if(!strcmp(buf, "[Rules]\n")) break;
    }
    
    i = 0;
    
    while(fgets(buf, sizeof(buf), ffis) != NULL){
        tk = strtok(buf, " ,():");
        
        for (j=0; j<ninputs; j++) {
            if(tk == NULL) printf("Erro lendo regras.");
            
            sscanf(tk, "%d", &mf);
            fprintf(fsaidapde, "    regras[%d].conjEntrada[%d] = %d;\n", i, j, mf);
            
            tk = strtok(NULL, " ,():");
        }
        
        for (j=0; j<noutputs; j++) {
            if(tk == NULL) printf("Erro lendo regras.");
            
            sscanf(tk, "%d", &mf);
            fprintf(fsaidapde, "    regras[%d].conjSaida[%d] = %d;\n", i, j, mf);
            
            tk = strtok(NULL, " ,():");
        }
        
        i++;
    }
    
    //fprintf(fsaidapde, "\n    rodarSistema(x, y, sistema);\n}");
    fprintf(fsaidapde, "\n    FuzzySystem fuzzy(");
    if (type == SUGENO) {
        fprintf(fsaidapde, "SUGENO, ");
    } else {
        fprintf(fsaidapde, "MAMDANI, ");
    }
    
    for (i=0;i<strlen(ormethod);i++) {
        ormethod[i] = toupper(ormethod[i]);
    }
    for (i=0;i<strlen(andmethod);i++) {
        andmethod[i] = toupper(andmethod[i]);
    }
    
    fprintf(fsaidapde, "ORMETHOD_%s, ANDMETHOD_%s, %d, %d, %d, var, regras);\n", ormethod, andmethod, ninputs, noutputs, nrules);
    fprintf(fsaidapde, "    return fuzzy.rodarSistema(x);\n\n}");
    fclose(ffis);
    fclose(fsaidapde);
    
    puts("5. Concluido.\n");
    printf("-> PARSER CONCLUÍDO COM SUCESSO\nO arquivo gerado encontra-se em '%s'.\n\n", arqsaida);
    
    return 0;
}

