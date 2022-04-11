# Multi-Core Cache Simulator

## SUMMARY:

We are going to implement an event-driven multi-core cache simulator. This simulator will support different snooping based cache coherence schemes. It will analyze memory accesses at run time and report various cache performance metrics. This simulator based tool will also provide cache performance optimization recommendations to the user to improve memory performance.

## BACKGROUND:

This project is an inspiration from Introduction to Computer Systems (15-213) Cache lab which is a trace-driven cache simulator for a uniprocessor. Our idea is to build an event-driven cache simulator that supports multi-core processor. Caches play a vital role in determining the performance of parallel programs on a multi-core processor. One of the major challenges that multi-core processors face is the coherency problem. Cache is coherent when all the loads and stores to a given memory location behave correctly. There are two main methods to ensure cache coherence: a) snooping based cache coherence schemes b) directory based cache coherence schemes. 

Snooping based cache coherence scheme work based on the idea that coherence-related activity is broadcast to all processors in the system. Cache controllers snoop on the bus to monitor memory operations, and react accordingly, to maintain cache coherence. There are a few snooping based cache coherence schemes like MI(VI), MSI, MESI and MOESI. 

### MI:

![image](https://user-images.githubusercontent.com/80713159/159839944-505bf792-c69b-4556-ac90-35a748906504.png)

### MSI

![tempsnip](https://user-images.githubusercontent.com/80713159/159840351-fe896d5c-6f04-4c65-b93e-15dc999fee7e.png)

### MESI

![tempsnip1](https://user-images.githubusercontent.com/80713159/159840596-00ec5f54-2cdf-4fae-b2b4-d2395418cc56.png)


## CHALLENGE:

1) Designing an event-driven cache simulator 
1) Designing bus arbitration for multi-core processor
2) Veifying the correctness of implemented snooping based cache coherency schemes

## RESOURCES:

1) We will be writing the event-driven cache simulator from scratch. We will be referencing the lecture slides for the various snooping based cache coherence schemes 
2) For the memory traces we will be using Intel's Pin Tool [Documentation](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-dynamic-binary-instrumentation-tool.html)
3) For data visualization we will be using gtk [Documentation](https://www.gtk.org/docs/)

## GOALS AND DELIVERABLES:

| Goal % | Goals |
| ----------- | ----------- |
| 75% | Implementing event-driven uni-processor cache simulator |
| 100% | Implementing MI (VI) and MSI snooping based cache coherence protocols | 
| 125% | Implementin MESI and visulization of stats and custom recommendations on how to improve memory performance | 
## PLATFORM CHOICE:

1) Programming Language:

We will be implementing the simulator in C++. Since, we are implementing multiple cache coherency protocols, we can use various aspects of Object Oriented Programming to our advantage. Also, C++ provides a GTK library which can help with data visualization.

2) Architecture:

Ideally, our simulator is architecture independent. You would need to provide the correct memory traces for different architectures.

3) Tools:

To generate the memory trace, we will be using Intel's Pin tool. This tool analyzes the code and generates memory traces for the same.

## SCHEDULE:

| Dates | Tasks |
| ----------- | ----------- |
| 03/28/22 - 04/03/22 | Understanding Pin tool and generating memory traces using Pin Tool |
| 04/04/22 - 04/06/22 | Designing event-driven uni-processor cache simulator | 
| 04/07/22 - 04/11/22 | Implementing an event-driven uni-processor cache simulator |
| 04/14/22 - 04/18/22 | Implement Atomic bus arbiter and account for various access delays |
| 04/19/22 - 04/24/22 | Implement MI and MSI snooping based cache coherence protocols  | 
| 04/25/22 - 04/29/22 | Testing and debugging of the project and bug fixes | 

## AUTHORS:

Nishant Poorswani [npoorswa@andrew.cmu.edu](npoorswa@andrew.cmu.edu)

Shridhar Ganiger [sganiger@andrew.cmu.edu](sganiger@andrew.cmu.edu)
