#!/usr/bin/env python3
"""
Build script for Theisslang executable
"""

import subprocess
import sys
import os

def run_command(cmd, description):
    """Run a command and handle errors."""
    print(f"Running: {description}")
    try:
        result = subprocess.run(cmd, shell=True, check=True, capture_output=True, text=True)
        print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}")
        print(f"stdout: {e.stdout}")
        print(f"stderr: {e.stderr}")
        return False

def main():
    print("Building Theisslang executable...")

    # Create virtual environment if it doesn't exist
    venv_dir = "build_venv"
    if not os.path.exists(venv_dir):
        print("Creating virtual environment...")
        if not run_command(f"python3 -m venv {venv_dir}", "Creating virtual environment"):
            print("Failed to create virtual environment.")
            sys.exit(1)

    # Activate virtual environment and install PyInstaller
    activate_script = os.path.join(venv_dir, "bin", "activate")
    pip_cmd = f"source {activate_script} && python -m pip install pyinstaller"

    # Check if PyInstaller is installed in venv
    try:
        # Try importing from venv
        sys.path.insert(0, os.path.join(venv_dir, "lib", "python3.12", "site-packages"))
        import PyInstaller
        print("PyInstaller is already installed in virtual environment.")
    except ImportError:
        print("Installing PyInstaller in virtual environment...")
        if not run_command(pip_cmd, "Installing PyInstaller in virtual environment"):
            print("Failed to install PyInstaller. Please install it manually.")
            sys.exit(1)

    # Clean previous builds
    print("Cleaning previous builds...")
    run_command("rm -rf build dist *.spec", "Cleaning build directories")

    # Build the executable using venv
    print("Building executable...")
    pyinstaller_cmd = f"source {activate_script} && python -m PyInstaller --onefile --name theisslang theisslang.py"
    if run_command(pyinstaller_cmd, "Building executable with PyInstaller"):
        print("\nBuild completed successfully!")
        print("Executable created: dist/theisslang")

        # Test the executable
        print("\nTesting executable...")
        if os.path.exists("dist/theisslang"):
            run_command("./dist/theisslang --help 2>/dev/null || ./dist/theisslang -e \"MR. THEISS! Mister Theiss?\"", "Testing executable")
        else:
            print("Warning: Executable not found in dist/ directory")

        print("\nTo distribute the executable:")
        print("  - Copy dist/theisslang to your desired location")
        print("  - Make sure it's executable: chmod +x theisslang")
        print("  - Run it: ./theisslang examples/hello_world.theiss")
    else:
        print("Build failed!")
        sys.exit(1)

if __name__ == "__main__":
    main()
