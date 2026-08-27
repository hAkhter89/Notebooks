# DS2 – Spring 2026 – L4

This repository contains **practice C++ code examples** for the **Data Structures II (DS2)** course, **Spring 2026**, **L4 section** at **Habib University**.

The goal of this repository is to help students **strengthen their C++ programming skills**, particularly as they relate to **data structures and algorithms** covered in the course.

Students are strongly encouraged to:
- Clone the repository
- Compile and run the code locally
- Modify and experiment with the examples
- Attempt the problems on their own before checking solutions

---

## 📂 Repository Contents
- Practice C++ programs
- Data structure implementations
- Algorithm examples discussed in class
- Supporting build files (Makefiles, etc.)

---

## 🛠 Setting Up `g++`

### 🪟 Windows
Follow the official Visual Studio Code guide for setting up **MinGW and g++** on Windows:

👉 https://code.visualstudio.com/docs/cpp/config-mingw

Make sure `g++` is added to your system **PATH**.

---

### 🐧 Linux
Install using your package manager:

```bash
# Ubuntu / Debian
sudo apt install g++

# Arch
sudo pacman -S gcc

# Fedora
sudo dnf install gcc-c++
```

Verify installation:
```bash
g++ --version
```

---

### 🍎 macOS
Install via **Homebrew**:
```bash
brew install gcc
```

Or install Xcode Command Line Tools:
```bash
xcode-select --install
```

Verify:
```bash
g++ --version
```

---

## ▶️ Compiling and Running Code
Typical compilation command:
```bash
g++ -std=c++20 -Wall -Wextra file.cpp -o file
./file
```

If a `Makefile` is provided:
```bash
make
./program_name
```

---

## ✅ Expectations
- Code shared here is for **practice and learning**
- Students should **not copy blindly**
- Understanding the logic is more important than memorization

## Practice 1 - Sorting algorithms: Bubble sort, Insertion sort and Quick sort 
The repository contains a folder sorts_cpp_algorithms containing empty bubble sort, insertion sort and quick sort functions. Your task is to implement these algorithms in their respective src files.
