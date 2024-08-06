# TP Sistemas Operativos - 1er Cuatrimestre 2024

### Simulación de un sistema operativo distribuido en 4 módulos
- CPU
- Kernel
- Memoria
- Entradasalida (interfaces E/S y File System)

### Features
- Algoritmos de planificacion: FIFO, Round Robin y Virtual Round Robin
- Memoria con esquema de Paginación Simple
- TLB
- File System de Asignación Contigua
- Interfaces de Input, Output y Generica

## Como ejecutar

### Requerimientos
- Entorno Unix (preferentemente alguna version de Ubuntu, ya que fue el entorno provisto por la catedra y el utilizado para su desarrollo)
- Libreria commons utnso - https://github.com/sisoputnfrba/so-commons-library

Realizar build de cada modulo (utilizar makefile) y ejecutar en el siguiente orden: Memoria - CPU - Kernel - interfaces </br>
Nota: Chequear direcciones IP en los configs si se levantan diferentes computadoras

Se recomienda utilizar la siguiente herramienta provista por la catedra, que facilita el proceso de deployment: https://github.com/sisoputnfrba/so-deploy

## Links útiles
- [Enunciado](https://docs.google.com/document/d/1-AqFTroovEMcA1BfC2rriB5jsLE6SUa4mbcAox1rPec/edit?usp=sharing)
- [Documento de pruebas](https://docs.google.com/document/d/1XsBsJynoN5A9PTsTEaZsj0q3zsEtcnLgdAHOQ4f_4-g/edit)
- [VM de la cátedra](https://docs.utnso.com.ar/recursos/vms)
