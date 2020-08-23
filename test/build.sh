# preguiça de fazer um makefile

mkdir -p ../obj/

gcc -c -Wall ../src/server/server.c -o ../obj/server.o
gcc -c -Wall ../src/util/util.c -o ../obj/util.o
gcc -c -Wall ../src/errors/errors.c -o ../obj/errors.o
gcc -c -Wall ../src/http/request/request.c -o ../obj/request.o
gcc -c -Wall ../src/http/request/methods/methods.c -o ../obj/methods.o
gcc -c -Wall ../src/http/response/status-codes/status_codes.c -o ../obj/status_codes.o
gcc -c -Wall ../src/http/response/response.c -o ../obj/response.o
gcc -c -Wall ../src/http/header/header.c -o ../obj/header.o

gcc main.c ../obj/*.o -o main