# NetlistActivityAnalyzer
This project is part of the **Low Power Design** course.

# Activity Analyzer for Digital Circuits

## Overview

The Activity Analyzer is a C++ program designed to analyze the activity of digital circuits. The aim of the project is to calculate the overall activity of a given digital circuit based on the probabilities of inputs being logically '1' (P1) and the fanout of each wire within the circuit.

The code reads a netlist input, and processes it to fill in data structures representing the circuit's gates and wires. It then calculates the activity of each wire and the total activity of the circuit.

## Method

The program contains several key functions and data structures:

- `Circuit::getNetList`: Reads the netlist and initializes the data structures.
- `Gate` struct: Represents a gate with a label, type, and a list of connected input wires.
- `wireP1Fanout`: A map that associates each wire with its probability of being '1' (P1) and its fanout.
- `Circuit::discoverInputs`: Identifies and stores the input wires of the circuit.
- `Circuit::getInputsP1`: Retrieves the P1 values of the circuit inputs.
- `Circuit::getActivity`: Calculates the total activity of the circuit by processing each wire's P1 and fanout.
- `Circuit::processWire`: A recursive function that computes the P1 value of a wire if not already calculated and updates the fanout for the wires connected to the gate's inputs.

The P1 value for a wire is calculated using the `Primitives::getP1` method, which considers the gate type and P1 values of the gate's inputs. The `Primitives` class contains the logic functions for each gate type and can be extended with additional logic gates if required.

## Example

The following is a step-by-step manual calculation of the activity for a given digital circuit, which serves as an example to validate the developed code. The circuit in question and the corresponding netlist are detailed below, along with the input probabilities and the calculated activities for each wire.

### Given Circuit

Consider the digital circuit depicted below (Image 1). The circuit consists of a combination of XOR, AND, and OR gates with inputs labeled A, B, and Cin, each with an associated probability (P1) of being logically '1'. The P1 values for the inputs are indicated in red in the images.

![image](https://github.com/SamanMohseni/NetlistActivityAnalyzer/assets/51726090/78d9aba8-449f-4a53-b909-89955974c276)
*Image 1. Example circuit schematic*

### Netlist

The netlist for the circuit is as follows:

```
xor x1(x1o, A, B);
and a1(a1o, A, B);
and a2(a2o, Cin, x1o);
xor x2(S, Cin, x1o);
or o1(Cout, a1o, a2o);
end
```

Input probabilities:

```
A 0.5
B 0.5
Cin 0.5
```

### Activity Calculation

First, we calculate the α (α = P1 * (1 - P1)) for each wire manually, as shown on the wires (Image 1). The activity for a wire is given by α * fanout. The calculated activity values are:

Total Activity = [A: 2 * 0.25] + [B: 2 * 0.25] + [x1o: 2 * 0.25] + [Cin: 2 * 0.25] + [a1o: 1 * 0.1875] + 
[a2o: 1 * 0.1875] + [S: 0 * 0.25] + [Cout: 0 * 0.246094] = 2.375


Please note that the fanout for the output wires (S and Cout) is considered zero for this manual calculation because it is not specified how many elements are connected to the outputs and because these activities are accounted for as inputs in other parts of the circuit if calculated separately.

After running the program with the above inputs, the results obtained match the manual calculations, confirming that the total activity of the circuit and the activity of all individual wires have been correctly computed.

## Usage

To use the Activity Analyzer, provide the netlist file along with the input probabilities as in `SampleNetlist.txt`. Run the program to calculate the activity levels of the digital circuit.
