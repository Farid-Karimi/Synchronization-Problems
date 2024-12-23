# Operating-Systems-Project

## Dining Philosophers Problem
### Edsger Dijkstra's Solution
the firts person to recognize the problem and give a solution for it was Edsger Dijkstra, his solution uses one mutex, one semaphore per philosopher and one state variable per philosopher.
The resource hierarchy solution places a partial ordering on the resources (chopsticks) and prevents access to the resources out of order. The “chopsticks”, in our philosophers illustration, are numbered from 1 to 5 and each philosopher will always pick up the lower-numbered chopstick before the higher-numbered chopstick from the chopsticks that they intend to use. The order in which the philosophers put down the chopsticks is irrelevant. The result is that, if four of the five philosophers simultaneously pick up their lower-numbered chopsticks, so that only the highest numbered chopstick remains on the table, the fifth philosopher will not pick up any chopstick. Moreover, only one philosopher will have access to that highest-numbered chopstick, so she will be able to eat using two chopsticks.

This solution avoids deadlocks, but is often impractical. For example, if a process possesses resources 3 and 5 and then finds that it needs resource 2, it has to release 5 and then 3 in that order, then obtain 2 and reacquire 3 and 5 always in that same order. Computer programs that access large numbers of database records would run very inefficiently if required to release higher-numbered resources before lower-numbered ones.

The resource hierarchy solution is also unfair. If philosopher 1 is slow to pick up a chopstick and philosopher two thinks fast and picks up his chopsticks immediately, philosopher 1 may face starvation. A genuinely fair solution would assure that each philosophy eventually eats regardless of his thinking speed or dexterity with chopsticks.

### Arbitrator
An alternative solution uses an arbitrator. This way it can be guaranteed that an individual philosopher can only pick up both or none of the chopsticks. We can imagine a waiter performing this service in the case of dining philosophers. The waiter gives permission to only one philosopher at a time until she picks up both chopsticks. Putting down a chopstick is always allowed. The waiter can be implemented in code as a mutex. This solution introduces a central entity (arbitrator) and can also result in reduces parallelism. If one philosopher is eating and another request a fork, all other philosophers must wait until this request is fulfilled, even if other chopsticks are still available to them.

### Chandy-Misra solution
In 1984, K. Chandy and J. Misra proposed a solution to a generalized Dining Philosophers problem, which doesn't require the philosophers to be in a circle or to share only two resources with only nearest neighbors.

Chandy-Misra's algorithm may be described in terms of "clean" and "dirty" chopsticks. Each chopstick is shared with a pair of philosophers.

Each chopstick is always in the possession of one of it's two philosophers.

Also, a dirty chopstick is always cleaned just before it is given to its other philosopher.

Initialization. Every process receives a unique integer ID number. For every pair of philosophers who contend for a chopstick, one chopstick is created, assigned to the philosopher with the lower ID number (lower neighbor), and marked as "dirty."

Thinking. When a philosopher p is thinking, if that philosopher p receives a request for a particular chopstick c from one of that philosopher's neighbors, then p gives the neighbor that chopstick c (after cleaning it).

Hungry. When a philosopher p is preparing to eat, p requests any chopsticks that p doesn't already have from the appropriate neighbor.

During this time, if a neighbor asks p for a chopstick that p possesses, then p sends that chopstick (after cleaning it) if it's dirty, and keeps that chopstick for the present if it's clean. p defers the requests for already-clean chopsticks (i.e., remembers the clean requests for later delivery).

Eating. A philosopher p may start eating as soon as p has all of p's chopsticks. While eating, all requests for chopsticks are deferred, and all chopsticks become dirty.

Cleanup. Immediately after eating, a philosopher delivers any chopsticks for which there are deferred requests (after cleaning them). That philosopher then proceeds to eat.

Deadlock avoidance of the Chandy-Misra algorithm can be proven using directed graphs: each philosopher represents a vertex, and each edge represents a chopstick, with an arrow going from "dirty towards clean". The ID ordering of philosophers can be used to show that this graph never has a closed cycles (i.e., deadlock circular waits), by reassigning a (lower) ID to philosophers just after they finish eating, thus insuring that the graph's arrows always point from lower towards higher IDs.

No starvation: Since a hungry philosopher p always keeps p's clean chopsticks, and since each of p's neighbors must deliver their shared chopstick to p, cleaned, either immediately (if the neighbor is thinking) or as soon as that neighbor finishes eating, then we conclude that a hungry philosopher p cannot be passed up more than once by any neighbor. By transitivity, each of p's hungry or eating neighbors must eventually finish eating, which guarantees that p won't starve. (But p may have to fast for a long time.)

Fairness (after eating, all resources are designated for the neighbors); high degree of concurrency; scalable (because after initialization; the resource management is local -- no central authority needed); generalizes to any number of processes and resources (as long as each resource is shared by exactly two processes).



Downside: potentially long wait chains when hungry.
