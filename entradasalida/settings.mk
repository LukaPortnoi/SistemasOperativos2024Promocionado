# Libraries
LIBS=utils commons pthread readline m

# Custom libraries' paths
STATIC_LIBPATHS=../utils

# Compiler flags
CDEBUG=-g -Wall -DDEBUG -fdiagnostics-color=always
CRELEASE=-O3 -Wall -DNDEBUG -fcommon

# Arguments when executing with start, memcheck or helgrind
#"./cfgs/generica.config", "./cfgs/stdin.config", "./cfgs/stdout.config", "./cfgs/dialfs.config"
ARGS=Interfaz2 ./cfgs/stdout.config

# Valgrind flags 
MEMCHECK_FLAGS=--track-origins=yes
HELGRIND_FLAGS=

