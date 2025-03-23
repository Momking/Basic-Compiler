#include "../include/symbol.h"

symbol* s = NULL;

int i = 0;

symbol* slook(char* a) {
    symbol* n = s;
    while (n != NULL) {
        if (strcmp(n->name, a) == 0) {
            return n;
        }
        n = n->next;
    }

    return NULL;
}

symbol* sUpdate(char* a, DataType t, int isArray, int* dimensions) 
{
    symbol* n = (symbol*)malloc(sizeof(symbol));
    
    n->name = strdup(a);
    n->type = t;
    n->isArray = isArray;
    n->dimensions = NULL;
    n->numDimensions = 0;
    n->next = s;
    s = n;

    if (n->isArray) 
    {
        n->dimensions = malloc(sizeof(int) * 10); 
        n->numDimensions = 0;
        int size = 1;
        for (int i = 0; dimensions[i] != -1; i++) 
        {
            n->dimensions[i] = dimensions[i];
            size *= dimensions[i];
            n->numDimensions++;
        }
        switch (n->type) 
        {
            case TYPE_INT:
                n->data.intArray = (int*) calloc(size, sizeof(int));
                break;
            case TYPE_FLOAT:
                n->data.floatArray = (float*) calloc(size, sizeof(float));
                break;
            case TYPE_CHAR:
                n->data.charArray = (char*) calloc(size, sizeof(char));
                break;
            case TYPE_STRING:
                n->data.stringArray = (char**) calloc(size, sizeof(char*));
                break;
        }
    }
    return n;
}

void sVal(char* a, int* indices, float value, char* value2)
{
    symbol* n = s;
    while(n != NULL)
    {
        if(strcmp(n->name, a) == 0)
        {
            if(!n->isArray)
            {
                switch (n->type) 
                {
                    case TYPE_INT:
                        n->data.intValue = (int)value;
                        break;
                    case TYPE_FLOAT:
                        n->data.floatValue = value;
                        break;
                    case TYPE_CHAR:
                        if (value2) 
                        {
                            n->data.charValue = value2[0];
                        }
                        break;
                    case TYPE_STRING:
                        if (value2) 
                        {
                            free(n->data.stringValue);
                            n->data.stringValue = strdup(value2);
                        }
                        break;
                    default:
                        break;
                }
            }
            else
            {
                int index = getIndex(indices, n->dimensions, n->numDimensions);
                switch (n->type)
                {
                    case TYPE_INT:
                        n->data.intArray[index] = (int)value;
                        break;
                    case TYPE_FLOAT:
                        n->data.floatArray[index] = value;
                        break;
                    case TYPE_CHAR:
                        if (value2) 
                        {
                            n->data.charArray[index] = value2[0];
                        }
                        break;
                    case TYPE_STRING:
                        if (value2) 
                        {
                            free(n->data.stringValue);
                            n->data.stringArray[index] = strdup(value2);
                        }
                        break;
                    default:
                        break;
                }
            }
            return;
        }
        n = n->next;
    }
}

int getIndex(int* indices, int* dimensions, int numDimensions) 
{
    int index = 0;
    int multiplier = 1;
    for (int i = numDimensions - 1; i >= 0; i--) 
    {
        index += indices[i] * multiplier;
        multiplier *= dimensions[i];
    }
    return index;
}

void freeSymbolTable() 
{
    symbol* n = s;
    while (n != NULL) 
    {
        symbol* temp = n;
        n = n->next;
        free(temp->name);

        if (temp->isArray) 
        {
            free(temp->dimensions);

            switch (temp->type) 
            {
                case TYPE_INT:
                    free(temp->data.intArray);
                    break;
                case TYPE_FLOAT:
                    free(temp->data.floatArray);
                    break;
                case TYPE_CHAR:
                    free(temp->data.charArray);
                    break;
                case TYPE_STRING:
                    for (int i = 0; i < temp->numDimensions; i++) 
                    {
                        free(temp->data.stringArray[i]);
                    }
                    free(temp->data.stringArray);
                    break;
            }
        } 
        else 
        {
            if (temp->type == TYPE_STRING) 
            {
                free(temp->data.stringValue);
            }
        }
        free(temp);
    }
}


void printSym()
{
    symbol* n = s;
    printf("==================================================================\n");
    printf("Name\tType\tIsArray\tDimensions\tValues\n");
    while (n != NULL) 
    {
        printf("%s\t", n->name);
        switch (n->type) 
        {
            case TYPE_INT:
                printf("int\t");
                break;
            case TYPE_FLOAT:
                printf("float\t");
                break;
            case TYPE_CHAR:
                printf("char\t");
                break;
            case TYPE_STRING:
                printf("string\t");
                break;
            default:
                printf("undefined\t");
        }

        if (n->isArray) 
        {
            printf("Yes\t[");
            for (int i = 0; i < n->numDimensions; i++) 
            {
                printf("%d", n->dimensions[i]);
                if (i < n->numDimensions - 1) printf(", ");
            }
            printf("]\t");

            int index = getIndex(n->dimensions, n->dimensions, n->numDimensions);
            
            if (n->numDimensions == 1) printf("\t");

            for (int i = 0; i < index; i++) 
            {
                switch (n->type) 
                {
                    case TYPE_INT:
                        printf("%d ", n->data.intArray[i]);
                        break;
                    case TYPE_FLOAT:
                        printf("%f ", n->data.floatArray[i]);
                        break;
                    case TYPE_CHAR:
                        printf("%c ", n->data.charArray[i]);
                        break;
                    case TYPE_STRING:
                        printf("%s ", n->data.stringArray[i]);
                        break;
                }
                if (n->numDimensions > 1 && (i+1) % ((index+1)/n->dimensions[0]) == 0) printf("\n\t\t\t\t\t");
            }
        }
        else 
        {
            printf("No\t-");
            switch (n->type) 
            {
                case TYPE_INT:
                    printf("\t\t%d", n->data.intValue);
                    break;
                case TYPE_FLOAT:
                    printf("\t\t%f", n->data.floatValue);
                    break;
                case TYPE_CHAR:
                    printf("\t\t%c", n->data.charValue);
                    break;
                case TYPE_STRING:
                    printf("\t\t%s", n->data.stringValue);
                    break;
            }
        }
        printf("\n");
        n = n->next;
    }
    printf("==================================================================\n");
}
