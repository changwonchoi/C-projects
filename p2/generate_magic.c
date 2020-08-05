////////////////////////////////////////////////////////////////////////////////
// Main File:        generate_magic.c
// This File:        generate_magic.c
// Semester:         CS 354 Fall 2017
//
// Author:           Chang Won Choi
// Email:            cchoi25@wisc.edu
// CS Login:         cchoi
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
	int size;
	int **array;
} Square;

int get_square_size();
Square * generate_magic(int size);
void write_to_file(Square * square, char *filename);

int main(int argc, char *argv[])
{
	// Check input arguments to get filename
	if (argc != 2) {
		printf("Usage: ./generate_magic <filename>");
		exit(0);
	}
	// Get size from user
	int n;
	n = get_square_size();
	
	// Generate the magic square
	Square *square = generate_magic(n);
	// Write the square to the output file
	write_to_file(square, argv[1]);
	
	return 0;
}

/* get_square_size prompts the user for the magic square size
 * checks if it is an odd number >= 3 and returns the number
 */
int get_square_size()
{
	int n;
	printf("Enter the size of magic square, must be odd\n");
	if (scanf("%d", &n) != 1) {
		printf("Invalid input format.");
		exit(0);
	} //Get the int size of magic square

	if (n == 1 || (n % 2) == 0) {
		printf("Size must be an odd number >= 3.");
		exit(0);
	} //Check if the number is odd that is bigger than 4
	return n;
}

/* generate_magic constructs a magic square of size n
 * using the Siamese algorithm and returns the Square struct
 */
Square * generate_magic(int n)
{
	Square *square;
	
	square = malloc(sizeof(Square));
	
	(square -> size) = n;
	(square -> array) = malloc(n * sizeof(int));
	
	if ((square -> array) == NULL) {
		printf("Memory allocation failed.");
		exit(0);
	}
	
	for (int i = 0; i < n; i++) {
		*((square -> array) + i) = calloc(n, sizeof(int));
		if (*((square -> array) + i) == NULL) {
			printf("Memory allocation failed.");
			exit(0);
		}
	} //Make 2D array initiallized with zeroes
	
	int row = 0;
	int col = n / 2;
	
	*(*((square -> array) + row) + col) = 1;

	for (int i = 2; i <= n*n; i++) {	
		row--;
		if (row < 0) {
			row = n - 1;
		}
		col++;
		if (col > n - 1) {
			col = 0;
		}
		if (*(*((square -> array) + row) + col) != 0) {
			row += 2;
			if (row > n - 1) {
				row = 1;
			}
			col--;
			if (col < 0) {
				col = n - 1;
			}
		}
		*(*((square -> array) + row) + col) = i;
	} //Follows the algorithm given on the webpage
	
	return square;
}

/* write_to_file opens up a new file(or overwrites the existing file)
 * and writes out the square in the format expected by verify_hetero.c
 */
void write_to_file(Square * square, char *filename)
{
	FILE *fp;
		
	fp = fopen(filename, "w");
	//Open file

	if (fp == NULL) {
		printf("Cannot open file for writing");
		exit(0);
	}
	
	if (fprintf(fp, "%d\n", (square -> size)) < 0) {
		printf("Writing to file failed");
		exit(0);
	}
	
	for (int i = 0; i < (square -> size); i++) {
		for (int j = 0; j < (square -> size); j++) {
			if (j != ((square -> size) - 1)) {
				if (fprintf(fp, "%d,", *(*((square -> array) + i) + j)) < 0) {
					printf("Writing to file failed");
					exit(0);
				}
			}
			else if (i != ((square -> size) - 1)) {
				if (fprintf(fp, "%d\n", *(*((square -> array) + i) + j)) < 0) {
					printf("Writing to file failed");
					exit(0);
				}
			}
			else {
				if (fprintf(fp,"%d", *(*((square -> array) + i) + j)) < 0) {
					printf("Writing to file failed");
					exit(0);
				}
			}
		}
	} //Write to the file in the correct format
	
	if (fclose(fp)) {
		printf("Error closing the file");
		exit(0);
	} //Close file
	
        for (int i = (square -> size) - 1; i >= 0; i--) {
                free(*((square -> array) + i));
        }
        free((square -> array));
        free(square);
        //Free the array

}
