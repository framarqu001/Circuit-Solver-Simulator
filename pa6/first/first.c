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
                char* name = gateToString(unknowns->unknownList[i]->gateType);
                char gateNum[10];
                snprintf(gateNum, sizeof(gateNum), "G%d", i+1);
                printf("%s %s\n", gateNum, name);
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

    for (int i = 0; i < iterations; i++) {
        int temp = i;
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
            value ^=1;
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

    bool found = solveCircuit(circuit, 0);
    if (found == false) {
        printf("INVALID\n");
    }
    freeCircuit(circuit);
}




