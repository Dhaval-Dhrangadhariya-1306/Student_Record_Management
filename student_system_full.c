// student_system_full.c
// Features: Add, Display All, Search, Update, Delete, Sort, Stats, Colored Menu
// Compile: gcc -std=c11 student_system_full.c -o student_system_full

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILENAME "records.dat"
#define SUBJECTS_COUNT 6
#define NAME_LEN 100
#define GRADE_LEN 20

// ANSI color codes (works on Unix & Windows 10+)
#define C_RESET  "\x1b[0m"
#define C_BOLD   "\x1b[1m"
#define C_BLUE   "\x1b[34m"
#define C_GREEN  "\x1b[32m"
#define C_YELLOW "\x1b[33m"
#define C_RED    "\x1b[31m"
#define C_CYAN   "\x1b[36m"

const char subjectNames[SUBJECTS_COUNT][24] = {
    "Mathematics", "Physics", "Chemistry", "English", "Computer", "Biology"
};

typedef struct {
    int roll;
    char name[NAME_LEN];
    int marks[SUBJECTS_COUNT];
    int total;
    float percentage;
    char grade[GRADE_LEN];
} Student;

// ------------------- Utility -------------------
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

char *str_tolower_new(const char *s) {
    size_t n = strlen(s);
    char *t = malloc(n + 1);
    if (!t) return NULL;
    for (size_t i = 0; i < n; ++i) t[i] = tolower((unsigned char)s[i]);
    t[n] = '\0';
    return t;
}

void pauseLine() {
    printf("\nPress Enter to continue...");
    clearInputBuffer();
}

// ---------------- File IO: load into array & save array ----------------

Student *loadAll(int *count) {
    *count = 0;
    FILE *fp = fopen(FILENAME, "rb");
    if (!fp) return NULL;

    // find file size
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    rewind(fp);

    if (filesize <= 0) { fclose(fp); return NULL; }

    size_t n = filesize / sizeof(Student);
    Student *arr = malloc(n * sizeof(Student));
    if (!arr) { fclose(fp); return NULL; }

    size_t read = fread(arr, sizeof(Student), n, fp);
    fclose(fp);

    *count = (int)read;
    return arr;
}

int saveAll(const Student *arr, int count) {
    FILE *fp = fopen(FILENAME, "wb");
    if (!fp) return 0;
    size_t written = fwrite(arr, sizeof(Student), count, fp);
    fclose(fp);
    return (int)written == count;
}

int rollExists(int roll) {
    int n = 0;
    Student *arr = loadAll(&n);
    if (!arr) return 0;
    for (int i = 0; i < n; ++i) {
        if (arr[i].roll == roll) { free(arr); return 1; }
    }
    free(arr);
    return 0;
}

// ---------------- Grade calculation (improved scale) ----------------
void computeDerived(Student *s) {
    int sum = 0;
    for (int i = 0; i < SUBJECTS_COUNT; ++i) sum += s->marks[i];
    s->total = sum;
    s->percentage = (sum / (SUBJECTS_COUNT * 100.0f)) * 100.0f; // assuming each subject is out of 100

    // Improved scale:
    // Below Average: < 50%
    // Average: 50 - 59%
    // Good: 60 - 74%
    // Very Good: 75 - 89%
    // Excellent: >= 90%
    if (s->percentage < 50.0f) strcpy(s->grade, "Below Average");
    else if (s->percentage < 60.0f) strcpy(s->grade, "Average");
    else if (s->percentage < 75.0f) strcpy(s->grade, "Good");
    else if (s->percentage < 90.0f) strcpy(s->grade, "Very Good");
    else strcpy(s->grade, "Excellent");
}

// ---------------- CRUD + Helpers ----------------

void addStudent() {
    int n = 0;
    Student *arr = loadAll(&n);

    Student s;
    printf(C_BLUE C_BOLD "\n--- Add New Student ---\n" C_RESET);
    printf("Enter Roll Number: ");
    while (scanf("%d", &s.roll) != 1) {
        clearInputBuffer();
        printf("Invalid. Enter integer roll: ");
    }
    clearInputBuffer();

    if (arr) {
        for (int i = 0; i < n; ++i) {
            if (arr[i].roll == s.roll) {
                printf(C_RED "A student already exists with Roll No %d\n" C_RESET, s.roll);
                free(arr);
                pauseLine();
                return;
            }
        }
    } else {
        // no existing file; n==0
    }

    printf("Enter Full Name: ");
    fgets(s.name, NAME_LEN, stdin);
    s.name[strcspn(s.name, "\n")] = '\0';

    for (int i = 0; i < SUBJECTS_COUNT; ++i) {
        printf("%s marks (0-100): ", subjectNames[i]);
        while (scanf("%d", &s.marks[i]) != 1 || s.marks[i] < 0 || s.marks[i] > 100) {
            clearInputBuffer();
            printf("Invalid. Enter integer 0-100 for %s: ", subjectNames[i]);
        }
        clearInputBuffer();
    }

    computeDerived(&s);

    // append to array and save
    Student *newArr = realloc(arr, (n + 1) * sizeof(Student));
    if (!newArr) {
        printf(C_RED "Memory error. Cannot add student.\n" C_RESET);
        free(arr);
        pauseLine();
        return;
    }
    newArr[n] = s;
    if (!saveAll(newArr, n + 1)) {
        printf(C_RED "Failed to save record to file.\n" C_RESET);
    } else {
        printf(C_GREEN "Student added successfully and saved.\n" C_RESET);
    }
    free(newArr);
    pauseLine();
}

void displayAll() {
    int n = 0;
    Student *arr = loadAll(&n);
    if (!arr || n == 0) {
        printf(C_YELLOW "\nNo records found.\n" C_RESET);
        free(arr);
        pauseLine();
        return;
    }

    printf(C_CYAN C_BOLD "\n===== All Student Records (%d) =====\n\n" C_RESET, n);
    for (int i = 0; i < n; ++i) {
        Student *s = &arr[i];
        printf(C_BLUE "Roll: %d  |  Name: %s\n" C_RESET, s->roll, s->name);
        printf("Marks: ");
        for (int j = 0; j < SUBJECTS_COUNT; ++j) {
            printf("%s:%d", subjectNames[j], s->marks[j]);
            if (j < SUBJECTS_COUNT - 1) printf(" , ");
        }
        printf("\nTotal: %d  |  Perc: %.1f%%  |  Grade: %s\n", s->total, s->percentage, s->grade);
        printf("-------------------------------\n");
    }
    free(arr);
    pauseLine();
}

void searchStudent() {
    int n = 0;
    Student *arr = loadAll(&n);
    if (!arr || n == 0) {
        printf(C_YELLOW "\nNo records to search.\n" C_RESET);
        free(arr);
        pauseLine();
        return;
    }

    printf(C_BLUE "\nSearch by: 1) Roll  2) Name (partial)\n" C_RESET);
    printf("Enter choice: ");
    int ch; while (scanf("%d", &ch) != 1) { clearInputBuffer(); printf("Enter 1 or 2: "); }
    clearInputBuffer();

    if (ch == 1) {
        int roll; printf("Enter Roll Number: "); while (scanf("%d", &roll) != 1) { clearInputBuffer(); printf("Enter integer roll: "); }
        clearInputBuffer();
        for (int i = 0; i < n; ++i) {
            if (arr[i].roll == roll) {
                printf(C_GREEN "\nStudent Found:\n" C_RESET);
                printf("Roll: %d\nName: %s\n", arr[i].roll, arr[i].name);
                printf("Marks: ");
                for (int j = 0; j < SUBJECTS_COUNT; ++j) {
                    printf("%s:%d", subjectNames[j], arr[i].marks[j]);
                    if (j < SUBJECTS_COUNT - 1) printf(" , ");
                }
                printf("\nTotal: %d  |  Perc: %.1f%%  |  Grade: %s\n", arr[i].total, arr[i].percentage, arr[i].grade);
                free(arr);
                pauseLine();
                return;
            }
        }
        printf(C_YELLOW "Student not found.\n" C_RESET);
    } else if (ch == 2) {
        char q[NAME_LEN];
        printf("Enter name or partial name: ");
        fgets(q, NAME_LEN, stdin); q[strcspn(q, "\n")] = '\0';
        char *qL = str_tolower_new(q);
        int found = 0;
        for (int i = 0; i < n; ++i) {
            char *nameL = str_tolower_new(arr[i].name);
            if (strstr(nameL, qL) != NULL) {
                if (!found) printf(C_GREEN "\nMatching Records:\n" C_RESET);
                printf("Roll: %d | Name: %s | Perc: %.1f%% | Grade: %s\n", arr[i].roll, arr[i].name, arr[i].percentage, arr[i].grade);
                found = 1;
            }
            free(nameL);
        }
        free(qL);
        if (!found) printf(C_YELLOW "No matching student found.\n" C_RESET);
    } else {
        printf(C_RED "Invalid option.\n" C_RESET);
    }

    free(arr);
    pauseLine();
}

void deleteStudent() {
    int n = 0;
    Student *arr = loadAll(&n);
    if (!arr || n == 0) {
        printf(C_YELLOW "\nNo records to delete.\n" C_RESET);
        free(arr);
        pauseLine();
        return;
    }

    printf("Enter Roll Number to delete: ");
    int roll; while (scanf("%d", &roll) != 1) { clearInputBuffer(); printf("Enter integer roll: "); }
    clearInputBuffer();

    int idx = -1;
    for (int i = 0; i < n; ++i) if (arr[i].roll == roll) { idx = i; break; }

    if (idx == -1) {
        printf(C_YELLOW "Student not found.\n" C_RESET);
        free(arr);
        pauseLine();
        return;
    }

    // Show student before delete
    printf(C_RED "Deleting record:\n" C_RESET);
    printf("Roll: %d | Name: %s | Perc: %.1f%% | Grade: %s\n", arr[idx].roll, arr[idx].name, arr[idx].percentage, arr[idx].grade);
    printf("Are you sure? (y/n): ");
    char c = getchar(); clearInputBuffer();
    if (c != 'y' && c != 'Y') {
        printf("Delete cancelled.\n");
        free(arr);
        pauseLine();
        return;
    }

    // shift left to remove
    for (int i = idx; i < n - 1; ++i) arr[i] = arr[i + 1];
    n--;
    if (!saveAll(arr, n)) printf(C_RED "Failed to update file after delete.\n" C_RESET);
    else printf(C_GREEN "Record deleted and file updated.\n" C_RESET);

    free(arr);
    pauseLine();
}

void updateStudent() {
    int n = 0;
    Student *arr = loadAll(&n);
    if (!arr || n == 0) {
        printf(C_YELLOW "\nNo records to update.\n" C_RESET);
        free(arr);
        pauseLine();
        return;
    }

    printf("Enter Roll Number to update: ");
    int roll; while (scanf("%d", &roll) != 1) { clearInputBuffer(); printf("Enter integer roll: "); }
    clearInputBuffer();

    int idx = -1;
    for (int i = 0; i < n; ++i) if (arr[i].roll == roll) { idx = i; break; }

    if (idx == -1) {
        printf(C_YELLOW "Student not found.\n" C_RESET);
        free(arr);
        pauseLine();
        return;
    }

    printf(C_CYAN "Current Details:\n" C_RESET);
    printf("Roll: %d\nName: %s\n", arr[idx].roll, arr[idx].name);
    printf("Marks: ");
    for (int j = 0; j < SUBJECTS_COUNT; ++j) printf("%s:%d ", subjectNames[j], arr[idx].marks[j]);
    printf("\nTotal: %d | Perc: %.1f%% | Grade: %s\n", arr[idx].total, arr[idx].percentage, arr[idx].grade);

    printf("\nWhat to update?\n1) Name\n2) All Marks\n3) Single Subject Mark\n4) Cancel\nChoice: ");
    int ch; while (scanf("%d", &ch) != 1) { clearInputBuffer(); printf("Enter 1-4: "); }
    clearInputBuffer();

    if (ch == 1) {
        printf("Enter new full name: ");
        fgets(arr[idx].name, NAME_LEN, stdin); arr[idx].name[strcspn(arr[idx].name, "\n")] = '\0';
    } else if (ch == 2) {
        for (int j = 0; j < SUBJECTS_COUNT; ++j) {
            printf("%s new marks: ", subjectNames[j]);
            while (scanf("%d", &arr[idx].marks[j]) != 1 || arr[idx].marks[j] < 0 || arr[idx].marks[j] > 100) {
                clearInputBuffer(); printf("Invalid. Enter 0-100: ");
            }
            clearInputBuffer();
        }
    } else if (ch == 3) {
        printf("Enter subject number (1-%d): ", SUBJECTS_COUNT);
        int sidx; while (scanf("%d", &sidx) != 1 || sidx < 1 || sidx > SUBJECTS_COUNT) {
            clearInputBuffer(); printf("Enter valid subject number: ");
        }
        clearInputBuffer();
        printf("%s new marks: ", subjectNames[sidx - 1]);
        while (scanf("%d", &arr[idx].marks[sidx - 1]) != 1 || arr[idx].marks[sidx - 1] < 0 || arr[idx].marks[sidx - 1] > 100) {
            clearInputBuffer(); printf("Enter 0-100: ");
        }
        clearInputBuffer();
    } else {
        printf("Update cancelled.\n");
        free(arr);
        pauseLine();
        return;
    }

    computeDerived(&arr[idx]);
    if (!saveAll(arr, n)) printf(C_RED "Failed to save updates to file.\n" C_RESET);
    else printf(C_GREEN "Record updated and saved.\n" C_RESET);

    free(arr);
    pauseLine();
}

// ---------------- Sorting ----------------
int cmpRollAsc(const void *a, const void *b) {
    const Student *A = a, *B = b;
    return A->roll - B->roll;
}
int cmpRollDesc(const void *a, const void *b) {
    return cmpRollAsc(b, a);
}
int cmpNameAsc(const void *a, const void *b) {
    const Student *A = a, *B = b;
    return strcasecmp(A->name, B->name);
}
int cmpNameDesc(const void *a, const void *b) {
    return cmpNameAsc(b, a);
}
int cmpPercAsc(const void *a, const void *b) {
    const Student *A = a, *B = b;
    if (A->percentage < B->percentage) return -1;
    if (A->percentage > B->percentage) return 1;
    return 0;
}
int cmpPercDesc(const void *a, const void *b) {
    return cmpPercAsc(b, a);
}

void sortStudents() {
    int n = 0;
    Student *arr = loadAll(&n);
    if (!arr || n == 0) {
        printf(C_YELLOW "\nNo records to sort.\n" C_RESET);
        free(arr);
        pauseLine();
        return;
    }

    printf("\nSort by:\n1) Roll\n2) Name\n3) Percentage\nChoice: ");
    int c; while (scanf("%d", &c) != 1) { clearInputBuffer(); printf("Enter 1-3: "); }
    clearInputBuffer();
    printf("Order: 1) Ascending  2) Descending\nChoice: ");
    int ord; while (scanf("%d", &ord) != 1) { clearInputBuffer(); printf("Enter 1 or 2: "); }
    clearInputBuffer();

    if (c == 1) qsort(arr, n, sizeof(Student), ord == 1 ? cmpRollAsc : cmpRollDesc);
    else if (c == 2) qsort(arr, n, sizeof(Student), ord == 1 ? cmpNameAsc : cmpNameDesc);
    else if (c == 3) qsort(arr, n, sizeof(Student), ord == 1 ? cmpPercAsc : cmpPercDesc);
    else {
        printf(C_RED "Invalid sort option.\n" C_RESET);
        free(arr);
        pauseLine();
        return;
    }

    // show sorted list
    printf(C_CYAN "\nSorted Records:\n" C_RESET);
    for (int i = 0; i < n; ++i) {
        printf("Roll:%d | Name:%s | Perc:%.1f%% | Grade:%s\n", arr[i].roll, arr[i].name, arr[i].percentage, arr[i].grade);
    }

    // ask if save back
    printf("\nSave this sorted order to file? (y/n): ");
    char ch = getchar(); clearInputBuffer();
    if (ch == 'y' || ch == 'Y') {
        if (!saveAll(arr, n)) printf(C_RED "Failed to save sorted order.\n" C_RESET);
        else printf(C_GREEN "Sorted order saved to file.\n" C_RESET);
    } else {
        printf("Sorted order not saved.\n");
    }

    free(arr);
    pauseLine();
}

// ---------------- Stats ----------------
void performanceStats() {
    int n = 0;
    Student *arr = loadAll(&n);
    if (!arr || n == 0) {
        printf(C_YELLOW "\nNo records for statistics.\n" C_RESET);
        free(arr);
        pauseLine();
        return;
    }

    int below=0, avg=0, good=0, vgood=0, exc=0;
    float highest = -1.0f, lowest = 101.0f;
    int idxHigh = -1, idxLow = -1;

    for (int i = 0; i < n; ++i) {
        float p = arr[i].percentage;
        if (p < 50.0f) below++;
        else if (p < 60.0f) avg++;
        else if (p < 75.0f) good++;
        else if (p < 90.0f) vgood++;
        else exc++;

        if (p > highest) { highest = p; idxHigh = i; }
        if (p < lowest) { lowest = p; idxLow = i; }
    }

    printf(C_CYAN C_BOLD "\n=== Performance Statistics ===\n" C_RESET);
    printf("Total Students      : %d\n", n);
    printf("Excellent (>=90%%)   : %d\n", exc);
    printf("Very Good (75-89)   : %d\n", vgood);
    printf("Good (60-74)        : %d\n", good);
    printf("Average (50-59)     : %d\n", avg);
    printf("Below Average (<50) : %d\n", below);

    if (idxHigh != -1) printf("\nTop Student: Roll %d | %s | %.1f%%\n", arr[idxHigh].roll, arr[idxHigh].name, arr[idxHigh].percentage);
    if (idxLow != -1) printf("Lowest : Roll %d | %s | %.1f%%\n", arr[idxLow].roll, arr[idxLow].name, arr[idxLow].percentage);

    free(arr);
    pauseLine();
}

// ---------------- Menu ----------------
void showMenu() {
    printf(C_BLUE C_BOLD "\n=== Student Record Management System ===\n" C_RESET);
    printf(C_GREEN "1. Add Student\n");
    printf("2. Display All Students\n");
    printf("3. Search Student\n");
    printf("4. Update Student\n");
    printf("5. Delete Student\n");
    printf("6. Sort Students\n");
    printf("7. Performance Statistics\n");
    printf("8. Exit\n" C_RESET);
    printf("Enter choice: ");
}

int main() {
    // Simple attempt to enable ANSI on Windows (no guarantee)
    #ifdef _WIN32
    // On Windows, user may need to enable virtual terminal processing manually.
    #endif

    int choice;
    while (1) {
        showMenu();
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf(C_RED "Invalid input. Enter numeric choice.\n" C_RESET);
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayAll(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: sortStudents(); break;
            case 7: performanceStats(); break;
            case 8: printf(C_CYAN "Exiting... Goodbye!\n" C_RESET); exit(0);
            default: printf(C_RED "Invalid choice. Try again.\n" C_RESET);
        }
    }
    return 0;
}
