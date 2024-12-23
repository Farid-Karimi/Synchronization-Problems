# Operating-Systems-Project

## Dining Philosophers Problem
### Edsger Dijkstra's Solution
the firts person to recognize the problem and give a solution for it was Edsger Dijkstra, his solution uses one mutex, one semaphore per philosopher and one state variable per philosopher.
The resource hierarchy solution places a partial ordering on the resources (chopsticks) and prevents access to the resources out of order. The “chopsticks”, in our philosophers illustration, are numbered from 1 to 5 and each philosopher will always pick up the lower-numbered chopstick before the higher-numbered chopstick from the chopsticks that they intend to use. The order in which the philosophers put down the chopsticks is irrelevant. The result is that, if four of the five philosophers simultaneously pick up their lower-numbered chopsticks, so that only the highest numbered chopstick remains on the table, the fifth philosopher will not pick up any chopstick. Moreover, only one philosopher will have access to that highest-numbered chopstick, so she will be able to eat using two chopsticks.

This solution avoids deadlocks, but is often impractical. For example, if a process possesses resources 3 and 5 and then finds that it needs resource 2, it has to release 5 and then 3 in that order, then obtain 2 and reacquire 3 and 5 always in that same order. Computer programs that access large numbers of database records would run very inefficiently if required to release higher-numbered resources before lower-numbered ones.

The resource hierarchy solution is also unfair. If philosopher 1 is slow to pick up a chopstick and philosopher two thinks fast and picks up his chopsticks immediately, philosopher 1 may face starvation. A genuinely fair solution would assure that each philosophy eventually eats regardless of his thinking speed or dexterity with chopsticks.

### Arbitrator
An alternative solution uses an arbitrator. This way it can be guaranteed that an individual philosopher can only pick up both or none of the chopsticks. We can imagine a waiter performing this service in the case of dining philosophers. The waiter gives permission to only one philosopher at a time until she picks up both chopsticks. Putting down a chopstick is always allowed. The waiter can be implemented in code as a mutex. This solution introduces a central entity (arbitrator) and can also result in reduces parallelism. If one philosopher is eating and another request a fork, all other philosophers must wait until this request is fulfilled, even if other chopsticks are still available to them.

### 
