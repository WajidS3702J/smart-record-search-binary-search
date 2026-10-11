/*
 * Smart Record Searching System using Binary Search
 * -------------------------------------------------
 * Subject : Analysis and Design of Algorithms (AOA) - Mini Project 8
 * Module  : 2 - Divide and Conquer
 *
 * Features
 *  - Stores student records (ID, name, marks) in an array
 *  - Validates / enforces the "sorted input" precondition (insertion sort)
 *  - Iterative AND recursive binary search, with low/high/mid trace
 *  - Duplicate IDs handled: the FIRST (leftmost) occurrence is always returned
 *  - Linear search on the same key + comparison-count comparison
 *  - Built-in test suite: first, middle, last, absent, single-item, duplicates
 *
 * Compile : gcc -Wall -Wextra -O2 -o smart_search smart_record_search.c
 * Run     : ./smart_search
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORDS 100
#define NAME_LEN    50

typedef struct {
    int   id;
    char  name[NAME_LEN];
    float marks;
} Record;

typedef struct {
    Record data[MAX_RECORDS];
    int    n;
    int    sorted;          /* 1 if data[] is known to be sorted by id */
} Database;

/* ------------------------------------------------------------------ */
/*  Safe input helpers                                                 */
/* ------------------------------------------------------------------ */

static void readLine(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    if (!fgets(buf, (int)size, stdin)) exit(0);       /* EOF -> quit */
    buf[strcspn(buf, "\n")] = '\0';
}

static int readInt(const char *prompt, int min, int max) {
    char buf[64], *end;
    for (;;) {
        readLine(prompt, buf, sizeof buf);
        long v = strtol(buf, &end, 10);
        if (end == buf || *end != '\0')  { printf("  Please enter a whole number.\n"); continue; }
        if (v < min || v > max)          { printf("  Value must be between %d and %d.\n", min, max); continue; }
        return (int)v;
    }
}

static float readMarks(const char *prompt) {
    char buf[64], *end;
    for (;;) {
        readLine(prompt, buf, sizeof buf);
        double v = strtod(buf, &end);
        if (end == buf || *end != '\0')  { printf("  Please enter a number.\n"); continue; }
        if (v < 0 || v > 100)            { printf("  Marks must be between 0 and 100.\n"); continue; }
        return (float)v;
    }
}

/* ------------------------------------------------------------------ */
/*  Database operations                                                */
/* ------------------------------------------------------------------ */

static int addRecord(Database *db, int id, const char *name, float marks) {
    if (db->n >= MAX_RECORDS) return 0;
    Record *r = &db->data[db->n];
    r->id = id;
    strncpy(r->name, name, NAME_LEN - 1);
    r->name[NAME_LEN - 1] = '\0';
    r->marks = marks;
    /* Keep the sorted flag up to date in O(1) instead of rescanning. */
    if (db->n > 0 && db->data[db->n - 1].id > id) db->sorted = 0;
    db->n++;
    return 1;
}

static void displayRecords(const Database *db) {
    if (db->n == 0) { printf("\n  (no records)\n"); return; }
    printf("\n  %-5s %-8s %-25s %-6s\n", "Idx", "ID", "Name", "Marks");
    printf("  ------------------------------------------------\n");
    for (int i = 0; i < db->n; i++)
        printf("  %-5d %-8d %-25s %-6.1f\n", i, db->data[i].id, db->data[i].name, db->data[i].marks);
    printf("  Sorted by ID: %s\n", db->sorted ? "YES" : "NO");
}

/* Insertion sort (syllabus algorithm). Stable, so equal IDs keep input order. */
static void insertionSortById(Database *db) {
    for (int i = 1; i < db->n; i++) {
        Record key = db->data[i];
        int j = i - 1;
        while (j >= 0 && db->data[j].id > key.id) {
            db->data[j + 1] = db->data[j];
            j--;
        }
        db->data[j + 1] = key;
    }
    db->sorted = 1;
}

static void loadSample(Database *db) {
    static const struct { int id; const char *name; float marks; } s[] = {
        {101, "Aarav Sharma", 88.5f}, {105, "Bhavna Patel", 76.0f},
        {110, "Chirag Mehta",  91.2f}, {115, "Divya Nair",   67.5f},
        {120, "Esha Khan",     82.0f}, {120, "Esha K. (dup)", 79.5f},
        {130, "Farhan Ansari", 73.4f}, {142, "Gauri Joshi",  95.0f},
        {150, "Harsh Verma",   58.9f}, {163, "Isha Kulkarni",84.3f}
    };
    db->n = 0;
    db->sorted = 1;
    for (size_t i = 0; i < sizeof s / sizeof s[0]; i++)
        addRecord(db, s[i].id, s[i].name, s[i].marks);
}

/* ------------------------------------------------------------------ */
/*  Searching                                                          */
/*  A "comparison" = one comparison of the search key with an element  */
/*  (a three-way outcome <, =, > counts as one).                       */
/* ------------------------------------------------------------------ */

/* Iterative binary search. Returns index of FIRST occurrence or -1. */
static int binarySearchIterative(const Record *a, int n, int key, int verbose, long *cmp) {
    int low = 0, high = n - 1, result = -1, step = 0;
    *cmp = 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;          /* avoids overflow */
        step++; (*cmp)++;
        if (verbose) printf("  Step %d: low=%d  high=%d  mid=%d  (ID at mid = %d) -> ",
                            step, low, high, mid, a[mid].id);
        if (a[mid].id == key) {
            result = mid;
            high = mid - 1;                         /* keep looking LEFT for an earlier duplicate */
            if (verbose) printf("match, check left half for earlier duplicate\n");
        } else if (a[mid].id < key) {
            low = mid + 1;
            if (verbose) printf("too small, search right half\n");
        } else {
            high = mid - 1;
            if (verbose) printf("too large, search left half\n");
        }
    }
    return result;
}

/* Recursive binary search. Same behaviour: returns FIRST occurrence or -1. */
static int binarySearchRecursive(const Record *a, int low, int high, int key,
                                 int verbose, long *cmp, int depth) {
    if (low > high) return -1;
    int mid = low + (high - low) / 2;
    (*cmp)++;
    if (verbose) printf("  Call %d: low=%d  high=%d  mid=%d  (ID at mid = %d) -> ",
                        depth, low, high, mid, a[mid].id);
    if (a[mid].id == key) {
        if (verbose) printf("match, check left half for earlier duplicate\n");
        int left = binarySearchRecursive(a, low, mid - 1, key, verbose, cmp, depth + 1);
        return (left != -1) ? left : mid;
    }
    if (a[mid].id < key) {
        if (verbose) printf("too small, search right half\n");
        return binarySearchRecursive(a, mid + 1, high, key, verbose, cmp, depth + 1);
    }
    if (verbose) printf("too large, search left half\n");
    return binarySearchRecursive(a, low, mid - 1, key, verbose, cmp, depth + 1);
}

/* Linear search baseline. Returns FIRST occurrence or -1. */
static int linearSearch(const Record *a, int n, int key, long *cmp) {
    *cmp = 0;
    for (int i = 0; i < n; i++) {
        (*cmp)++;
        if (a[i].id == key) return i;
    }
    return -1;
}

/* ------------------------------------------------------------------ */
/*  Menu actions                                                       */
/* ------------------------------------------------------------------ */

static int ensureSorted(Database *db) {
    if (db->n == 0) { printf("\n  No records. Add records or load sample data first.\n"); return 0; }
    if (db->sorted) return 1;
    printf("\n  Binary search needs data sorted by ID, but the list is NOT sorted.\n");
    if (readInt("  Sort it now with insertion sort? (1 = yes, 0 = no): ", 0, 1)) {
        insertionSortById(db);
        printf("  Sorted.\n");
        return 1;
    }
    printf("  Search cancelled (binary search would give wrong answers on unsorted data).\n");
    return 0;
}

static void printResult(const Database *db, int idx, int key, long cmp) {
    if (idx < 0) {
        printf("\n  RESULT: ID %d NOT FOUND   (comparisons: %ld)\n", key, cmp);
        return;
    }
    int dup = 1;
    while (idx + dup < db->n && db->data[idx + dup].id == key) dup++;
    printf("\n  RESULT: FOUND at index %d   (comparisons: %ld)\n", idx, cmp);
    printf("          ID=%d  Name=%s  Marks=%.1f\n", db->data[idx].id, db->data[idx].name, db->data[idx].marks);
    if (dup > 1)
        printf("          Note: %d records share this ID; the FIRST occurrence is returned.\n", dup);
}

static void doBinarySearch(Database *db, int recursive) {
    if (!ensureSorted(db)) return;
    int key = readInt("  Enter ID to search: ", 0, 2000000000);
    long cmp = 0;
    printf("\n  --- %s binary search trace ---\n", recursive ? "Recursive" : "Iterative");
    int idx = recursive ? binarySearchRecursive(db->data, 0, db->n - 1, key, 1, &cmp, 1)
                        : binarySearchIterative(db->data, db->n, key, 1, &cmp);
    printResult(db, idx, key, cmp);
}

static void doLinearSearch(const Database *db) {
    if (db->n == 0) { printf("\n  No records.\n"); return; }
    int key = readInt("  Enter ID to search: ", 0, 2000000000);
    long cmp;
    int idx = linearSearch(db->data, db->n, key, &cmp);
    printf("\n  --- Linear search ---");
    printResult(db, idx, key, cmp);
}

static void doComparison(Database *db) {
    if (!ensureSorted(db)) return;
    int key = readInt("  Enter ID to search: ", 0, 2000000000);
    long ci, cr = 0, cl;       /* recursive search accumulates into cr, so start at 0 */
    int ii = binarySearchIterative(db->data, db->n, key, 0, &ci);
    int ir = binarySearchRecursive(db->data, 0, db->n - 1, key, 0, &cr, 1);
    int il = linearSearch(db->data, db->n, key, &cl);
    printf("\n  Searching ID %d in %d records\n", key, db->n);
    printf("  %-20s %-10s %-12s\n", "Method", "Index", "Comparisons");
    printf("  --------------------------------------------\n");
    printf("  %-20s %-10d %-12ld\n", "Binary (iterative)", ii, ci);
    printf("  %-20s %-10d %-12ld\n", "Binary (recursive)", ir, cr);
    printf("  %-20s %-10d %-12ld\n", "Linear", il, cl);
    if (ii != ir || ii != il) printf("  WARNING: methods disagree - this should never happen!\n");
}

static void doAddRecord(Database *db) {
    if (db->n >= MAX_RECORDS) { printf("\n  Database full (%d records).\n", MAX_RECORDS); return; }
    int id = readInt("  Enter ID (non-negative integer): ", 0, 2000000000);
    char name[NAME_LEN];
    do { readLine("  Enter name: ", name, sizeof name); } while (name[0] == '\0');
    float marks = readMarks("  Enter marks (0-100): ");
    addRecord(db, id, name, marks);
    printf("  Record added.%s\n", db->sorted ? "" : " (List is now unsorted - sort before binary search.)");
}

/* ------------------------------------------------------------------ */
/*  Built-in test suite                                                */
/* ------------------------------------------------------------------ */

static int runCase(const char *label, const int *ids, int n, int key, int expected) {
    Database t = { .n = 0, .sorted = 1 };
    for (int i = 0; i < n; i++) addRecord(&t, ids[i], "test", 0);
    long c1, c2 = 0, c3;
    int a = binarySearchIterative(t.data, t.n, key, 0, &c1);
    int b = binarySearchRecursive(t.data, 0, t.n - 1, key, 0, &c2, 1);
    int c = linearSearch(t.data, t.n, key, &c3);
    int ok = (a == expected && b == expected && c == expected);
    printf("  %-34s key=%-4d expected=%-3d iter=%-3d rec=%-3d lin=%-3d [%s]\n",
           label, key, expected, a, b, c, ok ? "PASS" : "FAIL");
    return ok;
}

static void runTests(void) {
    const int ids[]    = {10, 20, 30, 40, 50, 60, 70};
    const int dups[]   = {5, 10, 10, 10, 20, 30};
    const int single[] = {42};
    int pass = 0, total = 0;
    printf("\n  --- Built-in test cases ---\n");
    #define T(l,arr,n,k,e) do { total++; pass += runCase(l,arr,n,k,e); } while (0)
    T("First element",             ids,    7, 10, 0);
    T("Middle element",            ids,    7, 40, 3);
    T("Last element",              ids,    7, 70, 6);
    T("Absent (smaller than all)", ids,    7,  5, -1);
    T("Absent (between values)",   ids,    7, 45, -1);
    T("Absent (larger than all)",  ids,    7, 99, -1);
    T("Single item - present",     single, 1, 42, 0);
    T("Single item - absent",      single, 1, 41, -1);
    T("Duplicates - first returned", dups, 6, 10, 1);
    T("Duplicates at start",       dups,   6,  5, 0);
    #undef T
    printf("\n  %d / %d tests passed.\n", pass, total);
}

/* ------------------------------------------------------------------ */

static void printMenu(void) {
    printf("\n==========================================\n");
    printf("   SMART RECORD SEARCHING SYSTEM\n");
    printf("==========================================\n");
    printf("  1. Add a record\n");
    printf("  2. Display all records\n");
    printf("  3. Load sample data\n");
    printf("  4. Sort records by ID (insertion sort)\n");
    printf("  5. Binary search - iterative (with trace)\n");
    printf("  6. Binary search - recursive (with trace)\n");
    printf("  7. Linear search\n");
    printf("  8. Compare binary vs linear\n");
    printf("  9. Run built-in test cases\n");
    printf("  0. Exit\n");
}

int main(void) {
    static Database db = { .n = 0, .sorted = 1 };
    for (;;) {
        printMenu();
        switch (readInt("Choice: ", 0, 9)) {
            case 1: doAddRecord(&db);                         break;
            case 2: displayRecords(&db);                      break;
            case 3: loadSample(&db); printf("\n  Sample data loaded (%d records, includes a duplicate ID 120).\n", db.n); break;
            case 4: insertionSortById(&db); printf("\n  Records sorted by ID.\n"); break;
            case 5: doBinarySearch(&db, 0);                   break;
            case 6: doBinarySearch(&db, 1);                   break;
            case 7: doLinearSearch(&db);                      break;
            case 8: doComparison(&db);                        break;
            case 9: runTests();                               break;
            case 0: printf("\n  Goodbye!\n"); return 0;
        }
    }
}
