# Message Queue 

The example shows how to use message queues on Linux. 

When executed, two threads are created, one that waits for user input, takes the
data and sends a message to the second thread. The second thread waits for the
message and everytime an 'i' is received, the counter is incremented.
