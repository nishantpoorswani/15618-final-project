# Multi-Core Cache Simulator

## SUMMARY:

We are going to implement a multi-core cache simulator

## BACKGROUND:

## CHALLENGE:

1) Designing bus arbitration for multi-core processor
2) Veifying the correctness of implemented snooping based cache coherency schemes

## RESOURCES:

1) We will be writing the cache simulator from scratch. We will be referencing the lecture slides for the various snooping based cache coherence schemes 
2) For the memory traces we will be using Intel's Pin Tool [Documentation](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-dynamic-binary-instrumentation-tool.html)
3) For data visualization we will be using gtk [Documentation](https://www.gtk.org/docs/)

## GOALS AND DELIVERABLES:

75% - Implementing MI (VI), MSI and MESI snooping based cache coherence protocols.
100% - Implement MOESI based cache coherence protocol and report different metrics - accesses, misses, hits and true/false sharing.
125% - Visualization of data and custom recommendations on how to improve memory performance. 

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
| 03/28/22 - 04/03/22 | Designing the cache simulator. Generating memory traces using Pin Tool |
| 04/04/22 - 04/13/22 | Implement Bus Arbitration as well as MI and MSI snooping cache coherence protocols | 
| 04/14/22 - 04/24/22 | Implement MESI and MOESI snooping cache coherence protocols and generating report for analysis | 
| 04/25/22 - 04/29/22 | Testing and debugging of the project and bug fixes | 


## AUTHORS:

Nishant Poorswani [npoorswa@andrew.cmu.edu](npoorswa@andrew.cmu.edu)

Shridhar Ganiger [sganiger@andrew.cmu.edu](sganiger@andrew.cmu.edu)
