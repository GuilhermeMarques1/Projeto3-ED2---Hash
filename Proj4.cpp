// NOME: GUILHERME CÂMARA MARQUES RA: 201154031
// NOME: BRUNO MOREIRA RA: 

#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_INSERT 8
#define SIZE_IDLIST 5

typedef struct veiculo {
  char cod_cli[3];
  char cod_vei[3];
  char client[50];
  char veiculo[50];
  char dias[4];
} veic_t;

typedef struct remove {
  char cod_cli[3];
  char cod_vei[3];
} reg_id_t;

void clearBuffer() {    
  while ( getchar() != '\n' );
}

int loadFiles(veic_t *regs_locs_vei, reg_id_t *regs_id_list) {
  FILE *insere, *busca;

  if ((insere = fopen("insere.bin", "rb")) == NULL) {
    printf("Nao foi possivel abrir o arquivo insere.bin \n");
    return 0;
  }

  if((busca = fopen("busca.bin", "rb")) == NULL) {
    printf("Nao foi possivel abrir o arquivo busca.bin \n");
    return 0;
  }

  for(int i=0; i<SIZE_INSERT; i++) {
    fread(&(regs_locs_vei[i]), sizeof(veic_t), 1, insere);
  }

  for(int i=0; i<SIZE_IDLIST; i++) {
    fread(&(regs_id_list[i]), sizeof(reg_id_t), 1 , busca);
  }

  fclose(insere);
  fclose(busca);
  return 1;
}

void insert(FILE *data, veic_t *regs_locs_vei) {
  int option, reg_size;
  char insert_register[124], ch;

  printf("\n=================================\n");
  printf("Digite o numero da opcao que deseja inserir:\n\n");

  for(int i=0;i<SIZE_INSERT;i++) {
    printf("%d- ", i+1);
    if(!(strcmp(regs_locs_vei[i].cod_cli, "***"))) { //Evita listar registros ja inseridos
      printf("***\n"); 
      continue;
    }
    printf("%s|", regs_locs_vei[i].cod_cli);
    printf("%s|", regs_locs_vei[i].cod_vei);
    printf("%s|", regs_locs_vei[i].client);
    printf("%s|", regs_locs_vei[i].veiculo);
    printf("%s| \n", regs_locs_vei[i].dias);
  }
  scanf("%d", &option);
  clearBuffer();

  while((option < 1 || option > SIZE_INSERT) || !(strcmp(regs_locs_vei[option-1].cod_cli, "***"))) { //verifica se a opcao e valida
    printf("Opcao invalida, por favor digite novamente: ");
    scanf("%d", &option);
    clearBuffer();
  }
  option--;

  sprintf(insert_register, "%s|%s|%s|%s|%s|", regs_locs_vei[option].cod_cli, regs_locs_vei[option].cod_vei, regs_locs_vei[option].client, regs_locs_vei[option].veiculo, regs_locs_vei[option].dias);
  reg_size = strlen(insert_register);

  while(fread(&ch, sizeof(char), 1, data)); //posiciona no fim do arquivo
  fwrite(&reg_size, sizeof(char), 1, data);
  fwrite(insert_register, sizeof(char), reg_size, data); //insere o registro no fim do arquivo

  strcpy(regs_locs_vei[option].cod_cli, "***"); //adiciona marcador no campo cod_cli para indicar que o registro ja foi inserido
  return;
}

int main() {
  veic_t regs_locs_vei[SIZE_INSERT]; //armazena os registros para serem inseridos
  reg_id_t regs_id_list[SIZE_IDLIST]; //armazena os registros para serem buscados
  FILE *data;
  int option;

  if(!(loadFiles(regs_locs_vei, regs_id_list))) { // chama loadFiles para carregar na memoria os arquivos insere.bin e busca.bin
    printf("Falha ao carregar os arquivos");
    return 0;
  }

  if(!(data = fopen("data.bin", "rb"))) { //Verifica se o arquivo de dados já existe
    if((data = fopen("data.bin", "w+b")) == NULL) { //Se nao existir será criado
      printf("Nao foi possivel criar o arquivo data.bin");
      return 0;
    }
  } else { //E se ja existir, vai abrir o arquivo para escrita e leitura
    fclose(data);
    if((data = fopen("data.bin", "r+b")) == NULL) {
      printf("Nao foi possivel abrir o arquivo data.bin");
      return 0;
    }
  }

  for(int i=0; i<SIZE_INSERT; i++) {
    printf("%s|%s|%s|%s|%s \n", regs_locs_vei[i].cod_cli, regs_locs_vei[i].cod_vei, regs_locs_vei[i].client, regs_locs_vei[i].veiculo, regs_locs_vei[i].dias);
  }

  printf("\n");

  for(int i=0; i<SIZE_IDLIST; i++) {
    printf("%s|%s \n", regs_id_list[i].cod_cli, regs_id_list[i].cod_vei);
  }

  //Menu de opções:
  do {
    printf("=================================\n");
    printf("1- Inserir registro\n");
    printf("2- Listar os dados de clientes\n");
    printf("3- Buscar por chave primaria\n");
    printf("4- Sair\n");
    printf("=================================\n");
    scanf("%d", &option);
    clearBuffer();

    switch(option) {
      case 1: {
        insert(data, regs_locs_vei); //Insere novo registro no fim do arquivo original e atualiza o index
        break;
      }
      case 2: {
        printf("Listar os dados\n");
        break;
      }
      case 3: {
        printf("Pesquisa por chave primaria\n");
        break;
      }
      case 4: {
        printf("Saindo...\n");
        break;
      }
      default:
        printf("Opcao invalida, digite novamente\n\n");
        break;
    }

  } while(option != 4);


  fclose(data);
  return 1;
}