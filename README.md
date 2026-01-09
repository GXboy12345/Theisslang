# Theisslang

A Turing-complete programming language where all operations are variations of "Mr. Theiss". Compiles to Brainfuck for execution.

## Why?

Because why not? It's a joke language that's actually capable of computing anything a Turing machine can compute.

## Token Reference

| Theisslang Token   | Brainfuck | Description |
|--------------------|-----------|-------------|
| `Mr. Theiss`       | `>`       | Move pointer right |
| `Mr Theiss`        | `<`       | Move pointer left |
| `MR. THEISS!`      | `+`       | Increment current cell |
| `mr. theiss...`    | `-`       | Decrement current cell |
| `Mister Theiss?`   | `.`       | Output current cell |
| `Mister Theiss.`   | `,`       | Input into current cell |
| `Professor Theiss` | `[`       | Loop start |
| `Student Theiss`   | `]`       | Loop end |

## Run-Length Encoding

For convenience, you can repeat operations:

- `MR. THEISS!!!` → `+++` (increment 3 times)
- `mr. theiss......` → `------` (decrement 6 times)

## Comments

- Line comments: `# this is ignored`
- Block comments: `(this is also ignored)`

## Examples

See the `examples/` directory for sample programs.

## Usage

### Using Python Interpreter

```bash
# Run a program file
python3 theisslang.py examples/cat.theiss

# Execute code directly
python3 theisslang.py -e "MR. THEISS! Mister Theiss?"

# Provide input
python3 theisslang.py examples/cat.theiss "Hello World"
```

### Using Standalone Executable (Python-based)

First, build the executable:

```bash
make build
# or
python3 build.py
```

Then use it:

```bash
# Run a program
./dist/theisslang examples/hello_world.theiss

# Execute code directly
./dist/theisslang -e "MR. THEISS! Mister Theiss?"

# With input
./dist/theisslang examples/cat.theiss "Hello"
```

### Using Native Executable (C-based, Recommended)

For maximum performance and minimal size, use the native C version:

```bash
make build-native  # Builds 34KB native executable
```

Then use it:

```bash
# Run a program
./theisslang-native examples/hello_world.theiss

# Execute code directly
./theisslang-native -e "MR. THEISS! Mister Theiss?"

# With input
./theisslang-native examples/cat.theiss "Hello"
```

### System-wide Installation

```bash
make install       # Python version (8.1MB)
# or
make install-native # Native version (34KB, recommended)
theisslang examples/hello_world.theiss
```

## Turing Completeness

Theisslang is isomorphic to Brainfuck, which is Turing-complete. Any Brainfuck program can be translated to Theisslang by replacing each BF instruction with its corresponding Theiss token.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.