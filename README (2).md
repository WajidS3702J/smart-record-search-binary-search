# Smart Record Searching System using Binary Search

**Subject:** Analysis and Design of Algorithms (AOA) – Mini Project 8
**Module:** 2 – Divide and Conquer Approach
**Language:** C (console, no GUI, no database)

A menu-driven console application that stores student records, keeps them sorted by ID, and finds a record using **binary search** – in both its **iterative** and **recursive** forms – while showing, step by step, how the search interval is halved. It also compares binary search against **linear search** using comparison counts.

---

## 1. What the problem statement asks for

| Requirement in the statement | How this project covers it |
|---|---|
| Manage a sorted list of IDs/records | Records (ID, name, marks) stored in an array; kept sorted by ID |
| Accept records and **validate sorted order or sort them** with a syllabus algorithm | A `sorted` flag is tracked on every insertion; if the list is unsorted, the user is prompted to sort it using **insertion sort** |
| Implement iterative **and** recursive binary search separately | `binarySearchIterative()` and `binarySearchRecursive()` |
| Log `low`, `high`, `mid` at each iteration/call | Trace printed for every step (menu options 5 and 6) |
| Handle duplicates and state which occurrence is returned | Always returns the **first (leftmost) occurrence**; the result also reports how many records share the ID |
| Compare with linear search on the same key | Menu option 8 shows index and comparison count for all three methods |
| Test first, middle, last, absent, single-item cases | Menu option 9 runs 10 built-in test cases (all pass) |

**Expected output covered:** found/not-found status, record and index, low/high/mid trace, comparison counts for binary vs linear.

---

## 2. The idea

Binary search is a **divide and conquer** algorithm:

1. **Divide** – look at the middle element of the current interval.
2. **Conquer** – if it equals the key, done; otherwise discard the half that cannot contain the key and search the other half.
3. **Combine** – nothing to combine; the answer comes straight from the final step.

It only works if the data is **sorted**. That is why the program refuses to run binary search on unsorted data (and offers to sort first).

Recurrence: `T(n) = T(n/2) + O(1)`, which solves to `O(log n)`.

---

## 3. Approach

1. **Record structure** – `{ int id; char name[50]; float marks; }`. The search key is `id`.
2. **Sorted flag** – instead of re-scanning the array before every search (which would cost O(n) and defeat the purpose), the program updates a `sorted` flag in O(1) when a record is added: if the new ID is smaller than the previous last ID, the flag becomes 0.
3. **Insertion sort** (a syllabus algorithm) restores the sorted state. It is stable, so records with equal IDs keep their entry order.
4. **Iterative binary search** – a `while (low <= high)` loop with `mid = low + (high - low) / 2` (avoids integer overflow).
5. **Recursive binary search** – the same logic as a function that calls itself on the left or right half.
6. **Duplicate handling** – when a match is found, the search does **not** stop. It continues into the left half to see if an earlier occurrence exists, so the result is deterministic: always the lowest index. Cost: a few extra comparisons, still `O(log n)`.
7. **Linear search** – scans from index 0, returns the first match; used as the baseline.
8. **Comparison counting** – one comparison = one test of the key against an element (a three-way `<, =, >` outcome counts as one).

---

## 4. Complexity

| Algorithm | Best | Average / Worst | Space |
|---|---|---|---|
| Binary search (iterative) | O(1)* | O(log n) | O(1) |
| Binary search (recursive) | O(1)* | O(log n) | O(log n) – call stack |
| Linear search | O(1) | O(n) | O(1) |
| Insertion sort (preparation) | O(n) | O(n²) | O(1) |

\*With the first-occurrence rule, the search continues after a match, so even a hit costs about `log n` comparisons.

For 10 records, binary search needs at most 4 comparisons while linear search may need 10. For 1,000,000 records: about 20 vs up to 1,000,000.

---

## 5. How to compile and run

```bash
gcc -Wall -Wextra -O2 -o smart_search smart_record_search.c
./smart_search            # Linux / macOS
smart_search.exe          # Windows (MinGW)
```

### Menu

```
1. Add a record
2. Display all records
3. Load sample data
4. Sort records by ID (insertion sort)
5. Binary search - iterative (with trace)
6. Binary search - recursive (with trace)
7. Linear search
8. Compare binary vs linear
9. Run built-in test cases
0. Exit
```

Quick start: choose `3` (load sample data) → `5` (search ID `120`).

---

## 6. Sample output

**Iterative search for ID 120 (a duplicate ID):**

```
  Step 1: low=0  high=9  mid=4  (ID at mid = 120) -> match, check left half for earlier duplicate
  Step 2: low=0  high=3  mid=1  (ID at mid = 105) -> too small, search right half
  Step 3: low=2  high=3  mid=2  (ID at mid = 110) -> too small, search right half
  Step 4: low=3  high=3  mid=3  (ID at mid = 115) -> too small, search right half

  RESULT: FOUND at index 4   (comparisons: 4)
          ID=120  Name=Esha Khan  Marks=82.0
          Note: 2 records share this ID; the FIRST occurrence is returned.
```

**Binary vs linear (ID 999, absent):**

```
  Method               Index      Comparisons
  --------------------------------------------
  Binary (iterative)   -1         4
  Binary (recursive)   -1         4
  Linear               -1         10
```

---

## 7. Test cases (menu option 9)

| Case | Key | Expected index |
|---|---|---|
| First element | 10 | 0 |
| Middle element | 40 | 3 |
| Last element | 70 | 6 |
| Absent – smaller than all | 5 | -1 |
| Absent – between values | 45 | -1 |
| Absent – larger than all | 99 | -1 |
| Single item – present | 42 | 0 |
| Single item – absent | 41 | -1 |
| Duplicates – first occurrence returned | 10 in `{5,10,10,10,20,30}` | 1 |
| Duplicate at start | 5 | 0 |

Each case runs iterative, recursive and linear search and checks that all three agree with the expected result.

---

## 8. Project structure

```
.
├── smart_record_search.c   # complete source code
└── README.md               # this file
```

---

## 9. Limitations

- Up to 100 records (`MAX_RECORDS`, easy to change).
- Search key is the integer ID only.
- Data is held in memory; nothing is saved to disk (no database, as per the problem statement).

## 10. Team

| Name | Role |
|---|---|
| Ansari Wajid Hussain | Implementation + Debugging|
| Sharvan Mishra | Testing + Research |
| Pranshu Wadekar | Documentation |
