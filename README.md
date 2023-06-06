# tp2so
This is a project for Operating Systems 1Q2023

## Dependencies

* **C Compiler:** [GCC]

* **Static Code Analysis Tools:** [Cppcheck](http://cppcheck.sourceforge.net/) and [PVS-Studio](https://pvs-studio.com/en/pvs-studio/)

## Static Code Analysis
To execute the static code analysis tools `Cppcheck` and `PVS-Studio` run:
```
$> make check
```

# para crear el contenedor

docker pull agodio/itba-so:1.0
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name capos agodio/itba-so:1.0

## PARA COMPILAR AUTOMATICAMENTE
# luego ejecutar

chmod +x compile.sh
./compile.sh


## PARA COMPILAR MANUALMENTE
# Environment setup:
1- Install the following packages before building the Toolchain and Kernel:

nasm qemu gcc make

# 2- Build the Toolchain

Execute the following commands on the x64BareBones project directory:

  user@linux:$ cd Toolchain
  user@linux:$ make all

# 3- Build the Kernel

From the x64BareBones project directory run:

  user@linux:$ make all

# 4- Run the kernel

From the x64BareBones project directory run:

  user@linux:$ ./run.sh



