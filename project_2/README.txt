We first started out by reading through the two problems and figuring out which one we wanted to do. We decided on teh second problem because it seemed like the more definied of the two, and it looked a little more interesting to do.

The next step was to make sure that we understood the thread creation logic. So we added just the bare minimum needed to spawn all the plane threads and print out the plane number.

After that, we added skeleton methods to establish how the planes would loop through and be controlled. Once completing that, we moved on to the actual threading logic.

Initially, we used ints to indicate which segments of runway were claimed by which plane, with -1 for unclaimed. Eventually, we decided to replace this with a system of semaphores rather than ints.

Once the semaphores were in place, we began implementing the action loop for the small planes. The first method we implemented was await_runway, which decided on which path the plane would take and then waited for all segments to be available. We did this by having the plane sem_wait() on the first segment of its path, and when that segment became ready, it would check if its other segment is also available. If it is, the thread would claim both semaphores and the method would return the path. if not, it would release its hold on the initial semaphore and return to the back of the queue.

Once this was in place, the remaining methods for each plane were relatively much easier, and once we finished implementing our methods for the small planes we simply made slightly modified verisons for the large planes.
