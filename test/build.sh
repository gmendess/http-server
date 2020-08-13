# preguiça de fazer um makefile

mkdir -p ../obj/

gcc -c ../src/server/server.c -o ../obj/server.o
gcc -c ../src/util/util.c -o ../obj/util.o
gcc -c ../src/http/request/request.c -o ../obj/request.o

gcc main.c ../obj/*.o -o main