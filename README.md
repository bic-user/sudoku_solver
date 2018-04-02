# sudoku_solver
Short piece of C code to solve sudoku puzzles.

Straightforward implementation of sudoku solver. Was developed just as an exercise.

## Compilation
```
gcc -o sudoku_solver sudoku_solver.c
```

##Usage
There are several examples of sudoku puzzles in the repo, more are avialable here:

* https://www.kaggle.com/bryanpark/sudoku/
* https://1sudoku.com/

Puzzle should be piped to the binary. Format - flattened array without spaces.

```
cat evil_example.txt | ./sudoku_solver
```

##TODO
Some thoughts on how to improve the code and algo, but since it already works, I was lazy to do so.

* Refactor recurrent implementation to use loops instead (recurrency is evil!).
* Refactor to be generic for different puzzle sizes.
* More testing! I did not really check how reliable the code is. It just works for given examples.
