#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define SUCCESS 0
#define ERROR 1
#define MIN_INITIAL_NUMBER_OF_PRODUCTS 1
#define MAX_INITIAL_NUMBER_OF_PRODUCTS 100
#define MIN_PRODUCT_ID 1
#define MAX_PRODUCT_ID 10000
#define MIN_PRODUCT_PRICE 1
#define MAX_PRODUCT_PRICE 100000
#define MIN_PRODUCT_Quantity 1
#define MAX_PRODUCT_Quantity 1000000
#define INITIAL_NUMBER_OF_PRODUCTS_LENGTH 10
#define PRODUCT_NAME_LENGTH 50
#define PRODUCT_ID_LENGTH 10
#define PRODUCT_PRICE_LENGTH 10
#define PRODUCT_QUANTITY_LENGTH 10

typedef struct Product
{
    int productId;
    char productName[PRODUCT_NAME_LENGTH];
    float productPrice;
    int productQuantity;
} Product;

void displayInventoryMenu(Product **products, int *totalNumberOfProducts);
int initializeInventory(Product **products, int *totalNumberOfProducts);
int getProductDetails(Product **products, int *totalNumberOfProducts, int *productId, char *productName, float *productPrice, int *productQuantity);
int isValidNumericValue(char *numericValuePointer);
int isValidTotalNumberOfProducts(char *totalNumberOfProductssInput, int *totalNumberOfProducts);
int isValidId(char *productIdInput, int *productId);
int isDuplicateId(Product *products, int productId, int numberOfProducts);
int isValidName(char *productName);
int isValidPrice(char *productPriceInput, float *productPrice);
int isValidQuantity(char *productQuantityInput, int *productQuantity);
int checkProductInventory(int *totalNumberOfProducts);
void addNewProduct(Product **products, int *totalNumberOfProducts);
void viewAllProducts(Product **products, int *totalNumberOfProducts);
void updateQuantity(Product **products, int *totalNumberOfProducts);
void searchProductById(Product **products, int *totalNumberOfProducts);
void searchProductByName(Product **products, int *totalNumberOfProducts);
void searchProductByPriceRange(Product **product, int *totalNumberOfProducts);
void deleteProduct(Product **products, int *totalNumberOfProducts);
void convertToLowerCase(char *productName);
void removeNewLineFromString(char *productIdPointer);
void clearInputBuffer();

int main()
{
    Product *products;
    int totalNumberOfProducts;

    if (initializeInventory(&products, &totalNumberOfProducts))
    {
        return ERROR;
    }

    displayInventoryMenu(&products, &totalNumberOfProducts);

    return 0;
}

void displayInventoryMenu(Product **products, int *totalNumberOfProducts)
{
    int isLoopCondition = 1;
    int userChoice;

    while (isLoopCondition)
    {
        printf("\n========= INVENTORY MENU =========\n");
        printf("1. Add New Product \n");
        printf("2. View All Products  \n");
        printf("3. Update Quantity  \n");
        printf("4. Search Product by ID \n");
        printf("5. Search Product by Name \n");
        printf("6. Search Product by Price Range \n");
        printf("7. Delete Product \n");
        printf("8. Exit \n");

        printf("Enter your choice: ");
        if (scanf("%d", &userChoice) != 1)
        {
            printf("Invalid choice. Must be a numeric value in range (1-8).\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        printf("\n");
        switch (userChoice)
        {
        case 1:
            addNewProduct(products, totalNumberOfProducts);
            break;
        case 2:
            viewAllProducts(products, totalNumberOfProducts);
            break;
        case 3:
            updateQuantity(products, totalNumberOfProducts);
            break;
        case 4:
            searchProductById(products, totalNumberOfProducts);
            break;
        case 5:
            searchProductByName(products, totalNumberOfProducts);
            break;
        case 6:
            searchProductByPriceRange(products, totalNumberOfProducts);
            break;
        case 7:
            deleteProduct(products, totalNumberOfProducts);
            break;
        case 8:
            free(*products);
            *products = NULL;
            printf("Memory released successfully. Exiting program... \n");
            isLoopCondition = 0;
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}

int getProductDetails(Product **products, int *totalNumberOfProducts, int *productId, char *productName, float *productPrice, int *productQuantity)
{
    char productIdInput[PRODUCT_ID_LENGTH];
    char productNameInput[PRODUCT_NAME_LENGTH];
    char productPriceInput[PRODUCT_PRICE_LENGTH];
    char productQuantityInput[PRODUCT_QUANTITY_LENGTH];

    printf("Product ID: ");
    fgets(productIdInput, sizeof(productIdInput), stdin);
    removeNewLineFromString(productIdInput);
    if (isValidId(productIdInput, productId))
    {
        return ERROR;
    }
    if (isDuplicateId(*products, *productId, *totalNumberOfProducts))
    {
        return ERROR;
    }

    printf("Product Name: ");
    fgets(productNameInput, sizeof(productNameInput), stdin);
    removeNewLineFromString(productNameInput);
    if (isValidName(productNameInput))
    {
        return ERROR;
    }

    strcpy(productName, productNameInput);

    printf("Product Price: ");
    fgets(productPriceInput, sizeof(productPriceInput), stdin);
    removeNewLineFromString(productPriceInput);
    if (isValidPrice(productPriceInput, productPrice))
    {
        return ERROR;
    }

    printf("Product quantity: ");
    fgets(productQuantityInput, sizeof(productQuantityInput), stdin);
    removeNewLineFromString(productQuantityInput);
    if (isValidQuantity(productQuantityInput, productQuantity))
    {
        return ERROR;
    }

    return SUCCESS;
}

int initializeInventory(Product **products, int *totalNumberOfProducts)
{
    char totalNumberOfProductsInput[INITIAL_NUMBER_OF_PRODUCTS_LENGTH];

    printf("Enter initial number of products: ");
    fgets(totalNumberOfProductsInput, sizeof(totalNumberOfProductsInput), stdin);
    removeNewLineFromString(totalNumberOfProductsInput);
    if (isValidTotalNumberOfProducts(totalNumberOfProductsInput, totalNumberOfProducts))
    {
        return ERROR;
    }

    *products = (Product *)calloc(*totalNumberOfProducts, sizeof(Product));
    if (*products == NULL)
    {
        printf("Memory allocation failed.\n");
        return ERROR;
    }

    for (int currentProductIndex = 0; currentProductIndex < *totalNumberOfProducts; currentProductIndex++)
    {
        int productId;
        char productName[PRODUCT_NAME_LENGTH];
        float productPrice;
        int productQuantity;

        printf("\nEnter details for product %d: \n", currentProductIndex + 1);

        if (getProductDetails(products, totalNumberOfProducts, &productId, productName, &productPrice, &productQuantity))
        {
            return ERROR;
        }

        (*products + currentProductIndex)->productId = productId;
        strcpy((*products + currentProductIndex)->productName, productName);
        (*products + currentProductIndex)->productPrice = productPrice;
        (*products + currentProductIndex)->productQuantity = productQuantity;
    }

    return SUCCESS;
}

int isValidNumericValue(char *numericValuePointer)
{
    while (*numericValuePointer != '\0')
    {
        if (!isdigit(*numericValuePointer))
        {
            return ERROR;
        }

        numericValuePointer++;
    }

    return SUCCESS;
}

int isValidTotalNumberOfProducts(char *totalNumberOfProductsInput, int *totalNumberOfProducts)
{
    if (isValidNumericValue(totalNumberOfProductsInput) == ERROR)
    {
        printf("Invalid initial number of products. Must be a numeric value in range (1-100).\n");
        return ERROR;
    }

    *totalNumberOfProducts = atoi(totalNumberOfProductsInput);

    if (*totalNumberOfProducts < MIN_INITIAL_NUMBER_OF_PRODUCTS || *totalNumberOfProducts > MAX_INITIAL_NUMBER_OF_PRODUCTS)
    {
        printf("Invalid input. Initial Number of products must be an integer in range (1-100).\n");
        return ERROR;
    }

    return SUCCESS;
}

int isValidId(char *productIdInput, int *productId)
{
    if (isValidNumericValue(productIdInput) == ERROR)
    {
        printf("Invalid product ID. Must be a numeric value in range (1-10000).\n");
        return ERROR;
    }

    *productId = atoi(productIdInput);

    if (*productId < MIN_PRODUCT_ID || *productId > MAX_PRODUCT_ID)
    {
        printf("Invalid Product ID. Must be in range(1-10000).\n");
        return ERROR;
    }

    return SUCCESS;
}

int isDuplicateId(Product *products, int productId, int numberOfProducts)
{
    for (int currentProductIndex = 0; currentProductIndex < numberOfProducts; currentProductIndex++)
    {
        Product currentProduct = *(products + currentProductIndex);

        if (currentProduct.productId == productId)
        {
            printf("Product ID %d already exist.\n", productId);
            return ERROR;
        }
    }

    return SUCCESS;
}

int isValidName(char *productName)
{
    char *productNamePointer = productName;

    while (*productNamePointer == ' ')
    {
        productNamePointer++;
    }

    if (*productNamePointer == '\0')
    {
        printf("Invalid product name. Product name cannot be empty.\n");
        return ERROR;
    }

    while (*productName != '\0')
    {
        char currentCharacter = *productName;

        if (!((currentCharacter >= 'A' && currentCharacter <= 'Z') ||
              (currentCharacter >= 'a' && currentCharacter <= 'z') ||
              (currentCharacter >= '0' && currentCharacter <= '9') ||
              (currentCharacter == ' ')))
        {
            printf("Invalid product name. Product Name must contain only letters and spaces.\n");
            return ERROR;
        }

        productName++;
    }

    return SUCCESS;
}

int isValidPrice(char *productPriceInput, float *productPrice)
{
    char *productPricePointer = productPriceInput;
    int decimalCount = 0;

    while (*productPricePointer != '\0')
    {
        if (!isdigit(*productPricePointer))
        {
            if (*productPricePointer != '.')
            {
                printf("Invalid product price. Must be a numeric value in range (1-100000).\n");
                return ERROR;
            }
            else
            {
                decimalCount++;

                if (decimalCount > 1)
                {
                    printf("Invalid product price. Must be a numeric value in range (1-100000).\n");
                    return ERROR;
                }
            }
        }

        productPricePointer++;
    }

    *productPrice = (float)(atof(productPriceInput));

    if (*productPrice < MIN_PRODUCT_PRICE || *productPrice > MAX_PRODUCT_PRICE)
    {
        printf("Invalid product price. Must be in range (1-100000).\n");
        return ERROR;
    }

    return SUCCESS;
}

int isValidQuantity(char *productQuantityInput, int *productQuantity)
{
    if (isValidNumericValue(productQuantityInput) == ERROR)
    {
        printf("Invalid product quantity. Must be a numeric value in range (1-1000000).\n");
        return ERROR;
    }

    *productQuantity = atoi(productQuantityInput);

    if (*productQuantity < MIN_PRODUCT_Quantity || *productQuantity > MAX_PRODUCT_Quantity)
    {
        printf("Invalid Product quantity. Must be in range(1-1000000).\n");
        return ERROR;
    }

    return SUCCESS;
}

int checkProductInventory(int *totalNumberOfProducts)
{
    if (*totalNumberOfProducts == 0)
    {
        printf("No products in inventory.\n");
        return ERROR;
    }

    return SUCCESS;
}

void addNewProduct(Product **products, int *totalNumberOfProducts)
{
    Product *newHeapMemoryAddress;
    int newProductIndex = *totalNumberOfProducts;
    int productId;
    char productName[PRODUCT_NAME_LENGTH];
    float productPrice;
    int productQuantity;

    printf("Enter new product details: \n");

    if (getProductDetails(products, totalNumberOfProducts, &productId, productName, &productPrice, &productQuantity))
    {
        return;
    }

    newHeapMemoryAddress = (Product *)realloc(*products, (*totalNumberOfProducts + 1) * sizeof(Product));
    if (newHeapMemoryAddress == NULL)
    {
        printf("Reallocation failed.\n");
        return;
    }

    *products = newHeapMemoryAddress;

    (*products + newProductIndex)->productId = productId;
    strcpy((*products + newProductIndex)->productName, productName);
    (*products + newProductIndex)->productPrice = productPrice;
    (*products + newProductIndex)->productQuantity = productQuantity;
    (*totalNumberOfProducts)++;

    printf("Product added successfully!\n");
    return;
}

void viewAllProducts(Product **products, int *totalNumberOfProducts)
{
    if (checkProductInventory(totalNumberOfProducts))
    {
        return;
    }

    printf("========= PRODUCT LIST =========\n");

    for (int currentProductIndex = 0; currentProductIndex < *totalNumberOfProducts; currentProductIndex++)
    {
        Product currentProduct = *(*products + currentProductIndex);

        printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
               currentProduct.productId, currentProduct.productName, currentProduct.productPrice, currentProduct.productQuantity);
    }
}

void updateQuantity(Product **products, int *totalNumberOfProducts)
{
    char productIdInput[PRODUCT_ID_LENGTH];
    int productId;
    char newproductQuantityInput[PRODUCT_QUANTITY_LENGTH];
    int newProductQuantity;
    int isProductFound = 0;

    if (checkProductInventory(totalNumberOfProducts))
    {
        return;
    }

    printf("Enter Product ID to update quantity: ");
    fgets(productIdInput, sizeof(productIdInput), stdin);
    removeNewLineFromString(productIdInput);
    if (isValidId(productIdInput, &productId))
    {
        return;
    }

    printf("Enter new Quantity: ");
    fgets(newproductQuantityInput, sizeof(newproductQuantityInput), stdin);
    removeNewLineFromString(newproductQuantityInput);
    if (isValidQuantity(newproductQuantityInput, &newProductQuantity))
    {
        return;
    }

    for (int currentProductIndex = 0; currentProductIndex < *totalNumberOfProducts; currentProductIndex++)
    {
        if ((*(*products + currentProductIndex)).productId == productId)
        {
            isProductFound = 1;
            (*(*products + currentProductIndex)).productQuantity = newProductQuantity;
            printf("Quantity updated successfully! \n");
            break;
        }
    }

    if (!isProductFound)
    {
        printf("Error: Product Id %d not found! \n", productId);
    }
}

void searchProductById(Product **products, int *totalNumberOfProducts)
{
    char productIdToBeSearchedInput[PRODUCT_ID_LENGTH];
    int productIdToBeSearched;
    int isProductFound = 0;

    if (checkProductInventory(totalNumberOfProducts))
    {
        return;
    }

    printf("Enter product Id to search: ");
    fgets(productIdToBeSearchedInput, sizeof(productIdToBeSearchedInput), stdin);
    removeNewLineFromString(productIdToBeSearchedInput);
    if (isValidId(productIdToBeSearchedInput, &productIdToBeSearched))
    {
        return;
    }

    for (int currentProductIndex = 0; currentProductIndex < *totalNumberOfProducts; currentProductIndex++)
    {
        Product currentProduct = *(*products + currentProductIndex);

        if (currentProduct.productId == productIdToBeSearched)
        {
            printf("Product Found: ");
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   currentProduct.productId, currentProduct.productName, currentProduct.productPrice, currentProduct.productQuantity);
            isProductFound = 1;
            break;
        }
    }

    if (!isProductFound)
    {
        printf("Error: No product found with Id %d! \n", productIdToBeSearched);
    }
}

void convertToLowerCase(char *productName)
{
    while (*productName != '\0')
    {
        *productName = tolower(*productName);
        productName++;
    }
}

void searchProductByName(Product **products, int *totalNumberOfProducts)
{
    char productNameToBeSearched[PRODUCT_NAME_LENGTH];
    char *searchProductNamePointer = productNameToBeSearched;
    int isProductFound = 0;

    if (checkProductInventory(totalNumberOfProducts))
    {
        return;
    }

    printf("Enter name to search (partial allowed): ");
    fgets(productNameToBeSearched, sizeof(productNameToBeSearched), stdin);
    removeNewLineFromString(productNameToBeSearched);
    if (isValidName(productNameToBeSearched) == ERROR)
    {
        return;
    }

    convertToLowerCase(searchProductNamePointer);

    printf("Products found: ");
    for (int currentProductIndex = 0; currentProductIndex < *totalNumberOfProducts; currentProductIndex++)
    {
        Product currentProduct = *(*products + currentProductIndex);
        char currentProductNameCopy[PRODUCT_NAME_LENGTH];
        strcpy(currentProductNameCopy, currentProduct.productName);
        char *currentProductNamePointer = currentProductNameCopy;
        convertToLowerCase(currentProductNameCopy);
        int isMatchingProductFound = 0;

        while (*currentProductNamePointer != '\0')
        {
            char *temporaryCurrentProductNamePointer = currentProductNamePointer;
            char *temporarySearchProductNamePointer = searchProductNamePointer;

            while (*temporaryCurrentProductNamePointer != '\0' && *temporarySearchProductNamePointer != '\0' &&
                   *temporaryCurrentProductNamePointer == *temporarySearchProductNamePointer)
            {
                temporaryCurrentProductNamePointer++;
                temporarySearchProductNamePointer++;
            }

            if (*temporarySearchProductNamePointer == '\0')
            {
                isMatchingProductFound = 1;
                break;
            }
            else
            {
                currentProductNamePointer++;
            }
        }

        if (isMatchingProductFound)
        {
            isProductFound = 1;
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   currentProduct.productId, currentProduct.productName, currentProduct.productPrice, currentProduct.productQuantity);
        }
    }

    if (!isProductFound)
    {
        printf("No product found! \n");
    }
}

void searchProductByPriceRange(Product **products, int *totalNumberOfProducts)
{
    char minimumProductPriceInput[PRODUCT_PRICE_LENGTH];
    char maximumProductPriceInput[PRODUCT_PRICE_LENGTH];
    float maximumProductPrice;
    float minimumProductPrice;
    int isProductFound = 0;

    if (checkProductInventory(totalNumberOfProducts))
    {
        return;
    }

    printf("Enter minimum price: ");
    fgets(minimumProductPriceInput, sizeof(minimumProductPriceInput), stdin);
    removeNewLineFromString(minimumProductPriceInput);
    if (isValidPrice(minimumProductPriceInput, &minimumProductPrice))
    {
        return;
    }

    printf("Enter maximum price: ");
    fgets(maximumProductPriceInput, sizeof(maximumProductPriceInput), stdin);
    removeNewLineFromString(maximumProductPriceInput);
    if (isValidPrice(maximumProductPriceInput, &maximumProductPrice))
    {
        return;
    }

    if (minimumProductPrice > maximumProductPrice)
    {
        printf("Minimum product price should be less than maximum product price.\n");
        return;
    }

    printf("Products in price range: \n");
    for (int currentProductIndex = 0; currentProductIndex < *totalNumberOfProducts; currentProductIndex++)
    {
        Product currentProduct = *(*products + currentProductIndex);

        if (currentProduct.productPrice >= minimumProductPrice && currentProduct.productPrice <= maximumProductPrice)
        {
            isProductFound = 1;
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d \n",
                   currentProduct.productId, currentProduct.productName, currentProduct.productPrice, currentProduct.productQuantity);
        }
    }

    if (!isProductFound)
    {
        printf("Error: No product found in price range (%.2f-%.2f)! \n", minimumProductPrice, maximumProductPrice);
    }
}

void deleteProduct(Product **products, int *totalNumberOfProducts)
{
    int deleteProductId;
    char deleteProductIdInput[PRODUCT_ID_LENGTH];
    int isProductFound = 0;

    if (checkProductInventory(totalNumberOfProducts))
    {
        return;
    }

    printf("Enter product Id to delete: ");
    fgets(deleteProductIdInput, sizeof(deleteProductIdInput), stdin);
    removeNewLineFromString(deleteProductIdInput);
    if (isValidId(deleteProductIdInput, &deleteProductId))
    {
        return;
    }

    for (int currentProductIndex = 0; currentProductIndex < *totalNumberOfProducts; currentProductIndex++)
    {
        Product currentProduct = *(*products + currentProductIndex);

        if (currentProduct.productId == deleteProductId)
        {
            isProductFound = 1;

            for (int productShiftIndex = currentProductIndex; productShiftIndex < *totalNumberOfProducts - 1; productShiftIndex++)
            {
                *(*products + productShiftIndex) = *(*products + productShiftIndex + 1);
            }
        }

        if (isProductFound)
        {
            break;
        }
    }

    if (isProductFound)
    {
        printf("Product deleted successfully!\n");
        (*totalNumberOfProducts)--;
        Product *newHeapMemoryPointer;
        newHeapMemoryPointer = (Product *)realloc(*products, *totalNumberOfProducts * sizeof(Product));

        if (*totalNumberOfProducts == 0)
        {
            free(*products);
            *products = NULL;
            return;
        }

        if (newHeapMemoryPointer != NULL)
        {
            *products = newHeapMemoryPointer;
        }
        else
        {
            printf("Reallocation failed, but product is deleted.\n");
        }
    }
    else
    {
        printf("Error: No product found with product Id %d. \n", deleteProductId);
    }
}

void removeNewLineFromString(char *stringPointer)
{
    while (*stringPointer != '\0' && *stringPointer != '\n')
    {
        stringPointer++;
    }

    if (*stringPointer == '\n')
    {
        *stringPointer = '\0';
    }
}

void clearInputBuffer()
{
    int character;
    while ((character = getchar()) != '\n' && character != EOF)
        ;
}
