#include "funcionalidades.h"

// a funcao recebe ponteiro de arquivo e printa suas informacoes da forma formatada
// representa a funcionalidade (2) do trabalho
void select_from(FILE *arquivo)
{
    // alocamos a memoria do registro temporario e do cabecalho
    Registro *registro_temporario = (Registro *)malloc(sizeof(Registro));
    Cabecalho *cabecalho = (Cabecalho *)malloc(sizeof(Cabecalho));

    // primeiro abrimos o arquivo e contamos o numero de registros
    int tamanho = tamanho_bytes(arquivo);

    // lemos o cabecalho do arquivo
    le_cabecalho(cabecalho, arquivo);

    // pegamos as informacoes e printamos
    for (int offset = 1600; offset < tamanho; offset += 160)
    {
        le_registro(registro_temporario, arquivo, offset);
        if (!removido(registro_temporario))
        {
            printa_formatado(registro_temporario);
            printf("numero de %s eh %ld\n", registro_temporario->nome.valor, converteNome(registro_temporario->nome.valor));
            printf("\n");
        }
    }

    // printamos numero de paginas
    printf("Numero de paginas de disco: %d\n\n", cabecalho->nroPagDisco);

    // por fim libera o registro temporario
    libera_registro(registro_temporario);
    free(cabecalho);
}

// a funcao recebe um arquivo e pede o comando "where" do SQL com o numero n de
// criterios e depois o nome do campo e seu valor, que devem vir do usuario.
// A funcao printa na tela os registros que conterem os campos com os valores
// pedidos.
// select_where representa a funcionalidade (3) do trabalho
void select_where(FILE* arquivo)
{
    // vamos primeiro receber a fila
    // precisamos receber o n para saber quantos criterios de selecao vao ter
    int n;
    scanf(" %d", &n);
    // depois precisamos receber a entrada e separar a string nos espacos
    char entrada[25*n];
    fgets(entrada, sizeof(entrada), stdin);
    // vamos criar uma fila
    Fila *fila = cria_fila();
    // vamos tokenizar separando pelos espacos " "
    No *no = (No *)malloc(sizeof(No));
    no->campo = strtok(entrada, " ");
    no->valor = strtok(NULL, " ");
    poe_na_fila(fila, no);
    for (int i = 1; i < n; i++)
    {
        No *no = (No *)malloc(sizeof(No));
        no->campo = strtok(NULL, " ");
        no->valor = strtok(NULL, " ");
        poe_na_fila(fila, no);
    }

    // agora vamos nos preparar para ler os dados do arquivo
    // alocamos a memoria do cabecalho
    Cabecalho *cabecalho = (Cabecalho *)malloc(sizeof(Cabecalho));
    // lemos o cabecalho do arquivo
    le_cabecalho(cabecalho, arquivo);
  
    // iteramos sobre cada busca 
    int campo;
    for (int i = 0; i < n; i++)
    {
        printf("Busca %d\n", i+1);
        // vamos receber o numero do campo para conseguir fazer a busca
        campo = int_campo(fila->cabeca->campo);
        // se encontramos algo printamos
        busca_e_printa(arquivo, campo, fila->cabeca->valor);
        // a cabeca da fila se torna o proximo da antiga cabeca e liberamos aquele
        // no
        proximo_na_fila(fila);
        // printamos numero de paginas
        printf("\nNumero de paginas de disco: %d\n\n", cabecalho->nroPagDisco);
    }

    // precisamos liberar o cabecalho
    free(cabecalho);
}

// a funcao recebe um arquivo e executa a funcionalidade (4) do trabalho, que eh
// a remocao logica de registros
// Para isso, a funcao vai perguntar quantas buscas e remocoes serao feitas e
// depois os campos e valores como especificado pelo trabalho
void remocao_logica(FILE *arquivo)
{
    // perceba que aqui, no fundamental, apenas recebemos os criterios de remocao
    // o que faz o codigo ser igual ao do select_where porque a forma como estamos
    // recebendo eh igual, a unica mudanca eh o busca_e_ que passa a ser retira 
    // ao inves de printa, o que tambem faz a formatacao da saida ser diferente

    // vamos primeiro receber a fila
    // precisamos receber o n para saber quantos criterios de selecao vao ter
    int n;
    scanf(" %d", &n);
    // depois precisamos receber a entrada e separar a string nos espacos
    char entrada[25*n];
    fgets(entrada, sizeof(entrada), stdin);
    // vamos criar uma fila
    Fila *fila = cria_fila();
    // vamos tokenizar separando pelos espacos " "
    No *no = (No *)malloc(sizeof(No));
    no->campo = strtok(entrada, " ");   // trocar por strsep dps, da pau por causa do csv
    no->valor = strtok(NULL, " ");
    poe_na_fila(fila, no);
    for (int i = 1; i < n; i++)
    {
        No *no = (No *)malloc(sizeof(No));
        no->campo = strtok(NULL, " ");
        no->valor = strtok(NULL, " ");
        poe_na_fila(fila, no);
    }

    // agora vamos nos preparar para ler os dados do arquivo
    // alocamos a memoria do cabecalho
    Cabecalho *cabecalho = (Cabecalho *)malloc(sizeof(Cabecalho));
    // lemos o cabecalho do arquivo
    le_cabecalho(cabecalho, arquivo);
  
    // iteramos sobre cada busca 
    int campo;
    for (int i = 0; i < n; i++)
    {
        // vamos receber o numero do campo para conseguir fazer a busca
        campo = int_campo(fila->cabeca->campo);
        // AQUI QUE O CODIGO DEIXA DE SER IGUAL AO DO SELECT_WHERE
        busca_e_retira(arquivo, cabecalho, campo, fila->cabeca->valor);
        // a cabeca da fila se torna o proximo da antiga cabeca e liberamos aquele
        // no
        proximo_na_fila(fila);
    }

    // precisamos liberar o cabecalho
    free(cabecalho);
}

// representa a funcionalidade (7) do trabalho
void create_index(FILE *arquivo_dados)
{
    // recebemos o nome do arquivo de indice
    char nome_indice[20];
    scanf(" %s", nome_indice);
    // vamos abrir o arquivo de indice e criar um cabecalho
    // como o cabecalho vai ser alterado a medida que inserimos
    // novos nos, ele vai ser escrito apenas no final da 
    // funcionalidade
    FILE *arquivo_indice = fopen(nome_indice, "r+");
    Cabecalho_indice *cabecalho = cria_cabecalho_indice();

    // primeiro abrimos o arquivo e contamos o numero de registros
    int tamanho = tamanho_bytes(arquivo_dados);
    // alocamos a memoria do registro temporario e do cabecalho
    Registro *registro_temporario = (Registro *)malloc(sizeof(Registro));
    // iteramos sobre o arquivo de dados, inserindo os indices
    for (long offset = 1600; offset < tamanho; offset += 160)
    {
        //insere_arvore_b(arquivo_indice, cabecalho, teste[i], -1);
        le_registro(registro_temporario, arquivo_dados, offset);
        if (!removido(registro_temporario))
        {
            // depois tem que inserir o nome do dinossauro no lugar de teste de novo
            insere_arvore_b(arquivo_indice, cabecalho, converteNome(registro_temporario->nome.valor), offset);
        }
    }
    // apos escrever os nos, escrevemos o cabecalho
    escreve_cabecalho_indice(arquivo_indice, cabecalho);

    // por fim, liberamos cabecalho e fechamos arquivo
    free(cabecalho);
    fclose(arquivo_indice);

    // por fim, chamamos o binario na tela
    binarioNaTela(nome_indice);
}


void executa_busca_nome(FILE *dados) {
    char nome[50];
    char nome_arquivo_indice[50];

    // Leitura do nome do arquivo de índice
    scanf("%s", nome_arquivo_indice);

    // Abrir o arquivo de índice
    FILE *indice = fopen(nome_arquivo_indice, "rb");
    if (indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Leitura do nome a ser buscado (ignora a palavra "nome")
    scanf("%s", nome);
    scan_quote_string(nome);

    // Converter o nome para long usando a função fornecida
    long chaveBusca = converteNome(nome);

    if (chaveBusca == -1) {
        printf("Registro inexistente.\n");
        fclose(indice);
        return;
    }

    // Ler o cabeçalho do índice para obter o RRN da raiz
    Cabecalho_indice cabecalho;
    le_cabecalho_indice(indice, &cabecalho);
    int rrnRaiz = cabecalho.noRaiz;


    // Declaração da estrutura do nó
    No_indice no_atual;

    // Realizar a busca na árvore-B
    int PR = buscarIndiceArvore(indice, rrnRaiz, chaveBusca);

    if (PR == -1) {
        printf("Registro inexistente.\n");
    } else {
        // Usar posicao_encontrada para buscar o registro no arquivo de dados
        Registro registro;
        le_registro(&registro, dados, PR);
        if (registro.removido == '1') {
            printf("Registro inexistente.\n");
        } else {
            printa_formatado(&registro);
        }
    }

    // Fechar o arquivo de índice
    fclose(indice);
}

