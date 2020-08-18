# preguiça de fazer um makefile

mkdir -p ../obj/

gcc -c ../src/server/server.c -o ../obj/server.o
gcc -c ../src/util/util.c -o ../obj/util.o
gcc -c ../src/http/request/request.c -o ../obj/request.o
gcc -c ../src/http/methods/methods.c -o ../obj/methods.o
gcc -c ../src/http/response/status-codes/status_codes.c -o ../obj/status_codes.o
gcc -c ../src/http/header/header.c -o ../obj/header.o

gcc main.c ../obj/*.o -o main