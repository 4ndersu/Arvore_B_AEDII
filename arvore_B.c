#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define T 2 //Grau minimo
#define MAX 2*T-1 //Numero maximo de chaves
#define MIN T-1 //Numero minimo de chaves

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
    
    //Inicializa o número de chaves e o indicador de folha
    newNode->n = 0;
    newNode->leaf = leaf;

    //Inicializa os ponteiros para os filhos como NULL
    for (int i = 0; i < MAX + 1; i++) {
        newNode->children[i] = NULL;
    }

    //Retorna o novo nó criado
    return newNode;
}

BTree* createBTree() {
    //Aloca memória para a árvore B
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    
    //Cria a raiz da árvore como uma folha
    tree->root = createNode(true);
    
    //Retorna a árvore B criada
    return tree;
}
void NodeFree(Node* node) {
    free(node);
}
void NodeFreeRec(Node* node) {
    if (node != NULL) {
        //Libera recursivamente os filhos do nó
        for (int i = 0; i <= node->n; i++) {
            NodeFree(node->children[i]);
        }
        //Libera a memória do nó atual
        free(node);
    }
}

void BTreeFree(BTree* tree) {
    if (tree != NULL) {
        //Libera recursivamente a raiz da árvore
        NodeFreeRec(tree->root);
        //Libera a memória da estrutura da árvore
        free(tree);
    }
}

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
        insertNonFull(x->children[i], k);
    }
}

//Função para inserir uma chave em um nó que não está cheio
void Btree_insert(BTree* tree, int k) {
    Node* root = tree->root;

    if (root->n == MAX) {
        //Se a raiz estiver cheia, cria um novo nó como raiz
        Node* s = createNode(false);
        tree->root = s;
        s->children[0] = root;
        s->n = 0;
        s->leaf = false;
        s->keys[0] = root->keys[MIN];
        splitChild(s, 0, root);
        insertNonFull(s, k);
    } else {
        insertNonFull(root, k);
    }
}


int main() {

    int DATA_INSERT[] = {10, 23, 39, 50, 67};
    int DATA_REMOVE[] = {10, 23, 39, 50, 67};

    return EXIT_SUCCESS;
}