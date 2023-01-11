# Constraint Satisifaction Problem (CSP)

Each input is split into two files: the .var files contain the set of variables and their domains, and the .con files contain the set of constraints, specifically comparison between two variables (>, <, =).

The program solves CSPs using backtracking search with an option for forward checking. The algorithm for backtracking search uses three heuristics to determine which variable and value to check to efficiently solve the problem. Forward checking eliminates inconsistent values for unassigned variables, further improving the efficiency of the algorithm. 

Run the problem using: ./csp <filename> <fc/none> (example: ./csp test1 fc)

# Resolution Method

Each input file contains clauses in conjunctive normal form, with each line containing a single clause. The last line contains the clause to test, and all other lines form the knowledge base.

The program uses the resolution method to determine whether the tested clause is true based on the other clauses in the knowledge base.

Run the program using: ./rm <filename>.txt (example: ./rm test1.txt)
