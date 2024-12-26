# Operating Systems Project: Synchronization Problems

This repository contains implementations of two classic synchronization problems in operating systems:  

1. **Dining Philosophers Problem**  
2. **Readers-Writers Problem**  

These problems serve as foundational examples for understanding resource sharing and synchronization in concurrent systems. By exploring their solutions, this project examines the challenges of deadlock prevention, fairness, and efficiency in multi-process environments.

## Table of Contents

- [Project Description](#project-description)
  - [Dining Philosophers Problem](#dining-philosophers-problem)
  - [Readers-Writers Problem](#readers-writers-problem)
- [Implementation Details](#implementation-details)
  - [Dining Philosophers Problem](#dining-philosophers-problem-1)
    - [Edsger Dijkstra's Solution](#edsger-dijkstras-solution)
    - [Chandy-Misra Solution](#chandy-misra-solution)
    - [Queue-Based Solution](#queue-based-solution)
  - [Readers-Writers Problem](#readers-writers-problem-1)
    - [Readers Preference](#readers-preference)
    - [Writers Preference](#writers-preference)
    - [Fair (Mixed) Approach](#fair-mixed-approach)
- [Conclusion](#conclusion)
- [Possible Improvements](#possible-improvements)

## Project Description

### Dining Philosophers Problem  
The **Dining Philosophers Problem** models a group of philosophers sitting at a table, alternating between thinking and eating. Each philosopher requires two adjacent chopsticks to eat, but chopsticks are shared between neighbors. The goal is to design a synchronization protocol that ensures:
- **Deadlock prevention**: No philosopher gets stuck waiting indefinitely for resources.
- **Fairness**: Each philosopher gets a chance to eat.
- **Efficiency**: Minimize the time philosophers spend waiting for resources.

This project implements and compares three approaches:
1. **Edsger Dijkstra's hierarchical solution**: Avoids deadlocks using a resource hierarchy.  
2. **Chandy-Misra's generalized algorithm**: Introduces clean and dirty states for chopsticks, ensuring fairness and scalability.  
3. **Queue-Based Solution**: A novel approach using separate queues for philosophers and chopsticks to allocate resources efficiently.


### Readers-Writers Problem  
The **Readers-Writers Problem** synchronizes access to a shared resource (e.g., a database) between:
- **Readers**, which can access the resource concurrently.
- **Writers**, which require exclusive access.  

This project explores three approaches:
1. **Readers Preference**: Prioritizes readers over writers, allowing multiple readers to access the resource simultaneously.  
2. **Writers Preference**: Gives priority to writers, ensuring they can access the resource without waiting excessively.  
3. **Fair (Mixed) Approach**: Balances access between readers and writers to ensure fairness.

Each approach is analyzed for its advantages, trade-offs, and suitability in different scenarios.


## Implementation Details

### Dining Philosophers Problem  

#### Edsger Dijkstra's Solution  
This approach relies on:
- **A mutex** for mutual exclusion.  
- **A semaphore per philosopher** to manage chopstick access.  
- **A state variable per philosopher** to track their activity (thinking, eating, or hungry).  

Philosophers follow a fixed order when picking up chopsticks, starting with the lower-numbered chopstick. This resource hierarchy prevents circular waits, eliminating deadlocks. However:
- It may cause **starvation** if a philosopher is slow or unlucky.
- Reordering resources can be computationally expensive in large systems.

#### Chandy-Misra Solution  
This algorithm generalizes the problem using:
- **Clean and dirty chopsticks**: Dirty chopsticks must be cleaned before being passed to another philosopher.  
- **Deferred requests**: Unfulfilled resource requests are queued.  

The algorithm represents resource ownership as a directed acyclic graph, ensuring deadlock prevention and fairness. However, it may lead to longer wait times in large systems.

#### Queue-Based Solution  
This custom solution uses:
- **Two queues**: One for chopsticks and another for philosophers.  
- Resources are allocated by popping elements from the queues, providing each philosopher with a pair of chopsticks.  

This approach is intuitive and efficient but requires careful handling of queue states to prevent deadlocks or resource starvation.


### Readers-Writers Problem  

#### Readers Preference  
- **Readers** can access the shared resource concurrently.  
- **Writers** must wait until all readers finish accessing the resource.  
- Best for scenarios where reading operations are more frequent than writing.

#### Writers Preference  
- **Writers** get priority, ensuring exclusive access when required.  
- Readers must wait if there are pending writer requests.  
- Suitable for write-intensive applications.

#### Fair (Mixed) Approach  
- Balances access between readers and writers to ensure fairness.  
- No process (reader or writer) experiences indefinite starvation.  
- Ideal for systems with a balanced read/write workload.


## Conclusion

This project provides a detailed exploration of synchronization challenges in operating systems. By implementing solutions to the Dining Philosophers and Readers-Writers Problems, it highlights key concepts like:
- Deadlock prevention techniques.  
- Fair and efficient resource allocation strategies.  
- Trade-offs between simplicity, fairness, and scalability.  

These problems underscore the complexities of concurrent programming and the importance of designing robust synchronization protocols.


## Possible Improvements

To further enhance this project, consider the following extensions:

1. **Visualization**  
   - Develop a graphical interface to simulate the philosophers' states or the readers and writers accessing the shared resource.  
   - This would provide an intuitive understanding of synchronization processes.

2. **Performance Metrics**  
   - Measure runtime statistics such as:  
     - Average wait time for philosophers or writers.  
     - Resource utilization rates.  
     - Frequency of deferred requests.  

3. **Additional Synchronization Problems**  
   - Expand the project to include other classic problems like the Sleeping Barber or Producers-Consumers Problem for a broader perspective.

4. **Real-World Applications**  
   - Implement algorithms using parallel programming frameworks like **Pthreads** or **OpenMP** to test scalability in real-world systems.  
