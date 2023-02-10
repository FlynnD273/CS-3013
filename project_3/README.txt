
In this directory, you should write the program `scheduler.c` and compile it into the binary `scheduler` (e.g., `gcc -o scheduler scheduler.c -Wall -Werror`). You should do it inside the Ubuntu machine you have setup, as grading will be done so inside the Ubuntu.

After doing so, you can run the tests from this directory by running the
`./run_tests.sh` script. If all goes well, you will see standard output that indicate that all tests are passed.


Passing all tests are not sufficient to obtain all points for this assignment. As an example, any assignment in which hardcodes the expected output to pass the test case will lead to point deductions.


We started out by first making sure we could read the command line arguments properly. After that, we added skeleton methods for running the three algorithms. We ran into a bit of trouble with the test cases not completing, but that was quickly resolved. It turns out the SJF output expected the use of a stable sorting algorithm. So after switching to BubbleSort, all the tests looking at the output of the scheduler task order passed. 

There's not really that much to talk about for the scheduling algorithms. I will note that at this point, the round-robin algorithm does mutate the list. If this becomes an issue later on, we can always just copy the list before running the algorithm. The rest of the algorithms are pretty simple. FIFO just traverses the list, SJF just sorts then traverses the list. 

Turns out removing the elements from the list in the round-robin algorithm causes issues down the line. We switched to storing a progress property in the job struct. This works much better now. 

For the analysis, we're keeping track of the start time, the total wait time, and the last run time. This lets us calculate all the statistics that we need for part 2 easily by iterating over the job list. 
