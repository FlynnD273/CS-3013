Bernhardt Adler and Flynn Duniho

Generation:
This one was pretty easy to do. One mistake that I made was that I was decrementing the counter at the beginning of the while loop instead of the end, resulting in a duplicate childcount value and an off-by-one error.
For this project, I compiled and tested each stage, using printf to make sure that state of the proram was what I expected. Once it was complete, I compared the output against the example output. 

Slug:
This took me forever to debug. Eventually I realized the issue was that I hadn't made the slug_seed files, so I was getting an error trying to read the seed. Once I figured that out, everything went pretty smoothly. 
I started by testing the command-line arguments, and choosing the correct file path for a given command-line argument. After that I moved on to the rest of the program, using printf statements to make sure the state of the program was correct.
