#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "circuit.h"

char* gateToString(GateType gate) {
    switch (gate){
        case INPUT:
            return "input";
        case AND:
            return "AND";
        case OR:
            return "OR";
        case XOR:
            return "XOR";
        case NOT:
            return "NOT";
        case MULTIPLEXER:
            return "MULTIPLEXER";
        case DECODER:
            return "DECODER";
        case UNKNOWN:
            return "UNKNOWN";
        default:
            return 0;
    }
}

unsigned int numGrayCode(unsigned int num) {
    return num ^ (num >> 1);
}

GateType getType(char* string) {
    if (strcmp(string, "AND") == 0) return AND;
    if (strcmp(string, "OR") == 0) return OR;
    if (strcmp(string, "NOT") == 0) return NOT;
    if (strcmp(string, "XOR") == 0) return XOR;
    if (strcmp(string, "DECODER") == 0) return DECODER;
    if (strcmp(string, "MULTIPLEXER") == 0) return MULTIPLEXER;
    return UNKNOWN;
}

void storeOutputs(Circuit* circuit, char* inputBuffer, unsigned int index) {
    unsigned int n = 1 << circuit->numInputs;
    unsigned int* output = circuit->answerArr[index];
    char* token = strtok(inputBuffer, " \n");
    for (int i = 0; i < n; i++) {
        token = strtok(NULL," \n");
        output[i] = atoi(token);
    }
}

void buildGates(Circuit* circuit, char* inputBuffer) {
    char* token = strtok(inputBuffer, " \n");
    GateType gateType = getType(token);
    int n = 0;
    if (gateType == AND || gateType == OR || gateType == XOR)  { //change to switch later
        n = 3;
    } else if (gateType == NOT) {
        n = 2;
    } else if (gateType == MULTIPLEXER) {
        token = strtok(NULL, " \n");
        n = atoi(token);
        n += highestPowerof2(n);
        n++;
    } else if (gateType == DECODER) {
        token = strtok(NULL, " \n");
        n = atoi(token);
        n += highestPowerof2(n);
    } else if (gateType == UNKNOWN)  {
        token = strtok(NULL, " \n");
        n = atoi(token);
    } 
    
    

    GateNode* node = allocate_gatenode(gateType, n);
    token = strtok(NULL, " \n");
    while (token!=NULL) {
        addString(node->gateArray, token);
        token = strtok(NULL, " \n");
    }
    addGate(circuit, node);
}

bool compareResults(Circuit* circuit) {
    int n = 1 << circuit->numInputs;
    unsigned int** answer = circuit->answerArr;
    unsigned int** result = circuit->outputsArr;

    for (int i = 0; i < circuit->numOutputs; i++) {
        for (int j = 0; j < n; j++) {
            if (answer[i][j] != result[i][j]) {
                return false;
            }
        }
    }

    return true;
}

bool solveCircuit(Circuit* circuit, int unknownsCount) {
    Unknowns* unknowns = circuit->unknowns;
    if (unknownsCount == unknowns->count) {
        runSimulation(circuit);
        bool match = compareResults(circuit);
        if (match == true) {
            for (int i =0; i < unknowns->count; i++) {
                // char* name = gateToString(unknowns->unknownList[i]->gateType);
                // char gateNum[10];
                // snprintf(gateNum, sizeof(gateNum), "G%d", i+1);
                // printf("%s %s\n", gateNum, name);
            }
            return true;
        } else {
            return false;
        }
    }

    for (int type = OR; type <= MULTIPLEXER; type++) { 
        unknowns->unknownList[unknownsCount]->gateType = type;
        bool found = solveCircuit(circuit, unknownsCount + 1);
        if (found) return found;
        
    }

    return false;
}

void runSimulation(Circuit* circuit) {
    int iterations = 1 << circuit->numInputs;
    int inputs = circuit->numInputs;
    char** inputVars = circuit->inputVar->stringArr;
    HashTable* table = circuit->table;

    for (unsigned int i = 0; i < iterations; i++) {
        int temp = numGrayCode(i);
        for (int j = inputs - 1; j >= 0; j--) {
            insert(table, inputVars[j], (temp & 0x1));
            temp = temp >> 1;
        }
        // printTable(table);
        turnOnCircuit(circuit);
        storeResults(circuit, i);
    }
    // printTable(table); //testing
}

void storeResults(Circuit* circuit, int column) {
    StrArr* strArr = circuit->outputVar;
    char** stringArr = strArr->stringArr;
    HashTable* table = circuit->table;

    unsigned int** output = circuit->outputsArr;

    for (int i = 0; i < circuit->numOutputs; i++) {
        output[i][column] = get(table, stringArr[i]);
    }

}

void turnOnCircuit(Circuit* circuit) {
    GateNode* currentGate = circuit->head;
    HashTable* table = circuit->table;
    while (currentGate!= NULL) {
        gateOperation(currentGate, table);
        currentGate= currentGate->next;
    }

}

unsigned int highestPowerof2(unsigned int n) {
    unsigned count = 0;

    while (n != 1) {
        n = n >> 1;
        count++;
    }
    return count;
}

unsigned int gateOperation(GateNode* gateNode, HashTable* table) {
    GateType gate = gateNode->gateType;
    int totalConnections = gateNode->count;
    char** stringArr = gateNode->gateArray->stringArr;
    unsigned* numList = gateNode->intArray->intArr;
    IntArr* ints = gateNode->intArray;
    unsigned int value;
    unsigned int power;
    unsigned int selectors;
    unsigned int inputs;
    unsigned int outputs;
    unsigned int index;
    int powerof;

    switch (gate){
        case AND:
            if (totalConnections < 3) {
                return 99;
            }
           addNum(ints, get(table, stringArr[0]));
           addNum(ints, get(table, stringArr[1]));
           insert(table, stringArr[2], numList[0] & numList[1]);
           clearNums(ints);
           break;
        case OR:
            if (totalConnections < 3) {
                return 99;
            }
            addNum(ints, get(table, stringArr[0]));
            addNum(ints, get(table, stringArr[1]));
            insert(table, stringArr[2], numList[0] | numList[1]);
            clearNums(ints);
            break;
        case XOR:
        if (totalConnections < 3) {
                return 99;
            }
            addNum(ints, get(table, stringArr[0]));
            addNum(ints, get(table, stringArr[1]));
            insert(table, stringArr[2], numList[0] ^ numList[1]);
            clearNums(ints);
            break;
        case NOT:

            value = get(table, stringArr[0]);
            value ^= 1;
            insert(table, stringArr[1], value);
            clearNums(ints);
            break;

        case MULTIPLEXER:
            if (totalConnections < 4) {
                return 99;
            }
            power = highestPowerof2(totalConnections - 1);
            selectors = power;
            inputs = 1 << power;
            outputs = 1;
            index = 0;

            for (int i = inputs; i < (inputs + selectors); i++){
                addNum(ints, get(table, stringArr[i]));
            }

            powerof = power - 1;
            for (int i = 0; i < selectors; i++) {
                index += numList[i] << (powerof);
                powerof--;
            }
            index = numGrayCode(index);

            insert(table, stringArr[totalConnections - 1], get(table, stringArr[index]));
            clearNums(ints);
            break;
        case DECODER:
            if (totalConnections < 3) {
                return 99;
            }   
            power = highestPowerof2(totalConnections);
            outputs = totalConnections - power;
            inputs = totalConnections - outputs;
            index = 0;

            for (int i = 0; i < inputs; i++){
                addNum(ints, get(table, stringArr[i]));
            }
            powerof = power - 1;
            for (int i = 0; i < inputs; i++) {
                index += numList[i] << (powerof);
                powerof--;
            }
            index = numGrayCode(index);

            unsigned int count = 0;
            for (int i = inputs; i < inputs + outputs; i++) {
                if (count == index) {
                    insert(table, stringArr[i], 1);
                } else {
                    insert(table, stringArr[i], 0);
                }
                count++;
            }
            clearNums(ints);
        case UNKNOWN:
        default:
            return 99;
    }
    return 99;
}

void readVariables(FILE* fp, unsigned int* numInputs, unsigned int* numOutputs, bool* inputFirst) {
    char buffer[500];
    unsigned int num;

    fscanf(fp, "%s %d", buffer, &num);

    if (strcmp(buffer, "INPUTVAR") == 0) {
        *numInputs = num;
        *inputFirst = true;
        fgets(buffer, sizeof(buffer), fp);
        fscanf(fp, "%s %d", buffer, numOutputs);
    } else {
        *numOutputs = num;
        fgets(buffer, sizeof(buffer), fp);
        fscanf(fp, "%s %d", buffer, numInputs);
    }
}

void readInputs(FILE* fp, Circuit* circuit) {
    char inputBuffer[300];
    unsigned int scanInput;
    fscanf(fp, "%s %d", inputBuffer, &scanInput);
    for (int i = 0; i < scanInput; i++) {
        fscanf(fp, "%s", inputBuffer);
        addString(circuit->inputVar, inputBuffer);
    }
}

void readOutputs(FILE* fp, Circuit* circuit) {
    char inputBuffer[300];
    unsigned int scanInput;
    fscanf(fp, "%s %d", inputBuffer, &scanInput);
    for (int i = 0; i < scanInput; i++) {
        fscanf(fp, "%s", inputBuffer);
        addString(circuit->outputVar, inputBuffer);
    }
}

bool compareOneStrings(GateNode* current, GateNode* explore, char** match) {
    char** currentNode = current->gateArray->stringArr;
    char** exploreNode = explore->gateArray->stringArr;
    for(int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if(strcmp(currentNode[i], exploreNode[j]) == 0) { 
                *match = currentNode[i];
                return true;
            }
        }
    }
    return false;
}

bool compareTwoInputs(GateNode** foundNodes, GateNode* exploreNode) {
    GateNode* first = foundNodes[0];
    GateNode* second = foundNodes[1];
    char* inputs[2] = {first->gateArray->stringArr[2], second->gateArray->stringArr[2]};

    if (strcmp(inputs[0], exploreNode->gateArray->stringArr[0]) != 0) {
        return false;
    }
    if (strcmp(inputs[1], exploreNode->gateArray->stringArr[1]) != 0) {
        return false;
    } 

    return true;
}

bool attemptDistrbutivity(GateNode** foundNodes) {
    GateNode* currentNode = foundNodes[0];
    // look to see if matching outputs match an opposite types input
    GateType findGatetype = currentNode->gateType;

    if (findGatetype == AND) {
        findGatetype = OR;
    } else {
        findGatetype = AND;
    }
    currentNode = currentNode->next;
    while (currentNode != NULL) {
        if (currentNode->gateType == findGatetype ) {
            if (compareTwoInputs(foundNodes, currentNode)) {
                foundNodes[2] = currentNode;
                return true;
            }
        }
        currentNode = currentNode->next;
    }
    return false;
}

bool attemptAssociative(GateNode** foundNodes) {
    GateNode* currentNode = foundNodes[0];
    GateNode* secondNode = foundNodes[1];
    // look to see if matching outputs match an opposite types input
    GateType findGatetype = currentNode->gateType;

    char* findString = currentNode->gateArray->stringArr[2];

    if (findGatetype != OR) {
        return false;
    }
    for (int i = 0; i < 2; i++) {
       if (strcmp(secondNode->gateArray->stringArr[i], findString) == 0) {
            return true;
        } 
    }
    
    
    return false;
}

bool canSimplify(GateNode* currentNode, GateNode** foundNodes, char** match, BoolAlgebra* law) {
    GateType findType = currentNode->gateType;
    GateNode* exploreNode;
    
    exploreNode = currentNode->next;;
    while (exploreNode != NULL) { // looking for matching type to see if they have matching strings.
        if (exploreNode->gateType == findType) {
            if (compareOneStrings(currentNode, exploreNode, match)) {
                foundNodes[1] = exploreNode; // Found 2 nodes that have matching inputs.
                if (attemptDistrbutivity(foundNodes)) { // Looks for an opposite gate that inputs matches the non matching inputs of the found gates
                    *law = DISTRIBUTE;
                    return true;
                }
                if (attemptAssociative(foundNodes)) { // if two OR's have matching inputs then can swap.
                    *law = ASSOCIATIVE;
                    return true;
                }
            }
        }
        exploreNode = exploreNode->next;
    }

    return false;
}

void printGateNodeList(Circuit* circuit) {
    GateNode* current = circuit->head;

    while (current!=NULL) {
        char* name = gateToString(current->gateType);
        printf("Node: %s -->", name);
        current = current->next;
    }
    printf("NULL\n");
}

char* extractNonMatchString(GateNode* node, char* match){
    char** nodeStrings = node->gateArray->stringArr;
    for (int i = 0; i < 2; i++) {
        if (strcmp(nodeStrings[i], match) != 0) {
            return nodeStrings[i];
        }
    }
    return "";
}

void removeNode(Circuit* circuit, GateNode* gatenode) { 
    if (gatenode->prev == NULL) {
        circuit->head = gatenode->next;
    }

    if (gatenode->next != NULL) {
        gatenode->next->prev = gatenode->prev;
    }
    if (gatenode->prev != NULL) {
        gatenode->prev->next = gatenode->next;
    }
    freeGateNode(gatenode);
}

void distribute(Circuit* circuit, GateNode** foundNodes, GateNode* previous, char* matchString) {
    GateNode* first = foundNodes[0];
    GateNode* second = foundNodes[1];
    GateNode* third = foundNodes[2];
    char strArr[3][50]; // creating a string array of length 3 with buffer size of 50
   

    StrArr* thirdStrArr = allocate_StrArr();
    StrArr* firstStrArr = allocate_StrArr();

    strcpy(strArr[0], extractNonMatchString(first, matchString));
    strcpy(strArr[1], extractNonMatchString(second, matchString));

    addString(firstStrArr, matchString);
    addString(firstStrArr, second->gateArray->stringArr[2]);
    addString(firstStrArr, third->gateArray->stringArr[2]);
    

    
    addString(thirdStrArr, strArr[0]);
    addString(thirdStrArr, strArr[1]);
    addString(thirdStrArr, second->gateArray->stringArr[2]);


    freeStrArr(first->gateArray);
    freeStrArr(third->gateArray);

    third->gateArray = firstStrArr;
    first->gateArray = thirdStrArr;
    //swap gatetypes
    if (third->gateType == OR) {
        third->gateType = AND;
    } else {
        third->gateType = OR;
    }

    if (first->gateType == OR) {
        first->gateType = AND;
    } else {
        first->gateType = OR;
    }
    
    removeNode(circuit, second);

    // printGateNodeList(circuit);
}

void associative(Circuit* circuit, GateNode** foundNodes, GateNode* previous, char* matchString) {
    GateNode* first = foundNodes[0];
    GateNode* second = foundNodes[1];
    StrArr* firstStrArr = allocate_StrArr();

    

    addString(firstStrArr, first->gateArray->stringArr[0]);
    addString(firstStrArr, first->gateArray->stringArr[1]);
    addString(firstStrArr, second->gateArray->stringArr[2]);
    freeStrArr(second->gateArray);
    second->gateArray = firstStrArr;

    removeNode(circuit, first);
    // printGateNodeList(circuit);
}

bool simplifyCircuit(Circuit* circuit) {
    GateNode* foundNodes[3];
    char* match = "";
    GateNode* current = circuit->head;
    GateNode* prev = NULL;
    BoolAlgebra law;

    while (current != NULL) {
        if (current->gateType == AND || current->gateType == OR) {
            foundNodes[0] = current;
            if(canSimplify(current, foundNodes, &match, &law)) { // Atleast two and/or nodes have matching inputs.
                switch (law) { //Simplify based off the found nodes
                    case DISTRIBUTE:
                        distribute(circuit, foundNodes, prev, match); 
                        return true;
                    case ASSOCIATIVE:
                        associative(circuit, foundNodes, prev, match);
                        return true;
                }  
                
            }
        }
        
        prev = current;
        current = current->next;
    }
    return false;
}

void printCircuit(FILE* fp, Circuit* circuit) {
    char inputBuffer[1000];
    fgets(inputBuffer, sizeof(inputBuffer), fp);
    printf("%s", inputBuffer);
    fgets(inputBuffer, sizeof(inputBuffer), fp);
    printf("%s", inputBuffer);
    fgets(inputBuffer, sizeof(inputBuffer), fp);
    printf("%s", inputBuffer);
    for (int i = 0; i < circuit->numOutputs; i++) {
        fgets(inputBuffer, sizeof(inputBuffer), fp);
        printf("%s", inputBuffer);
    }

    GateNode* current = circuit->head;
    char** gateArray = current->gateArray->stringArr;
    while (current != NULL) {
        gateArray = current->gateArray->stringArr;
        char* gateName = gateToString(current->gateType);
        printf("%s ", gateName);
        if (current->gateType == DECODER || current->gateType == MULTIPLEXER ) {
            int num = current->count - highestPowerof2(current->count);
            if (current->gateType == MULTIPLEXER) {
                num--;
            }
            printf("%d ", num);
        }
        for (int i = 0; i < current->count; i++) {
            if (i == current->count -1) {
                printf("%s\n", gateArray[i]);
            } else {
                printf("%s ", gateArray[i]);
            }
        }
        current = current->next;
    }
    
}


int main(int argc, char** argv) {  


    FILE* fp = fopen(argv[1], "r");

    if (fp == NULL) {
        printf("File was unable to be opened");
        exit(EXIT_SUCCESS);
    }

    unsigned int numInputs = 0;
    unsigned int numOutputs = 0;
    bool inputFirst = false;


    readVariables(fp, &numInputs, &numOutputs, &inputFirst);
    Circuit* circuit = allocate_circuit(numInputs, numOutputs);

    fclose(fp);

    char inputBuffer[1000];
    
    fp = fopen(argv[1], "r");


    if (inputFirst) {
        readInputs(fp, circuit);
        readOutputs(fp, circuit);
    } else {
        readOutputs(fp, circuit);
        readInputs(fp, circuit);
    }

    //-------------------------------------------------------------
    
    fscanf(fp, "%s \n", inputBuffer);
    for (int i = 0; i < circuit->outputVar->index; i++) {
        fgets(inputBuffer, sizeof(inputBuffer), fp);
        storeOutputs(circuit, inputBuffer, i);
    }

    while (fgets(inputBuffer, sizeof(inputBuffer), fp)) {
      buildGates(circuit, inputBuffer);
    }

    bool found = solveCircuit(circuit, 0); //dfs configure teh gates
    if (found == false) {
        printf("INVALID\n");
        freeCircuit(circuit);
        return EXIT_SUCCESS;
    }

    bool simplify = true;
    while (simplify) {
        simplify = simplifyCircuit(circuit);
    }

    fclose(fp);

    fp = fopen(argv[1], "r");
    printCircuit(fp, circuit);

    freeCircuit(circuit);
}




