# MGS_LOOKALIKE_Level

## 📝 Project Overview
Made a level for a Third Person Shooter, heavily inspired from *Metal Gear Solid 3: Snake Eater*. Also did the programming for an intelligent enemy AI, and perfectly incorporated the animations and other functionalities for the Player Character.

This repository showcases the **C++ Source Code** powering the game. It demonstrates a strong grasp of Object-Oriented Programming (OOP) principles, Unreal Engine's gameplay framework, advanced Artificial Intelligence, and seamless integration between C++ backend logic and Unreal Engine's UI systems.

---

## 🎮 Playable Demo
*   **Link:** [Download Playable Demo Here](https://drive.google.com/file/d/1wRTKue0tEtnluk9pksQIVYdBGXagDmg6/view?usp=sharing)
*   **Instructions:** Downloadable Zip File. Download > Extract > Open `.exe` file.

---

## 🧠 Technical Highlights & Architecture

### 1. Extremely Intelligent AI System
The game features a custom AI framework managed via C++ that drives three distinct enemy archetypes, making the environment unpredictable and dangerous.
*   **Patrolling Guards:** Programmed with complex, node-based pathfinding logic to traverse set routes and investigate audio/visual disturbances.
*   **Snipers:** Utilize long-range line-of-sight checks and specialized detection cones from elevated vantage points, locking down specific zones.
*   **Area Traverse Guards:** Employ dynamic, randomized sweep-and-search algorithms to actively hunt the player within designated sectors, requiring the player to constantly adapt.

### 2. Deep Stealth & Player Mechanics
The `APlayerCharacter` class handles a robust suite of stealth mechanics. It calculates visibility based on environmental cover (the forest and military camp assets) and manages the player's stance (crouching, crawling) to manipulate line-of-sight and evasion variables against the enemy AI. Animations are perfectly incorporated into the C++ state machine for smooth transitions.

### 3. Custom UI & Input Architecture
Instead of handling UI logic purely in Blueprints, the project uses a dedicated C++ `AUIHandler` (inheriting from `APlayerController`). 
*   **Safe Widget Management:** Spawns and manages UMG widgets (Start Screen, Pause Menu) directly from C++, preventing memory leaks and infinite loading loops.
*   **Input Mode Switching:** Seamlessly toggles between `SetInputModeUIOnly` and `SetInputModeGameOnly`, ensuring the mouse cursor and player inputs are strictly controlled based on the game state.
*   **Enhanced Input Integration:** Fully utilizes UE5's Enhanced Input system for scalable, modular control mappings.

---

## 🧱 Core C++ Class Breakdown

Below is an overview of the primary classes available in the `/Source/` directory:

| Class Name | Inheritance | Primary Responsibilities |
| :--- | :--- | :--- |
| **`APlayerCharacter`** | `ACharacter` | Handles movement, stealth state calculations, animation variables, camera control, and routing interaction inputs. |
| **`AEnemyAIController`** | `AAIController` | The brain behind the AI. Manages state machines, sensory perception (Sight/Hearing), and the distinct pathing logic for the three enemy types. |
| **`AUIHandler`** | `APlayerController` | Central hub for UI. Executes `StartGameplay()`, toggles the Pause state, and manages widget focus/visibility. |
| **`AIntelItem`** | `AActor` | An interactable collectible. Uses `UBoxComponent` for overlap events to detect the player and update mission objectives via C++ delegates. |
| **`AExtractionTrigger`** | `AActor` | Defines the win-state volume. Listens for player overlap and triggers the game-cleared sequence, communicating with the UI Handler to display the end screen. |

---

## 📂 Code Navigation
To review the code, navigate to the `/Source/MGS_PROJECT_FIRST/` directory. 
*   **Public Folder:** Contains all header (`.h`) files defining the class structures, `UPROPERTY` macros, and function declarations.
*   **Private Folder:** Contains all implementation (`.cpp`) files containing the core logic, AI pathfinding math, and widget management.

## 🛠️ Built With
*   **Engine:** Unreal Engine 5 (C++ & Blueprints)
*   **IDE:** Microsoft Visual Studio / JetBrains Rider
*   **Core Systems:** UE5 Enhanced Input, UMG, AI Behavior Trees / Blackboards, Navigation Mesh (NavMesh)
