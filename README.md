# 25A01 - GROUP 06 - 25125039 - 25125065 -25125071 - 25125078
# Data Structure Visualizer

A C++ interactive visualization tool for core data structures and algorithms, built with **Raylib** and **CMake**.  
This project helps users understand how data structures behave through real-time rendering and step-by-step animations.

---

## Features

### Data Structures
- Doubly Linked List
- Hash Table (Linear Probing, Double Hashing, Quadratic Probing, Chaining)
- AVL Tree
- Dijkstra's Algorithm

### Basic Operations
- Insert, delete elements
- Find nodes with visual traversal
- Update values dynamically
- The logic of finding shortest path (Dijkstra)

### Visualization
- Smooth animations
- Event-driven rendering system
- Adjustable speed slider
- Step-by-step controls (Play / Next / Prev)

### UI System
- Custom UI components (buttons, input box, slider)
- Pseudocode display panel
- Custom font rendering (Roboto / Segoe UI fallback)

---

## Instruction

### Requirements
- CMake ≥ 3.15  
- C++ compiler (GCC / MSVC / Clang)  
- Raylib (auto-fetched via CMake)

### Build (Windows - PowerShell)
powershell
cd <project_root>

#### Clean build (optional)
Remove-Item -Recurse -Force build
#### Configure
cmake -S . -B build
#### Build
cmake --build build
