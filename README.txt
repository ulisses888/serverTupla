Para o servercpp.cpp:

Compilação:
g++ -std=c++17 servidorcpp.cpp -pthread -o servidorcpp
Execução:
./servidorcpp

Fiz a compilação usando o WSL.
O servidor utiliza a porta TCP 54321 em uma constante definida logo apos os includes.
Para o ip: 127.0.0.1

Os serviços disponíveis são os mesmos usados como exemplo na especificação do projeto final:
Serviço 1: converte o valor para letras maiúsculas.
Serviço 2: inverte a string.
Serviço 3: retorna o tamanho da string como texto.

Após a conexão, os comandos podem ser enviados diretamente pelo terminal.

Exemplo de inserção de tupla:
WR teste1 valor1
Resposta esperada:
OK

Exemplo de leitura:
RD teste1
Resposta esperada:
OK valor1

Exemplo de remoção:
IN teste1
Resposta esperada:
OK valor1

Exemplo de execução de serviço:
WR chave abcdef
EX chave novaChave 1
RD novaChave
Resposta esperada:
OK ABCDEF

Caso seja informado um serviço inexistente:
EX chave outraChave 99
Resposta esperada:
NO-SERVICE

Para o serverRust.rs:

