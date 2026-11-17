/*****************************************************
Application: Inventory Management System (Standard ANSI C)
Compatible with: VS Code (GCC / Clang / MinGW)
*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#define TRUE 1
#define FALSE 0

typedef struct {
    char itemcode[8];
    char itemname[50];
    float itemrate;
    float itemqty;
    int minqty; // Reorder level minimum stock
} StockItem;

/* Function Prototypes */
void displayMenu();
int checkId(FILE *fp, const char *code, StockItem *matchedItem);
void addItem(FILE *fp);
void editItem(FILE *fp);
void showItemInfo(FILE *fp);
void viewStockReport(FILE *fp);
void issueItem(FILE *fp);
void viewReorderItems(FILE *fp);
void backupDatabase(FILE *fp);
void clearScreen();

int main() {
    FILE *dbfp;
    int choice;

    // Open database file in binary read/write mode; create if it doesn't exist
    dbfp = fopen("invstoc.dat", "r+b");
    if (dbfp == NULL) {
        dbfp = fopen("invstoc.dat", "w+b");
        if (dbfp == NULL) {
            printf("Error: Could not create database file!\n");
            return 1;
        }
        printf("Database file 'invstoc.dat' created successfully.\n");
    }

    while (1) {
        clearScreen();
        displayMenu();
        printf("\nEnter your choice (0-6): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        while (getchar() != '\n'); // Clear trailing newline

        switch (choice) {
            case 0:
                backupDatabase(dbfp);
                fclose(dbfp);
                printf("\nThank you for using Inventory Management System. Goodbye!\n");
                return 0;

            case 1:
                addItem(dbfp);
                break;

            case 2:
                editItem(dbfp);
                break;

            case 3:
                showItemInfo(dbfp);
                break;

            case 4:
                viewStockReport(dbfp);
                break;

            case 5:
                issueItem(dbfp);
                break;

            case 6:
                viewReorderItems(dbfp);
                break;

            default:
                printf("\nInvalid option! Please choose between 0 and 6.");
        }
        
        printf("\n\nPress Enter to continue...");
        getchar();
    }
    return 0;
}

/* Clear screen helper for cross-platform compatibility */
void clearScreen() {
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #else
        system("clear");
    #endif
}

/* Display Main Menu */
void displayMenu() {
    printf("=========================================\n");
    printf("     INVENTORY MANAGEMENT SYSTEM         \n");
    printf("=========================================\n");
    printf(" 1. Add New Item\n");
    printf(" 2. Edit Item Information\n");
    printf(" 3. Show Item Information\n");
    printf(" 4. View Stock Report\n");
    printf(" 5. Issue Items from Stock\n");
    printf(" 6. View Items to be Ordered (Reorder)\n");
    printf(" 0. Close Application & Backup\n");
    printf("=========================================\n");
}

// Check if Item Code exists in database
int checkId(FILE *fp, const char *code, StockItem *matchedItem) {
    StockItem temp;
    rewind(fp);
    while (fread(&temp, sizeof(StockItem), 1, fp) == 1) {
        if (strcmp(temp.itemcode, code) == 0) {
            if (matchedItem != NULL) {
                *matchedItem = temp;
            }
            return TRUE; // Found
        }
    }
    return FALSE; // Not found
}

/* Add an item to the database */
void addItem(FILE *fp) {
    StockItem item;
    char code[8];

    clearScreen();
    printf("--- ADD NEW ITEM ---\n");
    printf("Enter Item Code: ");
    scanf("%7s", code);
    while (getchar() != '\n');

    if (checkId(fp, code, NULL) == TRUE) {
        printf("\nError: An item with code '%s' already exists!\n", code);
        return;
    }

    strcpy(item.itemcode, code);
    printf("Enter Name of the Item: ");
    fgets(item.itemname, sizeof(item.itemname), stdin);
    item.itemname[strcspn(item.itemname, "\n")] = 0; // Remove newline

    printf("Enter Price of Each Unit: ");
    scanf("%f", &item.itemrate);

    printf("Enter Initial Quantity: ");
    scanf("%f", &item.itemqty);

    printf("Enter Reorder Level (Min Qty): ");
    scanf("%d", &item.minqty);
    while (getchar() != '\n');

    fseek(fp, 0, SEEK_END);
    fwrite(&item, sizeof(StockItem), 1, fp);
    printf("\nSuccess: Item has been successfully added!\n");
}

/* Edit existing item details */
void editItem(FILE *fp) {
    char code[8];
    StockItem item;
    long pos;

    clearScreen();
    printf("--- EDIT ITEM INFORMATION ---\n");
    printf("Enter Item Code to edit: ");
    scanf("%7s", code);
    while (getchar() != '\n');

    rewind(fp);
    int found = 0;
    while (fread(&item, sizeof(StockItem), 1, fp) == 1) {
        if (strcmp(item.itemcode, code) == 0) {
            pos = ftell(fp) - sizeof(StockItem);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("\nError: Item with code '%s' not found in database.\n", code);
        return;
    }

    printf("\nCurrent Details:\n");
    printf("Name: %s | Price: %.2f | Qty: %.4f | Min Qty: %d\n", item.itemname, item.itemrate, item.itemqty, item.minqty);

    printf("\nEnter New Name of the Item: ");
    fgets(item.itemname, sizeof(item.itemname), stdin);
    item.itemname[strcspn(item.itemname, "\n")] = 0;

    printf("Enter New Price of Each Unit: ");
    scanf("%f", &item.itemrate);

    printf("Enter New Quantity: ");
    scanf("%f", &item.itemqty);

    printf("Enter New Reorder Level: ");
    scanf("%d", &item.minqty);
    while (getchar() != '\n');

    fseek(fp, pos, SEEK_SET);
    fwrite(&item, sizeof(StockItem), 1, fp);
    printf("\nSuccess: Item information updated successfully!\n");
}

/* Show info for a specific item code */
void showItemInfo(FILE *fp) {
    char code[8];
    StockItem item;

    clearScreen();
    printf("--- SHOW ITEM INFORMATION ---\n");
    printf("Enter Item Code: ");
    scanf("%7s", code);
    while (getchar() != '\n');

    if (checkId(fp, code, &item) == TRUE) {
        printf("\n-----------------------------------------\n");
        printf(" Item Code    : %s\n", item.itemcode);
        printf(" Item Name    : %s\n", item.itemname);
        printf(" Unit Price   : Rs. %.2f\n", item.itemrate);
        printf(" Stock Qty    : %.4f\n", item.itemqty);
        printf(" Reorder Level: %d\n", item.minqty);
        printf("-----------------------------------------\n");
    } else {
        printf("\nError: Item not found in the database.\n");
    }
}

/* View Stock Report (All items & total investment) */
void viewStockReport(FILE *fp) {
    StockItem item;
    int numitems = 0;
    float tot_investment = 0;

    clearScreen();
    printf("--- STOCK REPORT ---\n\n");

    rewind(fp);
    while (fread(&item, sizeof(StockItem), 1, fp) == 1) {
        numitems++;
        tot_investment += (item.itemrate * item.itemqty);
        printf("[%d] Code: %s | Name: %s | Price: %.2f | Qty: %.4f | Min: %d\n", 
               numitems, item.itemcode, item.itemname, item.itemrate, item.itemqty, item.minqty);
    }

    printf("\n-----------------------------------------\n");
    printf(" Total Items in Stock: %d\n", numitems);
    printf(" Total Investment    : Rs. %.2f\n", tot_investment);
    printf("-----------------------------------------\n");
}

/* Issue items from stock */
void issueItem(FILE *fp) {
    char code[8];
    StockItem item;
    float issue_qty;
    long pos;

    clearScreen();
    printf("--- ISSUE ITEMS FROM STOCK ---\n");
    printf("Enter Item Code: ");
    scanf("%7s", code);
    while (getchar() != '\n');

    rewind(fp);
    int found = 0;
    while (fread(&item, sizeof(StockItem), 1, fp) == 1) {
        if (strcmp(item.itemcode, code) == 0) {
            pos = ftell(fp) - sizeof(StockItem);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("\nError: Item not found.\n");
        return;
    }

    printf("Item Name: %s | Available Qty: %.4f\n", item.itemname, item.itemqty);
    printf("Enter Quantity to Issue: ");
    scanf("%f", &issue_qty);
    while (getchar() != '\n');

    if ((item.itemqty - issue_qty) >= item.minqty) {
        item.itemqty -= issue_qty;
        fseek(fp, pos, SEEK_SET);
        fwrite(&item, sizeof(StockItem), 1, fp);
        printf("\nSuccess: Issued %.4f unit(s).\n", issue_qty);
        printf("Total Amount to Pay: Rs. %.2f\n", issue_qty * item.itemrate);
    } else {
        printf("\nError: Insufficient stock quantity! Cannot breach reorder level limit.\n");
        printf("Maximum allowable issue quantity: %.4f\n", item.itemqty - item.minqty);
    }
}

/* View items at or below reorder level */
void viewReorderItems(FILE *fp) {
    StockItem item;
    int count = 0;

    clearScreen();
    printf("--- ITEMS REQUIRING REORDER ---\n\n");

    rewind(fp);
    while (fread(&item, sizeof(StockItem), 1, fp) == 1) {
        if (item.itemqty <= item.minqty) {
            count++;
            printf("Code: %s | Name: %s | Current Qty: %.4f | Min Qty: %d\n", 
                   item.itemcode, item.itemname, item.itemqty, item.minqty);
        }
    }

    if (count == 0) {
        printf("All items are sufficiently stocked. No reorders needed at this time.\n");
    }
}

/* Backup database file */
void backupDatabase(FILE *fp) {
    FILE *fback;
    StockItem item;

    fback = fopen("Backup.dat", "wb");
    if (fback == NULL) {
        return;
    }
    rewind(fp);
    while (fread(&item, sizeof(StockItem), 1, fp) == 1) {
        fwrite(&item, sizeof(StockItem), 1, fback);
    }
    fclose(fback);
}