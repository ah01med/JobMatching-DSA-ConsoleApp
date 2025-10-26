# 🧠 JobMatchingDSA – Data Structures & Algorithms Project

## 📘 Overview

**JobMatchingDSA** is a C++ console application designed to simulate a job–resume matching system using two fundamental data structures — **Linked Lists** and **Arrays** — without using any STL containers.

The system allows users to load job and resume datasets, filter and match candidates by skills, perform searches, and compare performance metrics between linked lists and arrays.  
All operations are implemented using **custom-built data structures**, with sorting and searching algorithms crafted manually.

---

## 🧩 Core Objectives

1. Implement **custom Linked List** and **Array-based data structures**.
2. Load and process datasets from CSV files (`resume.csv`, `job_description.csv`).
3. Perform **matching between resumes and job descriptions** using the **Jaccard Similarity** algorithm.
4. Implement **searching and sorting algorithms**:
   - Linked List → **Iterative Merge Sort**
   - Array - **Quick Sort** and **Bubble Sort**
5. Provide a **performance comparison** (time & memory) between Linked List and Array operations.
6. Build a **menu-driven interface** to test and demonstrate all functions.

---

## 🏗️ Folder Structure

Repository/
├── JobMatchingDSA/
│ ├── data/ # Contains input CSV datasets
│ │ ├── resume.csv
│ │ └── job_description.csv
│ ├── src/
│ │ ├── arrays/ # Array-based components (quicksort, bubble sort, array search)
│ │ ├── io/ # CSV reading & data input utilities
│ │ ├── models/ # Linked List classes (JobLinkedList, ResumeLinkedList)
│ │ └── utils/ # Matching logic (Jaccard similarity), helper utilities
│ ├── x64/Debug/ # Compiled .exe and object files (auto-generated)
│ ├── JobMatchingDSA.sln # Visual Studio Solution file
│ ├── JobMatchingDSA.cpp # Main file containing the menu and program flow
│ └── README.md # Project documentation (this file)
└── .gitignore

## ⚙️ Features & Menu Options

When you run the program (`Ctrl + F5`), you’ll see the following interactive menu:

=== Job Matching DSA (Linked-List Core) ===

         "1) Load CSV data
         "2) Show resumes
         "3) Filter resumes by SKILL and sort by name (top 3 + jobs)
         "4) Match resumes -> jobs
         "5) Show jobs by TITLE keyword (paged x3)
         "6) Search job by ID
         "7) Search jobs by KEYWORD (paged x3)
         "8) Performance comparison (LL vs Array)
         "9) Array (load → quicksort → top 5)
         "0) Exit


### Example Workflows

####  Option 1: Load CSV Data
Loads the datasets from `/data/` and populates both array and linked list structures.

####  Option 3: Filter by Skill
Displays top resumes matching a keyword and recommends top 3 related jobs.

####  Option 4: Match Resume → Jobs
Uses **Jaccard Similarity** to measure how similar a candidate’s skills are to job descriptions.

####  Option 8: Performance Comparison
Compares:
- `[LL]` Merge Sort time (ms)
- `[Array]` Quick Sort time (ms)
- `[Array]` Bubble Sort baseline
- Optional linear search time & memory estimates

####  Option 9: Array Demo
Demonstrates array sorting & shows top 5 entries after quicksort.

---

## Algorithms Implemented

| Type | Algorithm | Data Structure | Purpose |
|------|------------|----------------|----------|
| Sorting | **Iterative Merge Sort** | Linked List | Sort resumes by name |
| Sorting | **Quick Sort / Bubble Sort** | Array | Sort job or resume arrays |
| Search | **Linear Search** | Array & Linked List | Keyword & ID search |
| Matching | **Jaccard Similarity** | Resume vs Job Skills | Match candidates to best jobs |

---

##  Performance Metrics

| Operation | Linked List (ms) | Array (ms) | Observation |
|------------|------------------|-------------|--------------|
| Sorting (n=500) | Merge Sort ≈ 2.3 ms | QuickSort ≈ 1.7 ms | Arrays are faster for sorting |
| Search | 0.6 ms | 0.5 ms | Comparable for small n |
| Memory | n*(node+ptr) | n*(object size) | LL uses slightly more memory but flexible size |

*(values vary by dataset size and machine speed)*

---

##  Team Presentation & Workload Matrix

| Member | Component | Code Folder | Key Algorithm / Functionality | Presentation Focus |
|:--|:--|:--|:--|:--|
| **Member 1** | Array Data Structure – Resumes | `/src/arrays/` | Array storage + QuickSort + BubbleSort + search | Explain array operations, efficiency, and how sorting works. |
| **Member 2** | Array Data Structure – Jobs | `/src/arrays/` | Job array search & filter logic | Demonstrate how arrays are used for job listing and title keyword search. |
| **Member 3** | Linked List – Jobs + Partial Match Logic | `/src/models/`, `/src/utils/` | `JobLinkedList` class (insert, traverse) + contribution to match logic | Show how jobs are linked, traversed, and partially matched using logic functions. |
| **Member 4** *(Lead)* | Linked List – Resumes + Matching Algorithm + CSV Integration + Main Menu | `/src/models/`, `/src/utils/`, `/src/io/` | `ResumeLinkedList`, `MatchLogic.cpp`, `CsvReader.cpp`, and `JobMatchingDSA.cpp` | Present full system integration, data flow, Jaccard similarity, and demo all features. |

---

##  Demonstration Plan

| Step | Description | Responsible |
|------|--------------|--------------|
| 1️⃣ | Load dataset and preview contents | Member 4 |
| 2️⃣ | Run Array demo (Option 9) | Member 1 |
| 3️⃣ | Demonstrate job search and keyword filtering | Member 2 |
| 4️⃣ | Show linked list sorting & traversal | Member 3 |
| 5️⃣ | Run resume-job matching (Option 4) | Member 3 + 4 |
| 6️⃣ | Run performance comparison (Option 8) | Member 1 + 4 |
| 7️⃣ | Conclude with system summary and error handling demo | Member 4 |

---

##  Workload Matrix (Documentation Summary)

| Member | Work Section / Task | Files Modified | Contribution % |
|:--|:--|:--|:--:|
| **1** | Array Resume module + sorting algorithms + Option 9 demo | `ArrayJobs.cpp`, `ArraySimilarity.cpp`, `JobMatchingDSA.cpp` (case 8/9) | 25 % |
| **2** | Array Job module + search/filter logic | `ArraySearch.cpp`, `JobArray.cpp` | 25 % |
| **3** | Job LinkedList + partial match logic integration | `JobLinkedList.cpp`, `MatchLogic.cpp` | 25 % |
| **4** *(Lead)* | Resume LinkedList + Jaccard Matching + CSV Reader + Main Menu Integration | `ResumeLinkedList.cpp`, `CsvReader.cpp`, `MatchLogic.cpp`, `JobMatchingDSA.cpp` | 25 % |

---

##  Jaccard Similarity – Matching Formula

The system uses **Jaccard Index** to compute similarity between a resume’s skills and a job’s required skills:

\[
J(A, B) = \frac{|A \cap B|}{|A \cup B|}
\]

- **A** = Set of candidate’s skills  
- **B** = Set of job required skills  
- Output: 0 → no similarity, 1 → perfect match

---

##  How to Run the Program (Team Setup)

1. Install **Visual Studio 2022** with “Desktop Development with C++”.
2. Copy the entire `Repository` folder to your PC.
3. Open `JobMatchingDSA.sln` in Visual Studio.
4. Build configuration: **Debug | x64**
5. Press **Ctrl + F5** (Run without debugging).
6. Start from Option **1** to load CSV data.

---

##  Tools Used

- **Language:** C++14 (no STL containers)
- **IDE:** Microsoft Visual Studio 2022
- **Dataset:** CSV files (resumes & job descriptions)
- **Algorithms:** Merge Sort, Quick Sort, Bubble Sort, Linear Search, Jaccard Similarity
- **Platform:** Console-based application (Windows)

---

##  Conclusion

This project successfully demonstrates the practical use of **Data Structures & Algorithms (DSA)** through a real-world simulation of job-resume matching.  
It provides a balanced implementation of **Linked List** and **Array** structures, with algorithmic comparisons on performance, efficiency, and scalability.

The system is modular, STL-free, and fully compliant with project guidelines — ready for demonstration, report submission, and further scalability testing.

---

###  Developed by:
**Team JobMatching**  
Asia Pacific University (APU)  
Faculty of Computing, Engineering & Technology  
Module: *Data Structures & Algorithms (DSA)*  
Semester: **AUG 2025**

---
