
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void print_board(int board[9][9]) {
	int i, j, val;
	for (i = 0; i < 9; i++) {
		fprintf(stderr, "| ");
		for (j = 0; j < 9; j++) {
			val = board[i][j];
			if (val == 0) {
				fprintf(stderr, "_ ");
			} else {
				fprintf(stderr, "%d ", val);
			}
			if ((j + 1) % 3 == 0) {
				fprintf(stderr, "| ");
			}
		}
		fprintf(stderr, "\n");
		if ((i + 1) % 3 == 0 && i < 8) {
			for (j = 0; j < 25; j++) {
				fprintf(stderr, "_");
			}
			fprintf(stderr, "\n");
		}
	}
}

typedef struct possible_values_s {
	int num;
	char is_possible[9];
	int i;
	int j;
} possible_values;

void possible_values_init(possible_values pvals[81]) {
	int i, j, k;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			possible_values *pval = &pvals[i * 9 + j];
			pval->num = 9;
			for (k = 0; k < 9; k++) {
				pval->is_possible[k] = 1;
			}
			pval->i = i;
			pval->j = j;
		}
	}
}

void possible_values_print(possible_values pvals[81]) {
	int i, j, k;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			fprintf(stderr, "%d %d; | possible values num %d | ", i+1, j+1, pvals[i * 9 + j].num);
			for (k = 0; k < 9; k++) {
				fprintf(stderr, "%d ", pvals[i * 9 + j].is_possible[k]);
			}
			fprintf(stderr, "\n");
		}
	}
}

void possible_values_remove(possible_values pvals[81], int i, int j, int value) {
	int k, ii, jj, iii, jjj;
	possible_values *pval = &pvals[i * 9 + j];
	value -= 1;
	pval->num = 1;
	memset(pval->is_possible, 0, sizeof(char) * 9);
	pval->is_possible[value] = 1;
	// 2.make given value "not possible" in the row and column
	for (k = 0; k < 9; k++) {
		pval = &pvals[i * 9 + k];
		if (k != j && pval->is_possible[value] == 1) {
			pval->num -= 1;
			pval->is_possible[value] = 0;
		}
		pval = &pvals[k * 9 + j];
		if (k != i && pval->is_possible[value] == 1) {
			pval->num -= 1;
			pval->is_possible[value] = 0;
		}
	}
	// 3. make given value "not possilbe" in the quadrant
	// find start of the quadrant
	ii = i;
	jj = j;
	while (ii != 0 && ii != 3 && ii != 6) {
		ii -= 1;
	}
	while (jj != 0 && jj != 3 && jj != 6) {
		jj -= 1;
	}
	// make value not possible
	for (iii = ii; iii < ii + 3; iii++) {
		for (jjj = jj; jjj < jj + 3; jjj++) {
			pval = &pvals[iii * 9 + jjj];
			if ((iii != i || jjj != j) && (pval->is_possible[value] == 1)) {
				pval->num -= 1;
				pval->is_possible[value] = 0;
			}
		}
	}
}

int possible_values_comp(const void* elem1, const void* elem2) {
	possible_values pv1 = *((possible_values *)elem1);
	possible_values pv2 = *((possible_values *)elem2);
	if (pv1.num > pv2.num) return 1;
	if (pv2.num > pv1.num) return -1;
	return 0;
}

int is_board_solved(int board[9][9]) {
	int i, j;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (board[i][j] == 0) {
				return 0;
			}
		}
	}
	return 1;
}

int solve(int inboard[9][9], int input_i, int input_j, int additional_val) {
	int board[9][9];
	int i, j, k;
	int value;
	int board_changed, board_solved;

	possible_values pvals[81];

	possible_values_init(pvals);
	memcpy(board, inboard, 9 * 9 * sizeof(int));
	if (additional_val > 0) {
		board[input_i][input_j] = additional_val;
	}

	//given board - fill all possible values
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (board[i][j] != 0) {
				possible_values_remove(pvals, i, j, board[i][j]);
			}
		}
	}

	// iterate over possible values and see if it's possible to add
	// anything in board
	board_changed = 1;
	while (board_changed) {
		board_changed = 0;
		for (i = 0; i < 9; i++) {
			for (j = 0; j < 9; j++) {
				if (pvals[i * 9 + j].num == 1 && board[i][j] == 0) {
					board_changed = 1;
					// only one possible value in pval that is not on board
					// find what value is possible
					value = -1;
					for (k = 0; k < 9; k++) {
						if (pvals[i * 9 + j].is_possible[k] == 1) {
							value = k + 1;
							break;
						}
					}
					assert(value != -1);
					board[i][j] = value;
					possible_values_remove(pvals, i, j, value);
				}
			}
		}
	}

	// if there is cell with no value possible - this is dead-end solution
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (pvals[i * 9 + j].num == 0) {
				return 0;
			}
		}
	}
	if (!is_board_solved(board)) {
		// board not solved, all the obvious cells were filled
		// need to guess some of the possible value.
		qsort(pvals, 81, sizeof(possible_values), possible_values_comp);
		for (i = 0; i < 81; i++) {
			possible_values *pval = &pvals[i];
			if (pval->num == 1) {
				continue;
			}
			for (k = 0; k < 9; k++) {
				if (pval->is_possible[k]) {
					if (solve(board, pval->i, pval->j, k + 1)) {
						memcpy(inboard, board, sizeof(int) * 9 * 9);
						return 1;
					}
				}
			}
		}
	} else {
		memcpy(inboard, board, sizeof(int) * 9 * 9);
		return 1;
	}
	return 0;
}

int main(int argc, char **argv) {
	char buf[100];
	char str_val[2];
	int board[9][9];
	int i, j;

	// read the puzzle from stdin
	fgets(buf, sizeof(buf), stdin);
	if (strlen(buf) != (9 * 9 + 1)) {
		fprintf(stderr, "**Error! Invalid line [%s]\n", buf);
		return -1;
	}

	//fill the board
	memset(board, 0, sizeof(board));
	str_val[1] = 0;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			str_val[0] = buf[i * 9 + j];
			board[i][j] = atoi(str_val);
		}
	}
	fprintf(stderr, "To solve:\n");
	print_board(board);

	solve(board, -1, -1, -1);

	fprintf(stderr, "\nSolved:\n");
	print_board(board);
}
