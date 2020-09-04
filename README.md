# Simple C HTTP Server library

Esse repositório contém uma simples implementação de um biblioteca para a criação de servidores HTTP em C. Esse projeto é apenas para fins de estudo e não deve ser usado em uma aplicação real.

## Exemplos

Um servidor é criado a partir da chamada à função `new_http_server`, que recebe como parâmetro um ponteiro para server_t (estrutura que conterá informações sobre o servidor criado), um endereço de IP e uma porta.

```C
server_t server = {0};
new_http_server(&server, "0.0.0.0", "80");
```

Após isso, a variável `server` conterá informações que descrevem seu servidor. O próximo passo será usar a função `handle_route`, que mapeia rotas para suas respectivas funções handlers. Nesses handlers você pode programar toda a resposta que será enviada ao cliente.

```C
// handler do caminho /home
void home(response_t* resp, request_t* req) {
  puts("Função handler para caminho /home do servidor");

  // envia a resposta
  send_http_response(resp, "Olá mundo!");
}

...

// adiciona handler para a rota /home do servidor
handle_route(&server, "/home", home, GET);
```

Ao requisitar o recurso contido em /home, a mensagem "Olá mundo!" será retornada para o cliente. Após configurar os handlers para as rotas, é necessário iniciar o servidor, isto é, colocá-lo em estado de *listening* para esperar por requisições.

```C
// servidor passa a escutar por requisições
start_listening(&server);
```

Por padrão, cada requisição é tratada por um processo separado, ou seja, um `fork` é realizado sempre que uma requisição nova chega. Essa característica do servidor pode ser alterada para que ele use threads, para isso é preciso chamar a função `config_threaded_server` antes de `start_listening`.

```C
// configura o servidor para que utilize threads ao invés de processos
config_threaded_server(&server, 20);
```

O parâmetro *20* especifica o tamanho da fila de conexões que será utilizada internamente pela threads. Toda vez que uma conexão chega, ela é enfileirada para que possa ser pega por uma das threads que esteja disponível na pool de threads do servidor. Por enquanto essa pool é constituída por no máximo 16 threads.

## Tratamento de erros

A maioria das funções dessa biblioteca retornam um inteiro que indica um erro. Caso o retorno seja 0 significa que a função foi executada com sucesso; diferente de zero constitui em um erro. Para que os erros possam ser identificados, foi criada a função `http_error`, que exibe na tela a mensagem padrão para o erro retornado. Exemplo:

```C
int err = 0;
server_t server = {0};

if( (err = new_http_server(&server, "0.0.0.0", "http")) != 0) {
  // ex.: "new_http_server: não foi possível atribuir nenhum endereço ao servidor"
  http_error("new_http_server", err);
  return EXIT_FAILURE;
}
```

## Exemplo completo de código

```C
void home(response_t* resp, request_t* req) {
  add_header_field(&resp->header, "Content-Type", "application/xml");

  int err = send_http_response(resp, "<person><name>Gabriel</name><age>20</age></person>");
  if(err != 0)
    http_error("send_http_response", err);
}

int main() {
  int err = 0;
  server_t server = {0};
  
  if( (err = new_http_server(&server, "0.0.0.0", "http")) != 0) {
    http_error("new_http_server", err);
    return EXIT_FAILURE;
  }

  config_threaded_server(&server, 20);

  if( (err = handle_route(&server, "/home", home, GET)) != 0)
    http_error("handle_route", err);

  if( (err = start_listening(&server)) != 0) {
    http_error("start_listening", err);
    return EXIT_FAILURE;
  }

  return 0;
}
```