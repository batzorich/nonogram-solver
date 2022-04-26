# Nonogram with black & white cells solver
Nonogram - picture logic puzzle
Given is number of rows & columns. For each row and column list of numbers, which indicates how many black cells in given line.
For example, length of line is 10 and list of numbers is [5,3], then this line could be XXXXX0XXX0, XXXXX00XXX,0XXXXX0XXX.
It is important that this nonogram could be solved by logical approach without any guess and it has one and only one solution.
In fact, I used openmp to measure how much this code got faster.
