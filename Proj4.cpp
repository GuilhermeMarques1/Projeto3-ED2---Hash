// NOME: GUILHERME CÂMARA MARQUES RA: 201154031
// NOME: BRUNO MOREIRA RA: 

#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_INSERT 8
#define SIZE_IDLIST 5
#define SIZE_HASHING 13

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

typedef struct hash {
  char cod_cli[3];
  char cod_vei[3];
  int rrn;
} hash_st;

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

void initialize_index_hash(FILE *index) {
  hash_st key;

  strcpy(key.cod_cli, "##");
  strcpy(key.cod_vei, "##");
  key.rrn = -1;

  for(int i=0; i<SIZE_HASHING; i++) {
    fwrite(&key, sizeof(hash_st), 1, index);
  }

  return;
}

int hash_function(char hashId[6]) {
  int address = atoi(hashId) % SIZE_HASHING;

  return address;
}

void update_index(FILE *index, char cod_cli[3], char cod_vei[3]) {
  hash_st key, verifyKey;
  char hashId[6];
  int address;

  strcpy(key.cod_cli, cod_cli);
  strcpy(key.cod_vei, cod_vei);

  sprintf(hashId, "%s%s", cod_cli, cod_vei);
  address = hash_function(hashId);
  printf("Endereco: %d \n", address);

  key.rrn = address;

  fseek(index, sizeof(hash_st)*address, SEEK_SET);
  fread(&verifyKey, sizeof(hash_st), 1, index);

  if(!strcmp(verifyKey.cod_cli, "##") && !strcmp(verifyKey.cod_vei, "##")) { //Endereço livre
    fseek(index, sizeof(hash_st)*address, SEEK_SET);
    fwrite(&key, sizeof(hash_st), 1, index);
    printf("Chave inserida no endereco: %d\n", address);
  } else { //Overflow Progressivo
    int tried=0;
    printf("Colisao \n");
    
    for(int i=address+1; i<SIZE_HASHING; i++) {
      fseek(index, sizeof(hash_st)*i, SEEK_SET);
      fread(&verifyKey, sizeof(hash_st), 1, index);

      tried++;
      printf("Tentativa %d->%d\n", tried, i);

      if(!strcmp(verifyKey.cod_cli, "##") && !strcmp(verifyKey.cod_vei, "##")) {
        key.rrn = i;
        fseek(index, sizeof(hash_st)*i, SEEK_SET);
        fwrite(&key, sizeof(hash_st), 1, index);
        printf("Chave inserida no endereco: %d\n", i);
        return;
      }
    }

    for(int i=0; i<address; i++) {
      fseek(index, sizeof(hash_st)*i, SEEK_SET);
      fread(&verifyKey, sizeof(hash_st), 1, index);

      tried++;
      printf("Tentativa %d->%d\n", tried, i);

      if(!strcmp(verifyKey.cod_cli, "##") && !strcmp(verifyKey.cod_vei, "##")) {
        key.rrn = i;
        fseek(index, sizeof(hash_st)*i, SEEK_SET);
        fwrite(&key, sizeof(hash_st), 1, index);
        printf("Chave inserida no endereco: %d\n", i);
        return;
      }
    }
  }

  return;
}

void insert(FILE *data, FILE *index, veic_t *regs_locs_vei) {
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

  update_index(index, regs_locs_vei[option].cod_cli, regs_locs_vei[option].cod_vei);

  strcpy(regs_locs_vei[option].cod_cli, "***"); //adiciona marcador no campo cod_cli para indicar que o registro ja foi inserido
  return;
}

int main() {
  veic_t regs_locs_vei[SIZE_INSERT]; //armazena os registros para serem inseridos
  reg_id_t regs_id_list[SIZE_IDLIST]; //armazena os registros para serem buscados
  FILE *data, *index;
  int option;

  if(!(loadFiles(regs_locs_vei, regs_id_list))) { // chama loadFiles para carregar na memoria os arquivos insere.bin e busca.bin
    printf("Falha ao carregar os arquivos");
    return 0;
  }


  //Arquivo de dados (armazena o registros e seus campos):
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


  //Arquivo index (hashing) para facilitar o acesso aos registros no arquivo de dados:
  if(!(index = fopen("index.bin", "rb"))) { //Verifica se o arquivo index já existe
    if((index = fopen("index.bin", "w+b")) == NULL) { //Se nao existir será criado
      printf("Nao foi possivel criar o arquivo index.bin");
      return 0;
    }
    initialize_index_hash(index); //Inicializa o arquivo de index

  } else { //E se ja existir, vai abrir o arquivo para escrita e leitura
    fclose(index);
    if((index = fopen("index.bin", "r+b")) == NULL) {
      printf("Nao foi possivel abrir o arquivo index.bin");
      return 0;
    }
  }


  //Menu de opções:
  do {
    printf("=================================\n");
    printf("1- Inserir registro\n");
    printf("2- Buscar por chave primaria\n");
    printf("3- Sair\n");
    printf("=================================\n");
    scanf("%d", &option);
    clearBuffer();

    switch(option) {
      case 1: {
        insert(data, index, regs_locs_vei); //Insere novo registro no fim do arquivo original e atualiza o index
        break;
      }
      case 2: {
        printf("Pesquisa por chave primaria\n");
        break;
      }
      case 3: {
        printf("Saindo...\n");
        break;
      }
      default:
        printf("Opcao invalida, digite novamente\n\n");
        break;
    }

  } while(option != 3);


  fclose(data);
  fclose(index);
  return 1;
}