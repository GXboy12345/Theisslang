#!/usr/bin/env python3
"""
Theisslang Interpreter v0

A Turing-complete programming language where all operations are variations of "Mr. Theiss".
Compiles to Brainfuck for execution.

Token mapping:
- Mr. Theiss      -> > (move pointer right)
- Mr Theiss       -> < (move pointer left)
- MR. THEISS!     -> + (increment cell)
- mr. theiss...   -> - (decrement cell)
- Mister Theiss?  -> . (output cell)
- Mister Theiss.  -> , (input cell)
- Professor Theiss -> [ (loop start)
- Student Theiss   -> ] (loop end)
"""

from dataclasses import dataclass
import sys

# Core token mapping to Brainfuck instructions
TOKENS = {
    "Mr. Theiss": ">",
    "Mr Theiss": "<",
    "MR. THEISS!": "+",
    "mr. theiss...": "-",
    "Mister Theiss?": ".",
    "Mister Theiss.": ",",
    "Professor Theiss": "[",
    "Student Theiss": "]",
}

def strip_paren_comments(src: str) -> str:
    """Remove parenthetical comments from source code."""
    out = []
    depth = 0
    for ch in src:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth = max(0, depth - 1)
        elif depth == 0:
            out.append(ch)
    return "".join(out)

def expand_emphasis(token: str) -> str:
    """Handle run-length encoding for repeated operations.

    MR. THEISS!!! -> +++ (3 increments)
    mr. theiss...... -> ------ (6 decrements)
    """
    # Handle increment repetition: MR. THEISS! with multiple !
    if token.startswith("MR. THEISS") and token.endswith("!"):
        bangs = token.count("!")
        return "+" * max(1, bangs)

    # Handle decrement repetition: mr. theiss... with multiple .
    if token.startswith("mr. theiss") and token.endswith("."):
        dots = len(token) - len(token.rstrip("."))
        return "-" * max(1, dots)

    return None  # no expansion

def theiss_to_bf(src: str) -> str:
    """Translate theisslang source code to Brainfuck."""
    src = strip_paren_comments(src)
    bf = []

    # Process line by line, handling # comments
    for raw_line in src.splitlines():
        line = raw_line.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()

        # Greedily match tokens from left to right
        i = 0
        while i < len(parts):
            matched = False

            # Try emphasis special-cases by joining progressively
            # Check up to 3 words ahead for multi-word tokens
            for j in range(min(len(parts), i + 4), i, -1):
                chunk = " ".join(parts[i:j])

                # Try emphasis expansion first
                exp = expand_emphasis(chunk)
                if exp is not None:
                    bf.append(exp)
                    i = j
                    matched = True
                    break

                # Try direct token match
                if chunk in TOKENS:
                    bf.append(TOKENS[chunk])
                    i = j
                    matched = True
                    break

            if not matched:
                # Unknown token -> treat as comment/no-op
                i += 1

    return "".join(bf)

def build_bracket_map(code: str):
    """Build a map of matching brackets for Brainfuck loops."""
    stack = []
    mp = {}
    for i, c in enumerate(code):
        if c == "[":
            stack.append(i)
        elif c == "]":
            if not stack:
                raise SyntaxError(f"Unmatched ']' at position {i}")
            j = stack.pop()
            mp[i] = j
            mp[j] = i
    if stack:
        raise SyntaxError(f"Unmatched '[' at position {stack[-1]}")
    return mp

def run_bf(code: str, inp: bytes = b"", cell_bits: int = 8) -> bytes:
    """Execute Brainfuck code and return output as bytes."""
    tape = [0]
    ptr = 0
    ip = 0
    in_i = 0
    out = bytearray()
    brackets = build_bracket_map(code)

    mod = 1 << cell_bits if cell_bits is not None else None

    while ip < len(code):
        c = code[ip]
        if c == ">":
            ptr += 1
            if ptr == len(tape):
                tape.append(0)
        elif c == "<":
            if ptr == 0:
                tape.insert(0, 0)  # Allow negative pointer movement
            else:
                ptr -= 1
        elif c == "+":
            tape[ptr] = tape[ptr] + 1
            if mod: tape[ptr] %= mod
        elif c == "-":
            tape[ptr] = tape[ptr] - 1
            if mod: tape[ptr] %= mod
        elif c == ".":
            out.append(tape[ptr] & 0xFF)
        elif c == ",":
            if in_i < len(inp):
                tape[ptr] = inp[in_i]
                in_i += 1
            else:
                tape[ptr] = 0  # EOF
        elif c == "[":
            if tape[ptr] == 0:
                ip = brackets[ip]
        elif c == "]":
            if tape[ptr] != 0:
                ip = brackets[ip]
        ip += 1

    return bytes(out)

def run_theisslang(src: str, inp: bytes = b"", cell_bits: int = 8) -> bytes:
    """Compile and run theisslang code."""
    bf_code = theiss_to_bf(src)
    return run_bf(bf_code, inp, cell_bits)

def main():
    """Command-line interface for theisslang interpreter."""
    if len(sys.argv) < 2:
        print("Usage: python theisslang.py <program_file> [input_string]")
        print("Or: python theisslang.py -e 'program code here'")
        sys.exit(1)

    input_data = b""
    if len(sys.argv) >= 3:
        input_data = sys.argv[2].encode('utf-8')

    if sys.argv[1] == "-e":
        # Execute code directly from command line
        if len(sys.argv) < 3:
            print("Usage: python theisslang.py -e 'program code'")
            sys.exit(1)
        program = sys.argv[2]
    else:
        # Read from file
        try:
            with open(sys.argv[1], 'r') as f:
                program = f.read()
        except FileNotFoundError:
            print(f"Error: File '{sys.argv[1]}' not found")
            sys.exit(1)

    try:
        bf_code = theiss_to_bf(program)
        print(f"Compiled to Brainfuck: {bf_code}", file=sys.stderr)

        output = run_bf(bf_code, input_data)
        # Try to decode as UTF-8, fall back to raw bytes
        try:
            print(output.decode('utf-8'), end='')
        except UnicodeDecodeError:
            print(f"Raw output: {output}", file=sys.stderr)

    except SyntaxError as e:
        print(f"Syntax error: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Runtime error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
