#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char type[20]; // "Deposit" or "Withdraw"
    double amount;
    char timestamp[30];
} Transaction;

typedef struct {
    char accountNumber[20];
    char accountHolderName[50];
    char password[20];
    double balance;
    Transaction* transactions;
    int transactionCount;
} Account;

typedef struct {
    Account* accounts;
    int accountCount;
} Bank;

void createTransaction(Transaction* transaction, const char* type, double amount) {
    strcpy(transaction->type, type);
    transaction->amount = amount;
    time_t now = time(0);
    strcpy(transaction->timestamp, ctime(&now));
}

void displayTransaction(const Transaction* transaction) {
    printf("%10s | %10.2f | %s", transaction->type, transaction->amount, transaction->timestamp);
}

void createAccount(Account* account, const char* accNumber, const char* accHolderName, const char* pwd, double initialBalance) {
    strcpy(account->accountNumber, accNumber);
    strcpy(account->accountHolderName, accHolderName);
    strcpy(account->password, pwd);
    account->balance = initialBalance;
    account->transactions = NULL;
    account->transactionCount = 0;
}

void deposit(Account* account, double amount) {
    if (amount > 0) {
        account->balance += amount;
        account->transactions = (Transaction*)realloc(account->transactions, (account->transactionCount + 1) * sizeof(Transaction));
        if (account->transactions == NULL) {
            printf("Memory allocation failed!\n");
            return;
        }
        createTransaction(&account->transactions[account->transactionCount], "Deposit", amount);
        account->transactionCount++;
        printf("Deposited %.2f. New balance: %.2f\n", amount, account->balance);
    } else {
        printf("Invalid deposit amount!\n");
    }
}

void withdraw(Account* account, double amount) {
    if (amount > 0 && amount <= account->balance) {
        account->balance -= amount;
        account->transactions = (Transaction*)realloc(account->transactions, (account->transactionCount + 1) * sizeof(Transaction));
        if (account->transactions == NULL) {
            printf("Memory allocation failed!\n");
            return;
        }
        createTransaction(&account->transactions[account->transactionCount], "Withdraw", amount);
        account->transactionCount++;
        printf("Withdrew %.2f. New balance: %.2f\n", amount, account->balance);
    } else {
        printf("Invalid withdrawal amount or insufficient balance!\n");
    }
}

void addInterest(Account* account, double rate) {
    double interest = account->balance * rate / 100;
    account->balance += interest;
    account->transactions = (Transaction*)realloc(account->transactions, (account->transactionCount + 1) * sizeof(Transaction));
    if (account->transactions == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    createTransaction(&account->transactions[account->transactionCount], "Interest", interest);
    account->transactionCount++;
    printf("Interest added: %.2f. New balance: %.2f\n", interest, account->balance);
}

void displayAccount(const Account* account) {
    printf("Account Number: %s\n", account->accountNumber);
    printf("Account Holder: %s\n", account->accountHolderName);
    printf("Balance: %.2f\n", account->balance);
}

void displayTransactionHistory(const Account* account) {
    printf("Transaction History for Account: %s\n", account->accountNumber);
    printf("Type       | Amount     | Timestamp\n");
    for (int i = 0; i < account->transactionCount; i++) {
        displayTransaction(&account->transactions[i]);
    }
}

int checkPassword(const Account* account, const char* pwd) {
    return strcmp(account->password, pwd) == 0;
}

void createBank(Bank* bank) {
    bank->accounts = NULL;
    bank->accountCount = 0;
}

void addAccountToBank(Bank* bank, const char* accNumber, const char* accHolderName, const char* pwd, double initialBalance) {
    bank->accounts = (Account*)realloc(bank->accounts, (bank->accountCount + 1) * sizeof(Account));
    if (bank->accounts == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    createAccount(&bank->accounts[bank->accountCount], accNumber, accHolderName, pwd, initialBalance);
    bank->accountCount++;
    printf("Account created successfully!\n");
}

void deleteAccountFromBank(Bank* bank, const char* accNumber) {
    for (int i = 0; i < bank->accountCount; i++) {
        if (strcmp(bank->accounts[i].accountNumber, accNumber) == 0) {
            free(bank->accounts[i].transactions);
            for (int j = i; j < bank->accountCount - 1; j++) {
                bank->accounts[j] = bank->accounts[j + 1];
            }
            bank->accountCount--;
            bank->accounts = (Account*)realloc(bank->accounts, bank->accountCount * sizeof(Account));
            printf("Account deleted successfully!\n");
            return;
        }
    }
    printf("Account not found!\n");
}

Account* findAccountInBank(Bank* bank, const char* accNumber) {
    for (int i = 0; i < bank->accountCount; i++) {
        if (strcmp(bank->accounts[i].accountNumber, accNumber) == 0) {
            return &bank->accounts[i];
        }
    }
    return NULL;
}

void depositToAccount(Bank* bank, const char* accNumber, double amount) {
    Account* account = findAccountInBank(bank, accNumber);
    if (account) {
        deposit(account, amount);
    } else {
        printf("Account not found!\n");
    }
}

void withdrawFromAccount(Bank* bank, const char* accNumber, double amount) {
    Account* account = findAccountInBank(bank, accNumber);
    if (account) {
        char password[20];
        printf("Enter Password: ");
        scanf("%s", password);
        if (checkPassword(account, password)) {
            withdraw(account, amount);
        } else {
            printf("Incorrect Password! Withdrawal denied.\n");
        }
    } else {
        printf("Account not found!\n");
    }
}

void transferFunds(Bank* bank, const char* fromAccNumber, const char* toAccNumber, double amount) {
    Account* fromAccount = findAccountInBank(bank, fromAccNumber);
    Account* toAccount = findAccountInBank(bank, toAccNumber);
    if (fromAccount && toAccount) {
        if (fromAccount->balance >= amount) {
            withdraw(fromAccount, amount);
            deposit(toAccount, amount);
            printf("Transfer successful!\n");
        } else {
            printf("Insufficient balance for transfer!\n");
        }
    } else {
        printf("One or both accounts not found!\n");
    }
}

void addInterestToAllAccounts(Bank* bank, double rate) {
    for (int i = 0; i < bank->accountCount; i++) {
        addInterest(&bank->accounts[i], rate);
    }
    printf("Interest added to all accounts!\n");
}

void displayAllAccounts(const Bank* bank) {
    printf("All Accounts in the Bank:\n");
    for (int i = 0; i < bank->accountCount; i++) {
        displayAccount(&bank->accounts[i]);
        printf("-------------------------\n");
    }
}

int main() {
    Bank bank;
    createBank(&bank);
    int choice;
    char accNumber[20], accHolderName[50], password[20], fromAccNumber[20], toAccNumber[20];
    double amount, rate;

    do {
        printf("\nBank Management System\n");
        printf("1. Create Account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Transfer Funds\n");
        printf("5. Display Account Details\n");
        printf("6. Display Transaction History\n");
        printf("7. Delete Account\n");
        printf("8. Add Interest to All Accounts\n");
        printf("9. Display All Accounts (Admin)\n");
        printf("10. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                printf("Enter Account Holder Name: ");
                scanf(" %[^\n]", accHolderName);
                printf("Enter Password: ");
                scanf("%s", password);
                printf("Enter Initial Balance: ");
                scanf("%lf", &amount);
                addAccountToBank(&bank, accNumber, accHolderName, password, amount);
                break;

            case 2:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                printf("Enter Amount to Deposit: ");
                scanf("%lf", &amount);
                depositToAccount(&bank, accNumber, amount);
                break;

            case 3:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                printf("Enter Amount to Withdraw: ");
                scanf("%lf", &amount);
                withdrawFromAccount(&bank, accNumber, amount);
                break;

            case 4:
                printf("Enter From Account Number: ");
                scanf("%s", fromAccNumber);
                printf("Enter To Account Number: ");
                scanf("%s", toAccNumber);
                printf("Enter Amount to Transfer: ");
                scanf("%lf", &amount);
                transferFunds(&bank, fromAccNumber, toAccNumber, amount);
                break;

            case 5:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                {
                    Account* account = findAccountInBank(&bank, accNumber);
                    if (account) {
                        printf("Enter Password: ");
                        scanf("%s", password);
                        if (checkPassword(account, password)) {
                            displayAccount(account);
                        } else {
                            printf("Incorrect Password!\n");
                        }
                    } else {
                        printf("Account not found!\n");
                    }
                }
                break;

            case 6:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                {
                    Account* account = findAccountInBank(&bank, accNumber);
                    if (account) {
                        printf("Enter Password: ");
                        scanf("%s", password);
                        if (checkPassword(account, password)) {
                            displayTransactionHistory(account);
                        } else {
                            printf("Incorrect Password!\n");
                        }
                    } else {
                        printf("Account not found!\n");
                    }
                }
                break;

            case 7:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                deleteAccountFromBank(&bank, accNumber);
                break;

            case 8:
                printf("Enter Interest Rate: ");
                scanf("%lf", &rate);
                addInterestToAllAccounts(&bank, rate);
                break;

            case 9:
                displayAllAccounts(&bank);
                break;

            case 10:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 10);

    // Free allocated memory
    for (int i = 0; i < bank.accountCount; i++) {
        free(bank.accounts[i].transactions);
    }
    free(bank.accounts);

    return 0;
}
