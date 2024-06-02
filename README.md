# Circuit Simulator

## Overview
This project is a comprehensive Circuit Simulator, Solver, and Simplification tool. Given a truth table for each output and the specified number of inputs, the program identifies which gate configurations would satisfy the truth table. This functionality enhances understanding of logic gates and circuit design by allowing users to explore various configurations to achieve the desired outputs efficiently and effectively.

## Directives
The circuit is described using various directives. Below are the key directives used:

- **INPUTVAR**: Specifies the input variables. Format: `INPUTVAR <number_of_inputs> <input1> <input2> ...`
- **OUTPUTVAR**: Specifies the output variables. Format: `OUTPUTVAR <number_of_outputs> <output1> <output2> ...`
- **OUTPUTVAL**: Specifies the output values for each permutation of the inputs. Format: `OUTPUTVAL <output_name> <value1> <value2> ...`
- **OR**: Represents an OR gate. Format: `OR <input1> <input2> <output>`
- **AND**: Represents an AND gate. Format: `AND <input1> <input2> <output>`
- **XOR**: Represents an XOR gate. Format: `XOR <input1> <input2> <output>`
- **NOT**: Represents a NOT gate. Format: `NOT <input> <output>`
- **DECODER**: Represents a decoder. Format: `DECODER <number_of_inputs> <input1> <input2> ... <output1> <output2> ...`
- **MULTIPLEXER**: Represents a multiplexer. Format: `MULTIPLEXER <number_of_inputs> <input_values> <selector1> <selector2> ... <output>`

## Usage

### Compilation
To compile the program, navigate to the appropriate directory and run the `make` command:
```bash
cd first
make
cd ../second
make
cd ../third
make
```

## Project Parts

### Part I: Circuit Completer
**Description:** Complete a circuit by selecting appropriate gates to fulfill given constraints. Input permuations and the ordering for decoders and multiplexers are in standard binary order.

### Part II: Circuit Completer for Gray Code Inputs
**Description:** Same as part 1 but input permutations and the ordering for decords and multiplexers are in Gray Code

### Part III: Circuit Reducer
**Descriptions:** After completing the circuit, the program uses boolean algebra laws to simplify the circuit according to the Associative law and Distributivity law

## Running the Program
Each part of the project has its own executable. To run the program, provide the circuit description file as an argument.
```bash
./first/first <circuit_description_file>
```

## Examples

### Example Circuit Description
circuit.txt
```bash
INPUTVAR 3 IN1 IN2 IN3
OUTPUTVAR 1 Out1
OUTPUTVAL
Out1 0 1 1 1 0 1 0 0
AND IN1 IN2 temp1
G1 3 IN3 IN2 temp2
G2 2 temp1 temp3
OR temp2 IN2 temp4
AND temp3 temp4 Out1
```
Image of the described circuit
![image](https://github.com/framarqu001/Circuit-Solver-Simulator/assets/119390184/aa82d452-9545-4e90-96e7-4fc269cf0e8f)

building executable
```bash 
make first
```

solving circuit.txt
```bash
./first circuit.txt
```

### Example Output for circuit.txt
```bash
G1 OR
G2 NOT
```

For the given file circuit.txt, the output provided solves the circuit for the given output values 0 1 1 1 0 1 0 0

### Logic Circuit Explanation:

### AND Gate:
- **Inputs**: IN1, IN2
- **Output**: temp1
- **Expression**: 
  `temp1 = IN1 AND IN2`

### OR Gate:
- **Inputs**: IN3, IN2
- **Output**: temp2
- **Expression**: 
  `temp2 = IN3 OR IN2`

### NOT Gate:
- **Input**: temp1
- **Output**: temp3
- **Expression**: 
  `temp3 = NOT temp1`
- **Substituting temp1**: 
  `temp3 = NOT (IN1 AND IN2)`

### OR Gate:
- **Inputs**: temp2, IN2
- **Output**: temp4
- **Expression**: 
  `temp4 = temp2 OR IN2`
- **Substituting temp2**: 
  `temp4 = (IN3 OR IN2) OR IN2`
- **Simplifying**: 
  `temp4 = IN3 OR IN2`

### AND Gate:
- **Inputs**: temp3, temp4
- **Output**: Out1
- **Expression**: 
  `Out1 = temp3 AND temp4`
- **Substituting temp3 and temp4**: 
  `Out1 = NOT (IN1 AND IN2) AND (IN3 OR IN2)`

Thus, the final output expression for the circuit is:
`Out1 = NOT (IN1 AND IN2) AND (IN3 OR IN2)`

The third part of the program will show this simplication is true by providing the output:

```bash
INPUTVAR 3 IN1 IN2 IN3
OUTPUTVAR 1 Out1
OUTPUTVAL
Out1 0 1 1 1 0 1 0 0
AND IN1 IN2 temp1
NOT temp1 temp3
OR IN2 IN3 temp4
AND temp3 temp4 Out1
```
This is the simplified circuit for circuit.txt





