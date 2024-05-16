#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "circuit.h"



StrArr* allocate_StrArr(){
    int INITIAL_CAPAICTY = 10;
    StrArr* temp = malloc(sizeof(StrArr));
    char** stack = malloc(INITIAL_CAPAICTY * sizeof(char*));
    temp->stringArr = stack;
    temp->index = 0;
    temp->size = INITIAL_CAPAICTY;
    return temp;  
}

Unknowns* allocate_Unknowns() {
    int INITIAL_CAPACITY = 10;
    Unknowns* temp = malloc(sizeof(Unknowns));
    temp->count = 0;
    temp->unknownList = malloc(INITIAL_CAPACITY * sizeof(Unknowns*));
    temp->size = INITIAL_CAPACITY;
    return temp;
}

void addUnknown(Unknowns *unknown, GateNode* node) {
    if (unknown->count == unknown->size) {
        GateNode** temp = malloc((unknown->size * 2) * sizeof(GateNode*));
        for (int i = 0; i < unknown->size; i++) {
            temp[i] = unknown->unknownList[i];
        }
        free(unknown->unknownList);
        unknown->unknownList = temp;  
        unknown->size = unknown->size*2;
    }
    int index = unknown->count;
    unknown->unknownList[index] = node;
    unknown->count += 1;
}

void addString(StrArr* stack, char* vertex) {
    if (stack->index == stack->size) {
        char** temp = malloc((stack->size * 2) * sizeof(char*));
        for (int i = 0; i < stack->size; i++) {
            temp[i] = stack->stringArr[i];
        }
        free(stack->stringArr);
        stack->stringArr = temp;  
        stack->size = stack->size*2;
    }
    int index = stack->index;
    stack->stringArr[index] = strdup(vertex);
    stack->index += 1;
}

IntArr* allocate_intarr() {
    int INITIAL_CAPACITY = 10;
    IntArr* temp = malloc(sizeof(IntArr));
    temp->count = 0;
    temp->intArr = malloc(INITIAL_CAPACITY * sizeof(unsigned));
    temp->size = INITIAL_CAPACITY;
    return temp;
}



void addNum(IntArr* intArr, unsigned int val) {
    if (intArr->count == intArr->size) {
        unsigned int* temp = malloc((intArr->size * 2) * sizeof(unsigned int));
        for (int i = 0; i < intArr->size; i++) {
            temp[i] = intArr->intArr[i];
        }
        free(intArr->intArr);
        intArr->intArr = temp;  
        intArr->size = intArr->size*2;
    }
    int index = intArr->count;
    intArr->intArr[index] = val;
    intArr->count += 1;
}

void clearNums(IntArr* intArr) {
    intArr->count = 0;
}

GateNode* allocate_gatenode(GateType gate, int count) {
    GateNode* temp = malloc(sizeof(GateNode));
    temp->gateType = gate;
    temp->count = count;
    temp->gateArray = allocate_StrArr();
    temp->intArray = allocate_intarr();
    temp->next = NULL;
    temp->prev = NULL;
    return temp;
}

void addGate(Circuit* circuit, GateNode* node) {
    circuit->numGates++;
    GateNode* current = circuit->head;


    if (node->gateType == UNKNOWN) {
        addUnknown(circuit->unknowns, node);
    }


    if (current == NULL) {
        circuit->head = node;
        return;
    }

    while (current->next != NULL) {
        current = current-> next;
    }

    current->next = node;
    node->prev = current;
}

HashNode* allocate_hashNode(char* key, unsigned int value) {
    HashNode* temp = malloc(sizeof(HashNode));
    temp->key = strdup(key);
    temp->value = value;
    temp->next = NULL;
    return temp;
}

HashTable* allocate_HashTable(int size) {
    HashTable* temp = malloc(sizeof(HashTable));
    temp->size = size;
    temp->nodes = malloc(size * sizeof(HashNode*));
    for (int i = 0; i < size; i++) {
        temp->nodes[i] = NULL;
    }
    return temp;
}

void freeHashTable(HashTable* hashtable) {
    int size = hashtable->size;
    for(int i = 0; i < size; i++) {
        freeHashList(hashtable->nodes[i]);
    }
    free(hashtable->nodes);
    free(hashtable);
}

unsigned int hash(char* key, int size) {
    int length = strlen(key);
    int value = 0;
    for (int i = 0; i < length; i++) {
        value += key[i];
    }
    return (value % size);
}


void freeHashList(HashNode* root) {
    HashNode* current = root;
    while (current != NULL){
        HashNode* temp = current->next;
        free(current->key);
        free(current);
        current = temp;
    }
}

Circuit* allocate_circuit(int numInputs, int numOutputs) {
    Circuit* temp = malloc(sizeof(Circuit));
    temp->head = NULL;
    temp->numInputs = numInputs;
    temp->numOutputs = numOutputs;
    temp->numGates = 0;
    temp->outputsArr = malloc(numOutputs * sizeof(unsigned int*));
    temp->answerArr = malloc(numOutputs * sizeof(unsigned int*));
    for (int i = 0; i < numOutputs; i++) {
        temp->outputsArr[i] = malloc((1 << numInputs) * sizeof(unsigned int));
        temp->answerArr[i] = malloc((1 << numInputs) * sizeof(unsigned int));
    }
    temp->inputVar = allocate_StrArr();
    temp->outputVar = allocate_StrArr();
    temp->unknowns = allocate_Unknowns();
    temp->table = allocate_HashTable(20);
    insert(temp->table, "1", 1);
    insert(temp->table, "0", 0);
    return temp;
}

void freeGateNode(GateNode* current) {
    freeStrArr(current->gateArray);
    free(current->intArray->intArr);
    free(current->intArray);
    free(current);
}

void freeCircuit(Circuit* circuit) {
    GateNode* current = circuit->head;
    while (current != NULL){
        GateNode* temp = current->next;
        freeGateNode(current);
        current = temp;
    }
    for (int i = 0; i < circuit->numOutputs; i++) {
        free(circuit->outputsArr[i]);
        free(circuit->answerArr[i]);
    }
    free(circuit->outputsArr);
    free(circuit->answerArr);
    freeStrArr(circuit->inputVar);
    freeStrArr(circuit->outputVar);
    free(circuit->unknowns->unknownList);
    free(circuit->unknowns);
    freeHashTable(circuit->table);
    free(circuit);
}

void freeStrArr(StrArr* stringArray) {
    for(int i = 0; i < stringArray->index; i++) {
        free(stringArray->stringArr[i]);
    }
    free(stringArray->stringArr);
    free(stringArray);
}

unsigned int get(HashTable* table, char* key) {
    int index = hash(key, table->size);
    HashNode* node = table->nodes[index];
    if (node == NULL) {
        return 0;
    }
    while(node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    return 0;
}

void insert(HashTable* table, char* key, unsigned int value) {
    int index = hash(key, table->size);
    HashNode* current = table->nodes[index];
    HashNode* prev = NULL;
    
    
    if (current == NULL) {
        HashNode* newNode = allocate_hashNode(key, value);
        table->nodes[index] = newNode;
        return;
    }
    
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        prev = current;
        current = current->next;    
    }
    HashNode* newNode = allocate_hashNode(key, value);
    prev->next = newNode;
}

void printTable(HashTable* table) {
    int size = table->size;
    HashNode** nodes = table->nodes;
    printf("Start\n");
    for (int i = 0; i < size; i++) {
        if (nodes[i] == NULL) {
            printf("\t%i\tEmpty\n", i);
        } else {
            HashNode* current = nodes[i];
            printf("\t%i\t(%s,%d)",i, current->key, current->value);
            current = current->next;
            while (current!= NULL) {
                printf(" => (%s,%d)", current->key, current->value);
                current = current->next;
            }
           printf("\n");
        }
    }
    printf("End\n");
}




