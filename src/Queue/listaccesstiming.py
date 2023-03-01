import datetime 
import random
import time
import PyList
import my_queue

def main3():
    # Write an XML file with the results
    file = open("QueueComparisonTiming.xml", "w")
    file.write('<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n')
    file.write('<Plot title="Queue Fill and Empty Performance">\n')
    file.write('  <Axes>\n')
    file.write('    <XAxis min="'+str(0)+'" max="'+str(5000)+'">Queue Size</XAxis>\n')
    file.write('    <YAxis min="'+str(0)+'" max="'+str(7500000)+'">Nanoseconds</YAxis>\n')
    file.write('  </Axes>\n')

    # Warm up
    dummy = []
    for i in range(10000):
        dummy.append(i)
    del dummy

    file.write('  <Sequence title="Fill and Empty Time for Array-List-Based Queues" color="red">\n')
    
    for size in range(10, 5000, 50):
        queue = my_queue.ListQueue()
        start = time.monotonic_ns()
        for i in range(size):
            queue.put(None)
        for i in range(size):
            queue.get()
        end = time.monotonic_ns()
        elapsed_time = end - start
        file.write('    <DataPoint x="'+str(size)+'" y="'+str(elapsed_time)+'"/>\n')    

    file.write('  </Sequence>\n')

    file.write('  <Sequence title="Fill and Empty Time for Linked-List-Based Queues" color="blue">\n')
    
    for size in range(10, 5000, 50):
        queue = my_queue.LinkedQueue()
        start = time.monotonic_ns()
        for i in range(size):
            queue.put(None)
        for i in range(size):
            queue.get()
        end = time.monotonic_ns()
        elapsed_time = end - start
        file.write('    <DataPoint x="'+str(size)+'" y="'+str(elapsed_time)+'"/>\n')       

    file.write('  </Sequence>\n')

    file.write('</Plot>\n')
    file.close()

def main2():
    # Write an XML file with the results
    file = open("ListAppendTiming.xml", "w")
    file.write('<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n')
    file.write('<Plot title="Append Performance">\n')
    file.write('  <Axes>\n')
    file.write('    <XAxis min="'+str(0)+'" max="'+str(1000)+'">List Size</XAxis>\n')
    file.write('    <YAxis min="'+str(0)+'" max="'+str(500000)+'">Nanoseconds</YAxis>\n')
    file.write('  </Axes>\n')
    file.write('  <Sequence title="Append Time for List Size" color="red">\n')

    # Warm up
    dummy = []
    for i in range(100):
        dummy.append(i)
    del dummy

    
    for size in range(10, 1000, 10):
        lst = PyList.PyList()
        start = time.monotonic_ns()
        for i in range(size):
            lst.append(None)
        end = time.monotonic_ns()
        elapsed_time = end - start
        file.write('    <DataPoint x="'+str(size)+'" y="'+str(elapsed_time)+'"/>\n')    

    file.write('  </Sequence>\n')

    file.write('  <Sequence title="Append Time for Built-in List Size" color="blue">\n')
    
    for size in range(10, 1000, 10):
        lst = []
        start = time.monotonic_ns()
        for i in range(size):
            lst.append(None)
        end = time.monotonic_ns()
        elapsed_time = end - start
        file.write('    <DataPoint x="'+str(size)+'" y="'+str(elapsed_time)+'"/>\n')    

    file.write('  </Sequence>\n')

    file.write('</Plot>\n')
    file.close()  
        
def main():
    
    # Write an XML file with the results
    file = open("ListAccessTiming.xml","w")
    
    file.write('<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n')
    
    file.write('<Plot title="Average List Element Access Time">\n')
    
    # Test lists of size 1000 to 20000. 
    xmin = 1000
    xmax = 20000
    
    # Record the list sizes in xList and the average access time within
    # a list that size in yList for 1000 retrievals. 
    xList = []
    yList = []
    
    for x in range(xmin, xmax+1, 1000):

        xList.append(x)
        
        prod = 0
        
        # Creates a list of size x with all 0's
        lst = [0] * x
        
        # let any garbage collection/memory allocation complete or at least
        # settle down
        time.sleep(1)
        
        # Time before the 1000 test retrievals
        starttime = datetime.datetime.now()
        
        for v in range(1000):
            # Find a random location within the list
            # and retrieve a value. Do a dummy operation
            # with that value to ensure it is really retrieved. 
            index = random.randint(0,x-1)
            val = lst[index]
            prod = prod * val
        # Time after the 1000 test retrievals  
        endtime = datetime.datetime.now()
        
        # The difference in time between start and end.
        deltaT = endtime - starttime
        
        # Divide by 1000 for the average access time
        # But also multiply by 1000000 for microseconds.
        accessTime = deltaT.total_seconds() * 1000
        
        yList.append(accessTime)
     
    file.write('  <Axes>\n')
    file.write('    <XAxis min="'+str(xmin)+'" max="'+str(xmax)+'">List Size</XAxis>\n')
    file.write('    <YAxis min="'+str(min(yList))+'" max="'+str(60)+'">Microseconds</YAxis>\n')
    file.write('  </Axes>\n')
    
    file.write('  <Sequence title="Average Access Time vs List Size" color="red">\n')   
    
    for i in range(len(xList)):   
        file.write('    <DataPoint x="'+str(xList[i])+'" y="'+str(yList[i])+'"/>\n')    
        
    file.write('  </Sequence>\n')
    
    # This part of the program tests access at 100 random locations within a list
    # of 200,000 elements to see that all the locations can be accessed in 
    # about the same amount of time.
    xList = lst
    yList = [0] * 20000
    
    time.sleep(2)
    
    for i in range(100):
        starttime = datetime.datetime.now()
        index = random.randint(0,20000-1)
        xList[index] = xList[index] + 1
        endtime = datetime.datetime.now()
        deltaT = endtime - starttime   
        yList[index] = yList[index] + deltaT.total_seconds() * 1000000
        
    file.write('  <Sequence title="Access Time Distribution" color="blue">\n')           
  
    for i in range(len(xList)):
        if xList[i] > 0:
            file.write('    <DataPoint x="'+str(i)+'" y="'+str(yList[i]/xList[i])+'"/>\n')    
        
    file.write('  </Sequence>\n') 
    file.write('</Plot>\n')
    file.close()  
    
if __name__ == "__main__":
    main3()
