# TPE_ARQUI

Trabajo practico especial para Arquitectura de Computadoras en el segundo cuatrimestre de 2022

# para crear el contenedor

docker pull agodio/itba-so:1.0
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name capos agodio/itba-so:1.0

# luego ejecutar

chmod +x compile.sh
./compile.sh
