#!/bin/bash

if [ $# -lt 1 ]
then
echo "Usage: $0 <input FIS file>"
exit -1
fi

mkdir -p copy;
cp evaluator/* copy;
./fis_parser "${@:1}" copy/meufis.cpp

echo "-> ARQUIVOS GERADOS COM SUCESSO\nVoce deverá ter 3 arquivos dentro da pasta 'copy'. Copie-os para seu projeto e siga as instruções de como usá-los.\n";
