# AlgorithmVisualizer
Our project is an algorithm visualization learning tool for computer science students in the Five Colleges. Many students learn algorithms through lectures, textbook diagrams, and code examples, but those can make it difficult to build intuition about how an algorithm actually works step by step. Our system takes this problem by allowing users to create their own graphs, run algorithms visually in real time, and do various other learning activities such as quizzes.

The target users are primarily computer science students, especially those taking courses such as CS 311 or other classes involving data structures and algorithms. Secondary stakeholders include instructors and TAs who may use the system for demonstrations or classroom support. The main objective is to help students better understand algorithm behavior, algorithm efficiency, and tradeoffs through interactive simulation instead of only static examples. This system serves the Five College community by providing a shared educational tool that supports algorithm learning across multiple campuses, while also reinforcing our own understanding as we build it.

# Developer Setup
### 1. Install Doxygen (For Documentation)
* **Mac:** `brew install doxygen`
* **Windows:** Download the installer from [Doxygen's website](https://www.doxygen.nl/download.html) and add it to your System PATH.
* **Linux:** `sudo apt-get install doxygen`

### 2. Generating Docs Locally
Once installed, open your terminal, navigate to the `docs/` folder, and run:
`doxygen Doxyfile`
This will generate an `html/` folder. Open `docs/html/index.html` in your browser to view the documentation.

## Description

AlgorithmVisualizer is a C++ algorithm learning tool that lets students build graphs, run graph algorithms step by step, compare strategies side by side, inspect metrics, save scenarios, and use quiz-style identification exercises.

The repository now contains two runnable front ends:

- `bin/algorithm_visualizer_gui`: the SFML desktop simulator
- `bin/algorithm_visualizer`: the CLI/debug renderer

This implementation follows the architecture described in the project PDFs:

- Model-View-Controller separation
- Strategy pattern for algorithms
- Observer pattern between the graph model and renderer
- Memento-style playback history for stepping through algorithm state

## Implemented Features

- Custom graph creation through the `Graph` model and the CLI/controller layer
- Real-time-style step playback via recorded visualization frames
- Side-by-side comparison for two algorithms on the same input graph
- Playback controls: pause, resume, step forward, step backward, reset
- Metrics: steps, visited nodes, edges considered, relaxations, comparisons, MST edges, total weight, elapsed time
- Save/load scenario support
- Quiz generation and answer checking
- Core graph algorithms:
  - BFS
  - DFS
  - Dijkstra
  - Kruskal

## Project Layout

- `include/Model`, `src/Model`: graph domain objects and observer-aware graph state
- `include/Algorithm`, `src/Algorithm`: algorithm interface and concrete strategies
- `include/Playback`, `src/Playback`: playback session management
- `include/View`, `src/View`: dependency-free textual renderer
- `include/Controller`, `src/Controller`: orchestration, comparisons, scenario persistence, quiz flow
- `src/main.cpp`: CLI entry point
- `src/gui_main.cpp`: SFML desktop application entry point
- `tests/test_main.cpp`: regression tests

## Build

The repository includes a simple `Makefile`.

```bash
make
make test
make coverage
```

`make` builds both the CLI and GUI binaries. The GUI build expects SFML to be installed. On macOS with Homebrew:

```bash
brew install sfml
```

## Usage

Desktop simulator:

```bash
./bin/algorithm_visualizer_gui
```

CLI/debug renderer:

```bash
./bin/algorithm_visualizer help
./bin/algorithm_visualizer show
./bin/algorithm_visualizer demo
./bin/algorithm_visualizer compare BFS Dijkstra A E
./bin/algorithm_visualizer quiz BFS A BFS
./bin/algorithm_visualizer save sample.scn
./bin/algorithm_visualizer load sample.scn
```

## GUI Controls

- `Mode: Select / Move`: click nodes or edges to select them, drag nodes to reposition them
- `Mode: Add Node`: click the canvas to create a node
- `Mode: Add Edge`: click two nodes to connect them
- `Mode: Delete`: click a node or edge to remove it
- `Set Start Node` / `Set Goal Node`: choose the selected node as the source or destination
- `Primary` / `Secondary` algorithm buttons: choose algorithms for single-run or comparison mode
- `Comparison: On`: render two algorithms side by side on the same graph
- `Run`, `Play / Resume`, `Step Back`, `Step Forward`, `Reset Playback`: playback controls
- `Save Scenario` / `Load Scenario`: persist the current graph to `saved/scenario.scn`
- `Start Quiz`: generate a random compatible algorithm quiz from the current graph

Keyboard shortcuts:

- `Space`: run or play/pause
- `Left` / `Right`: step backward or forward
- `1` `2` `3` `4`: select BFS, DFS, Dijkstra, or Kruskal
- `Delete` / `Backspace`: delete the current selection
- `Ctrl+S` / `Ctrl+L`: save or load the scenario

## Documentation

A Doxygen configuration is provided in `docs/Doxyfile`.

```bash
doxygen docs/Doxyfile
```

Coverage output can be generated with:

```bash
make coverage
```

This produces a `coverage/summary.txt` report and the raw `gcov` files under `coverage/gcov/`.

## Notes

The original project brief mentioned Dear ImGui and a lightweight graphics library. This implementation now uses SFML for the desktop simulator while keeping the original core logic reusable and testable outside the GUI.

