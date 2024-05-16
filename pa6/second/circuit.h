#ifndef CIRCUIT_H
#define CIRCUIT_H

typedef enum {OR, AND, XOR, NOT, DECODER, MULTIPLEXER, UNKNOWN, INPUT} GateType;



typedef struct hashNode {
    char* key;
    unsigned int value;
    struct hashNode* next;
} HashNode;

typedef struct hashtable {
    HashNode** nodes;
    int size;
} HashTable;

typedef struct strArr {
    char** stringArr;
    int index;
    int size;
} StrArr;

typedef struct intArr {
    unsigned int* intArr;
    int count;
    int size;
} IntArr;

typedef struct gatenode {
    GateType gateType;
    int count;
    StrArr* gateArray;
    IntArr* intArray;
    struct gatenode* next;
} GateNode;

typedef struct uknowns {
    GateNode** unknownList;
    int count;
    int size;
} Unknowns;

typedef struct circuit {
    GateNode* head; //Linked list of gate nodes
    int numInputs;
    int numOutputs;
    unsigned int** outputsArr; //2darray of truth table for each outputvar
    unsigned int** answerArr; // stores the wanted result of the truth table
    StrArr* inputVar; // Strings containing all the inputvars
    StrArr* outputVar; // strings containing all the outpat vars;
    Unknowns* unknowns; // refrences to the unknown gates nodes to iterate over
    HashTable* table;
} Circuit;

GateNode* allocate_gatenode(GateType gate, int count) ;
StrArr* allocate_StrArr();
HashNode* allocate_hashNode(char* key, unsigned int value);
void addString(StrArr* stack, char* vertex);
IntArr* allocate_intarr();
void addNum(IntArr* intArr, unsigned int val);
void freeHashTable(HashTable* hashtable);
void freeHashList(HashNode* root);
void freeCircuit(Circuit* circuit);
void freeStrArr(StrArr* stringArray);
HashTable* allocate_HashTable(int size);
unsigned int hash(char* key, int size);
unsigned int get(HashTable* table, char* key);
void insert(HashTable* table, char* key, unsigned int value);
void printTable(HashTable* table);
Circuit* allocate_circuit(int outputs, int outputBits);
void addGate(Circuit* circuit, GateNode* node);
void test();
void runSimulation(Circuit* circuit);
bool solveCircuit(Circuit* circuit, int unknowns);
void buildGates(Circuit* circuit, char* inputBuffer);
void turnOnCircuit(Circuit* circuit);
unsigned int gateOperation(GateNode* gateNode, HashTable* table);
void clearNums(IntArr* intArr);
void storeResults(Circuit* circuit, int column);
unsigned int highestPowerof2(unsigned int n);
#endif
