#include "../include/ast.h"
#include "../include/symbol.h"
#include <stdint.h>

NodeType getTypeEnum(char* type) 
{
	if (strcmp(type, "start") == 0) return NODE_START;
	if (strcmp(type, "decl") == 0) return NODE_DECL;
    if (strcmp(type, "int") == 0) return NODE_INT;
	if (strcmp(type, "float") == 0) return NODE_FLOAT;
	if (strcmp(type, "char") == 0) return NODE_CHAR;
	if (strcmp(type, "string") == 0) return NODE_STRING;
    if (strcmp(type, "array") == 0) return NODE_ARRAY_ACCESS;
    if (strcmp(type, "op") == 0) return NODE_OP;
	if (strcmp(type, "var") == 0) return NODE_VAR;
	if (strcmp(type, "break") == 0) return NODE_BREAK;
	if (strcmp(type, "assign") == 0) return NODE_ASSIGN;
	if (strcmp(type, "write") == 0) return NODE_WRITE;
	if (strcmp(type, "for") == 0) return NODE_FOR;
	if (strcmp(type, "while") == 0) return NODE_WHILE;
	if (strcmp(type, "if") == 0) return NODE_IF;
	if (strcmp(type, "ifelse") == 0) return NODE_IF_ELSE;
	if (strcmp(type, "stmtList") == 0) return NODE_STMT_LIST;
	if (strcmp(type, "intDec") == 0) return NODE_INT_DEC;
	if (strcmp(type, "strVar") == 0) return NODE_STR_VAR;
    return -1; 
}

node* createNode(char* s, char* type, node* sib, node* chid, float val1, char* val2)
{
	node* n = (node*) malloc(sizeof(node));
	n->type = getTypeEnum(type);
	switch (n->type)
	{
	case NODE_INT:
		n->data.intValue = (int)val1;
		break;
	case NODE_FLOAT:
		n->data.floatValue = val1;
		break;
	case NODE_CHAR:
		n->data.charValue = val2[0];
		break;
	case NODE_STRING:
		n->data.stringValue = val2;
		break;
	default:
		break;
	}
	
	n->varName = strdup(s);
	n->sibling = sib;
	n->child = chid;
	n->childCount = 0;
	
	return n;
}

void print_level(node* root, int level, int isLast) 
{
    if (root == NULL) return;

    for (int i = 0; i < level - 1; i++) 
	{
        printf("|   ");
    }

    if (level > 0) 
	{
        printf(isLast ? "└──── " : "├──── ");
    }
    if (root->type == NODE_INT)
	{
        printf("[%d]\n", root->data.intValue);
	}
	else if (root->type == NODE_FLOAT)
	{
        printf("[%lf]\n", root->data.floatValue);
	}
	else if (root->type == NODE_CHAR)
	{
        printf("[%c]\n", root->data.charValue);
	}
	else if (root->type == NODE_STRING)
	{
        printf("[%s]\n", root->data.stringValue);
	}
    else
	{
        printf("[%s]\n", root->varName);
	}

    node* sibling = root->child;
    int siblingCount = 0;
    while (sibling) 
	{
        siblingCount++;
        sibling = sibling->sibling;
    }

    sibling = root->child;
    int i = 0;
    while (sibling) 
	{
        print_level(sibling, level + 1, i == siblingCount - 1);
        sibling = sibling->sibling;
        i++;
    }
}

int height(node *tree, int* a, int depth) 
{
	if (tree == NULL)
		return 0;
	else 
	{
		a[depth] += 1;
		int hl 	= height(tree->child, a, depth+1);
		int hr = 0;
		if(tree->sibling)
		{
			hr = height(tree->sibling, a, depth);
			hr -= 1;
		}
		int hmax = (hl > hr) ? hl : hr;
		return hmax + 1;
	}
}

void count_children(node* tree) 
{
    if (tree == NULL) return;

    int count = 0;
    node* temp = tree->child;
    while (temp != NULL) 
	{
        count++;
        temp = temp->sibling;
    }
    tree->childCount = count;

    count_children(tree->child);
    count_children(tree->sibling);
}

eval* createIntEval(int value) 
{
    eval* e = (eval*)malloc(sizeof(eval));
    e->type = NODE_INT;
    e->data.intValue = value;
    return e;
}

eval* createFloatEval(float value) 
{
    eval* e = (eval*)malloc(sizeof(eval));
    e->type = NODE_FLOAT;
    e->data.floatValue = value;
    return e;
}

eval* createStringEval(const char* value) 
{
    eval* e = (eval*)malloc(sizeof(eval));
    e->type = NODE_STRING;
    e->data.stringValue = strdup(value);
    return e;
}

eval* createCharEval(const char value) 
{
    eval* e = (eval*)malloc(sizeof(eval));
    e->type = NODE_CHAR;
    e->data.charValue = value;
    return e;
}

void freeEval(eval* e) {
    if (e->type == NODE_STRING) 
	{
        free(e->data.stringValue);
    }
    free(e);
}

eval* handleArithmeticOp(eval* left_val, eval* right_val, const char* op) 
{
    if (left_val->type == TYPE_INT && right_val->type == TYPE_INT) 
	{
        int result;
        if (strcmp(op, "+") == 0) result = left_val->data.intValue + right_val->data.intValue;
        else if (strcmp(op, "-") == 0) result = left_val->data.intValue - right_val->data.intValue;
        else if (strcmp(op, "*") == 0) result = left_val->data.intValue * right_val->data.intValue;
        else if (strcmp(op, "/") == 0) result = left_val->data.intValue / right_val->data.intValue;
        freeEval(left_val);
        freeEval(right_val);
        return createIntEval(result);
    }

    if ((left_val->type == TYPE_FLOAT || left_val->type == TYPE_INT) &&
        (right_val->type == TYPE_FLOAT || right_val->type == TYPE_INT)) 
	{
        float left = left_val->type == TYPE_INT ? (float)left_val->data.intValue : left_val->data.floatValue;
        float right = right_val->type == TYPE_INT ? (float)right_val->data.intValue : right_val->data.floatValue;
        float result;
        if (strcmp(op, "+") == 0) result = left + right;
        else if (strcmp(op, "-") == 0) result = left - right;
        else if (strcmp(op, "*") == 0) result = left * right;
        else if (strcmp(op, "/") == 0) result = left / right;
        freeEval(left_val);
        freeEval(right_val);
        return createFloatEval(result);
    }

    if (left_val->type == TYPE_STRING && right_val->type == TYPE_STRING && strcmp(op, "+") == 0) 
	{
        char* result = (char*)malloc(strlen(left_val->data.stringValue) + strlen(right_val->data.stringValue) + 1);
        strcpy(result, left_val->data.stringValue);
        strcat(result, right_val->data.stringValue);
        freeEval(left_val);
        freeEval(right_val);
        return createStringEval(result);
    }

    return NULL;
}

eval* handleComparisonOp(eval* left_val, eval* right_val, const char* op) 
{
    int result = 0;

    if (left_val->type == TYPE_INT && right_val->type == TYPE_INT) 
	{
        if (strcmp(op, "==") == 0) result = (left_val->data.intValue == right_val->data.intValue);
        else if (strcmp(op, "!=") == 0) result = (left_val->data.intValue != right_val->data.intValue);
        else if (strcmp(op, "<") == 0) result = (left_val->data.intValue < right_val->data.intValue);
        else if (strcmp(op, "<=") == 0) result = (left_val->data.intValue <= right_val->data.intValue);
        else if (strcmp(op, ">") == 0) result = (left_val->data.intValue > right_val->data.intValue);
        else if (strcmp(op, ">=") == 0) result = (left_val->data.intValue >= right_val->data.intValue);
    }

    if ((left_val->type == TYPE_FLOAT || left_val->type == TYPE_INT) &&
        (right_val->type == TYPE_FLOAT || right_val->type == TYPE_INT)) 
	{
        float left = left_val->type == TYPE_INT ? (float)left_val->data.intValue : left_val->data.floatValue;
        float right = right_val->type == TYPE_INT ? (float)right_val->data.intValue : right_val->data.floatValue;

        if (strcmp(op, "==") == 0) result = (left == right);
        else if (strcmp(op, "!=") == 0) result = (left != right);
        else if (strcmp(op, "<") == 0) result = (left < right);
        else if (strcmp(op, "<=") == 0) result = (left <= right);
        else if (strcmp(op, ">") == 0) result = (left > right);
        else if (strcmp(op, ">=") == 0) result = (left >= right);
    }

    freeEval(left_val);
    freeEval(right_val);
    return createIntEval(result);
}


eval* evaluate(node* root) 
{
    if (!root) return NULL;

    if (root->type == NODE_INT) 
	{
        return createIntEval(root->data.intValue);
    }
    if (root->type == NODE_FLOAT) 
	{
        return createFloatEval(root->data.floatValue);
    }
    if (root->type == NODE_VAR) 
	{
        symbol* sym = slook(root->varName);
        if (!sym) return NULL;

        switch (sym->type) 
		{
            case TYPE_INT:
                return createIntEval(sym->data.intValue);
            case TYPE_FLOAT:
                return createFloatEval(sym->data.floatValue);
            case TYPE_STRING:
                return createStringEval(sym->data.stringValue);
            case TYPE_CHAR:
                return createCharEval(sym->data.charValue);
        }
    }
	if (root->type == NODE_STRING)
	{
		return createStringEval(root->data.stringValue);
	}
	if (root->type == NODE_CHAR)
	{
		return createCharEval(root->data.charValue);
	}

    if (root->type == NODE_ARRAY_ACCESS) 
	{
        symbol* sym = slook(root->varName);
        if (!sym) return NULL;

		node* temp = root->child;
		int* indices = malloc(sizeof(int) * 10);
		int i = 0;
		while(temp)
		{
			if (evaluate(temp)->type != NODE_INT) break;
        	indices[i++] = evaluate(temp)->data.intValue;
			temp = temp->sibling;
		}

		int index = getIndex(indices, sym->dimensions, sym->numDimensions);
		switch (sym->type) 
		{
            case TYPE_INT:
                return createIntEval(sym->data.intArray[index]);
            case TYPE_FLOAT:
                return createFloatEval(sym->data.floatArray[index]);
            case TYPE_STRING:
                return createStringEval(sym->data.stringArray[index]);
            case TYPE_CHAR:
                return createStringEval(&sym->data.charArray[index]);
        }
    }

    eval* left_val = evaluate(root->child);
    eval* right_val = evaluate(root->child->sibling);

    if (root->type == NODE_OP) 
	{
        if (strcmp(root->varName, "+") == 0 ||
            strcmp(root->varName, "-") == 0 ||
            strcmp(root->varName, "*") == 0 ||
            strcmp(root->varName, "/") == 0) 
		{
            return handleArithmeticOp(left_val, right_val, root->varName);
        }

        if (strcmp(root->varName, "==") == 0 ||
            strcmp(root->varName, "!=") == 0 ||
            strcmp(root->varName, "<") == 0 ||
            strcmp(root->varName, "<=") == 0 ||
            strcmp(root->varName, ">") == 0 ||
            strcmp(root->varName, ">=") == 0) 
		{
            return handleComparisonOp(left_val, right_val, root->varName);
        }
    }

    return NULL;
}


void forStmt(node* s)
{
	if(s->child->type == NODE_ASSIGN)
	{
		assignStmt(s->child);

		if(s->child->sibling->type == NODE_OP)
		{
			while(evaluate(s->child->sibling)->data.intValue == 1)
			{
				node* t = s->child->sibling->sibling;

				int a = tableUpdate(t->sibling);
				if(a == 1) break;
				assignStmt(t);
			}
		}
	}
	else if (s->child->type == NODE_OP)
	{
		if(s->child->sibling->type != NODE_ASSIGN)
		{
			while(evaluate(s->child)->data.intValue == 1)
			{
				node* t = s->child->sibling;

				int a = tableUpdate(t);
				if(a == 1) break;
			}
		}
	}
}

void whileStmt(node* s)
{
	while(evaluate(s->child)->data.intValue == 1)
	{
		int temp = tableUpdate(s->child->sibling);
		if(temp == 1) break;
	}
}

void assignStmt(node* s)
{
	symbol* temp = slook(s->child->varName);

	if(s->child->type == NODE_VAR)
	{
		switch (temp->type)
		{
		case TYPE_INT:
			int value = evaluate(s->child->sibling)->data.intValue;
			sVal(temp->name, NULL, (float)value, NULL);
			break;
		case TYPE_FLOAT:
			float value2 = evaluate(s->child->sibling)->data.floatValue;
			sVal(temp->name, NULL, value2, NULL);
			break;
		case TYPE_CHAR:
			char value3[2] = {evaluate(s->child->sibling)->data.charValue, '\0'};
			sVal(temp->name, NULL, 0,  value3);
			break;
		case TYPE_STRING:
			char* value4 = evaluate(s->child->sibling)->data.stringValue;
			sVal(temp->name, NULL, 0, value4);
			break;
		default:
			break;
		}
	}
	else if(s->child->type == NODE_ARRAY_ACCESS)
	{
		node* temp2 = s->child->child;
		int* indices = malloc(sizeof(int) * 10);
		int i = 0;
		while(temp2 != NULL)
		{
			indices[i++] = evaluate(temp2)->data.intValue;
			printf("hhaaaa %s\n",temp2->varName);
			temp2 = temp2->sibling;
		}
		switch (temp->type)
		{
		case TYPE_INT:
			int value = evaluate(s->child->sibling)->data.intValue;
			sVal(temp->name, indices, (float)value, NULL);
			break;
		case TYPE_FLOAT:
			float value2 = evaluate(s->child->sibling)->data.floatValue;
			sVal(temp->name, indices, value2, NULL);
			break;
		case TYPE_CHAR:
			char value3[2] = {evaluate(s->child->sibling)->data.charValue, '\0'};
			sVal(temp->name, indices, 0,  value3);
			break;
		case TYPE_STRING:
			char* value4 = evaluate(s->child->sibling)->data.stringValue;
			sVal(temp->name, indices, 0, value4);
			break;
		default:
			break;
		}
	}
}

int tableUpdate(node* t)
{
	node*s = t;

	if(s->type == NODE_START) s = s->child;

	while(s != NULL)
	{
		if(s->type == NODE_DECL)
		{
			s = s->sibling;
		}
		else if(s->type == NODE_ASSIGN)
		{
			assignStmt(s);
			s = s->sibling;
		}
		else if(s->type == NODE_IF)
		{
			if(evaluate(s->child)->data.intValue == 1)
			{
				int a = tableUpdate(s->child->sibling);
				if(a == 1) return 1;
			}
			s = s->sibling;
		}
		else if(s->type == NODE_IF_ELSE)
		{
			if(evaluate(s->child)->data.intValue == 1)
			{
				int a = tableUpdate(s->child->sibling);
				if(a == 1) return 1;
			}else
			{
				int a = tableUpdate(s->child->sibling->sibling);
				if(a == 1) return 1;
			}
			s = s->sibling;
		}
		else if(s->type == NODE_FOR)
		{
			forStmt(s);
			s = s->sibling;
		}
		else if(s->type == NODE_WHILE)
		{
			whileStmt(s);
			s = s->sibling;
		}
		else if(s->type == NODE_STMT_LIST)
		{
			s = s->child;
		}
		else if(s->type == NODE_WRITE)
		{
			eval* value = evaluate(s->child);
			switch (value->type)
			{
			case NODE_INT:
				printf("%d\n", value->data.intValue);
				break;
			case NODE_FLOAT:
				printf("%lf\n", value->data.floatValue);
				break;
			case NODE_CHAR:
				printf("%c\n", value->data.charValue);
				break;
			case NODE_STRING:
				printf("%s\n", value->data.stringValue);
				break;
			default:
				break;
			}

			s = s->sibling;
		}
		else if(s->type == NODE_BREAK)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

void print_tree(node* t) 
{
	int a[100] = {0};
	int h = height(t, a, 0);
	int maxNode = a[0];

	for(int i=0; i<h; i++)
	{
		if(a[i] > maxNode)
		{
			maxNode = a[i];
		}
	}
	printf("\n");

    count_children(t);
	printf("\n==================================================================\n");

	printf("\nTree like structure 1:\n");
	print_level2(t, a, maxNode);
	printf("\n==================================================================\n");

	printf("\nTree like structure 2:\n\n");
	print_level(t, 0, 1);
	printf("\n==================================================================\n");

	printf("\nOutput values: \n");
	tableUpdate(t);
	printf("\n");

	return;
}
