#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME (64)

// i called Directory the |_Directory*| instead of |_Directory| because it will be used more frequently
typedef struct _Directory* Directory;
typedef struct _Directory {
	char name[MAX_NAME];
	Directory child; // this will point to first in LIST of children
	Directory sibling; // this will point to another sibling - root doesn't have siblings :'(
} DirectoryCreator;

// for the simplicity, i separated stack and directroy
typedef struct _Stack* Stack;
typedef struct _Stack {
	Directory directory;
	Stack next;
} StackCreator;

Directory createDirectory();
int menu(Stack);
int md(Directory);
int addChild(Directory, Directory);

int findDirectory(Stack, char*);

int cdDotDot(Stack);

Stack createStack();
int pushDirectoryToStack(Stack, Directory);
int popStack(Stack);

int main() {
	// initialize Directory and Stack
	// they are the same thing
	// * directory is data structer for storage of files and folders
	// * stack is our data structer for keeping track of past directories
	// for the simplicity i will create stack separately

	// directory and stack are our one and only "fake" structures and are used only to "point" at real structures

	Directory rootDirectory = NULL;
	Stack stack = NULL;

	rootDirectory = createDirectory();
	stack = createStack();
	
	// i did not separate directory and stack nullcheck because this error rarely happens
	if (rootDirectory == NULL || stack == NULL) {
		printf("\n\tERROR - could not create directory or stack!");
	}

	// rootDirectory should be on top of stack
	pushDirectoryToStack(stack, rootDirectory);

	menu(stack);

	return 0;
}

// this function does not handle null values - this means that calling createDirectory can return || Directory or NULL ||
Directory createDirectory() {
	Directory newDirectory = malloc(sizeof(DirectoryCreator));
	if (newDirectory) {
		newDirectory->child = NULL;
		newDirectory->sibling = NULL;
		return newDirectory;
	}
	return NULL;
}

int menu(Stack stack) {
	char input[MAX_NAME] = "";
	int index = -1;
	do {
		printf("\n--- OPTIONS ---\n");
		printf("\n - md"); // create directory - create directory -> add to list of children
		printf("\n - cd"); // enter directory - add directory to stack
		printf("\n - cd.."); // go back - remove directory from stack
		printf("\n - dir"); // list all siblings/children
		printf("\n - rm"); // delete with all children
		printf("\n - exit");
		printf("\n\n Your choice: ");
		scanf("%s", input);

		if (input == NULL) {
			printf("\n\twrong input");
			continue;
		}

		if (!strcmp(input, "md")) {
			md(stack);
			continue;
		}

		if (!strcmp(input, "cd")) {
			cd(stack);
			continue;
		}

		if (!strcmp(input, "cd..")) {
			cdDotDot(stack);
			continue;
		}

		if (!strcmp(input, "dir")) {
			dir(stack);
			continue;
		}

		if (!strcmp(input, "exit")) {
			index = 0;
			continue;
		}

		printf("\n\t wrong choice!\n");

	} while (index != 0);

	return 0;
}

// 1. function for create directory and add to child list

int md(Stack stack) {
	printf("\n\n\t\tEntered md\n\n");
	char newName[MAX_NAME];
	Directory newDirectory = NULL;

	printf("\n Name of new directory: ");
	scanf("%s", newName);
	printf("\n");

	// if there is no name dont create new directory - you dont need this
	if (newName == NULL || strlen(newName) == 0) {
		return 0;
	}

	newDirectory = createDirectory();

	if (!newDirectory) {
		printf("\n\tERROR - cannot create directory");
		return 0; // it is important to return something in order to stop the function from progressing
	}

	strcpy(newDirectory->name, newName);

	// up to here, directory is created, now we have to add it to child of current parrent

	// check stack
	if (stack->next == NULL || stack->next->directory == NULL) {
		printf("\n\tERROR - No directory in stack");
		return 0;
	}
	// above we check if stack next directory is null, and to check that we need to check if stack next is null
	addChild(stack->next->directory, newDirectory);

	return 0;
}

int addChild(Directory current, Directory newDirectory) {
	if (current->child == NULL) {
		current->child = newDirectory;
		return 0;
	}
	// we can go through list and add it to the end but i like to place it on the first place for simplicity of code
	// put first child as sibling (because newDirectory is now the first child, and old first child is now sibling of newDirectory)
	newDirectory->sibling = current->child;
	current->child = newDirectory;

	return 0;
}

// 2. Enter directory

int cd(Stack stack) {
	char name[MAX_NAME];

	printf("\nEnter name of directory to go to: ");
	scanf("%s", name);
	printf("\n\n");

	if (stack->next == NULL || stack->next->directory == NULL) {
		printf("\n\tERROR - empty stack\n");
		return 0;
	}

	findDirectory(stack, name);
	
	return 0;
}

int findDirectory(Stack stack, char* name) {
	Directory parentDirectory = stack->next->directory;
	if (parentDirectory->child == NULL) {
		return 0;
	}

	Directory tempDirectory = parentDirectory->child;
	while (tempDirectory != NULL) {
		if (strcmp(tempDirectory->name, name) == 0) {
			pushDirectoryToStack(stack, tempDirectory);
			return 0;
		}
		tempDirectory = tempDirectory->sibling;
	}

	printf("\n\tCould not find directory!\n");
	return 0;
}

// 3. Go back

int cdDotDot(Stack stack) {
	// 1. case - no stack
	if (stack->next == NULL) {
		printf("\n\tERROR - stack does not exist\n");
		return 0;
	}

	// 2. case - already on top
	if (stack->next->next == NULL) {
		printf("\n\n Cannot go back!\n");
		return 0;
	}

	// 3. case - we can go back
	popStack(stack);
	return 0;
}

// 4. List directory

int dir(Stack stack) {
	if (stack->next == NULL || stack->next->directory == NULL) {
		printf("\n\tERROR - empty stack\n");
		return 0;
	}

	Directory parentDirectory = stack->next->directory;
	Directory tempDirectory = parentDirectory->child;

	int counter = 0;

	while (tempDirectory != NULL) {
		counter++;
		printf("\n %s", tempDirectory->name);
		tempDirectory = tempDirectory->sibling;
	}

	printf("\n\nTotal %d directories!\n", counter);
	return 0;
}

// STACK FUNCTIONS

Stack createStack() {
	Stack newStack = malloc(sizeof(StackCreator));
	if (newStack) {
		newStack->next = NULL;
		newStack->directory = NULL;
		return newStack;
	}
	return NULL;
}

int pushDirectoryToStack(Stack stack, Directory directory) {
	Stack newStackItem = createStack();
	if (!newStackItem) {
		return 0;
	}
	newStackItem->directory = directory;
	if (stack->next == NULL) {
		stack->next = newStackItem;
		return 0;
	}
	newStackItem->next = stack->next;
	stack->next = newStackItem;
	return 0;
}

int popStack(Stack stack) {
	if (stack->next == NULL) {
		return 0;
	}
	// Stack tempStack = stack->next;
	stack->next = stack->next->next;
	// should free tempStack;
	return 0;
}