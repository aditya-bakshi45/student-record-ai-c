/*
 * AI-Assisted Student Record and Result Management System
 * Course: Programming in C (PRC1001), UPES B.Tech CSE Sem I
 *
 * NOTE: Rename this file to <your SAP ID>.c before submitting.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS 100
#define MAX_SEM 8

typedef struct {
    long sap;               /* SAP ID */
    char roll[16];          /* Roll No. */
    char name[50];
    char phone[16];
    char email[50];
    int year;               /* 1 to 4 */
    int sem;                /* semesters completed (SGPA available for these) */
    float sgpa[MAX_SEM];    /* SGPA of semester 1..8 (0 if not available) */
} Student;

Student students[MAX_STUDENTS];
int count = 0;

/* ---------- input helpers ---------- */

int readLine(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) return 0;
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

int readInt(const char *prompt, int min, int max) {
    char buf[64];
    int val;
    while (1) {
        if (!readLine(prompt, buf, sizeof(buf))) exit(0);
        if (sscanf(buf, "%d", &val) == 1 && val >= min && val <= max) return val;
        printf("  Invalid input. Enter a number between %d and %d.\n", min, max);
    }
}

long readLong(const char *prompt, long min, long max) {
    char buf[64];
    long val;
    while (1) {
        if (!readLine(prompt, buf, sizeof(buf))) exit(0);
        if (sscanf(buf, "%ld", &val) == 1 && val >= min && val <= max) return val;
        printf("  Invalid input. Enter a number between %ld and %ld.\n", min, max);
    }
}

float readSGPA(const char *prompt) {
    char buf[64];
    float val;
    while (1) {
        if (!readLine(prompt, buf, sizeof(buf))) exit(0);
        if (sscanf(buf, "%f", &val) == 1 && val >= 0.0f && val <= 10.0f) return val;
        printf("  Invalid SGPA. Enter a value between 0 and 10.\n");
    }
}

void readName(char *name, int size) {
    while (1) {
        if (!readLine("Enter Name: ", name, size)) exit(0);
        if (strlen(name) > 0) return;
        printf("  Name cannot be empty.\n");
    }
}

void readPhone(char *phone, int size) {
    while (1) {
        int i, ok;
        if (!readLine("Enter Phone No. (10 digits): ", phone, size)) exit(0);
        ok = (strlen(phone) == 10);
        for (i = 0; ok && phone[i]; i++)
            if (!isdigit((unsigned char)phone[i])) ok = 0;
        if (ok) return;
        printf("  Phone number must be exactly 10 digits.\n");
    }
}

void readEmail(char *email, int size) {
    while (1) {
        if (!readLine("Enter Email ID: ", email, size)) exit(0);
        if (strchr(email, '@') != NULL && strchr(email, '.') != NULL) return;
        printf("  Invalid email. It must contain '@' and '.'.\n");
    }
}

/* ---------- helper functions ---------- */

int findBySap(long sap) {
    int i;
    for (i = 0; i < count; i++)
        if (students[i].sap == sap) return i;
    return -1;
}

/* CGPA = average of the available SGPA values */
float calculateCGPA(const Student *s) {
    int i;
    float sum = 0.0f;
    if (s->sem == 0) return 0.0f;
    for (i = 0; i < s->sem; i++) sum += s->sgpa[i];
    return sum / s->sem;
}

/* Part B: decision-making function using if-else */
const char *getAdvice(float cgpa) {
    if (cgpa >= 8.5f)
        return "Excellent Performance - maintain your current study pattern.";
    else if (cgpa >= 7.0f)
        return "Good Performance - keep it up and try to push your CGPA higher.";
    else if (cgpa >= 5.0f)
        return "Needs Improvement - focus more on subjects with lower SGPA.";
    else
        return "Requires Immediate Attention - meet your faculty advisor and make a study plan now.";
}

void printAdvisor(const Student *s) {
    float cgpa = calculateCGPA(s);
    int i, low = 0;
    printf("%ld | %-20s | CGPA: %.2f\n", s->sap, s->name, cgpa);
    if (s->sem == 0) {
        printf("  No SGPA available, so no advice can be generated.\n");
        return;
    }
    printf("  Advice: %s\n", getAdvice(cgpa));
    for (i = 1; i < s->sem; i++)
        if (s->sgpa[i] < s->sgpa[low]) low = i;
    if (cgpa < 7.0f)
        printf("  Weakest semester: Sem %d (SGPA %.2f)\n", low + 1, s->sgpa[low]);
}

void printStudentFull(const Student *s) {
    int i;
    printf("----------------------------------------\n");
    printf("SAP ID   : %ld\n", s->sap);
    printf("Roll No. : %s\n", s->roll);
    printf("Name     : %s\n", s->name);
    printf("Phone    : %s\n", s->phone);
    printf("Email    : %s\n", s->email);
    printf("Year     : %d\n", s->year);
    printf("Semesters completed: %d\n", s->sem);
    printf("SGPA     : ");
    for (i = 0; i < MAX_SEM; i++) {
        if (i < s->sem) printf("S%d=%.2f ", i + 1, s->sgpa[i]);
        else printf("S%d=NA ", i + 1);
    }
    printf("\nCGPA     : %.2f\n", calculateCGPA(s));
}

void printTableHeader(void) {
    printf("%-10s %-10s %-20s %-11s %-26s %-4s %-4s %-5s\n",
           "SAP ID", "Roll No.", "Name", "Phone", "Email", "Year", "Sem", "CGPA");
    printf("-------------------------------------------------------------------------------------------\n");
}

void printTableRow(const Student *s) {
    printf("%-10ld %-10s %-20s %-11s %-26s %-4d %-4d %-5.2f\n",
           s->sap, s->roll, s->name, s->phone, s->email,
           s->year, s->sem, calculateCGPA(s));
}

/* ---------- main features ---------- */

void addStudent(void) {
    Student s;
    int i;
    if (count >= MAX_STUDENTS) {
        printf("Record limit reached.\n");
        return;
    }
    memset(&s, 0, sizeof(s));
    s.sap = readLong("Enter SAP ID: ", 1, 999999999L);
    if (findBySap(s.sap) != -1) {
        printf("A student with this SAP ID already exists.\n");
        return;
    }
    if (!readLine("Enter Roll No.: ", s.roll, sizeof(s.roll))) exit(0);
    readName(s.name, sizeof(s.name));
    readPhone(s.phone, sizeof(s.phone));
    readEmail(s.email, sizeof(s.email));
    s.year = readInt("Enter Year (1-4): ", 1, 4);
    s.sem = readInt("Enter number of semesters completed (0-8): ", 0, MAX_SEM);
    for (i = 0; i < s.sem; i++) {
        char prompt[40];
        sprintf(prompt, "Enter SGPA of Semester %d: ", i + 1);
        s.sgpa[i] = readSGPA(prompt);
    }
    students[count++] = s;
    printf("Student added successfully.\n");
}

void searchStudent(void) {
    long sap = readLong("Enter SAP ID to search: ", 1, 999999999L);
    int idx = findBySap(sap);
    if (idx == -1) printf("Student not found.\n");
    else printStudentFull(&students[idx]);
}

void displayAll(void) {
    int i;
    if (count == 0) {
        printf("No records available.\n");
        return;
    }
    printTableHeader();
    for (i = 0; i < count; i++) printTableRow(&students[i]);
}

void displayCGPAAll(void) {
    int i;
    if (count == 0) {
        printf("No records available.\n");
        return;
    }
    printf("%-10s %-20s %-5s\n", "SAP ID", "Name", "CGPA");
    for (i = 0; i < count; i++)
        printf("%-10ld %-20s %-5.2f\n", students[i].sap, students[i].name,
               calculateCGPA(&students[i]));
}

void yearWiseAverage(void) {
    int y, i;
    for (y = 1; y <= 4; y++) {
        float sum = 0.0f;
        int n = 0;
        for (i = 0; i < count; i++) {
            if (students[i].year == y && students[i].sem > 0) {
                sum += calculateCGPA(&students[i]);
                n++;
            }
        }
        if (n > 0) printf("Year %d: Average CGPA = %.2f (%d students)\n", y, sum / n, n);
        else printf("Year %d: No students with SGPA data\n", y);
    }
}

void deleteStudent(void) {
    long sap = readLong("Enter SAP ID to delete: ", 1, 999999999L);
    int idx = findBySap(sap), i;
    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }
    for (i = idx; i < count - 1; i++) students[i] = students[i + 1];
    count--;
    printf("Record deleted.\n");
}

/* case-insensitive name comparison */
int compareNames(const char *a, const char *b) {
    while (*a && *b) {
        int ca = tolower((unsigned char)*a), cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* mode 0 = sort by SAP ID, mode 1 = sort by Name (bubble sort on a copy) */
void displaySorted(int mode) {
    Student temp[MAX_STUDENTS], t;
    int i, j;
    if (count == 0) {
        printf("No records available.\n");
        return;
    }
    for (i = 0; i < count; i++) temp[i] = students[i];
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - 1 - i; j++) {
            int swap;
            if (mode == 0) swap = temp[j].sap > temp[j + 1].sap;
            else swap = compareNames(temp[j].name, temp[j + 1].name) > 0;
            if (swap) {
                t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }
    printTableHeader();
    for (i = 0; i < count; i++) printTableRow(&temp[i]);
}

void advisorOne(void) {
    long sap = readLong("Enter SAP ID for advice: ", 1, 999999999L);
    int idx = findBySap(sap);
    if (idx == -1) printf("Student not found.\n");
    else printAdvisor(&students[idx]);
}

void advisorAll(void) {
    int i;
    if (count == 0) {
        printf("No records available.\n");
        return;
    }
    for (i = 0; i < count; i++) printAdvisor(&students[i]);
}

/* 5 pre-loaded students so the program always has at least 5 records */
void addSample(long sap, const char *roll, const char *name, const char *phone,
               const char *email, int year, int sem, const float *sg) {
    Student *s = &students[count++];
    int i;
    memset(s, 0, sizeof(*s));
    s->sap = sap;
    strcpy(s->roll, roll);
    strcpy(s->name, name);
    strcpy(s->phone, phone);
    strcpy(s->email, email);
    s->year = year;
    s->sem = sem;
    for (i = 0; i < sem; i++) s->sgpa[i] = sg[i];
}

void loadSampleData(void) {
    float a[] = {9.2f, 9.0f, 9.3f, 9.1f};
    float b[] = {7.8f, 8.0f, 7.5f};
    float c[] = {6.5f, 6.0f, 6.1f, 6.2f, 6.2f, 6.2f};
    float d[] = {4.8f, 4.2f};
    float e[] = {8.7f, 8.5f, 8.6f, 8.6f, 8.8f, 8.4f, 8.6f, 8.6f};
    addSample(500091001L, "R2501001", "Rohan Verma",  "9876543210", "rohan@example.com",  2, 4, a);
    addSample(500091005L, "R2501005", "Ananya Singh", "9876543211", "ananya@example.com", 2, 3, b);
    addSample(500091003L, "R2501003", "Karan Mehta",  "9876543212", "karan@example.com",  3, 6, c);
    addSample(500091009L, "R2501009", "Neha Gupta",   "9876543213", "neha@example.com",    1, 2, d);
    addSample(500091002L, "R2501002", "Simran Kaur",  "9876543214", "simran@example.com", 4, 8, e);
}

void showMenu(void) {
    printf("\n===== Student Record & Result Management System =====\n");
    printf(" 1. Add student\n");
    printf(" 2. Search student (by SAP ID)\n");
    printf(" 3. Display all students\n");
    printf(" 4. Show CGPA of each student\n");
    printf(" 5. Year-wise average CGPA\n");
    printf(" 6. Delete student\n");
    printf(" 7. Display sorted by SAP ID\n");
    printf(" 8. Display sorted by Name\n");
    printf(" 9. Academic Advisor (one student)\n");
    printf("10. Academic Advisor (all students)\n");
    printf(" 0. Exit\n");
}

int main(void) {
    int choice;
    loadSampleData();
    do {
        showMenu();
        choice = readInt("Enter your choice: ", 0, 10);
        printf("\n");
        switch (choice) {
            case 1:  addStudent();      break;
            case 2:  searchStudent();   break;
            case 3:  displayAll();      break;
            case 4:  displayCGPAAll();  break;
            case 5:  yearWiseAverage(); break;
            case 6:  deleteStudent();   break;
            case 7:  displaySorted(0);  break;
            case 8:  displaySorted(1);  break;
            case 9:  advisorOne();      break;
            case 10: advisorAll();      break;
            case 0:  printf("Thank you. Exiting...\n"); break;
        }
    } while (choice != 0);
    return 0;
}
