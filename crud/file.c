#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_PRODUCTS 100
#define ID_SIZE 9
#define NAME_SIZE 31
#define PRICE_SIZE 9
#define QUANTITY_SIZE 5

typedef struct {
    char id[ID_SIZE];
    char name[NAME_SIZE];
    char price[PRICE_SIZE];
    char quantity[QUANTITY_SIZE];
} Product;

typedef struct {
    Product products[MAX_PRODUCTS];
    int count;
} ProductList;

ProductList productList = { .count = 0 };

void serialize() {
    int fd = open("file.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    if (write(fd, &productList, sizeof(ProductList)) == -1) {
        perror("Error writing to file");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void deserialize() {
    int fd = open("file.bin", O_RDONLY);
    if (fd != -1) {
        if (read(fd, &productList, sizeof(ProductList)) == -1) {
            perror("Error reading from file");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);
    }
    toTxt();
}

void spaces(char *str, int size) {
    int len = strlen(str);
    if (str[len - 1] == '\n') {
        str[len -1] = ' ';
    }
    if (len < size) {
        for (int i = len; i < size; i++) {
            str[i] = ' ';
        }
        str[size] = '\0';
    } else if (len > size) {
        str[size] = '\0';
    }
}

void show() {
    FILE *file = fopen("file.txt", "r");
    char line[128];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        printf("%s", line);
    }
    
}

void add(const char *name, const char *price, const char *quantity) {
    if (productList.count >= MAX_PRODUCTS) {
        printf("Product list is full\n");
        return;
    }
    Product *product = &productList.products[productList.count];
    snprintf(product->id, ID_SIZE, "%d", productList.count + 1);
    strncpy(product->name, name, NAME_SIZE - 1);
    strncpy(product->price, price, PRICE_SIZE - 1);
    strncpy(product->quantity, quantity, QUANTITY_SIZE - 1);
    spaces(product->id, ID_SIZE - 1);
    spaces(product->name, NAME_SIZE - 1);
    spaces(product->price, PRICE_SIZE - 1);
    spaces(product->quantity, QUANTITY_SIZE - 1);
    productList.count++;
    serialize();
    toTxt();
}

void update(const char *id, const char *name, const char *price, const char *quantity) {
    char temp_id[ID_SIZE];
    strncpy(temp_id, id, ID_SIZE - 1);
    spaces(temp_id, ID_SIZE - 1);
    for (int i = 0; i < productList.count; i++) {
        if (strncmp(productList.products[i].id, temp_id, ID_SIZE - 1) == 0) {
            strncpy(productList.products[i].name, name, NAME_SIZE - 1);
            strncpy(productList.products[i].price, price, PRICE_SIZE - 1);
            strncpy(productList.products[i].quantity, quantity, QUANTITY_SIZE - 1);
            spaces(productList.products[i].name, NAME_SIZE - 1);
            spaces(productList.products[i].price, PRICE_SIZE - 1);
            spaces(productList.products[i].quantity, QUANTITY_SIZE - 1);
            serialize();
            toTxt();
            return;
        }
    }
}

void delete(const char *id) {
    int index = -1;
    spaces(id, ID_SIZE - 1);
    for (int i = 0; i < productList.count; i++) {
        if (strncmp(productList.products[i].id, id, ID_SIZE - 1) == 0) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return;
    }
    for (int i = index; i < productList.count - 1; i++) {
        productList.products[i] = productList.products[i + 1];
    }
    productList.count--;
    for (int i = index; i < productList.count; i++) {
        snprintf(productList.products[i].id, ID_SIZE, "%d", i + 1);
        spaces(productList.products[i].id, ID_SIZE - 1);
    }
    serialize();
    toTxt();
}

void toTxt() {
    int fd = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < productList.count; i++) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%s%s%s%s\n",
                productList.products[i].id,
                productList.products[i].name,
                productList.products[i].price,
                productList.products[i].quantity);
        if (write(fd, buffer, strlen(buffer)) == -1) {
            perror("Error writing to file");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    close(fd);
}

void massage() {
    printf("  Hi, this is the \"Prices\" project.\n");
    printf("This little program stores in file.txt data about goods, their prices and quantity.\n");
    printf("Data can be added, deleted, and modified. To add it, use the '-c' command, then write down the name, price and quantity.\n");
    printf("To modify, use the '-u' command, then write down the id of the field where you need to change the data and new data.\n");
    printf("To delete, use the '-d' command, then enter the id of the field to delete.\n");
    printf("And if you want to see the content of the database, enter '-s'\n");
    printf("Example:\n");
    printf("\n");
    printf("        -s\n");
    printf("        -c name price quantity\n");
    printf("        -u id name price quantity\n");
    printf("        -d id\n");
    printf("\n");
    printf("\n");
    printf("Good luck!!!\n");
    printf("\n");
}

void getCommand() {
    char input[256];
    while (1) {
        printf("Your command ->  ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        if (strncmp(input, "exit", 4) == 0) {
            break;
        }
        if (strncmp(input, "-s", 2) == 0) {
            show();
        }
        char *command = strtok(input, " ");
        if (command == NULL) {
            continue;
        }
        if (strcmp(command, "-c") == 0) {
            char *name = strtok(NULL, " ");
            char *price = strtok(NULL, " ");
            char *quantity = strtok(NULL, " ");
            if (name && price && quantity) {
                add(name, price, quantity);
            }
        } else if (strcmp(command, "-u") == 0) {
            char *id = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            char *price = strtok(NULL, " ");
            char *quantity = strtok(NULL, " ");
            if (id && name && price && quantity) {
                update(id, name, price, quantity);
            }
        } else if (strcmp(command, "-d") == 0) {
            char *id = strtok(NULL, " ");
            if (id) {
                delete(id);
            }
        }
    }
}

int main() {
    massage();
    deserialize();
    getCommand();
    return 0;
}
