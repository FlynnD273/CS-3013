Bernhardt Adler and Flynn Duniho
We're using a Github repository to share code, since this wau we'll have access to version control, and be able to easily merge changes if we work separately. 

Generation:
This one was pretty easy to do. One mistake that I made was that I was decrementing the counter at the beginning of the while loop instead of the end, resulting in a duplicate childcount value and an off-by-one error.
For this project, I compiled and tested each stage, using printf to make sure that state of the proram was what I expected. Once it was complete, I compared the output against the example output. 

Slug:
This took me forever to debug. Eventually I realized the issue was that I hadn't made the slug_seed files, so I was getting an error trying to read the seed. Once I figured that out, everything went pretty smoothly. 
I started by testing the command-line arguments, and choosing the correct file path for a given command-line argument. After that I moved on to the rest of the program, using printf statements to make sure the state of the program was correct.

Slug Race:
This was fairly simple to make. The only hiccup was printing all the running processes, but I quickly realized that I separate them out into different printf statements inside a for loop, and now it works well. The example output didn't show that much precision for the running time of the slugs, so I think maybe adding the nanoseconds wasn't necessary. 
