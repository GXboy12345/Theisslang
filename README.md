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

```bash
# Run a program file
python theisslang.py examples/cat.theiss

# Execute code directly
python theisslang.py -e "MR. THEISS! Mister Theiss?"

# Provide input
python theisslang.py examples/cat.theiss "Hello World"
```

## Turing Completeness

Theisslang is isomorphic to Brainfuck, which is Turing-complete. Any Brainfuck program can be translated to Theisslang by replacing each BF instruction with its corresponding Theiss token.

Funny

Why not license this? MIT LICENSE!!

# LICENSES
MIT LICENSE