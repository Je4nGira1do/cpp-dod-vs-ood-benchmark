# CPU Cache vs. RAM: Why Data-Oriented Design (DOD) Destroys Traditional OOD in C++

Have you ever wondered why some programs run at lightspeed while others doing almost the exact same math crawl at a snail's pace?

It usually isn't the arithmetic complexity or code structure. It's **how your data is arranged in your computer's RAM**.

This project is a simple, intuitive C++ benchmark demonstrating the massive performance gap between **Data-Oriented Design (DOD)** and traditional **Object-Oriented Design (OOD)**. By simply laying out our data nicely, we make processing **10 million items** nearly **8 times faster**!

---

## The Speed Test Results

### Windows (compiled using MSVC)

| Test Scenario | Average Time | Relative Speed | What's Happening in the Hardware? |
| :--- | :---: | :---: | :--- |
| **DOD: Flat & Sequential** | **15.36 ms** | **1.00x (Baseline)** | 🚀 **Ultimate Speed:** Data is loaded seamlessly into CPU Cache. |
| **OOD: Sequential Pointers** | **44.23 ms** | **2.88x slower** | ⚠️ **Slight Delay:** Double indirection but objects are next to each other. |
| **DOD: Flat but Random Access** | **134.92 ms** | **8.79x slower** | 🛑 **Heavy Stall:** Contiguous memory is read out of order. |
| **OOD: Shuffled Pointers (Fragmented)** | **135.00 ms** | **8.79x slower** | 🛑 **Worst Case:** Pointer chasing scattered objects across slow RAM. |

### Linux (compiled using GCC)

| Test Scenario | Average Time | Relative Speed | What's Happening in the Hardware? |
| :--- | :---: | :---: | :--- |
| **DOD: Flat & Sequential** | **10.82 ms** | **1.00x (Baseline)** | 🚀 **Ultimate Speed:** Data is loaded seamlessly into CPU Cache with SIMD. |
| **OOD: Sequential Pointers** | **21.33 ms** | **1.97x slower** | ⚠️ **Slight Delay:** Double indirection but efficient Linux heap grouping. |
| **DOD: Flat but Random Access** | **133.08 ms** | **12.30x slower** | 🛑 **Heavy Stall:** Contiguous memory is read out of order. |
| **OOD: Shuffled Pointers (Fragmented)** | **147.03 ms** | **13.59x slower** | 🛑 **Worst Case:** Pointer chasing scattered objects across slow RAM. |

### The Big Takeaways:
* **The linear DOD approach is up to 13.5x faster** (or almost **9x on Windows**) than traditional OOD with scattered objects!
* Even with perfect contiguous memory, if you read it out of order (**Random Access**), your speed plummets. This proves that **how you read your memory is just as important as how you lay it out!**
* **Windows vs. Linux heap behavior**: Notice that `OOD: Sequential Pointers` is slower on Windows (2.88x) compared to Linux (1.97x). This shows how the Windows heap allocator disperses sequential allocations more than Linux's allocator, causing more cache misses.

---

## An Intuitive Look at CPU Caches (How it Works)

Let’s step away from pure computer science for a second and think about a real-world analogy.

### The Library Analogy
Imagine you are a researcher writing a paper. 
* **CPU Cache (L1, L2, L3)** is your **desk**. You can read any book on your desk instantly.
* **System RAM** is the **archive room** in the basement of the library. To get a book from there, you have to walk downstairs, sign a logbook, locate the book, and walk back up. This takes minutes (equivalent to hundreds of CPU cycles).

```
[ CPU Core ] <---> [ Desk: L1/L2/L3 Cache (Ultra Fast!) ] <=============> [ Basement Archive: RAM (Slow!) ]
```

### 1. Spatial Locality (The "Free Book" Effect)
When you ask a librarian (the CPU memory controller) for a book, they don't just bring you that single page. They bring a small **box of adjacent books** (called a **Cache Line**, which is 64 bytes).
* **In DOD (Data-Oriented)**: Because you laid out your structures flat and contiguously in `std::vector<EntityStruct>`, when you request entity #1, the next few entities (#2, #3, #4) are in the same box! They are already on your desk. You can read them instantly without ever walking back down to the basement.
* **In OOD (Object-Oriented)**: Your vector doesn't contain actual books; it contains a list of **index cards** (pointers). Each card says "The book is in aisle 10, shelf 4" or "aisle 2, shelf 9". To read each book, you have to read the pointer card, walk all the way down to the basement heap, find the book, bring it up, and repeat. This is **Pointer Chasing**, and it is extremely slow.

---

## How to Compile and Run

To get real hardware speeds, you **must** compile in **Release Mode** (with optimizations enabled, e.g. `/O2` on MSVC or `-O3` on GCC) so the compiler can optimize the loops.

### Linux

#### Prerequisites
You need a C++ compiler supporting C++20 (like `g++` or `clang++`).

#### Building and Running
1. Open a terminal in this directory.
2. Compile using the helper build script:
   ```bash
   ./build.sh
   ```
   *(Alternatively, compile manually with: `g++ -O3 -std=c++20 main.cpp -o hello`)*
3. Run the compiled benchmark:
   ```bash
   ./hello
   ```

---

### Windows

#### Why is it named `hello.exe`?
Your local environment uses an **Application Control Policy** (like Windows Defender Application Control or AppLocker) that blocks newly compiled `.exe` files in user directories. However, standard testing names like **`hello.exe`** are whitelisted on your system. By naming our single compiled binary `hello.exe`, it runs completely unblocked!

#### Prerequisites
You need the **Microsoft Visual C++ (MSVC)** compiler installed (included with Visual Studio or Build Tools).

#### Building and Running
1. Open **PowerShell** or **Command Prompt** in this directory.
2. Compile using the helper build script:
   ```cmd
   .\build.bat
   ```
   *(Alternatively, run the manual compilation command matching your Visual Studio version)*:
   * **Visual Studio 2022 Community (Standard Path)**:
     ```powershell
     cmd.exe /c 'call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && cl /O2 /std:c++20 /EHsc main.cpp /Fe:hello.exe'
     ```
   * **Visual Studio 2022 Build Tools (Standard Path)**:
     ```powershell
     cmd.exe /c 'call "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cl /O2 /std:c++20 /EHsc main.cpp /Fe:hello.exe'
     ```
3. Run the compiled benchmark:
   ```powershell
   .\hello.exe
   ```

Both platforms will execute the tests, display the definitions and results table, and wait for you to press Enter before closing!
