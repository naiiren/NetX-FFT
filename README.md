# NetX-FFT

This project uses [NetX](https://github.com/pascal-lab/NetX), the schematic hardware description language, to implement an 8-point Fast Fourier Transform (FFT) accelerator. The design follows the conventional decimation-in-time (DIT) butterfly algorithm, as described in this [Verilog FFT tutorial](https://www.runoob.com/w3cnote/verilog-fft.html).

This project demonstrates how NetX's schematic model can be used to design digital systems where code is arranged to resemble the actual schematic diagram, making the design easier to understand and maintain.

## Architecture Overview

The FFT implementation consists of three main stages of butterfly operations, processing 8 complex input samples to produce 8 complex frequency-domain outputs. The architecture follows the classic radix-2 DIT structure:

![FFT Flow Diagram](https://static.jyshare.com/images/mix/uuE5FKpYLflJt5EF.jpg)


## Project Structure

```
├── _netx.toml             # NetX project configuration
├── main.cpp               # C++ simulation testbench
├── Makefile               # Build configuration
└── build/                 # Build artifacts and outputs
```
## Getting Started

### Prerequisites
- NetX compiler and runtime
- C++23 compatible compiler (clang++ recommended)

### Running Tests
```bash
make run
```

This will:
1. Compile the NetX design to a circuit representation
2. Build the C++ testbench
3. Run the simulation with test vectors

## Development

### Core Component
The 8-point FFT is implemented using a series of butterfly units, each performing the core FFT computation:
```
Y_p = X_p + X_q × W_N^k
Y_q = X_p - X_q × W_N^k
```
where `X_p`, `X_q` are complex inputs; `Y_p`, `Y_q` are complex outputs; and `W_N^k` is the complex twiddle factor.

Here, the input/output are 48-bit fixed-point signed complex numbers (24-bit real + 24-bit imaginary), and the twiddle factors are represented as 16-bit fixed-point signed complex coefficients. 
Note that for the internal calculations, we set the precision to 40 bits to prevent overflow.

Each butterfly component takes 3 clock cycles to complete, with a total of 9 clock cycles required for the entire FFT operation.

### Test Framework
This project includes a C++ testbench (`main.cpp`) that

- customizes the reset behavior by inheriting from the base rule, using an if-else structure rather than the default multi-gate approach to streamline the logic. This allows for the branch prediction in modern processors to take effect and improves simulation performance.

  This approach is known as "native modeling" in NetX, where we use C++ to implement the simulation logic for certain components. This allows simulation of unavailable components (e.g., IP cores) and can improve simulation performance. Also, this is useful for system-level simulation with verified sub-components.

- Provides simulation stimuli to the FFT core, including input patterns and control signals. The expected outputs of the final cycle (the 65536th cycle) should be 

  ```
  24'b010000001111111001111111 (4259455) 24'b001011011111111001100010 (3014242)
  24'b110111110010110100001111 (-2151153) 24'b111000111001011100000111 (-1861881)
  24'b111100110000000001001101 (-851891) 24'b111110000000000001110000 (-524176)
  24'b111011110010110001110111 (-1102729) 24'b000110000110100100110100 (1599796)
  24'b001110001111110111010111 (3734999) 24'b111110100000000000110110 (-393162)
  24'b111001001101010100001100 (-1780468) 24'b111001100110101011011110 (-1676578)
  24'b111100110000000001001101 (-851891) 24'b111110000000000000100000 (-524256)
  24'b111101001101010010000100 (-732028) 24'b000101011001010100101101 (1414445)
  ```

  
