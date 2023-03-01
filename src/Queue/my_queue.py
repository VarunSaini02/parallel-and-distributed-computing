""" 
Below are two implementations of a queue in Python: ListQueue and LinkedQueue.

ListQueue is built on Python's built-in list class, and LinkedQueue is built on a linked list, 
which itself is built from a custom Node class.

When plotting the performance of both lists (when filling and emptying the queues with anywhere
from 10 to 5000 elements), ListQueue originally performed much better than LinkedQueue. I believe
that this is because of the amount of time needed to allocate memory for each Node in the LinkedQueue,
as each Node is a more complicated data structure than an individual element in a list.

For input sizes larger than about N = 2000, however, ListQueue's run time began to take on an exponential
shape, which comes from its get() method's poor performance. This method removes the first element of a list,
which is an O(N) time operation: all other elements in the list must be shifted back. ListQueue's put()
method runs in O(1) time (amortized), as it relies on Python's list.append() function. Thus, putting N elements
into a ListQueue takes O(N) time (amortized), but removing them one-by-one takes O(N^2) time.

LinkedQueue, on the other hand, has more efficient implementations of put() and get(). The put() method appends
a Node to the end of the linked list, and the get() method removes an element from the other end of the linked
list. Since the class keeps track of the head and tail of the linked list, these are both O(1) time operations.
Thus, putting N elements into a LinkedQueue takes O(N) time, and removing them one-by-one also takes O(N) time.

As we can see in the plot, as N tends to infinity, ListQueue's plot takes on a quadratic shape, representing
its O(N^2) time performance. LinkedQueue's plot remains linear, representing its O(N) time performance.
Even though ListQueue originally performs better than ListQueue, this is likely just because of the additional
overhead required to maintain a linked list as compared to a normal list; this is insignificant when analyzing
asymptotical run time performance. As we can see, LinkedQueue is the more efficient data structure for 
implementing a queue as the input size N tends to infinity.

"""

class ListQueue:
    __slots__ = ("queue") 

    def __init__(self):
        self.queue = []

    def qsize(self):
        return len(self.queue)

    def empty(self):
        return self.qsize() == 0

    def put(self, item):
        self.queue.append(item)

    def get(self):
        if self.empty():
            raise Exception("Cannot get element, queue is empty")
        return self.queue.pop(0)

class Node:
    __slots__ = ("val", "next") 

    def __init__(self, val, next=None):
        self.val = val
        self.next = next

class LinkedQueue:
    __slots__ = ("dummy", "curr", "size")

    def __init__(self):
        self.dummy = Node(None)
        self.curr = self.dummy
        self.size = 0

    def qsize(self):
        return self.size

    def empty(self):
        return self.qsize() == 0

    def put(self, item):
        new_node = Node(item, None)
        self.curr.next = new_node
        self.curr = new_node
        self.size += 1
        return None

    def get(self):
        if self.empty():
            raise Exception("Cannot get element, queue is empty")
        to_return = self.dummy.next.val
        if self.size > 1:
            self.dummy.next = self.dummy.next.next
        else:
            self.dummy.next = None
        self.size -= 1
        return to_return