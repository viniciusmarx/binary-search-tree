#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PALAVRAS 500

typedef struct Palavra
{
  char texto[30];
  struct Palavra *esq, *dir;
  int ocorrencias;
  int altura;
} Palavra;

typedef struct
{
  char letra;
  Palavra *lista;
} Alfabeto;

typedef struct
{
  int maiorOcorrencia;
  char palavras[MAX_PALAVRAS][30];
  int total;
} ResultadoOcorrencias;

void iniciaAlfabeto(Alfabeto *alfabeto)
{
  for (int i = 0; i < 26; i++)
  {
    alfabeto[i].letra = 'a' + i;
    alfabeto[i].lista = NULL;
  }
}

int altura(Palavra *raiz)
{
  return (raiz == NULL) ? 0 : raiz->altura;
}

int atualizarAltura(Palavra *raiz)
{
  if (raiz == NULL)
    return 0;

  int altEsq = altura(raiz->esq);
  int altDir = altura(raiz->dir);
  return (altEsq > altDir ? altEsq : altDir) + 1;
}

int fatorBalanceamento(Palavra *raiz)
{
  if (raiz == NULL)
    return 0;

  return altura(raiz->esq) - altura(raiz->dir);
}

Palavra *rotacaoDireita(Palavra *raizDesbalanceada)
{
  Palavra *novaRaiz = raizDesbalanceada->esq;
  Palavra *subArvoreTemporaria = novaRaiz->dir;

  novaRaiz->dir = raizDesbalanceada;
  raizDesbalanceada->esq = subArvoreTemporaria;

  raizDesbalanceada->altura = atualizarAltura(raizDesbalanceada);
  novaRaiz->altura = atualizarAltura(novaRaiz);

  return novaRaiz;
}

Palavra *rotacaoEsquerda(Palavra *raizDesbalanceada)
{
  Palavra *novaRaiz = raizDesbalanceada->dir;
  Palavra *subArvoreTemporaria = novaRaiz->esq;

  novaRaiz->esq = raizDesbalanceada;
  raizDesbalanceada->dir = subArvoreTemporaria;

  raizDesbalanceada->altura = atualizarAltura(raizDesbalanceada);
  novaRaiz->altura = atualizarAltura(novaRaiz);

  return novaRaiz;
}

Palavra *criarPalavra(char *palavra)
{
  Palavra *novaPalavra = (Palavra *)malloc(sizeof(Palavra));
  strcpy(novaPalavra->texto, palavra);
  novaPalavra->esq = novaPalavra->dir = NULL;
  novaPalavra->ocorrencias = 1;
  novaPalavra->altura = 1;
  return novaPalavra;
}

Palavra *inserePalavraAVL(Palavra *raiz, char *palavra)
{
  if (raiz == NULL)
    return criarPalavra(palavra);

  int cmp = strcmp(palavra, raiz->texto);

  if (cmp == 0)
  {
    raiz->ocorrencias++;
    return raiz;
  }

  if (cmp < 0)
    raiz->esq = inserePalavraAVL(raiz->esq, palavra);
  else
    raiz->dir = inserePalavraAVL(raiz->dir, palavra);

  raiz->altura = atualizarAltura(raiz);

  int fb = fatorBalanceamento(raiz);

  if (fb > 1 && strcmp(palavra, raiz->esq->texto) < 0)
    return rotacaoDireita(raiz);

  // Caso 2: Rotação simples à esquerda
  if (fb < -1 && strcmp(palavra, raiz->dir->texto) > 0)
    return rotacaoEsquerda(raiz);

  // Caso 3: Rotação dupla esquerda-direita
  if (fb > 1 && strcmp(palavra, raiz->esq->texto) > 0)
  {
    raiz->esq = rotacaoEsquerda(raiz->esq);
    return rotacaoDireita(raiz);
  }

  // Caso 4: Rotação dupla direita-esquerda
  if (fb < -1 && strcmp(palavra, raiz->dir->texto) < 0)
  {
    raiz->dir = rotacaoDireita(raiz->dir);
    return rotacaoEsquerda(raiz);
  }
  return raiz;
}

int obterIndiceLetra(char *palavra)
{
  if (palavra == NULL || palavra[0] == '\0')
    return -1;

  char letra = palavra[0];

  if (letra < 'a' || letra > 'z')
    return -1;

  return letra - 'a';
}

void inserirNoAlfabeto(Alfabeto *alfabeto, char *palavra)
{
  if (palavra == NULL || palavra[0] == '\n')
    return;

  char primeiraLetra = palavra[0];
  if (primeiraLetra < 'a' || primeiraLetra > 'z')
  {
    printf("Palavra inválida\n");
    return;
  }

  int indice = primeiraLetra - 'a';
  alfabeto[indice].lista = inserePalavraAVL(alfabeto[indice].lista, palavra);
}

Palavra *encontrarMenor(Palavra *raiz)
{
  Palavra *atual = raiz;

  while (atual && atual->esq != NULL)
    atual = atual->esq;

  return atual;
}

Palavra *removerPalavraAVL(Palavra *raiz, char *palavra)
{
  if (raiz == NULL)
    return NULL;

  int cmp = strcmp(palavra, raiz->texto);

  if (cmp < 0)
    raiz->esq = removerPalavraAVL(raiz->esq, palavra);
  else if (cmp > 0)
    raiz->dir = removerPalavraAVL(raiz->dir, palavra);
  else
  {
    if (raiz->ocorrencias > 1)
    {
      raiz->ocorrencias--;
      return raiz;
    }

    // caso 1: sem filhos
    if (raiz->esq == NULL && raiz->dir == NULL)
    {
      free(raiz);
      return NULL;
    }
    // caso 2: um filho
    else if (raiz->esq == NULL || raiz->dir == NULL)
    {
      Palavra *temp = (raiz->esq) ? raiz->esq : raiz->dir;
      free(raiz);
      return temp;
    }
    // cas0 3: dois filhos
    else
    {
      Palavra *substituto = encontrarMenor(raiz->dir);
      strcpy(raiz->texto, substituto->texto);
      raiz->ocorrencias = 1;
      raiz->dir = removerPalavraAVL(raiz->dir, substituto->texto);
    }
  }

  raiz->altura = atualizarAltura(raiz);
  int fb = fatorBalanceamento(raiz);

  // Casos de rotação (iguais à inserção)
  if (fb > 1 && fatorBalanceamento(raiz->esq) >= 0)
    return rotacaoDireita(raiz);

  if (fb > 1 && fatorBalanceamento(raiz->esq) < 0)
  {
    raiz->esq = rotacaoEsquerda(raiz->esq);
    return rotacaoDireita(raiz);
  }

  if (fb < -1 && fatorBalanceamento(raiz->dir) <= 0)
    return rotacaoEsquerda(raiz);

  if (fb < -1 && fatorBalanceamento(raiz->dir) > 0)
  {
    raiz->dir = rotacaoDireita(raiz->dir);
    return rotacaoEsquerda(raiz);
  }

  return raiz;
}

void removerPalavraAlfabeto(Alfabeto *alfabeto, char *Palavra)
{
  int indice = obterIndiceLetra(Palavra);
  if (indice == -1)
  {
    printf("Palavra invalida\n");
    return;
  }
  alfabeto[indice].lista = removerPalavraAVL(alfabeto[indice].lista, Palavra);
}

void exibirPalavrasEmOrdem(Palavra *raiz)
{
  if (raiz != NULL)
  {
    exibirPalavrasEmOrdem(raiz->esq);
    printf("  Palavra: %-20s | Ocorrências: %d\n", raiz->texto, raiz->ocorrencias);
    exibirPalavrasEmOrdem(raiz->dir);
  }
}

void mostrarAlfabetoCompleto(Alfabeto *alfabeto)
{
  for (int i = 0; i < 26; i++)
  {
    if (alfabeto[i].lista != NULL)
    {
      printf("\n Letra: %c\n", alfabeto[i].letra);
      exibirPalavrasEmOrdem(alfabeto[i].lista);
    }
  }
}

int consultarOcorrencias(Palavra *raiz, char *palavra)
{
  if (raiz == NULL)
    return 0;

  int cmp = strcmp(palavra, raiz->texto);

  if (cmp == 0)
    return raiz->ocorrencias;
  else if (cmp < 0)
    return consultarOcorrencias(raiz->esq, palavra);
  else
    return consultarOcorrencias(raiz->dir, palavra);
}

void consultarOcorrenciasAlfabeto(Alfabeto *alfabeto, char *palavra)
{
  int indice = obterIndiceLetra(palavra);
  if (indice == -1)
  {
    printf("Palavra invalida.\n");
    return;
  }

  int ocorrencias = consultarOcorrencias(alfabeto[indice].lista, palavra);

  if (ocorrencias == 0)
    printf("Palavra %s nao encontrada\n", palavra);
  else
    printf("Palavra %s encontrada com %d ocorrencia(s)\n", palavra, ocorrencias);
}

int contarPalavrasAVL(Palavra *raiz)
{
  if (raiz == NULL)
    return 0;

  return 1 + contarPalavrasAVL(raiz->esq) + contarPalavrasAVL(raiz->dir);
}

int contarPalavrasTotais(Alfabeto *alfabeto)
{
  int total = 0;
  for (int i = 0; i < 26; i++)
  {
    total += contarPalavrasAVL(alfabeto[i].lista);
  }
  return total;
}

int contarOcorrenciasAVL(Palavra *raiz)
{
  if (raiz == NULL)
    return 0;

  return raiz->ocorrencias + contarOcorrenciasAVL(raiz->esq) + contarOcorrenciasAVL(raiz->dir);
}

int contarOcorrenciasTotais(Alfabeto *alfabeto)
{
  int total = 0;
  for (int i = 0; i < 26; i++)
  {
    total += contarOcorrenciasAVL(alfabeto[i].lista);
  }
  return total;
}

void exibirPalavrasPorLetra(Alfabeto *alfabeto, char letra)
{
  if (!isalpha(letra))
  {
    printf("Letra invalida");
    return;
  }

  letra = tolower(letra);
  int indice = letra - 'a';

  if (alfabeto[indice].lista == NULL)
  {
    printf("Nenhuma palavra registrada com a letra '%c'\n", letra);
    return;
  }

  printf("\n Palavras com a letra '%c'\n", letra);
  exibirPalavrasEmOrdem(alfabeto[indice].lista);
}

void buscarMaiorOcorrencia(Palavra *raiz, ResultadoOcorrencias *resultado)
{
  if (raiz == NULL)
    return;

  // visita esquerda
  buscarMaiorOcorrencia(raiz->esq, resultado);

  // verifica nó atual
  if (raiz->ocorrencias > resultado->maiorOcorrencia)
  {
    resultado->maiorOcorrencia = raiz->ocorrencias;
    resultado->total = 0;
    strcpy(resultado->palavras[resultado->total++], raiz->texto);
  }
  else if (raiz->ocorrencias == resultado->maiorOcorrencia)
  {
    if (resultado->total < MAX_PALAVRAS)
      strcpy(resultado->palavras[resultado->total++], raiz->texto);
  }

  // visita direita
  buscarMaiorOcorrencia(raiz->dir, resultado);
}

void exibirPalarasMaiorOcorrencia(Alfabeto *alfabeto)
{
  ResultadoOcorrencias resultado;
  resultado.maiorOcorrencia = 0;
  resultado.total = 0;

  for (int i = 0; i < 26; i++)
  {
    buscarMaiorOcorrencia(alfabeto[i].lista, &resultado);
  }

  if (resultado.total == 0)
  {
    printf("Nenhuma palavra cadastrada\n");
    return;
  }

  printf("\nMaior numero de ocorrencias: %d\n", resultado.maiorOcorrencia);
  printf("Palavras:\n");
  for (int i = 0; i < resultado.total; i++)
  {
    printf(" - %s\n", resultado.palavras[i]);
  }
}

void exibirPalavrasUmaOcorrenciaAVL(Palavra *raiz)
{
  if (raiz == NULL)
    return;

  exibirPalavrasUmaOcorrenciaAVL(raiz->esq);

  if (raiz->ocorrencias == 1)
    printf(" - %s\n", raiz->texto);

  exibirPalavrasUmaOcorrenciaAVL(raiz->dir);
}

void exibirPalavrasUmaOcorrencia(Alfabeto *alfabeto)
{
  printf("\nPalavras com apenas uma ocorrencia:\n");

  for (int i = 0; i < 26; i++)
  {
    if (alfabeto[i].lista != NULL)
    {
      exibirPalavrasUmaOcorrenciaAVL(alfabeto[i].lista);
    }
  }
}

void menu(Alfabeto *alfabeto)
{
  int opcao;
  char palavra[30];

  do
  {
    printf("\nMENU\n");
    printf("1. Inserir palavra\n");
    printf("2. Consultar palavra\n");
    printf("3. Remover palavra\n");
    printf("4. Retorna o numero total de palavras unicas da estrutura\n");
    printf("5. Retorna o numero total de ocorrencias (todas insercoes)\n");
    printf("6. Exibe a lista de todas as palavras da estrutura\n");
    printf("7. Exibe a lista de palavras iniciadas por uma determinada letra\n");
    printf("8. Exibe a palavra com maior numero de ocorrencias\n");
    printf("9. Exibe as palavras que so tem uma ocorrencia\n");
    printf("10. Sair\n");
    scanf("%d", &opcao);
    getchar();

    switch (opcao)
    {
    case 1:
      printf("Digite a palavra: ");
      scanf("%s", palavra);
      inserirNoAlfabeto(alfabeto, palavra);
      break;

    case 2:
      printf("Digite a palavra para consultar: ");
      scanf("%s", palavra);
      consultarOcorrenciasAlfabeto(alfabeto, palavra);
      break;

    case 3:
      printf("Digite a palavra para remover: ");
      scanf("%s", palavra);
      removerPalavraAlfabeto(alfabeto, palavra);
      break;

    case 4:
      printf("Total de palavras unicas: %d\n", contarPalavrasTotais(alfabeto));
      break;

    case 5:
      printf("Total de ocorrencias (todas insercoes): %d\n", contarOcorrenciasTotais(alfabeto));
      break;

    case 6:
      mostrarAlfabetoCompleto(alfabeto);
      break;

    case 7:
    {
      char letra;
      printf("Digite a letra para exibir as palavras: ");
      scanf("%c", &letra);
      exibirPalavrasPorLetra(alfabeto, letra);
      break;
    }

    case 8:
      exibirPalarasMaiorOcorrencia(alfabeto);
      break;

    case 9:
      exibirPalavrasUmaOcorrencia(alfabeto);
      break;

    case 10:
      printf("Encerrando...\n");
      break;

    default:
      printf("Opção inválida\n");
    }
  } while (opcao != 10);
}

void strToLower(char *palavra)
{
  for (int i = 0; palavra[i]; i++)
  {
    palavra[i] = tolower((unsigned char)palavra[i]);
  }
}

void inputInicial(Alfabeto *alfabeto)
{
  FILE *file = fopen("../input.txt", "r");
  if (!file)
  {
    printf("Erro ao abrir o arquivo");
    return;
  }

  char palavra[50];
  while (fscanf(file, "%49s", palavra) == 1)
  {
    strToLower(palavra);

    inserirNoAlfabeto(alfabeto, palavra);
  }
  fclose(file);
  printf("Palavras do arquivo foram inseridas com sucesso.\n");
  ;
}

int main()
{
  Alfabeto alfabeto[26];
  iniciaAlfabeto(alfabeto);
  inputInicial(alfabeto);

  menu(alfabeto);

  return 0;
}