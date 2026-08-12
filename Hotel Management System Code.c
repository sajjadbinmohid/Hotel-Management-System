#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int nextCustomerId = 5000;

// ---------------------------------------------------------
// LOGIN CONFIGURATION
// ---------------------------------------------------------
#define LOGIN_USERNAME "admin"
#define LOGIN_PASSWORD "hotel123"
#define MAX_LOGIN_ATTEMPTS 3

// ---------------------------------------------------------
// DATA STRUCTURES
// ---------------------------------------------------------

struct Room {
    int roomNumber;
    char category[20];
    char bedType[20];
    int price;
    int isBooked;
    char hotline[20];

    char customerID[30];
    char customerName[50];
    char customerTier[20];
    char phoneNumber[20];
    char adminComment[100];
    char checkInDate[20];
    int stayDays;
    double paidAmount;

    struct Room* next;
};

// ফিডব্যাকের জন্য নতুন স্ট্রাকচার
struct Feedback {
    char customerID[30];
    char customerName[50];
    int rating; // 1 = Excellent, 2 = Good, 3 = Bad
    char reason[200];
    struct Feedback* next;
};

struct Room* head = NULL;
struct Room* tail = NULL;

struct Feedback* fHead = NULL;
struct Feedback* fTail = NULL;

// STAFF MANAGEMENT DATA STRUCTURE
struct Employee {
    char id[20];
    char name[50];
    char phone[15];
    char shift[20];
    char role[20];
};

const char* FILE_NAME = "staff_database.txt";

// ---------------------------------------------------------
// UTILITY FUNCTIONS
// ---------------------------------------------------------

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// ---------------------------------------------------------
// ADMIN LOGIN SYSTEM
// Default Username: admin
// Default Password: hotel123
// ---------------------------------------------------------
int loginSystem() {
    char username[50];
    char password[50];

    for (int attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++) {
        clearScreen();
        printf("\n=========================================\n");
        printf("          HOTEL ADMIN LOGIN              \n");
        printf("=========================================\n");

        printf("Username: ");
        scanf("%49s", username);

        printf("Password: ");
        scanf("%49s", password);

        if (strcmp(username, LOGIN_USERNAME) == 0 &&
            strcmp(password, LOGIN_PASSWORD) == 0) {
            printf("\n[SUCCESS] Login successful. Welcome, Admin!\n");
            printf("Press Enter to continue...");
            getchar();
            getchar();
            return 1;
        }

        int remaining = MAX_LOGIN_ATTEMPTS - attempt;
        printf("\n[ERROR] Invalid username or password.\n");

        if (remaining > 0) {
            printf("Attempts remaining: %d\n", remaining);
            printf("Press Enter to try again...");
            getchar();
            getchar();
        }
    }

    return 0;
}

void formatForSave(char* str) {
    for(int i=0; str[i]!='\0'; i++) {
        if(str[i] == ' ') str[i] = '_';
    }
}

void printWithSpaces(const char* str) {
    for(int i=0; str[i]!='\0'; i++) {
        if(str[i] == '_') putchar(' ');
        else putchar(str[i]);
    }
}

void getCurrentDate(char* dateStr) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(dateStr, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

void setTierByCategory(char* tier, const char* category) {
    if (strcmp(category, "Classic") == 0) strcpy(tier, "Silver");
    else if (strcmp(category, "Premium") == 0) strcpy(tier, "Gold");
    else if (strcmp(category, "Presidential") == 0) strcpy(tier, "Platinum");
}

// ---------------------------------------------------------
// FILE HANDLING FOR FEEDBACK
// ---------------------------------------------------------

void saveFeedbackToTXT() {
    FILE *file = fopen("feedback_data.txt", "w");
    if (file == NULL) return;

    struct Feedback* temp = fHead;
    while (temp != NULL) {
        fprintf(file, "%s %s %d %s\n", temp->customerID, temp->customerName, temp->rating, temp->reason);
        temp = temp->next;
    }
    fclose(file);
}

void loadFeedbackFromTXT() {
    FILE *file = fopen("feedback_data.txt", "r");
    if (file == NULL) return;

    char cid[30], cname[50], reason[200];
    int rating;

    while (fscanf(file, "%s %s %d %s\n", cid, cname, &rating, reason) == 4) {
        struct Feedback* temp = (struct Feedback*)malloc(sizeof(struct Feedback));
        strcpy(temp->customerID, cid);
        strcpy(temp->customerName, cname);
        temp->rating = rating;
        strcpy(temp->reason, reason);
        temp->next = NULL;

        if (fHead == NULL) { fHead = temp; fTail = temp; }
        else { fTail->next = temp; fTail = temp; }
    }
    fclose(file);
}

void addFeedback(char* cid, char* cname, int rating, char* reason) {
    struct Feedback* temp = (struct Feedback*)malloc(sizeof(struct Feedback));
    strcpy(temp->customerID, cid);
    strcpy(temp->customerName, cname);
    temp->rating = rating;
    strcpy(temp->reason, reason);
    temp->next = NULL;

    if (fHead == NULL) { fHead = temp; fTail = temp; }
    else { fTail->next = temp; fTail = temp; }

    saveFeedbackToTXT();
}

// ---------------------------------------------------------
// FILE HANDLING FOR ROOMS
// ---------------------------------------------------------

void saveDataToTXT() {
    FILE *file = fopen("hotel_data.txt", "w");
    if (file == NULL) return;

    struct Room* temp = head;
    while (temp != NULL) {
        formatForSave(temp->customerName);
        formatForSave(temp->adminComment);

        fprintf(file, "%d %s %s %d %d %s %s %s %s %s %s %s %d %.2lf\n",
                temp->roomNumber, temp->category, temp->bedType, temp->price,
                temp->isBooked, temp->hotline, temp->customerID, temp->customerName,
                temp->customerTier, temp->phoneNumber, temp->adminComment,
                temp->checkInDate, temp->stayDays, temp->paidAmount);
        temp = temp->next;
    }
    fclose(file);
}

void appendRoom(int rNum, const char* cat, const char* bed, int price, int booked, const char* cid, const char* cname, const char* tier, const char* phone, const char* comment, const char* date, int days, double paid) {
    struct Room* temp = (struct Room*)malloc(sizeof(struct Room));
    temp->roomNumber = rNum; strcpy(temp->category, cat); strcpy(temp->bedType, bed);
    temp->price = price; temp->isBooked = booked;
    sprintf(temp->hotline, "16200-%d", rNum);
    strcpy(temp->customerID, cid); strcpy(temp->customerName, cname);
    strcpy(temp->customerTier, tier); strcpy(temp->phoneNumber, phone);
    strcpy(temp->adminComment, comment); strcpy(temp->checkInDate, date);
    temp->stayDays = days; temp->paidAmount = paid;
    temp->next = NULL;

    if (head == NULL) { head = temp; tail = temp; }
    else { tail->next = temp; tail = temp; }
}

void generateInitialSystem() {
    int i, rNum, price, isBooked, bookedCount = 0;
    char cat[20], bed[20], cid[30], cname[50], tier[20], phone[20], comment[100], date[20];
    getCurrentDate(date);

    for (i = 1; i <= 226; i++) {
        if (i <= 75) {
            strcpy(cat, "Classic"); rNum = 100 + i;
            if (i <= 25) { strcpy(bed, "Single"); price = 200; }
            else if (i <= 50) { strcpy(bed, "Double"); price = 250; }
            else { strcpy(bed, "Multiple"); price = 300; }
        } else if (i <= 150) {
            strcpy(cat, "Premium"); rNum = 200 + (i - 75);
            if (i <= 100) { strcpy(bed, "Single"); price = 300; }
            else if (i <= 125) { strcpy(bed, "Double"); price = 350; }
            else { strcpy(bed, "Multiple"); price = 400; }
        } else {
            strcpy(cat, "Presidential"); rNum = 300 + (i - 150);
            if (i <= 175) { strcpy(bed, "Single"); price = 500; }
            else if (i <= 200) { strcpy(bed, "Double"); price = 600; }
            else { strcpy(bed, "Multiple"); price = 700; }
        }

        double vatP = (strcmp(cat, "Classic") == 0) ? 0.15 : (strcmp(cat, "Premium") == 0) ? 0.20 : 0.30;
        double dummyPaid = (3 + 1) * price * (1 + vatP);

        if (i % 2 != 0 && bookedCount < 100) {
            isBooked = 1; bookedCount++;
            sprintf(cid, "%c%c-%d-%d", cat[0], bed[0], rNum, nextCustomerId++);
            sprintf(cname, "Guest_%d", i); setTierByCategory(tier, cat);
            sprintf(phone, "01700%05d", i); strcpy(comment, "Initial_Booking");
        } else {
            isBooked = 0; strcpy(cid, "N/A"); strcpy(cname, "N/A"); strcpy(tier, "N/A");
            strcpy(phone, "N/A"); strcpy(comment, "N/A"); strcpy(date, "N/A"); dummyPaid = 0.0;
        }
        appendRoom(rNum, cat, bed, price, isBooked, cid, cname, tier, phone, comment, date, (isBooked? 3:0), dummyPaid);
    }
    saveDataToTXT();
}

void loadDataFromTXT() {
    FILE *file = fopen("hotel_data.txt", "r");
    if (file == NULL) { generateInitialSystem(); return; }
    struct Room t;
    while (fscanf(file, "%d %s %s %d %d %s %s %s %s %s %s %s %d %lf\n",
                  &t.roomNumber, t.category, t.bedType, &t.price,
                  &t.isBooked, t.hotline, t.customerID, t.customerName,
                  t.customerTier, t.phoneNumber, t.adminComment,
                  t.checkInDate, &t.stayDays, &t.paidAmount) == 14) {
        appendRoom(t.roomNumber, t.category, t.bedType, t.price, t.isBooked, t.customerID, t.customerName, t.customerTier, t.phoneNumber, t.adminComment, t.checkInDate, t.stayDays, t.paidAmount);
        if(t.isBooked) nextCustomerId++;
    }
    fclose(file);
}

// ---------------------------------------------------------
// HISTORY & REVENUE
// ---------------------------------------------------------

void logGuestHistory(struct Room* r, double totalBill, const char* checkOutDate) {
    FILE *file = fopen("guest_history.txt", "a");
    if (file == NULL) return;

    char savedName[50];
    strcpy(savedName, r->customerName);
    formatForSave(savedName);

    fprintf(file, "%s %s %d %s %s %s %d %.2f\n",
            r->customerID, savedName, r->roomNumber, r->category,
            r->checkInDate, checkOutDate, r->stayDays, totalBill);
    fclose(file);
}

double getTotalHistoricalRevenue() {
    FILE *file = fopen("guest_history.txt", "r");
    if (file == NULL) return 0.0;

    double total = 0.0;
    char dummy1[30], dummy2[50], dummy3[20], dummy4[20], dummy5[20];
    int dummyInt;
    double rev;

    while (fscanf(file, "%s %s %d %s %s %s %d %lf\n",
                  dummy1, dummy2, &dummyInt, dummy3, dummy4, dummy5, &dummyInt, &rev) == 8) {
        total += rev;
    }
    fclose(file);
    return total;
}

void showGuestHistory() {
    clearScreen();
    FILE *file = fopen("guest_history.txt", "r");
    if (file == NULL) {
        printf("\nNo guest history recorded yet.\n");
        printf("Press Enter to go back..."); getchar(); getchar();
        return;
    }

    printf("\n=============================================================\n");
    printf("                     GUEST HISTORY (All Time)                   \n");
    printf("=============================================================\n");
    printf("ID             | Name                  | Room | Category | Check-In   | Check-Out  | Days | Total Paid\n");
    printf("---------------------------------------------------------------------------------------------\n");

    char cid[30], cname[50], category[20], checkIn[20], checkOut[20];
    int roomNum, stayD;
    double totPaid;

    while (fscanf(file, "%s %s %d %s %s %s %d %lf\n", cid, cname, &roomNum, category, checkIn, checkOut, &stayD, &totPaid) == 8) {
        printf("%-15s | ", cid);
        printWithSpaces(cname);
        printf(" | %-4d | %-10s | %-10s | %-10s | %-4d | $%.2f\n", roomNum, category, checkIn, checkOut, stayD, totPaid);
    }
    fclose(file);

    printf("\nPress Enter to go back...");
    getchar(); getchar();
}

void showRevenueHistory() {
    clearScreen();
    FILE *file = fopen("guest_history.txt", "r");
    if (file == NULL) {
        printf("\nNo revenue history yet.\n");
        printf("Press Enter to go back..."); getchar(); getchar();
        return;
    }

    printf("\n=============================================================\n");
    printf("                   REVENUE HISTORY                      \n");
    printf("=============================================================\n");
    printf("Check-Out Date | Customer ID     | Room | Revenue ($)\n");
    printf("-----------------------------------------------------\n");

    char cid[30], checkOut[20];
    int roomNum;
    double rev;

    while (fscanf(file, "%s %*s %d %*s %*s %s %*d %lf\n", cid, &roomNum, checkOut, &rev) == 4) {
        printf("%-14s | %-15s | %-4d | $%.2f\n", checkOut, cid, roomNum, rev);
    }
    fclose(file);

    printf("\nPress Enter to go back...");
    getchar(); getchar();
}

void revenueSection() {
    clearScreen();
    printf("\n=== REVENUE SECTION ===\n");

    double currentRevenue = 0.0;
    struct Room* temp = head;
    while (temp != NULL) {
        if (temp->isBooked == 1) {
            currentRevenue += temp->paidAmount;
        }
        temp = temp->next;
    }

    double historicalRevenue = getTotalHistoricalRevenue();

    printf("Current Active Revenue (Deposits from ongoing stays): $%.2f\n", currentRevenue);
    printf("Historical Revenue (Completed Check-outs): $%.2f\n", historicalRevenue);
    printf("Grand Total Revenue: $%.2f\n", currentRevenue + historicalRevenue);

    printf("\nPress Enter to go back...");
    getchar(); getchar();
}

void historyMenu() {
    int choice;
    while (1) {
        clearScreen();
        printf("\n=== HISTORY ===\n");
        printf("1. Guest History (Beginning to End)\n");
        printf("2. Revenue History\n");
        printf("0. Back to Main Menu\n");
        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 1) showGuestHistory();
        else if (choice == 2) showRevenueHistory();
        else if (choice == 0) return;
        else printf("Invalid choice!\n");
    }
}

// ---------------------------------------------------------
// STAFF MANAGEMENT FUNCTIONS
// ---------------------------------------------------------

void checkAndGenerateDatabase();
void mainMenu();
void shiftMenu();
void roleMenu(const char* shift);
void actionMenu(const char* shift, const char* role, int max_limit);
void recruitEmployee(const char* shift, const char* role, int max_limit);
void viewAll(const char* shift, const char* role);
void removeEmployee(const char* shift, const char* role);
void searchByID();
void generateID(char* newID, const char* shift, const char* role);
void clearBuffer();

void checkAndGenerateDatabase() {
    FILE *file = fopen(FILE_NAME, "r");
    if (file != NULL) {
        fclose(file);
        return;
    }

    file = fopen(FILE_NAME, "w");
    if (file == NULL) {
        printf("[ERROR] Database creation failed!\n");
        return;
    }

    char shifts[3][10] = {"Day", "Evening", "Night"};
    char roles[3][10] = {"Admin", "Manager", "Staff"};
    char shiftCodes[3][3] = {"DS", "ES", "NS"};
    char roleCodes[3][4] = {"101", "201", "301"};

    int limits[3] = {2, 5, 50};

    char firstNames[10][20] = {"Zihad", "Rezuan", "Sajjad", "Rahim", "Tarek", "Mehedi", "Fahim", "Nusrat", "Sadia", "Kamrul"};
    char lastNames[10][20] = {"Hasan", "Hassan", "Ahmed", "Islam", "Hossain", "Uddin", "Rahman", "Khan", "Akter", "Mia"};

    for (int s = 0; s < 3; s++) {
        for (int r = 0; r < 3; r++) {
            for (int i = 1; i <= limits[r]; i++) {
                int fIdx = (s * 11 + r * 7 + i * 3) % 10;
                int lIdx = (s * 13 + r * 5 + i * 7) % 10;
                char fullName[50];
                sprintf(fullName, "%s %s", firstNames[fIdx], lastNames[lIdx]);

                char id[20];
                sprintf(id, "%s-%s-%02d", shiftCodes[s], roleCodes[r], i);

                fprintf(file, "%s,%s,01700%06d,%s,%s\n",
                        id, fullName, i * 1111, shifts[s], roles[r]);
            }
        }
    }
    fclose(file);
    printf("\n[INFO] Initial Database with 171 employees automatically generated!\n");
}

void mainMenu() {
    int choice;
    while (1) {
        clearScreen();
        printf("\n=========================================\n");
        printf("   HOTEL MANAGEMENT - STAFF SYSTEM       \n");
        printf("=========================================\n");
        printf("1. Manage Staff by Shift\n");
        printf("2. Search Employee with ID\n");
        printf("0. Return to Hotel Admin Portal\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
            case 1: shiftMenu(); break;
            case 2: searchByID(); break;
            case 0:
                printf("Returning to Hotel Admin Portal...\n");
                return;
            default: printf("[ERROR] Invalid choice! Try again.\n");
        }
    }
}

void shiftMenu() {
    int choice;
    while (1) {
        clearScreen();
        printf("\n--- SELECT SHIFT ---\n");
        printf("1. Day Shift\n2. Evening Shift\n3. Night Shift\n0. Go to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
            case 1: roleMenu("Day"); break;
            case 2: roleMenu("Evening"); break;
            case 3: roleMenu("Night"); break;
            case 0: return;
            default: printf("[ERROR] Invalid choice!\n");
        }
    }
}

void roleMenu(const char* shift) {
    int choice;
    while (1) {
        clearScreen();
        printf("\n--- %s SHIFT: SELECT ROLE ---\n", shift);
        printf("1. Admin\n2. Manager\n3. Staff\n0. Go Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
            case 1: actionMenu(shift, "Admin", 2); break;
            case 2: actionMenu(shift, "Manager", 5); break;
            case 3: actionMenu(shift, "Staff", -1); break;
            case 0: return;
            default: printf("[ERROR] Invalid choice!\n");
        }
    }
}

void actionMenu(const char* shift, const char* role, int max_limit) {
    int choice;
    while (1) {
        clearScreen();
        printf("\n--- %s SHIFT: %s ACTIONS ---\n", shift, role);
        printf("1. View All %s\n2. Recruit %s\n3. Remove %s\n0. Go Back\n", role, role, role);
        printf("Enter choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
            case 1: viewAll(shift, role); break;
            case 2: recruitEmployee(shift, role, max_limit); break;
            case 3: removeEmployee(shift, role); break;
            case 0: return;
            default: printf("[ERROR] Invalid choice!\n");
        }
    }
}

void generateID(char* newID, const char* shift, const char* role) {
    char sCode[3], rCode[4];

    if (strcmp(shift, "Day") == 0) strcpy(sCode, "DS");
    else if (strcmp(shift, "Evening") == 0) strcpy(sCode, "ES");
    else strcpy(sCode, "NS");

    if (strcmp(role, "Admin") == 0) strcpy(rCode, "101");
    else if (strcmp(role, "Manager") == 0) strcpy(rCode, "201");
    else strcpy(rCode, "301");

    FILE *file = fopen(FILE_NAME, "r");
    int maxSerial = 0;

    if (file != NULL) {
        struct Employee emp;
        while (fscanf(file, " %[^,],%[^,],%[^,],%[^,],%[^\n]", emp.id, emp.name, emp.phone, emp.shift, emp.role) == 5) {
            if (strcmp(emp.shift, shift) == 0 && strcmp(emp.role, role) == 0) {
                char *lastDash = strrchr(emp.id, '-');
                if (lastDash != NULL) {
                    int currentSerial = atoi(lastDash + 1);
                    if (currentSerial > maxSerial) {
                        maxSerial = currentSerial;
                    }
                }
            }
        }
        fclose(file);
    }
    sprintf(newID, "%s-%s-%02d", sCode, rCode, maxSerial + 1);
}

void recruitEmployee(const char* shift, const char* role, int max_limit) {
    FILE *file = fopen(FILE_NAME, "a+");
    if (file == NULL) {
        printf("[ERROR] Cannot open database!\n");
        return;
    }

    struct Employee emp;
    int count = 0;
    rewind(file);

    while (fscanf(file, " %[^,],%[^,],%[^,],%[^,],%[^\n]", emp.id, emp.name, emp.phone, emp.shift, emp.role) == 5) {
        if (strcmp(emp.shift, shift) == 0 && strcmp(emp.role, role) == 0) {
            count++;
        }
    }

    if (max_limit != -1 && count >= max_limit) {
        printf("\n[WARNING] Registration full for %s in %s Shift! Limit reached.\n", role, shift);
        fclose(file);
        printf("\nPress Enter to go back...");
        getchar(); getchar();
        return;
    }

    struct Employee newEmp;
    generateID(newEmp.id, shift, role);

    printf("\n--- Recruit New %s ---\n", role);
    printf("[INFO] Auto-generated ID: %s\n", newEmp.id);

    printf("Enter Name: ");
    fgets(newEmp.name, 50, stdin);
    newEmp.name[strcspn(newEmp.name, "\n")] = 0;

    printf("Enter Contact Number: ");
    fgets(newEmp.phone, 15, stdin);
    newEmp.phone[strcspn(newEmp.phone, "\n")] = 0;

    strcpy(newEmp.shift, shift);
    strcpy(newEmp.role, role);

    fprintf(file, "%s,%s,%s,%s,%s\n", newEmp.id, newEmp.name, newEmp.phone, newEmp.shift, newEmp.role);

    printf("\n[SUCCESS] %s successfully recruited! (ID: %s)\n", newEmp.name, newEmp.id);
    fclose(file);

    printf("\nPress Enter to go back...");
    getchar(); getchar();
}

void viewAll(const char* shift, const char* role) {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        printf("\n[INFO] Database is empty.\n");
        printf("\nPress Enter to go back...");
        getchar(); getchar();
        return;
    }

    struct Employee emp;
    int found = 0;

    printf("\n--- List of %s (%s Shift) ---\n", role, shift);
    printf("%-12s | %-25s | %-15s\n", "ID", "NAME", "CONTACT");
    printf("----------------------------------------------------------\n");

    while (fscanf(file, " %[^,],%[^,],%[^,],%[^,],%[^\n]", emp.id, emp.name, emp.phone, emp.shift, emp.role) == 5) {
        if (strcmp(emp.shift, shift) == 0 && strcmp(emp.role, role) == 0) {
            printf("%-12s | %-25s | %-15s\n", emp.id, emp.name, emp.phone);
            found = 1;
        }
    }

    if (!found) printf("No records found for this category.\n");
    printf("----------------------------------------------------------\n");
    fclose(file);

    printf("\nPress Enter to go back...");
    getchar(); getchar();
}

void removeEmployee(const char* shift, const char* role) {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        printf("\n[ERROR] Database not found.\n");
        printf("\nPress Enter to go back...");
        getchar(); getchar();
        return;
    }

    FILE *tempFile = fopen("temp.txt", "w");
    char removeId[20];
    int found = 0;

    printf("\nEnter ID of %s to remove (e.g., DS-101-01): ", role);
    scanf("%19s", removeId);
    clearBuffer();

    struct Employee emp;

    while (fscanf(file, " %[^,],%[^,],%[^,],%[^,],%[^\n]", emp.id, emp.name, emp.phone, emp.shift, emp.role) == 5) {
        if (strcmp(emp.id, removeId) == 0 && strcmp(emp.shift, shift) == 0 && strcmp(emp.role, role) == 0) {
            found = 1;
        } else {
            fprintf(tempFile, "%s,%s,%s,%s,%s\n", emp.id, emp.name, emp.phone, emp.shift, emp.role);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove(FILE_NAME);
        rename("temp.txt", FILE_NAME);
        printf("\n[SUCCESS] Employee with ID %s removed successfully.\n", removeId);
    } else {
        remove("temp.txt");
        printf("\n[ERROR] No %s found with ID %s in %s Shift.\n", role, removeId, shift);
    }

    printf("\nPress Enter to go back...");
    getchar(); getchar();
}

void searchByID() {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        printf("\n[ERROR] Database not found.\n");
        printf("\nPress Enter to go back...");
        getchar(); getchar();
        return;
    }

    char searchId[20];
    int found = 0;
    printf("\nEnter Employee ID to search (e.g., NS-301-15): ");
    scanf("%19s", searchId);
    clearBuffer();

    struct Employee emp;

    while (fscanf(file, " %[^,],%[^,],%[^,],%[^,],%[^\n]", emp.id, emp.name, emp.phone, emp.shift, emp.role) == 5) {
        if (strcmp(emp.id, searchId) == 0) {
            printf("\n==================================\n");
            printf("       EMPLOYEE DETAILS           \n");
            printf("==================================\n");
            printf(" ID       : %s\n", emp.id);
            printf(" Name     : %s\n", emp.name);
            printf(" Contact  : %s\n", emp.phone);
            printf(" Shift    : %s Shift\n", emp.shift);
            printf(" Role     : %s\n", emp.role);
            printf("==================================\n");
            found = 1;
            break;
        }
    }
    if (!found) printf("\n[ERROR] No employee found with ID %s.\n", searchId);
    fclose(file);

    printf("\nPress Enter to go back...");
    getchar(); getchar();
}

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ---------------------------------------------------------
// SECTION 1: INVOICE & CHECK-IN MODULE
// ---------------------------------------------------------

void generateInvoice(struct Room* r, double total, double paid, double vatAmt, const char* method) {
    clearScreen();
    printf("\n***************************************************");
    printf("\n             InterContinental Dhaka                ");
    printf("\n***************************************************");
    printf("\n Invoice To      : "); printWithSpaces(r->customerName);
    printf("\n Customer ID     : %s", r->customerID);
    printf("\n Date            : %s", r->checkInDate);
    printf("\n Room Number     : %d (%s %s)", r->roomNumber, r->category, r->bedType);
    printf("\n Stay Duration   : %d Days (+1 Day Security)", r->stayDays);
    printf("\n---------------------------------------------------");
    printf("\n Room Charge     : $%d/day", r->price);
    printf("\n VAT Amount      : $%.2f", vatAmt);
    printf("\n TOTAL PAYABLE   : $%.2f", total);
    printf("\n PAID AMOUNT     : $%.2f", paid);
    printf("\n CHANGE/BALANCE  : $%.2f", paid - total);
    printf("\n PAYMENT METHOD  : %s", method);
    printf("\n---------------------------------------------------");
    printf("\n >>> DIGITAL ROOM KEY GENERATED: [KEY-%d-%X] <<<", r->roomNumber, rand() % 10000);
    printf("\n***************************************************\n");
    printf("\nCheck-in Successful! Press Enter to go back...");
    getchar(); getchar();
}

void processCheckIn(struct Room* r, int isReturning, char* oldName, char* oldPhone, char* oldTier, char* oldID) {
    clearScreen();
    printf("\n--- Finalizing Check-In for Room %d (%s %s) ---\n", r->roomNumber, r->category, r->bedType);

    if(!isReturning) {
        printf("Enter Customer Name: ");
        scanf(" %[^\n]s", r->customerName);
        formatForSave(r->customerName);
        printf("Enter Phone Number: ");
        scanf(" %s", r->phoneNumber);
        setTierByCategory(r->customerTier, r->category);
        sprintf(r->customerID, "%c%c-%d-%d", r->category[0], r->bedType[0], r->roomNumber, nextCustomerId++);
    } else {
        strcpy(r->customerName, oldName); strcpy(r->phoneNumber, oldPhone);
        setTierByCategory(r->customerTier, r->category); strcpy(r->customerID, oldID);
    }

    printf("Enter Stay Duration (Days) (0 to cancel): ");
    scanf("%d", &r->stayDays);
    if(r->stayDays == 0) return;

    getCurrentDate(r->checkInDate);
    printf("Enter Admin Comment: ");
    scanf(" %[^\n]s", r->adminComment);
    formatForSave(r->adminComment);

    double vatPercent = (strcmp(r->category, "Classic") == 0) ? 0.15 : (strcmp(r->category, "Premium") == 0) ? 0.20 : 0.30;
    double baseCost = (r->stayDays + 1) * r->price;
    double vatAmount = baseCost * vatPercent;
    double totalCost = baseCost + vatAmount;

    printf("\nTotal Cost (incl. VAT & 1 day security deposit): $%.2f", totalCost);

    int payMethod;
    printf("\nSelect Payment Method:\n1. Cash\n2. Bank Transfer\n0. Cancel Check-in\nChoice: ");
    scanf("%d", &payMethod);
    if(payMethod == 0) return;
    char methodStr[20];
    strcpy(methodStr, (payMethod == 1) ? "Cash" : "Bank Transfer");

    double deposit;
    do {
        printf("\nEnter Deposit Amount (Min $%.2f) or 0 to Cancel: ", totalCost);
        scanf("%lf", &deposit);
        if (deposit == 0) return;
        if (deposit < totalCost) printf("Insufficient amount!\n");
    } while (deposit < totalCost);

    r->isBooked = 1; r->paidAmount = deposit;
    saveDataToTXT();
    generateInvoice(r, totalCost, deposit, vatAmount, methodStr);
}

void checkInMenu() {
    int catChoice, bedChoice, rNum; char selectedCat[20], selectedBed[20];
    clearScreen();
    printf("\n=== New Check-In ===\n1. Classic\n2. Premium\n3. Presidential\n0. Back\nChoice: ");
    scanf("%d", &catChoice); if (catChoice == 0) return;
    if (catChoice == 1) strcpy(selectedCat, "Classic"); else if (catChoice == 2) strcpy(selectedCat, "Premium"); else strcpy(selectedCat, "Presidential");

    clearScreen();
    printf("\nSelect Bed Type for %s:\n1. Single\n2. Double\n3. Multiple\n0. Back\nChoice: ", selectedCat);
    scanf("%d", &bedChoice); if(bedChoice == 0) return;
    strcpy(selectedBed, (bedChoice == 1) ? "Single" : (bedChoice == 2) ? "Double" : "Multiple");

    clearScreen();
    printf("\nAvailable %s %s Rooms:\n", selectedCat, selectedBed);
    struct Room* temp = head; int count = 0;
    while (temp != NULL) {
        if (temp->isBooked == 0 && strcmp(temp->category, selectedCat) == 0 && strcmp(temp->bedType, selectedBed) == 0) {
            printf("[%d] ", temp->roomNumber); count++;
        }
        temp = temp->next;
    }
    if (count == 0) { printf("\nNo rooms available.\nPress Enter to go back..."); getchar(); getchar(); return; }

    printf("\n\nEnter Room Number to Check-in (0 to go back): "); scanf("%d", &rNum); if(rNum == 0) return;
    temp = head;
    while (temp != NULL) {
        if (temp->roomNumber == rNum && temp->isBooked == 0) { processCheckIn(temp, 0, "", "", "", ""); return; }
        temp = temp->next;
    }
    printf("Invalid Room!\nPress Enter to go back..."); getchar(); getchar();
}

// ---------------------------------------------------------
// RETURNING CUSTOMER
// ---------------------------------------------------------

void returningCustomerCheckIn() {
    while(1) {
        clearScreen();
        printf("\n=== Returning Customer Section ===\n");
        printf("1. Returning Customer - Book New Room (by Mobile Number)\n");
        printf("2. Renew Existing Room (by Customer ID)\n");
        printf("0. Back to Main Menu\n");
        printf("Choice: ");
        int mainChoice;
        scanf("%d", &mainChoice);

        if(mainChoice == 0) return;

        if(mainChoice == 1) {   // === Returning Customer - New Booking ===
            char searchPhone[20];
            clearScreen();
            printf("\n=== Returning Customer - New Booking ===\n");
            printf("Enter Your Mobile Number: ");
            scanf("%s", searchPhone);

            struct Room* temp = head;
            char oldName[50], oldPhone[20], oldTier[20], oldID[30];
            int found = 0;

            while(temp != NULL) {
                if(strcmp(temp->phoneNumber, searchPhone) == 0 && strcmp(temp->phoneNumber, "N/A") != 0) {
                    strcpy(oldName, temp->customerName);
                    strcpy(oldPhone, temp->phoneNumber);
                    strcpy(oldTier, temp->customerTier);
                    strcpy(oldID, temp->customerID);
                    found = 1;
                    break;
                }
                temp = temp->next;
            }

            if(!found) {
                printf("\nNo previous booking found with this mobile number!\n");
                printf("Press Enter to try again..."); getchar(); getchar();
                continue;
            }

            printf("\nWelcome back, "); printWithSpaces(oldName); printf("!\n");
            printf("Mobile number matched. You can now book a NEW room.\n");

            int catChoice, bedChoice, rNum;
            char selectedCat[20], selectedBed[20];

            clearScreen();
            printf("\n=== Book a New Room ===\n1. Classic\n2. Premium\n3. Presidential\n0. Back\nChoice: ");
            scanf("%d", &catChoice);
            if(catChoice == 0) continue;
            if(catChoice == 1) strcpy(selectedCat, "Classic");
            else if(catChoice == 2) strcpy(selectedCat, "Premium");
            else strcpy(selectedCat, "Presidential");

            clearScreen();
            printf("\nSelect Bed Type for %s:\n1. Single\n2. Double\n3. Multiple\n0. Back\nChoice: ", selectedCat);
            scanf("%d", &bedChoice);
            if(bedChoice == 0) continue;
            strcpy(selectedBed, (bedChoice==1)?"Single":(bedChoice==2)?"Double":"Multiple");

            clearScreen();
            printf("\nAvailable %s %s Rooms:\n", selectedCat, selectedBed);
            temp = head; int count = 0;
            while(temp != NULL) {
                if(temp->isBooked == 0 && strcmp(temp->category, selectedCat)==0 && strcmp(temp->bedType, selectedBed)==0) {
                    printf("[%d] ", temp->roomNumber); count++;
                }
                temp = temp->next;
            }
            if(count == 0) {
                printf("\nNo rooms available.\nPress Enter..."); getchar(); getchar(); continue;
            }

            printf("\n\nEnter Room Number to Check-in (0 to go back): ");
            scanf("%d", &rNum);
            if(rNum == 0) continue;

            temp = head; int roomFound = 0;
            while(temp != NULL) {
                if(temp->roomNumber == rNum && temp->isBooked == 0) {
                    processCheckIn(temp, 1, oldName, oldPhone, oldTier, oldID);
                    roomFound = 1;
                    return;
                }
                temp = temp->next;
            }
            if(!roomFound) {
                printf("Invalid Room or already booked!\nPress Enter..."); getchar(); getchar();
            }
        }
        else if(mainChoice == 2) {   // === Renew Existing Room ===
            char searchID[30];
            clearScreen();
            printf("\n=== Renew Existing Room ===\nEnter Customer ID: ");
            scanf("%s", searchID);

            struct Room* currentRoom = NULL;
            struct Room* temp = head;
            while(temp != NULL) {
                if(strcmp(temp->customerID, searchID) == 0 && temp->isBooked == 1) {
                    currentRoom = temp;
                    break;
                }
                temp = temp->next;
            }

            if(currentRoom == NULL) {
                printf("\nNo active booking found with this Customer ID!\nPress Enter...");
                getchar(); getchar(); continue;
            }

            printf("\nWelcome back, "); printWithSpaces(currentRoom->customerName); printf("!\n");

            int extraDays;
            printf("\nEnter additional days to stay (0 to go back): ");
            scanf("%d", &extraDays);
            if(extraDays == 0) continue;

            double vatPercent = (strcmp(currentRoom->category, "Classic") == 0) ? 0.15 :
                                (strcmp(currentRoom->category, "Premium") == 0) ? 0.20 : 0.30;
            double baseCost = extraDays * currentRoom->price;
            double vatAmount = baseCost * vatPercent;
            double totalCost = baseCost + vatAmount;

            printf("\nRenewal Cost (including VAT): $%.2f\n", totalCost);
            int payMethod;
            printf("Select Payment Method:\n1. Cash\n2. Bank Transfer\n0. Back\nChoice: ");
            scanf("%d", &payMethod);
            if(payMethod == 0) continue;

            double deposit;
            do {
                printf("\nEnter Payment Amount (Min $%.2f, 0 to cancel): ", totalCost);
                scanf("%lf", &deposit);
                if(deposit == 0) break;
                if(deposit < totalCost) printf("Insufficient amount!\n");
            } while(deposit < totalCost);

            if(deposit == 0) continue;

            currentRoom->stayDays += extraDays;
            currentRoom->paidAmount += deposit;
            saveDataToTXT();

            clearScreen();
            printf("\n***************************************************\n             ROOM RENEWAL INVOICE                  \n***************************************************");
            printf("\n Invoice To      : "); printWithSpaces(currentRoom->customerName);
            printf("\n Customer ID     : %s\n Room Number     : %d (%s)", currentRoom->customerID, currentRoom->roomNumber, currentRoom->category);
            printf("\n Renewed For     : %d Extra Days\n Total Stay Now  : %d Days\n---------------------------------------------------", extraDays, currentRoom->stayDays);
            printf("\n VAT Amount      : $%.2f\n TOTAL PAYABLE   : $%.2f\n PAID AMOUNT     : $%.2f\n CHANGE/BALANCE  : $%.2f", vatAmount, totalCost, deposit, deposit - totalCost);
            printf("\n***************************************************\n\nRenewal Successful! Press Enter to go back...");
            getchar(); getchar();
            return;
        }
        else {
            printf("Invalid choice!\n");
        }
    }
}

// ---------------------------------------------------------
// SECTION 2: CHECK-OUT MODULE
// ---------------------------------------------------------

void executeCheckOut(struct Room* r) {
    clearScreen();
    char checkOutDate[20];
    getCurrentDate(checkOutDate);

    double vatPercent = (strcmp(r->category, "Classic") == 0) ? 0.15 : (strcmp(r->category, "Premium") == 0) ? 0.20 : 0.30;
    double baseCost = (r->stayDays + 1) * r->price;
    double vatAmount = baseCost * vatPercent;
    double totalBill = baseCost + vatAmount;
    double refundOrChange = r->paidAmount - totalBill;

    printf("\n***************************************************");
    printf("\n             InterContinental Dhaka             ");
    printf("\n               CHECK-OUT INVOICE                   ");
    printf("\n***************************************************");
    printf("\n Customer Name   : "); printWithSpaces(r->customerName);
    printf("\n Customer ID     : %s", r->customerID);
    printf("\n Room Number     : %d (%s %s)", r->roomNumber, r->category, r->bedType);
    printf("\n Check-In Date   : %s", r->checkInDate);
    printf("\n Check-Out Date  : %s", checkOutDate);
    printf("\n Total Stay      : %d Days", r->stayDays);
    printf("\n---------------------------------------------------");
    printf("\n TOTAL BILL      : $%.2f", totalBill);
    printf("\n TOTAL PAID      : $%.2f", r->paidAmount);
    printf("\n REFUND/CHANGE   : $%.2f", refundOrChange);
    printf("\n***************************************************");

    printf("\n\nThank you for staying with us! We hope you had a wonderful time.");
    printf("\nHow was your experience?\n1. Excellent  2. Good  3. Bad\nChoice: ");
    int feedback;
    scanf("%d", &feedback);

    char reason[200];
    printf("\nPlease tell us why: ");
    scanf(" %[^\n]s", reason);
    formatForSave(reason);

    addFeedback(r->customerID, r->customerName, feedback, reason);

    if(feedback == 1) printf("\n[System]: We are thrilled to hear that! Safe travels.\n");
    else if(feedback == 2) printf("\n[System]: Thank you! We're glad you had a good stay.\n");
    else printf("\n[System]: Thank you for your feedback. We will work hard to improve.\n");

    logGuestHistory(r, totalBill, checkOutDate);

    r->isBooked = 0;
    strcpy(r->customerID, "N/A"); strcpy(r->customerName, "N/A"); strcpy(r->customerTier, "N/A");
    strcpy(r->phoneNumber, "N/A"); strcpy(r->adminComment, "N/A"); strcpy(r->checkInDate, "N/A");
    r->stayDays = 0; r->paidAmount = 0.0;

    saveDataToTXT();
    printf("\n[System]: Check-out completed successfully. Room %d is now available.", r->roomNumber);
    printf("\nPress Enter to go back to Main Menu...");
    getchar(); getchar();
}

void checkOutMenu() {
    clearScreen();
    int choice;
    printf("\n=== Check-Out System ===\n1. Search by Room Number\n2. Search by Customer ID\n0. Back\nChoice: ");
    scanf("%d", &choice);
    if(choice == 0) return;

    struct Room* temp = head; struct Room* targetRoom = NULL;

    if (choice == 1) {
        int rNum; printf("Enter Room Number: "); scanf("%d", &rNum);
        while (temp != NULL) {
            if (temp->roomNumber == rNum && temp->isBooked == 1) { targetRoom = temp; break; }
            temp = temp->next;
        }
    } else if (choice == 2) {
        char cid[30]; printf("Enter Customer ID: "); scanf("%s", cid);
        while (temp != NULL) {
            if (strcmp(temp->customerID, cid) == 0 && temp->isBooked == 1) { targetRoom = temp; break; }
            temp = temp->next;
        }
    }

    if (targetRoom != NULL) {
        printf("\nCustomer Found: "); printWithSpaces(targetRoom->customerName);
        printf(" | Room: %d\n", targetRoom->roomNumber);
        char confirm;
        printf("Do you want to confirm check-out? (y/n): ");
        scanf(" %c", &confirm);
        if (confirm == 'y' || confirm == 'Y') executeCheckOut(targetRoom);
        else { printf("Check-out cancelled. Press Enter to go back..."); getchar(); getchar(); }
    } else {
        printf("\n[Error]: No active booking found.\nPress Enter to go back..."); getchar(); getchar();
    }
}

// ---------------------------------------------------------
// SECTION 3: FEEDBACK MODULE
// ---------------------------------------------------------

void showFeedbackDetails(int ratingFilter, const char* ratingName) {
    clearScreen();
    struct Feedback* temp = fHead;
    int count = 0;

    while (temp != NULL) {
        if (temp->rating == ratingFilter) count++;
        temp = temp->next;
    }

    printf("\n=============================================================\n");
    printf("                  %s REVIEWS (%d)                    \n", ratingName, count);
    printf("=============================================================\n\n");

    temp = fHead;
    while (temp != NULL) {
        if (temp->rating == ratingFilter) {
            printf("Customer ID : %s\n", temp->customerID);
            printf("Name        : "); printWithSpaces(temp->customerName); printf("\n");
            printf("Reason      : "); printWithSpaces(temp->reason); printf("\n");
            printf("-------------------------------------------------------------\n");
        }
        temp = temp->next;
    }

    if (count == 0) printf("No %s reviews found yet.\n", ratingName);

    printf("\nPress Enter to go back...");
    getchar(); getchar();
}

void feedbackMenu() {
    int choice;
    while(1) {
        clearScreen();
        printf("\n=== Customer Feedbacks ===\n");
        printf("1. Excellent Reviews\n");
        printf("2. Good Reviews\n");
        printf("3. Bad Reviews\n");
        printf("0. Back to Main Menu\n");
        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 1) showFeedbackDetails(1, "EXCELLENT");
        else if (choice == 2) showFeedbackDetails(2, "GOOD");
        else if (choice == 3) showFeedbackDetails(3, "BAD");
        else if (choice == 0) return;
    }
}

// ---------------------------------------------------------
// SECTION 4: ROOM INVENTORY & SEARCH MODULE
// ---------------------------------------------------------

void searchRoom() {
    clearScreen();
    int rNum, found = 0; char choice;
    printf("\n=== Search Room ===\nEnter Room Number to search (0 to go back): ");
    scanf("%d", &rNum); if(rNum == 0) return;

    struct Room* temp = head;
    while (temp != NULL) {
        if (temp->roomNumber == rNum) {
            found = 1;
            printf("\n--- Room Basic Details ---\nRoom Number : %d | Category: %s | Bed: %s\n", temp->roomNumber, temp->category, temp->bedType);
            printf("Hotline     : %s | Status: %s\n", temp->hotline, (temp->isBooked) ? "BOOKED" : "AVAILABLE");

            if (temp->isBooked) {
                printf("\n--- Active Customer Details ---\nName: "); printWithSpaces(temp->customerName);
                printf(" | ID: %s | Tier: %s\nPhone: %s | Current Comment: ", temp->customerID, temp->customerTier, temp->phoneNumber);
                printWithSpaces(temp->adminComment);
                printf("\nCheck-In Date: %s | Stay Duration: %d Days\n", temp->checkInDate, temp->stayDays);

                printf("\nDo you want to change the admin comment? (y/n): ");
                scanf(" %c", &choice);
                if (choice == 'y' || choice == 'Y') {
                    printf("Enter new comment: "); scanf(" %[^\n]s", temp->adminComment);
                    formatForSave(temp->adminComment); saveDataToTXT();
                    printf("\n[System]: Comment updated successfully!\n");
                }
            }
            break;
        }
        temp = temp->next;
    }
    if (!found) printf("\n[System]: Room Number %d not found.\n", rNum);
    printf("\nPress Enter to go back..."); getchar(); getchar();
}

void showSpecificAvailableRooms(const char* cat, const char* bed) {
    clearScreen(); struct Room* temp = head; int count = 0;
    printf("\n=== Available %s %s Rooms ===\n", cat, bed);
    while (temp != NULL) {
        if (temp->isBooked == 0 && strcmp(temp->category, cat) == 0 && strcmp(temp->bedType, bed) == 0) {
            printf("Room Number: %d | Hotline: %s\n", temp->roomNumber, temp->hotline); count++;
        }
        temp = temp->next;
    }
    if (count == 0) printf("\nNo available rooms in this category.\n");
    printf("\nPress Enter to go back..."); getchar(); getchar();
}

void availableRoomsMenu() {
    int catChoice, bedChoice; char selectedCat[20];
    while (1) {
        clearScreen();
        printf("\n=== Available Rooms ===\n1. Classic\n2. Premium\n3. Presidential\n0. Back\nChoice: ");
        scanf("%d", &catChoice); if (catChoice == 0) return;

        int p1, p2, p3;
        if (catChoice == 1) { strcpy(selectedCat, "Classic"); p1=200; p2=250; p3=300; }
        else if (catChoice == 2) { strcpy(selectedCat, "Premium"); p1=300; p2=350; p3=400; }
        else if (catChoice == 3) { strcpy(selectedCat, "Presidential"); p1=500; p2=600; p3=700; }
        else continue;

        clearScreen();
        printf("\nSelect Bed Type for %s:\n1. Single ($%d)\n2. Double ($%d)\n3. Multiple ($%d)\n0. Back\nSelect Bed: ", selectedCat, p1, p2, p3);
        scanf("%d", &bedChoice); if (bedChoice == 0) continue;
        if (bedChoice == 1) showSpecificAvailableRooms(selectedCat, "Single");
        else if (bedChoice == 2) showSpecificAvailableRooms(selectedCat, "Double");
        else if (bedChoice == 3) showSpecificAvailableRooms(selectedCat, "Multiple");
    }
}

void showActiveCustomers() {
    clearScreen(); struct Room* temp = head; int count = 0;
    printf("\n=========================================================================\n");
    printf("                      ACTIVE CUSTOMER DETAILS                            \n");
    printf("=========================================================================\n");
    while (temp != NULL) {
        if (temp->isBooked == 1) {
            printf("ID: %-15s | Name: ", temp->customerID); printWithSpaces(temp->customerName);
            printf("\nTier: %-13s | Room: %d (%s)\n", temp->customerTier, temp->roomNumber, temp->category);
            printf("Phone: %-14s | Days: %-14d | Comment: ", temp->phoneNumber, temp->stayDays);
            printWithSpaces(temp->adminComment);
            printf("\n-------------------------------------------------------------------------\n");
            count++;
        }
        temp = temp->next;
    }
    printf("\nTotal Active Customers: %d\n", count);
    printf("Press Enter to go back..."); getchar(); getchar();
}

void roomInventoryMenu() {
    int choice;
    while (1) {
        clearScreen();
        printf("\n=== Room Inventory Menu ===\n1. Available Rooms\n2. Booked Rooms / Active Customers\n3. Search Room & Update Comment\n0. Back\nChoice: ");
        scanf("%d", &choice);
        if (choice == 1) availableRoomsMenu();
        else if (choice == 2) showActiveCustomers();
        else if (choice == 3) searchRoom();
        else if (choice == 0) return;
    }
}

// ---------------------------------------------------------
// MAIN FUNCTION
// ---------------------------------------------------------
int main() {
    srand(time(0));

    if (!loginSystem()) {
        clearScreen();
        printf("\n[ACCESS DENIED] Too many failed login attempts.\n");
        printf("The program will now close.\n");
        return 0;
    }

    loadDataFromTXT();
    loadFeedbackFromTXT();

    checkAndGenerateDatabase();

    int choice;
    while (1) {
        clearScreen();
        printf("\n=================================\n");
        printf("      HOTEL ADMIN PORTAL       \n");
        printf("=================================\n");
        printf("1. Room Inventory & Search\n");
        printf("2. Active Customer Details\n");
        printf("3. New Customer Check-In\n");
        printf("4. Returning Customer Check-In\n");
        printf("5. Check-Out\n");
        printf("6. Customer Feedbacks\n");
        printf("7. Staff Management\n");
        printf("8. Revenue Section\n");
        printf("9. History\n");
        printf("0. Exit System\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: roomInventoryMenu(); break;
            case 2: showActiveCustomers(); break;
            case 3: checkInMenu(); break;
            case 4: returningCustomerCheckIn(); break;
            case 5: checkOutMenu(); break;
            case 6: feedbackMenu(); break;
            case 7: mainMenu(); break;
            case 8: revenueSection(); break;
            case 9: historyMenu(); break;
            case 0:
                saveDataToTXT();
                saveFeedbackToTXT();
                clearScreen();
                printf("System Exited Safely. All Data Saved Successfully.\n");
                exit(0);
            default:
                printf("Invalid choice!\n");
        }
    }
    return 0;
}
