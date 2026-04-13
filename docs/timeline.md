# Our Roadmap (CS520)

This is the plan for getting the Algorithm Visualizer done by the end of the semester.

## Who's doing what:
- **Matt**: High-level architecture, tech stack writeup, header files, and Doxygen docs.
- **Aiden**: UI/frontend, getting ImGui and dependencies working.
- **Deepak**: The data model and the backend classes.
- **Johan**: The timeline/roadmap, implementing core algorithms (BFS/DFS), and unit testing.

## The Schedule:

### Sprint 2: Midpoint Check-in (Deadline: April 13)
*Goal: Get the core structure built and show that the back-end actually works.*
- [x] **Design Docs**: Finish the architecture diagram, UI mockups, and tech stack justifications.
- [x] **Core Model**: Get the `Graph`, `Node`, and `Edge` classes setup.
- [x] **First Algorithm**: Implement BFS using the Strategy and Memento patterns so we can "step" through it.
- [x] **Testing**: Get Google Test combined into the build and verify the logic.
- [x] **Midpoint Fair**: Show it off and get feedback.
- [ ] **Submission**: Get the 5-10 page PDF and the demo video uploaded.

### Sprint 3: Adding Features (April 14 – May 1)
*Goal: More algorithms and the playback engine.*
- [ ] **More Algorithms**: Add DFS, Dijkstra’s, and Kruskal’s.
- [ ] **Playback Code**: Write the system to move forward and backward through the simulation snapshots.
- [ ] **Graph Tools**: Build the UI buttons for adding nodes/edges manually.
- [ ] **Quiz Mode**: Build the "guess the algorithm" game.
- [ ] **Real-time updates**: Use an Observer pattern to make the View redraw automatically when the Model changes.
- [ ] **UI Polish**: Clean up the ImGui menus for metrics and settings.

### Sprint 4: Final Polish (May 2 – End of Term)
*Goal: Make it fast and fix all the bugs.*
- [ ] **Side-by-Side**: Get two algorithms running next to each other for comparison.
- [ ] **Save/Load**: Let users save their graph setups to a file.
- [ ] **Speed**: Optimize the OpenGL drawing so we hit at least 30 FPS.
- [ ] **User Testing**: Have some other CS students try it out.
- [ ] **Final Docs**: Run Doxygen for the last time and clean up the code comments.
- [ ] **Final Demo**: Record the project video and get ready for the presentation.

## Things we're worried about:
1. **Memory/Snapshots**: Storing a full snapshot for every step might get heavy if the graph is huge. We might need to switch to "delta" states later if it lags.
2. **UI Clutter**: ImGui can get messy fast with lots of windows. Need to keep our overlays organized so students aren't confused.
3. **Async Logic**: Keeping the animation loop smooth while the algorithm is crunching numbers.
