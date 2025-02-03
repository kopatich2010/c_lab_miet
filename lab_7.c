#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define NUMBER_OF_CLIENTS 2
#define INITIALS_LEN 50
#define BANK_PERIOD 5 //srok godnosti v godah
//name validation???
struct fio
{
    char* name;
    char* surname;
    char* fathername;
} typedef fio;

struct date
{
    int day;
    int month;
    int year;
} typedef date;

struct bank
{
    date create_date;
    fio person;
    double sum;
} typedef bank;

struct database
{
    bank** clients;
    int len;
    int capacity;
} typedef database;

database* data_base_init(int);
database* load_database_from_file();
bank* find_client_by_fio(const char*, const char*, const char*, database*);
bank* create_client(database*, date);
bank* find_client_by_fio(const char*, const char*, const char*, database*);
date get_current_date();
int account_expired(int, int);
int account_warning(int, int);
int cmp_clients(bank*, const char*, const char*, const char*);
int check_bank_accounts(bank**, date, database*, int (*)(int, int));
int find_clients_by_balance(bank**, database*, int);
int save_database(database*);
void add_client(database*, date);
void show_data_base(database* data);
void find_show_client(database*);
void free_database(database*);
void show_find_clients_by_balance(database*);
void sort_base_by_surname(database*);
void take_money_from_account(database*);
void show_check_bank_accounts(database*, date, int (*)(int, int));
void free_client(bank*);
void menu();



int main(){
    menu();
    return 0;
}

database* load_database_from_file(){
    FILE* f;
    int len;
    int capacity;

    if((f = fopen("data.dat", "rb")) == NULL){
        printf("File wasn`t open...");
        return NULL; 
    }

    fread(&capacity, sizeof(int), 1, f);
    fread(&len, sizeof(int), 1, f);
    database* data = data_base_init(capacity);
    data->len = len;

    for (int i = 0; i < data->len; i++)
    {
        data->clients[i] = (bank*) calloc(1, sizeof(bank));
        data->clients[i]->person.name = (char*) calloc(INITIALS_LEN, sizeof(char));
        data->clients[i]->person.surname = (char*) calloc(INITIALS_LEN, sizeof(char));
        data->clients[i]->person.fathername = (char*) calloc(INITIALS_LEN, sizeof(char));

        fread(&(data->clients[i]->create_date), sizeof(date), 1, f);
        fread(&(data->clients[i]->sum), sizeof(double), 1, f);
        fread(data->clients[i]->person.name, sizeof(char), INITIALS_LEN, f);
        fread(data->clients[i]->person.surname, sizeof(char), INITIALS_LEN, f);
        fread(data->clients[i]->person.fathername, sizeof(char), INITIALS_LEN, f);
    } 
    return data;
}

int save_database(database* data){
    FILE* f;
    if((f = fopen("data.dat", "wb")) == NULL){
        printf("File wasn`t open...");
        return -1;
    }
    fwrite(&(data->capacity), sizeof(int), 1, f);
    fwrite(&(data->len), sizeof(int), 1, f);
    for (int i = 0; i < data->len; i++)
    {
        fwrite(&(data->clients[i]->create_date), sizeof(date), 1, f);
        fwrite(&(data->clients[i]->sum), sizeof(double), 1, f);
        fwrite(data->clients[i]->person.name, sizeof(char), INITIALS_LEN, f);
        fwrite(data->clients[i]->person.surname, sizeof(char), INITIALS_LEN, f);
        fwrite(data->clients[i]->person.fathername, sizeof(char), INITIALS_LEN, f);
    }  
    fclose(f);    
}

void take_money_from_account(database* data){
    char name[INITIALS_LEN];
    char surname[INITIALS_LEN];
    char fathername[INITIALS_LEN];
    double requested_amount;

    printf("Enter your name: ");
    scanf("%s", (char*)&name);

    printf("Enter your surname: ");
    scanf("%s", (char*)&surname);

    printf("Enter your fathername: ");
    scanf("%s", (char*)&fathername);

    printf("Enter requested amount: ");
    scanf("%lf", &requested_amount);

    bank* client = find_client_by_fio(name, surname, fathername, data);
    if (client){
        if (client->sum >= requested_amount)
        {
            client->sum -= requested_amount;
            printf("Money was successfully withdrawn from the account!");
        }
        else
            printf("You don't have enough money in your account...");
    }
    else
        printf("Client not found...");
}

bank* create_client(database* data, date current_date){
    char buff[INITIALS_LEN];
    double balance;
    bank* new_client = (bank*) calloc(1, sizeof(bank));
    new_client->person.name = (char*) calloc(INITIALS_LEN, sizeof(char));
    new_client->person.surname = (char*) calloc(INITIALS_LEN, sizeof(char));
    new_client->person.fathername = (char*) calloc(INITIALS_LEN, sizeof(char));
    new_client->create_date = current_date;

    printf("Enter your name: ");
    scanf("%s", (char*)&buff);
    strcpy(new_client->person.name, buff);

    printf("Enter your surname: ");
    scanf("%s", (char*)&buff);
    strcpy(new_client->person.surname, buff);

    printf("Enter your fathername: ");
    scanf("%s", (char*)&buff);
    strcpy(new_client->person.fathername, buff);

    printf("Enter your balance: ");
    scanf("%lf", &balance);
    new_client->sum = balance;

    if (find_client_by_fio(new_client->person.name, new_client->person.surname,
                    new_client->person.fathername, data) == NULL)
        return new_client;
    return  NULL;
}

void add_client(database* data, date current_date){
    bank* new_clinet;
    if (!(new_clinet = create_client(data, current_date))){
        printf("Apparently that user alredy exist...");
        return;
    }
    if ((++(data->len)) == (data->capacity) - 1)
    {
        data->capacity *= 2;
        data->clients = (bank**) realloc(data->clients, data->capacity * sizeof(bank*));
    }
    data->clients[data->len - 1] = new_clinet;
    printf("User created successfully!");
}

database* data_base_init(int capacity){
    bank** clients = (bank**) calloc(capacity, sizeof(bank*));
    database* data = (database*) malloc(sizeof(database));
    data->clients = clients;
    data->capacity = capacity;
    data->len = 0;
    return data;
}

void show_data_base(database* data){
    if (data->len <= 0)
    {
        printf("It's empty here\n");
        return;
    }
    printf("\n---------------------------------------data---------------------------------------\n");
    printf("%-10s\t%-15s\t%-10s\t%-15s\t%-15s", "NAME", "SURNAME", "FATHERNAME", 
                                                "BALANCE","CREATION DATE");
    printf("\n");
    for (int i = 0; i < data->len; i++){
        printf("%-10s\t%-15s\t%-15s\t%-8.2lf\t%2d.%2d.%4d\n", 
            data->clients[i]->person.name, 
            data->clients[i]->person.surname,
            data->clients[i]->person.fathername, 
            data->clients[i]->sum,
            data->clients[i]->create_date.day,
            data->clients[i]->create_date.month,
            data->clients[i]->create_date.year
            );
    }
    printf("----------------------------------------------------------------------------------\n");
}   

int cmp_clients(bank* c, const char* n, const char* s, const char* f){
    if ((strlen(n) != strlen(c->person.name)) || (strlen(s) != strlen(c->person.surname)))
        return 0;
    for (int i = 0; n[i] != '\0'; i++)
        if (tolower(n[i]) != tolower(c->person.name[i]))
            return 0;
    for (int i = 0; s[i] != '\0'; i++)
        if (tolower(s[i]) != tolower(c->person.surname[i]))
            return 0;
    for (int i = 0; f[i] != '\0'; i++)
        if (tolower(f[i]) != tolower(c->person.fathername[i]))
            return 0;
    return 1;
}

bank* find_client_by_fio(const char* name, const char* surname, const char* fathername, database* data){
    for (int i = 0; i < data->len; i++)
        if (cmp_clients(data->clients[i], name, surname, fathername))
            return data->clients[i];
    return NULL;
}   

int account_expired(int date_creation_in_days, int current_date_in_days){
    printf("Expired accounts!!!\n");
    if(current_date_in_days > (date_creation_in_days + BANK_PERIOD * 365))
        return 1;
    return 0;
}

int account_warning(int date_creation_in_days, int current_date_in_days){
    printf("Accounts with warnings!!!\n");
    if(current_date_in_days > (date_creation_in_days + BANK_PERIOD * 365 - 10))
        return 1;
    return 0;
}

int check_bank_accounts(bank** res, date current_date, database* data, int (*func)(int, int)){
    bank** t_res = res;  
    for (int i = 0; i < data->len; i++)
    {
        date date_creation = data->clients[i]->create_date;
        int current_date_in_days = current_date.day + 30 * current_date.month + 365 * current_date.year;
        int date_creation_in_days = date_creation.day + 30 * date_creation.month + 365 * date_creation.year;
        if (func(date_creation_in_days, current_date_in_days))
        {
            *t_res = data->clients[i];
            t_res++;
        }

    }
    return t_res - res;   
}

void show_check_bank_accounts(database* data, date current_date, int (*func)(int, int)){
    database* expired_accounts = malloc(sizeof(database));
    bank** res = calloc(data->len, sizeof(bank*));

    int size = check_bank_accounts(res, current_date, data, func);
    res = realloc(res, size);

    expired_accounts->clients = res;
    expired_accounts->capacity = data->capacity;
    expired_accounts->len = size;

    show_data_base(expired_accounts);

    free(res);
    free(expired_accounts);
}

void find_show_client(database* data){
    char name[INITIALS_LEN];
    char surname[INITIALS_LEN];
    char fathername[INITIALS_LEN];

    printf("\nEnter his name: ");
    scanf("%s", (char*)&name);

    printf("\nEnter his surname: ");
    scanf("%s", (char*)&surname);

    printf("\nEnter his fathername: ");
    scanf("%s", (char*)&fathername);

    bank* c = find_client_by_fio(name, surname, fathername, data);
    printf("\n");
    if (c)
    {
        printf("%-10s\t%-15s\t%-10s\t%-15s\t%-15s", 
        "NAME", "SURNAME", "FATHERNAME", "BALANCE", "CREATION DATE");
        printf("%-10s\t%-15s\t%-15s\t%-8.2lf\t%d.%d.%d\n", 
        c->person.name, c->person.surname, 
        c->person.fathername, c->sum, c->create_date.day, 
        c->create_date.month, c->create_date.year);
    }
    else
        printf("There's no one like that here...\n");
}

void free_database(database* data){
    for (int i = 0; i < data->len; i++)
        free_client(data->clients[i]);
    free(data->clients);
    free(data); 
}

void free_client(bank* client){
    free(client->person.name);
    free(client->person.surname);
    free(client->person.fathername);
    free(client);
}

int find_clients_by_balance(bank** res, database* data, int balance){
    bank** t_res = res;  
    for (int i = 0; i < data->len; i++)
    {
        date date_creation = data->clients[i]->create_date;
        if (data->clients[i]->sum > balance)
        {
            *t_res = data->clients[i];
            t_res++;
        }
    }
    return t_res - res; 
}

void show_find_clients_by_balance(database* data){
    double bal;
    printf("\nEnter the balance: \n");
    scanf("%lf", &bal);

    database* _accounts = malloc(sizeof(database));
    bank** res = calloc(data->len, sizeof(bank*));

    int size = find_clients_by_balance(res, data, bal);
    res = realloc(res, size);

    _accounts->clients = res;
    _accounts->capacity = data->capacity;
    _accounts->len = size;

    show_data_base(_accounts);

    free(res);
    free(_accounts);
}

void sort_base_by_surname(database* data){
    for (int i = 0; i < data->len; i++)
    {
        for (int j = i; j < data->len; j++)
        {
            if (strcmp(data->clients[i]->person.surname, data->clients[j]->person.surname) > 0)
            {
                bank* tmp = data->clients[i];
                data->clients[i] = data->clients[j];
                data->clients[j] = tmp;
            }
        }
    }
}

date get_current_date(){
    date date;
    printf("Enter current date in format '(int)YEAR.(int)MONTH.(int)DAY': ");
    scanf("%d.%d.%d", &date.year, &date.month, &date.day);
    return date;
}

void menu(){
    int choice = -1;
    int load_is_complete;

    database* data;
    if ((data = load_database_from_file()) == NULL){
        data = data_base_init(NUMBER_OF_CLIENTS);
        printf("Database wasn`t loaded from the file...\n");
    }
    else
        printf("Database was loaded from the file successfully!\n");

    date current_date = get_current_date();

    while (choice != 8)
    {
        printf(
           "\nCreate user......................1\
            \nPrint data.......................2\
            \nFind user by fio.................3\
            \nFind users by balance............4\
            \nSort base by surname.............5\
            \nTake money from account..........6\
            \nCheck bank accounts..............7\
            \nEnd the program..................8\n"
            );
        printf("Choose the function: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            printf("\nLets put that lad IN!\n\n");
            add_client(data, current_date);
            break;
        case 2:
            printf("\nLets print some DATA!\n\n");
            show_data_base(data);
            break;
        case 3:
            printf("\nLets find that BASTARD!\n\n");
            find_show_client(data);
            break;
        case 4:
            printf("\nLets find some rich NIGGERS!\n\n");
            show_find_clients_by_balance(data);
            break;
        case 5:
            printf("\nLets sort that SHIT!\n\n");
            sort_base_by_surname(data);
            show_data_base(data);
            break;
        case 6:
            take_money_from_account(data);
            break; 
        case 7:
            show_check_bank_accounts(data, current_date, account_expired);
            show_check_bank_accounts(data, current_date, account_warning);
            break;       
        default:
            if (choice != 8)
                printf("\nTry again...\n");
            else
            {
                save_database(data);
                printf("Database has been saved!!!\n");
            }
            free_database(data);
            break;
        }
    }
}