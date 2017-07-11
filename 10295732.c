// *******************************************************
// FEITO POR: JOAO PEDRO RODRIGUES MATTOS
// nUSP: 10295732
// ICMC/USP - 2017
// *******************************************************

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct photo{
  int x; //XDIM
  int y; //YDIM
  int maiorPixel;
  int menorPixel;
  unsigned char** img;
  int imgType;
}photo;

// **********************************************************
// Funcao que aloca a matriz que armazenara' os pixels da imagem
// **********************************************************
unsigned char** alocaPhoto(photo* xyOwner){
  unsigned char **p;
  p = (unsigned char**) malloc(sizeof(unsigned char*) * (*xyOwner).y);
  for (int i = 0; i < (*xyOwner).y; i++){
    p[i] = (unsigned char*) malloc(sizeof(unsigned char) * (*xyOwner).x);
  }
  return p;
}

// **********************************************************
//Funcao que exibe os pixels da imagem no stdout
// **********************************************************
void showPhoto(photo imagem){
  for (int i = 0; i < imagem.y; i++){
    for (int j = 0; j < imagem.x; j++){
      printf("%d\n", imagem.img[i][j]);
    }
  }
}

// **********************************************************
// Funcao que realiza a operacao de logaritmo
// **********************************************************
void operaLog(photo* valueOwner){
  double c = 255 / log10(1 + (double)(*valueOwner).maiorPixel);
  for (int i = 0; i < (*valueOwner).y; i++){
    for (int j = 0; j < (*valueOwner).x; j++){
      (*valueOwner).img[i][j] = c * log10(1 + (double)(*valueOwner).img[i][j]);
    }
  }
}

// **********************************************************
// Funcao que realiza o Contrast Stretching
// **********************************************************
void colorStret(photo* stret){
  for (int i = 0; i < (*stret).y; i++){
    for (int j = 0; j < (*stret).x; j++){
      (*stret).img[i][j] =  (unsigned char)(stret->img[i][j] - stret->menorPixel) * (255) / (double)(stret->maiorPixel - stret->menorPixel);
    }
  }
}

// **********************************************************
// Funcao que carrega o arquivo para dentro do programa.
// OBS: A alocacao e' estatica devido ao fato de o run codes
// nao ter aceitado o envio deste arquivo com a alocacao dinamica(deixei
// a alocacao dinamica comentada dentro da funcao)
// **********************************************************
void* loadFile(){
  // long unsigned int nomeArqLen = 10;
  // char* nomeArq = (char*) malloc(sizeof(char) * (nomeArqLen + 1));
  // getline(&nomeArq, &nomeArqLen, stdin);
  // Para eliminar o '\n' capturado pelo getline()
  // nomeArq[strlen(nomeArq) - 1] = '\0';
  char nomeArq[1000];
  scanf("%s", nomeArq);
  return fopen(nomeArq, "r");
}

// **********************************************************
// Funcao que armazena detalhes do cabecalho da imagem em si
// Variaveis : header[] (o tipo da imagem), comment[](o comentario do cabecalho)
// **********************************************************
void getHeader(FILE* arquivo, photo* headerOwner){
  char header[4];
  char comment[99];
  fgets(header, 4, arquivo);
  (*headerOwner).imgType = header[1] - '0';
  fgets(comment, 99, arquivo);
  fscanf(arquivo, "%d %d", &(*headerOwner).x, &(*headerOwner).y);
  fscanf(arquivo, "%d", &(*headerOwner).maiorPixel);
}

// **********************************************************
// Funcao que coloca dentro da matriz os valores dos pixels
// e que descobre os valores MAX E MIN dos pixels.
// **********************************************************
void preenche(FILE* arquivo, photo* headerOwner){
  if ((*headerOwner).imgType == 2){
    (*headerOwner).menorPixel = (*headerOwner).maiorPixel;
    int truemax = 0;
    int test;
    for (int i = 0; i < (*headerOwner).y; i++){
      for (int j = 0; j < (*headerOwner).x; j++){
        fscanf(arquivo, "%d", &test);
        (*headerOwner).img[i][j] = test;
        if((*headerOwner).img[i][j] < (*headerOwner).menorPixel){
          (*headerOwner).menorPixel = (*headerOwner). img[i][j];
        }
        if((*headerOwner).img[i][j] > truemax){
          truemax = (*headerOwner).img[i][j];
        }
      }
      (*headerOwner).maiorPixel = truemax;
    }
  }else if ((*headerOwner).imgType == 5){
    (*headerOwner).menorPixel = (*headerOwner).maiorPixel;
    unsigned char stream [(*headerOwner).y * (*headerOwner).x];
    fseek(arquivo, 1, SEEK_CUR);
    int truemax = 0;
    fread(stream, sizeof(unsigned char), (*headerOwner).x * (*headerOwner).y, arquivo);
    for (int i = 0; i < (*headerOwner).y; i++){
      for (int j = 0; j < (*headerOwner).x; j++){
        (*headerOwner).img[i][j] = stream[i*((*headerOwner).x) + j];
        if((*headerOwner).img[i][j] < (*headerOwner).menorPixel){
        (*headerOwner).menorPixel = (*headerOwner). img[i][j];
        }
        if((*headerOwner).img[i][j] > truemax){
          truemax = (*headerOwner).img[i][j];
        }
      }
    }
    (*headerOwner).maiorPixel = truemax;
  }
}

// **********************************************************
// Funcao que realiza a recursao para a realizacao do Flood Fill
// **********************************************************
int baldinho(photo* modifica, int y, int x){
  (*modifica).img[y][x] = 100; //Pinta o espaco atual
  // Se disponivel, vai para cima,
  if ((*modifica).img[y - 1][x] != 255 && (*modifica).img[y - 1][x] != 100) baldinho(modifica, y - 1, x);
  // ou para a esquerda,
  if ((*modifica).img[y][x - 1] != 255 && (*modifica).img[y][x - 1] != 100) baldinho(modifica, y, x - 1);
  // ou para baixo,
  if ((*modifica).img[y + 1][x] != 255 && (*modifica).img[y + 1][x] != 100) baldinho(modifica, y + 1, x);
  // ou para a direita.
  if ((*modifica).img[y][x + 1] != 255 && (*modifica).img[y][x + 1] != 100) baldinho(modifica, y, x + 1);
  return 1;
}

// **********************************************************
// Funcao que libera a memoria heap alocada
// **********************************************************
void freeImagem(photo* imagem){
  for (int i = 0; i < imagem->y; i++){
    free(imagem->img[i]);
  }
  free(imagem->img);
}

int main(){
  photo imagem;
  FILE* arq;
  int operacao;
  scanf("%d", &operacao);
  arq = (FILE*) loadFile();
  getHeader(arq, &imagem);
  imagem.img = alocaPhoto(&imagem);
  printf("P2\n# CREATOR: Image Generator SCC-221 - ICC I\n%d %d\n%d\n", imagem.x, imagem.y, imagem.maiorPixel);
  preenche(arq, &imagem);
  if (operacao == 1){
    operaLog(&imagem);
  }else if (operacao == 2){
    colorStret(&imagem);
  }else if (operacao == 3){
    baldinho(&imagem, imagem.y/2, imagem.x/2);
  }

  showPhoto(imagem);
  freeImagem(&imagem);
  fclose(arq);
  return 0;
}
