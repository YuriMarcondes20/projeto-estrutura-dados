# Projeto Prático — Estrutura de Dados (Tema A: Redes)

FATEC Ribeirão Preto — ADS, 3º semestre
Docente: Prof. Me. Heber Castro
Autor: Yuri de Souza Marcondes

Aplicação de terminal em C que cadastra **equipamentos de rede** em uma
**BST** (indexada pelo ID) e representa as **conexões** entre eles em um
**grafo** não direcionado e não ponderado (matriz de adjacência com
mapeamento ID → índice, capacidade de 100 vértices).

## Estrutura do repositório

| Pasta/arquivo | Conteúdo |
|---|---|
| `include/` | Interfaces (.h): `registro`, `bst`, `grafo`, `lista`, `fila`, `pilha`, `ordenacao`, `sistema`, `entrada` |
| `src/` | Implementações (.c) e `main.c` (menu) |
| `tests/` | Testes unitários (`teste_*.c`) e cenários do programa completo (`cenarios/`) |
| `docs/` | Relatório técnico (PDF) |

Papel de cada módulo:

- `bst` — cadastro principal: inserir, buscar, remover (0, 1 ou 2 filhos), em-ordem, menor/maior.
- `grafo` — vértices, arestas, BFS e existência de caminho.
- `lista` — lista encadeada com inserção no fim (ordem de descoberta da BFS).
- `fila` — fila encadeada usada na BFS.
- `pilha` — histórico de ações (push, pop, peek e consulta das 5 últimas, sem remover).
- `ordenacao` — Insertion Sort por nome, desempate por ID (sem `qsort`).
- `sistema` — integra BST + grafo + pilha, com rollback.
- `entrada` — leitura segura (`fgets` e `strtol`).

## Compilação

Dentro da pasta do projeto (Windows, com gcc):

```
gcc -Wall -Wextra -std=c99 -Iinclude src/main.c src/sistema.c src/entrada.c src/bst.c src/grafo.c src/lista.c src/fila.c src/pilha.c src/ordenacao.c -o sistema.exe
```

Ou, onde houver `make`: `make` (gera `bin/sistema`).

## Execução

```
sistema.exe
```

## Testes

### Testes unitários (cada um imprime `[OK]`/`[FALHOU]`)

```
gcc -Wall -Wextra -std=c99 -Iinclude tests/teste_estruturas.c src/lista.c src/fila.c src/pilha.c -o teste_estruturas.exe
teste_estruturas.exe

gcc -Wall -Wextra -std=c99 -Iinclude tests/teste_bst.c src/bst.c src/ordenacao.c -o teste_bst.exe
teste_bst.exe

gcc -Wall -Wextra -std=c99 -Iinclude tests/teste_grafo.c src/grafo.c src/lista.c src/fila.c -o teste_grafo.exe
teste_grafo.exe

gcc -Wall -Wextra -std=c99 -Iinclude tests/teste_sistema.c src/sistema.c src/bst.c src/grafo.c src/lista.c src/fila.c src/pilha.c -o teste_sistema.exe
teste_sistema.exe
```

Cobertura: lista, fila e pilha; BST (inserção, duplicado, busca, remoção de
folha / um filho / dois filhos, em-ordem, menor e maior); ordenação (empate
por ID, maiúsculas e minúsculas); grafo (aresta e duplicata invertida, laço,
ID inexistente, remoção de vértice com várias arestas, BFS em ordem
crescente, ciclo, componente desconectado, origem inexistente, caminho,
limite de 100 vértices); integração (cadastro cria vértice, remoção apaga
vértice e arestas, 101º cadastro rejeitado com rollback, falha simulada de
criação do vértice com rollback, histórico só com ações bem-sucedidas).

### Cenários do programa completo (entrada redirecionada)

Cada arquivo `tests/cenarios/N_nome.txt` contém as entradas digitadas no
menu; o `N_nome.saida.txt` correspondente é o resultado esperado.

```
sistema.exe < tests\cenarios\1_fluxo_basico.txt
sistema.exe < tests\cenarios\2_erros_integracao.txt
sistema.exe < tests\cenarios\3_entradas_invalidas.txt
sistema.exe < tests\cenarios\4_ordenacao.txt
sistema.exe < tests\cenarios\5_capacidade_101.txt
```

| Cenário | O que verifica |
|---|---|
| 1 | Fluxo completo: cadastro, busca, listagem, ordenação, conexões, BFS, caminho, histórico, remoção |
| 2 | Duplicado, laço, ID inexistente, conexão/remoção inexistente |
| 3 | ID zero, negativo, letras, lixo após o número, overflow, vazio; texto vazio, só espaços, 80 caracteres (rejeitado) e 79 (aceito); opção de menu inválida |
| 4 | Ordenação com empate textual (desempate por ID) e maiúsculas/minúsculas |
| 5 | 100 cadastros aceitos e 101º rejeitado com rollback |

## Estratégia de consistência (rollback)

- **Cadastro:** 1) insere na BST; 2) cria o vértice com o mesmo ID; 3) empilha
  no histórico. Se a etapa 2 ou 3 falhar (grafo cheio, falha de alocação), as
  etapas anteriores são desfeitas. Nunca fica registro sem vértice.
- **Remoção:** só ocorre se o ID existe; remove o vértice (e suas arestas) e o
  nó da BST. ID inexistente é rejeitado sem alterar nada.
- **Conexões:** se o registro no histórico falhar, a aresta é desfeita.
- O histórico recebe somente operações concluídas com sucesso.

## Declaração de uso de IA

Foi utilizada a ferramenta de IA Claude (Anthropic) como apoio ao estudo e ao
desenvolvimento: explicação dos conceitos (BST, grafo, BFS, pilha, fila),
orientação sobre Git e ambiente de compilação, e geração de código-base dos
módulos e dos testes. O código foi lido, compilado, executado e testado pelo
autor, que é responsável por ele.
