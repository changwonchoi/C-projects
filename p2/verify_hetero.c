////////////////////////////////////////////////////////////////////////////////
// Main File:        verify_hetero.c
// This File:        verify_hetero.c
// Semester:         CS 354 Fall 2017
//
// Author:           Chang Won Choi
// Email:            cchoi25@wisc.edu
// CS Login:         cchoi
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
	int size;
	int **array;
} Square;

Square * construct_square(char *filename);
void insertion_sort(int* arr, int size);
int verify_hetero(Square * square);

int main(int argc, char *argv[])
{
	// Check input arguments to get filename
	if (argc != 2) {
		printf("Usage: ./verify_hetero <filename>");
		exit(0);
	}
	// Construct square
	Square *squptr = construct_square(argv[1]);

	// Verify if it's a heterosquare and print true or false
	int verify;
	verify = verify_hetero(squptr);
	
	if (verify) {
		printf("true");
	}
	else {
		printf("false");
	}
	return 0;
}

/* construct_square reads the input file to initialize a square struct
 * from the contents of the file and returns the square.
 * The format of the file is defined in the assignment specifications
 */
Square * construct_square(char *filename)
{
	Square *square;
	FILE *fp;
	char str[50];
	char *p;
	int value;
	int col;
	char check_n[2] = "0\n";
	
	square = malloc(sizeof(Square));

	// Open and read the file
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open file for reading.");
		exit(0);
	} //Checks for error in opening file

	// Read the first line to get the square size
	if (fgets(str, 50, fp) != NULL) {
		(square -> size) = atoi(str);
		if ((square -> size) == 0 && (strcmp(str, check_n) != 0)) {
			printf("String conversion to int failed.");
			exit(0);
		}
	}
	else {
		printf("Invalid file format.");
		exit(0);
	}

	// Initialize a new Square struct of that size
	(square -> array) = malloc((square -> size) * sizeof(int*));
	if ((square -> array) == NULL) {
		printf("Memory allocation failed.");
		exit(0);
	}

	for (int i = 0; i < (square -> size); i++) {
		*((square -> array) + i) = malloc((square -> size) * sizeof(int));
		if (*((square -> array) + i) ==  NULL) {
			printf("Memory allocation failed.");
			exit(0);
		}
	} //2D array heap allocation

	// Read the rest of the file to fill up the square
	char check[1] = "0";
	for (int i = 0; i < (square -> size); i++) {
	//go over a line for each row
		if (fgets(str,50,fp)!= NULL) {
			col = 0;
			while (col < (square -> size)) {
				p = strtok(str, ",");
				while (p != NULL) {
					value = atoi(p);
					if (value == 0 && (strcmp(p,check) != 0)) {
						printf("String conversion to int failed.");
						exit(0);
					} //Check if atoi was successful. Also consider case where the input was 0
					p = strtok(NULL, ",");
					*(*((square -> array) + i) + col) = value;
					col++;
				}
			}
		}
		else {
			printf("Not enough information");
			exit(0);
		}
	}

	if (fclose(fp)) {
		printf("File could not be closed.");
		exit(0);
	} //Close file

	return square;
}

/* insertion_sort sorts the arr in ascending order
 *
 */
void insertion_sort(int* arr, int size)
{
	int pointer = 1;
	int index;
	int temp;

	while (pointer < size) {
		index = pointer;
		while (index > 0 && (arr[index-1] > arr[index])) {
			temp = arr[index];
			arr[index] = arr[index-1];
			arr[index-1] = temp;
			index--;
		} //Sort through the sorted section of the array.
		pointer++;
		//Increase the sorted section
	}
}

/* verify_hetero verifies if the square is a heterosquare
 * 
 * returns 1(true) or 0(false)
 */
int verify_hetero(Square * square) {

	//Create an array that holds all the sum
	int sums[2*(square -> size)+2];
	int sum;
	int slot = 0;
	int col_num;

	while (slot < (2*(square -> size)+2)) {
		//sum for each row
		for (int i = 0; i < (square -> size); i++) {
			sum = 0;
			for (int j = 0; j < (square -> size); j++) {
				sum += *(*((square -> array) + i) + j);
			}
			sums[slot] = sum;
			slot++;
		}
		//sum for each col
		for (int i = 0; i < (square -> size); i++) {
			sum = 0;
			for (int j = 0; j < (square -> size); j++) {
				sum += *(*((square -> array) + j) + i);
			}
			sums[slot] = sum;
			slot++;
		}
		//sum for left to right diagonal
		sum = 0;
		for (int i = 0; i < (square -> size); i++) {
			sum += *(*((square -> array) + i) + i);
		}
		sums[slot] = sum;
		slot++;
		//sum for right to left diagonal
		sum = 0;
		col_num = (square -> size) - 1;
		for (int i = 0; i < (square -> size); i++) {
			sum += *(*((square -> array) + i) + col_num);
			col_num--;
		}
		sums[slot] = sum;
		slot++;	
	}

	// Pass the array to insertion_sort func
	insertion_sort(sums, (square -> size));
		
	// Check the sorted array for duplicates
	for (int i = 0; i < (2*(square -> size)+1); i++) {
		if (sums[i] == sums[i+1]) {
			return 0;
		}
	}

        for (int i = (square -> size) - 1; i >= 0; i--) {
                free(*((square -> array) + i));
        }
        free((square -> array));
        free(square);
        //Free the array

	return 1;
}
