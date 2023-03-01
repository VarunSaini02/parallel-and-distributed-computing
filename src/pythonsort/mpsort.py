import multiprocessing as mp
from random import shuffle
from queue import PriorityQueue

class PQEntry:
   def __init__(self, value, queue_index):
      self._value = value
      self._queue_index = queue_index

   def __cmp__(self, other):
      return cmp(self._value, other.value)

   def __lt__(self, other):
      return self.value < other.value

   @property
   def queue_index(self):
      return self._queue_index

   @property
   def value(self):
      return self._value

   def __repr__(self):
      return 'PQEntry('+str(self.value)+','+str(self.queue_index)+')'

   def __str__(self):
      return repr(self)
   
def is_ascending(lst):
    for i in range(len(lst) - 1):
        if lst[i] > lst[i+1]:
            return False
    return True

def process_routine(lst, queue, queue_index):
    sorted_list = sorted(lst)
    for item in sorted_list:
        queue.put(PQEntry(item, queue_index))
    queue.put(None) # Sentinel
   
def test_mpsort():
    # Set up list
    lst = [i for i in range(10000)]
    
    # Shuffle list
    shuffle(lst)

    # Set up chunks
    chunk_size = 100
    num_chunks = len(lst) // chunk_size
    chunks = [lst[which * chunk_size : (which + 1) * chunk_size] for which in range(num_chunks)]

    # Set up queues and processes
    queues = []
    processes = []
    for i in range(num_chunks):
        queues.append(mp.Queue(maxsize=1))
        p = mp.Process(target=process_routine, args=(chunks[i], queues[i], i))
        p.start()
        processes.append(p)
    
    # Initialize priority queue with maxsize equal to num_chunks
    pqueue = PriorityQueue(maxsize=num_chunks)
    for i in range(num_chunks):
        pqueue.put(queues[i].get())

    # Run heap algorithm
    sorted_list = []
    while not pqueue.empty():
        item = pqueue.get()
        sorted_list.append(item.value)
        next_index = item.queue_index
        put_item = queues[next_index].get()
        if put_item is not None:
            pqueue.put(put_item)

    # Sanity check: all processes should have terminated by now
    for i in range(num_chunks):
        processes[i].join()

    # Result should be sorted (ascending)
    print("\nPre Sort:")
    print(lst)
    print("\nPost Sort:")
    print(sorted_list)
    if is_ascending(sorted_list):
        print("\nPASS: This list is sorted\n")
    else:
        print("\nFAIL: This list is not sorted\n")

if __name__ == "__main__":
    test_mpsort()