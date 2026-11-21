#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#define T 2 //Grau minimo
#define MAX (2*T-1) //Numero maximo de chaves
#define MIN (T-1) //Numero minimo de chav

//--------------Criação e liberação da arvore B e dos nós-----------------

//Estrutura de um nó da árvore B
typedef struct node {
    int n; //Numero de chaves
    int keys[MAX]; //Chaves
    struct node *children[MAX+1]; //Filhos
    bool leaf; //Indica se é folha
} Node;

typedef struct bTree {
    Node *root; //Raiz da árvore
} BTree;

//Função para criar um novo nó caso seja folha ou não
Node* createNode(bool leaf) {
    //Aloca memória para o novo nó
    Node* newNode = (Node*)malloc(sizeof(Node));
    
    if(newNode){
        //Inicializa o número de chaves e o indicador de folha
        newNode->n = 0;
        newNode->leaf = leaf;

        //Inicializa os ponteiros para os filhos como NULL
        for (int i = 0; i < MAX + 1; i++) {
        newNode->children[i] = NULL;
        }
    }
    //Retorna o novo nó criado
    return newNode;
}

//Função para criar uma árvore B
BTree* createBTree() {
    //Aloca memória para a árvore B
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    
    if(tree){
        //Cria a raiz da árvore como uma folha
        tree->root = createNode(true);
    }

    //Retorna a árvore B criada
    return tree;
}

//Função para liberar a memória de um nó
void NodeFree(Node* node) {
    free(node);
}

//Função para liberar a memória da árvore B recursivamente
void NodeFreeRec(Node* node) {
    if (node != NULL) {
        //Libera recursivamente os filhos do nó (folhas não têm filhos)
        if (!node->leaf) {
            for (int i = 0; i <= node->n; i++) {
                NodeFreeRec(node->children[i]);
            }
        }
        //Libera a memória do nó atual
        NodeFree(node);
    }
}

//Função para liberar a memória da árvore B
void BTreeFree(BTree* tree) {
    if (tree != NULL) {
        //Libera recursivamente a raiz da árvore
        NodeFreeRec(tree->root);
        //Libera a memória da estrutura da árvore
        free(tree);
    }
}

//-----------------Funções auxiliares para inspeção e remoção-----------------

//Função para buscar uma chave em um nó da árvore B
Node* searchNode(Node* node, int key) {
    int i = 0;
    //Encontra a posição da chave no nó
    while (i < node->n && key > node->keys[i]) {
        i++;
    }

    //Se a chave for encontrada, retorna o nó
    if (i < node->n && key == node->keys[i]) {
        return node;
    }

    //Se o nó for folha, a chave não está presente
    if (node->leaf) {
        return NULL;
    }

    //Chama recursivamente a função de busca no filho apropriado
    return searchNode(node->children[i], key);
}

//Função para dividir um nó filho y de x na posição i
void splitChild(Node* x, int i, Node* y) {
    //Cria um novo nó para armazenar as chaves do filho
    Node* z = createNode(y->leaf);
    z->n = MIN; //Número mínimo de chaves

    //Copia as últimas MIN chaves do filho para o novo nó
    for (int j = 0; j < MIN; j++) {
        z->keys[j] = y->keys[j + T];
    }

    //Se o filho não for folha, copia os filhos correspondentes
    if (!y->leaf) {
        for (int j = 0; j < T; j++) {
            z->children[j] = y->children[j + T];
        }
    }

    //Reduz o número de chaves do filho original
    y->n = MIN;

    //Desloca os filhos do pai para abrir espaço para o novo filho
    for (int j = x->n; j >= i + 1; j--) {
        x->children[j + 1] = x->children[j];
    }
    x->children[i + 1] = z;

    //Desloca as chaves do pai para abrir espaço para a chave do filho
    for (int j = x->n - 1; j >= i; j--) {
        x->keys[j + 1] = x->keys[j];
    }
    
    //Move a chave do meio do filho para o pai
    x->keys[i] = y->keys[MIN];
    
    //Incrementa o número de chaves do pai
    x->n++;
}

//Função para inserir uma chave em um nó que não está cheio
void insertNonFull(Node* x, int k) {
    int i = x->n - 1;

    //Se o nó for folha, insere a chave diretamente
    if (x->leaf) {
        //Desloca as chaves para abrir espaço para a nova chave
        while (i >= 0 && k < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        //Insere a nova chave
        x->keys[i + 1] = k;
        x->n++;
    } else {
        //Se o nó não for folha, encontra o filho apropriado para inserir a chave
        while (i >= 0 && k < x->keys[i]) {
            i--;
        }
        i++;

        //Se o filho estiver cheio, divide-o
        if (x->children[i]->n == MAX) {
            splitChild(x, i, x->children[i]);
            //Após dividir, decide qual dos dois filhos deve receber a nova chave
            if (k > x->keys[i]) {
                i++;
            }
        }
        //Chama recursivamente a função de inserção no filho apropriado
        insertNonFull(x->children[i], k);
    }
}

//Função para inserir uma chave na árvore B
void Btree_insert(BTree* tree, int k) {
    Node* root = tree->root;

    if (root->n == MAX) {
        //Se a raiz estiver cheia, cria um novo nó como raiz
        Node* s = createNode(false);
        tree->root = s;//A nova raiz é um nó interno
        s->children[0] = root;//O filho 0 da nova raiz é a antiga raiz
        s->n = 0;//A nova raiz ainda não tem chaves
        s->leaf = false;//A nova raiz não é folha
        s->keys[0] = root->keys[MIN];//A chave do meio da antiga raiz sobe para a nova raiz

        //Divide a antiga raiz e insere a chave no filho apropriado
        splitChild(s, 0, root);
        insertNonFull(s, k);
    } else {
        //Se a raiz não estiver cheia, insere a chave diretamente
        insertNonFull(root, k);
    }
}

//Retorna a maior chave da subárvore com raiz em x (predecessor, caso 2a)
int getPredecessor(Node* x) {
    while (!x->leaf) {
        x = x->children[x->n]; //desce sempre pelo filho mais à direita
    }
    return x->keys[x->n - 1];
}

//Retorna a menor chave da subárvore com raiz em x (sucessor, caso 2b)
int getSuccessor(Node* x) {
    while (!x->leaf) {
        x = x->children[0]; //desce sempre pelo filho mais à esquerda
    }
    return x->keys[0];
}

//Função para fundir dois filhos de um nó x na posição i
void mergeChildren(Node* x, int i) {
    //Pega os dois filhos a serem fundidos
    Node* y = x->children[i];
    Node* z = x->children[i + 1];

    //A chave de x desce para y e vira a chave mediana (posição T-1 = MIN)
    y->keys[MIN] = x->keys[i];

    //Copia as chaves de z para y
    for (int j = 0; j < z->n; j++) {
        y->keys[j + T] = z->keys[j];
    }

    //Se não forem folhas, copia também os filhos de z para y
    if (!y->leaf) {
        for (int j = 0; j <= z->n; j++) {
            y->children[j + T] = z->children[j];
        }
    }

    //y agora tem 2T-1 chaves (nó cheio)
    y->n = MAX;

    //Fecha o buraco em x: remove a chave i e o ponteiro para o filho i+1
    for (int j = i + 1; j < x->n; j++) {
        x->keys[j - 1] = x->keys[j];
    }
    for (int j = i + 2; j <= x->n; j++) {
        x->children[j - 1] = x->children[j];
    }
    x->children[x->n] = NULL; //ponteiro que sobrou (não usado mais)
    x->n--;

    //Libera o nó z (já foi absorvido por y)
    NodeFree(z);
}

//Caso 3a (irmão esquerdo): x.ci pega uma chave extra emprestada do irmão esquerdo
void borrowFromPrev(Node* x, int i) {
    Node* child = x->children[i];
    Node* sibling = x->children[i - 1];

    //Abre espaço no início de child
    for (int j = child->n - 1; j >= 0; j--) {
        child->keys[j + 1] = child->keys[j];
    }
    if (!child->leaf) {
        for (int j = child->n; j >= 0; j--) {
            child->children[j + 1] = child->children[j];
        }
    }

    //A chave de x desce para child
    child->keys[0] = x->keys[i - 1];

    //O último filho do irmão passa para child
    if (!child->leaf) {
        child->children[0] = sibling->children[sibling->n];
        sibling->children[sibling->n] = NULL;
    }

    //A última chave do irmão sobe para x
    x->keys[i - 1] = sibling->keys[sibling->n - 1];

    child->n++;
    sibling->n--;
}

//Caso 3a (irmão direito): x.ci pega uma chave extra emprestada do irmão direito
void borrowFromNext(Node* x, int i) {
    Node* child = x->children[i];
    Node* sibling = x->children[i + 1];

    //A chave de x desce para o fim de child
    child->keys[child->n] = x->keys[i];

    //O primeiro filho do irmão passa para o fim de child
    if (!child->leaf) {
        child->children[child->n + 1] = sibling->children[0];
    }

    //A primeira chave do irmão sobe para x
    x->keys[i] = sibling->keys[0];

    //Fecha o buraco no irmão
    for (int j = 1; j < sibling->n; j++) {
        sibling->keys[j - 1] = sibling->keys[j];
    }
    if (!sibling->leaf) {
        for (int j = 1; j <= sibling->n; j++) {
            sibling->children[j - 1] = sibling->children[j];
        }
        sibling->children[sibling->n] = NULL;
    }

    child->n++;
    sibling->n--;
}

/*
 * Caso 3: garante que o filho i de x tenha pelo menos T chaves antes de descer.
 *   3a: se algum irmão imediato tem >= T chaves, empresta uma chave;
 *   3b: senão, junta o filho com um irmão (descendo uma chave de x).
 * Retorna o índice do filho para o qual a recursão deve descer
 * (muda quando o último filho é fundido com o irmão da esquerda).
 */

int fillChild(Node* x, int i) {
    if (i > 0 && x->children[i - 1]->n >= T) {
        borrowFromPrev(x, i);              //3a (irmão esquerdo)
    } else if (i < x->n && x->children[i + 1]->n >= T) {
        borrowFromNext(x, i);              //3a (irmão direito)
    } else if (i < x->n) {
        mergeChildren(x, i);               //3b (com o irmão direito)
    } else {
        mergeChildren(x, i - 1);           //3b (com o irmão esquerdo)
        i--;                               //o nó fundido agora está em i-1
    }
    return i;
}

/*
 * B-TREE-DELETE: elimina a chave k da subárvore com raiz em x.
 * Retorna true se a chave foi encontrada e removida, false caso contrário.
 * Pré-condição: x tem pelo menos T chaves, ou x é a raiz.
 */
bool deleteFromNode(Node* x, int k) {
    //Encontra o menor índice i tal que k <= x.chave[i]
    int i = 0;
    while (i < x->n && k > x->keys[i]) {
        i++;
    }

    if (i < x->n && x->keys[i] == k) {
        //A chave k está no nó x
        if (x->leaf) {
            //Caso 1: x é folha, apenas remove k
            for (int j = i + 1; j < x->n; j++) {
                x->keys[j - 1] = x->keys[j];
            }
            x->n--;
            return true;
        }

        //Caso 2: x é nó interno
        Node* y = x->children[i];     //filho que precede k
        Node* z = x->children[i + 1]; //filho que segue k

        if (y->n >= T) {
            //Caso 2a: substitui k pelo predecessor e o elimina de y
            int pred = getPredecessor(y);
            x->keys[i] = pred;
            return deleteFromNode(y, pred);
        } else if (z->n >= T) {
            //Caso 2b: substitui k pelo sucessor e o elimina de z
            int succ = getSuccessor(z);
            x->keys[i] = succ;
            return deleteFromNode(z, succ);
        } else {
            //Caso 2c: y e z têm T-1 chaves; junta k e z em y, depois elimina k de y
            mergeChildren(x, i);
            return deleteFromNode(y, k);
        }
    }

    //A chave k não está em x
    if (x->leaf) {
        return false; //chegou em uma folha sem achar k: k não está na árvore
    }

    //Caso 3: k, se existir, está na subárvore x.ci
    if (x->children[i]->n < T) {
        i = fillChild(x, i); //3a ou 3b, para descer em um nó com >= T chaves
    }
    return deleteFromNode(x->children[i], k);
}

//Remove a chave k da árvore. Retorna true se removeu, false se k não existe.
bool Btree_delete(BTree* tree, int k) {
    bool removed = deleteFromNode(tree->root, k);

    //Se a raiz ficou interna e sem chaves (casos 2c/3b), seu único filho
    //vira a nova raiz e a altura da árvore diminui em uma unidade.
    Node* r = tree->root;
    if (r->n == 0 && !r->leaf) {
        tree->root = r->children[0];
        NodeFree(r);
    }
    return removed;
}

//-----------------Funções auxiliarespara testes ----------

//Retorna o número total de chaves da subárvore com raiz em x
int Btree_size(Node* x) {
    if (x == NULL) {
        return 0;
    }

    //Conta as chaves do próprio nó
    int total = x->n;

    //Soma as chaves de cada um dos n+1 filhos (folhas não têm filhos)
    if (!x->leaf) {
        for (int i = 0; i <= x->n; i++) {
            total += Btree_size(x->children[i]);
        }
    }
    return total;
}

//Imprime as chaves da subárvore com raiz em x em ordem crescente (em-ordem)
void Btree_printRec(Node* x) {
    if (x == NULL) {
        return;
    }

    for (int i = 0; i < x->n; i++) {
        //Imprime primeiro tudo o que é menor que keys[i]
        if (!x->leaf) {
            Btree_printRec(x->children[i]);
        }
        printf("%02d ", x->keys[i]);
    }

    //Por último, o filho mais à direita (maiores que keys[n-1])
    if (!x->leaf) {
        Btree_printRec(x->children[x->n]);
    }
}

//Imprime a árvore em uma linha em ordem crescente
void Btree_print(BTree* tree) {
    printf("BTree: [ ");
    Btree_printRec(tree->root);
    printf("]\n");
}

//Guarda em arr as chaves da subárvore com raiz em x, em ordem crescente.
//*index é a próxima posição livre de arr.
void Btree_store(Node* x, int* arr, int* index) {
    if (x == NULL) {
        return;
    }

    for (int i = 0; i < x->n; i++) {
        //Primeiro tudo o que é menor que keys[i]
        if (!x->leaf) {
            Btree_store(x->children[i], arr, index);
        }
        arr[(*index)++] = x->keys[i];
    }

    //Por último, o filho mais à direita
    if (!x->leaf) {
        Btree_store(x->children[x->n], arr, index);
    }
}

//Comparador para qsort (não depende da árvore, então serve igual)
int int_comp(const void* a, const void* b) {
    int x = *(const int*)a;
    int y = *(const int*)b;
    return (x > y) - (x < y);
}

//-----------------Funções auxiliares: propriedades da árvore B-----------------

//Propriedade 1: número de chaves de cada nó dentro dos limites.
//  - todo nó tem no máximo MAX = 2T-1 chaves;
//  - todo nó, exceto a raiz, tem no mínimo MIN = T-1 chaves;
//  - a raiz interna tem pelo menos 1 chave (a raiz folha pode ter 0: árvore vazia).
bool Btree_checkKeyCount(Node* x, bool isRoot) {
    if (x->n > MAX) {
        return false;
    }
    if (!isRoot && x->n < MIN) {
        return false;
    }
    if (isRoot && !x->leaf && x->n < 1) {
        return false;
    }

    //Confere os filhos (nó interno tem exatamente n+1 filhos)
    if (!x->leaf) {
        for (int i = 0; i <= x->n; i++) {
            if (x->children[i] == NULL) {
                return false;
            }
            if (!Btree_checkKeyCount(x->children[i], false)) {
                return false;
            }
        }
    }
    return true;
}

//Propriedade 2: chaves em ordem não decrescente dentro do nó e dentro da faixa
//que o pai permite (lo e hi são os limites herdados; NULL = sem limite).
//Para o filho i de x: keys[i-1] <= chaves do filho <= keys[i].
bool Btree_checkOrder(Node* x, const int* lo, const int* hi) {
    //Ordem dentro do nó
    for (int i = 1; i < x->n; i++) {
        if (x->keys[i - 1] > x->keys[i]) {
            return false;
        }
    }

    //Todas as chaves do nó devem estar dentro da faixa [lo, hi]
    if (x->n > 0) {
        if (lo != NULL && x->keys[0] < *lo) {
            return false;
        }
        if (hi != NULL && x->keys[x->n - 1] > *hi) {
            return false;
        }
    }

    if (x->leaf) {
        return true;
    }

    //Cada filho recebe a faixa delimitada pelas chaves vizinhas de x
    for (int i = 0; i <= x->n; i++) {
        if (x->children[i] == NULL) {
            return false;
        }
        const int* l = (i == 0) ? lo : &x->keys[i - 1];
        const int* h = (i == x->n) ? hi : &x->keys[i];
        if (!Btree_checkOrder(x->children[i], l, h)) {
            return false;
        }
    }
    return true;
}

//Propriedade 3: todas as folhas têm a mesma profundidade.
//*leafDepth começa em -1 e guarda a profundidade da primeira folha encontrada.
bool Btree_checkLeafDepth(Node* x, int depth, int* leafDepth) {
    if (x->leaf) {
        if (*leafDepth < 0) {
            *leafDepth = depth;
        }
        return *leafDepth == depth;
    }

    for (int i = 0; i <= x->n; i++) {
        if (x->children[i] == NULL) {
            return false;
        }
        if (!Btree_checkLeafDepth(x->children[i], depth + 1, leafDepth)) {
            return false;
        }
    }
    return true;
}

//Altura da árvore (número de arestas da raiz até uma folha)
int Btree_height(Node* x) {
    int h = 0;
    while (!x->leaf) {
        x = x->children[0];
        h++;
    }
    return h;
}

//Teorema 18.1: se N >= 1, então N >= 2*T^h - 1 (ou seja, h <= log_T((N+1)/2))
bool Btree_checkHeight(BTree* tree, int N) {
    if (N == 0) {
        return true;
    }
    int h = Btree_height(tree->root);
    long pw = 1;
    for (int i = 0; i < h; i++) {
        pw *= T;
    }
    return N >= 2 * pw - 1;
}

//-----------------Verificação completa-----------------

//Verifica todas as propriedades da árvore B + conteúdo dos dados
//data são os N valores que deveriam estar na árvore (em qualquer ordem)
bool Btree_check(BTree* tree, const int* const data, const int N) {
    if (tree == NULL || tree->root == NULL) {
        return false;
    }

    //Quantidade de chaves precisa bater com o esperado
    const int tsize = Btree_size(tree->root);
    if (tsize != N) {
        return false;
    }

    //Propriedades estruturais (feitas antes de percorrer a árvore para extrair
    //os valores, pois garantem que não há filhos NULL em nós internos)
    if (!Btree_checkKeyCount(tree->root, true)) {
        return false;
    }
    if (N == 0) {
        return true;
    }
    if (!Btree_checkOrder(tree->root, NULL, NULL)) {
        return false;
    }
    int leafDepth = -1;
    if (!Btree_checkLeafDepth(tree->root, 0, &leafDepth)) {
        return false;
    }
    if (!Btree_checkHeight(tree, N)) {
        return false;
    }

    //Conteúdo: ordena uma cópia dos dados esperados
    int* arr = (int*)malloc(sizeof(int) * N);
    int* tarr = (int*)malloc(sizeof(int) * tsize);
    if (arr == NULL || tarr == NULL) {
        free(arr);
        free(tarr);
        return false;
    }
    memcpy(arr, data, sizeof(int) * N);
    qsort(arr, N, sizeof(int), int_comp);

    //e extrai os valores da árvore em ordem crescente
    int index = 0;
    Btree_store(tree->root, tarr, &index);

    //Os dois vetores ordenados devem ser idênticos
    bool match = (index == N);
    for (int i = 0; i < N && match; i++) {
        if (arr[i] != tarr[i]) {
            match = false;
        }
    }

    //Libera os valores auxiliares da memória
    free(tarr);
    free(arr);

    //Retorna se é uma árvore B válida com os dados esperados
    return match;
}

//Imprime a árvore B "deitada" (a raiz fica à esquerda): filhos de índice
//maior acima do nó, filhos de índice menor abaixo.
void Btree_printTreeRec(Node* x, int space) {
    if (x == NULL) {
        return;
    }

    space += 4;

    //Filhos da metade direita (do último até o do meio), acima do nó
    int mid = (x->n + 1) / 2;
    if (!x->leaf) {
        for (int i = x->n; i >= mid; i--) {
            Btree_printTreeRec(x->children[i], space);
        }
    }

    //O nó em si: todas as chaves em uma linha
    printf("\n");
    for (int i = 4; i < space; i++) {
        printf(" ");
    }
    printf("[");
    for (int i = 0; i < x->n; i++) {
        printf(i ? " %02d" : "%02d", x->keys[i]);
    }
    printf("]\n");

    //Filhos da metade esquerda (do primeiro até antes do meio), abaixo do nó
    if (!x->leaf) {
        for (int i = mid - 1; i >= 0; i--) {
            Btree_printTreeRec(x->children[i], space);
        }
    }
}

void Btree_printTree(BTree* tree) {
    Btree_printTreeRec(tree->root, 0);
}

/*------------------------------------------------------------------------------
 * Permutação de um Arranjo
 *
 * Implementan o algoritmo iterativo de Narayana Pandita para a permutação de
 * um arranjo em ordem lexicográfica.
 */

void swap(int *a, int *b)
{
    const int temp = *a;
    *a = *b;
    *b = temp;
}

void perm_invert(int *arr, int inicio, int fim)
{
    while (inicio < fim) {
        swap(&arr[inicio], &arr[fim]);
        inicio++;
        fim--;
    }
}

bool perm_next(int *arr, int tamanho)
{
    int i = tamanho - 2;

    while (i >= 0 && arr[i] >= arr[i + 1]) {
        i--;
    }

    if (i < 0) {
        return false;
    }

    int j = tamanho - 1;
    while (arr[j] <= arr[i]) {
        j--;
    }

    swap(&arr[i], &arr[j]);

    perm_invert(arr, i + 1, tamanho - 1);

    return true;
}

/*------------------------------------------------------------------------------
 * Funções Auxiliares
 */

void data_print(const int * const data, const int N)
{
    printf("data: [ ");
    for (int i = 0; i < N; i++) {
        printf("%02d ", data[i]);
    }
    printf("]\n");
}

int arr_remove(int *arr, int N, int value)
{
    for (int i = 0; i < N; i++) {
        if (arr[i] == value) {
            for (int j = i; j < N - 1; j++) {
                arr[j] = arr[j + 1];
            }
            N--;
            i--;
        }
    }

    return N;
}

int main() {

    int DATA_INSERT[] = {10, 23, 39, 50, 67};
    int DATA_REMOVE[] = {10, 23, 39, 50, 67};
    const int N = 5; // tamanho dos arranjos de inserção e remoção

    int *data_insert, *data_remove;

    data_insert = (int *)malloc(sizeof(int) * N);
    memcpy(data_insert, DATA_INSERT, sizeof(int) * N);

    do { // Loop de Inserção

        data_remove = (int *)malloc(sizeof(int) * N);
        memcpy(data_remove, DATA_REMOVE, sizeof(int) * N);

        do { // Loop de Remoção
            BTree *tree = createBTree();
            assert(tree != NULL);

            printf("--------------------------------------------\n");
            printf("Dados para Insercao:\n\t");
            data_print(data_insert, N);

            for (int i = 0; i < N; i++) {
                printf("Inserindo: %02d\n", data_insert[i]);
                Btree_insert(tree, data_insert[i]);
                Btree_print(tree);
                Btree_printTree(tree);   
                assert(Btree_check(tree, data_insert, i + 1));
            }

            printf("Arvore apos todas as INSERCOES:\n");
            Btree_printTree(tree);

            printf("Dados para Remocao:\n\t");
            data_print(data_remove, N);
            int *arr = (int *)malloc(sizeof(int) * N);
            int asize = N;
            memcpy(arr, data_insert, sizeof(int) * asize);

            for (int i = 0; i < N; i++) {
                printf("Removendo: %02d\n", data_remove[i]);
                bool removed = Btree_delete(tree, data_remove[i]);
                assert(removed); // a chave existe
                (void)removed; 
                Btree_print(tree);
                Btree_printTree(tree);   
                asize = arr_remove(arr, asize, data_remove[i]);
                assert(Btree_check(tree, arr, asize));
            }

            printf("Arvore apos todas as REMOCOES:\n");
            Btree_printTree(tree);

            free(arr);
            BTreeFree(tree);

        } while (perm_next(data_remove, N));

        free(data_remove);

    } while (perm_next(data_insert, N));

    free(data_insert);

    return EXIT_SUCCESS;
}