# LabicFuzzyC
LabicFuzzyC é uma biblioteca de **Lógica Nebulosa** para permitir a utilização de sistemas de inferência fuzzy gerados pelo MATLAB em projetos em C++.

A biblioteca foi desenvolvida com o objetivo de ser usada em um **Arduino**, então teve como foco de desenvolvimento ser o mais leve possível a fim de caber na memória de um.

- Desenvolvido no Laboratório de Inteligência Computacional - NCE/UFRJ ([http://www.labic.nce.ufrj.br](http://www.labic.nce.ufrj.br))
- Desenvolvedor: Mario Cecchi ([http://meriw.com](http://meriw.com))
- Orientação: Adriano Joaquim de Oliveira Cruz ([http://equipe.nce.ufrj.br/adriano](http://equipe.nce.ufrj.br/adriano))

## Objetivo
A biblioteca converte um sistema fuzzy do MATLAB, especificado em um arquivo de extensão FIS, para uma simples função que recebe como parâmetro as entradas e retorna a saída do sistema fuzzy, que pode ser chamada em qualquer lugar no seu projeto.


## Convertendo seu sistema
Para usar a biblioteca, você precisa fazer apenas duas coisas: executar o *parser*, que fará a conversão do seu sistema, e copiar os arquivos necessários para dentro do seu projeto.

Primeiro, é necessário compilar o parser. Se você está num sistema *UNIX*, o arquivo script `compile.sh` faz isso:

	git clone https://github.com/macecchi/labicfuzzyc.git
	cd labicfuzzyc
	./compile.sh
	
Uma vez compilado o parser, o script `bundle.sh` se encarrega de converter seu sistema e exportar os arquivos necessários, bastando fornecer a ele o endereço do seu sistema FIS. Exemplo:

	./bundle.sh examples/caminhao.fis

Se executado com sucesso, no final você terá gerado uma pasta `copy`, contendo 3 arquivos necessários para o seu projeto: `FuzzySystem.h`, `FuzzySystem.cpp` e `meufis.cpp`.

## Usando em seu projeto
Copie o conteúdo da pasta `copy` para o seu projeto. O arquivo `meufis.cpp` contém a função com seu sistema, e os demais as funções para os cálculos fuzzy.

Para executar o sistema no seu projeto, use a função `rodarMeuFis`. **As instruções de uso estão dentro do próprio arquivo que contém essa função, `meufis.cpp`.**

Observe um exemplo de um sistema fuzzy de duas entradas e uma saída:

	float entrada[2];
	entrada[0] = 0;
	entrada[1] = 10;
	float *saida = rodarMeuFis(entrada);
	printf("A saída foi %f\n", saida[0]);
	
## Suporte e copyright
O projeto adota a licença GPL, podendo ser modificado e distrubuído contanto que a licença seja mantida.

Caso necessite de suporte, por favor entre em contato conosco através do e-mail macecchi@gmail.com ou através de um bug report no GitHub.

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
