/*
 * Solves the N-Queens for N = 8.
 */

int N;
int **board;


/*
 * Returns nonzero if it is okay to place a queen at the specified row
 * and column.  Otherwise (the queen could attack some other queen),
 * zero is returned.
 */

int okay(int row, int col)
{
    int i, j;


    j = 0;

    while (j < col) {


	/* Check for an attack along the same row. */

	if (board[row][j] != 0)
	    return 0;


	/* Check for an attack along the upper diagonal. */

	i = (row - col) + j;
	if (i >= 0 && board[i][j] != 0)
	    return 0;


	/* Check for an attack along the lower diagonal. */

	i = (row + col) - j;
	if (i < N && board[i][j] != 0)
	    return 0;

	j = j + 1;
    }

    return 1;
}


/*
 * Try to place a queen in the specified column.  If the queen can be
 * successfully placed, then nonzero is returned.  Otherwise, if the
 * queen cannot be placed in the column because another it could
 * attack another queen, then zero is returned.
 */

int place(int col)
{
    int row;


    /* Try each row in this column. */

    row = 0;
    while (row < N) {
	if (okay(row, col)) {


	    /* Place the queen and move to the next column. */

	    board[row][col] = 1;

	    if (col + 1 == N || place(col + 1) == 1)
		return 1;
	    else
		board[row][col] = 0;
	}

	row = row + 1;
    }

    return 0;
}


int main(void)
{
    int row, col;


    N = 8;
    row = 0;
    board = (int **) malloc(sizeof(int *) * N);

    while (row < N) {
	col = 0;
	board[row] = (int *) malloc(sizeof(int) * N);

	while (col < N) {
	    board[row][col] = 0;
	    col = col + 1;
	}

	row = row + 1;
    }

    place(0);
    row = 0;

    while (row < N) {
	col = 0;

	while (col < N) {
	    printf ("%d", board[row][col]);
	    col = col + 1;
	}

	printf("\n");
	row = row + 1;
    }
}
