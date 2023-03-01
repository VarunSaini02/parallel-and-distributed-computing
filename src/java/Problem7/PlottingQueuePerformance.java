import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Queue;

class ArrayListQueue<E> implements Queue<E> {
    private List<E> queue = new ArrayList<>();
    private SpinLock lock = new SpinLock();

    @Override
    public boolean add(E e) {
        try {
            lock.acquire();
            queue.add(e);
        } catch (Exception ex) {
        } finally {
            lock.release();
        }
        return true;
    }

    @Override
    public E remove() {
        E val = null;
        try {
            lock.acquire();
            val = queue.remove(0);
        } catch (Exception ex) {
        } finally {
            lock.release();
        }
        return val;
    }

    @Override
    public boolean offer(E e) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public E poll() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public E element() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public E peek() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public int size() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean isEmpty() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean contains(Object o) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public Iterator<E> iterator() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public Object[] toArray() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public <T> T[] toArray(T[] a) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean remove(Object o) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean containsAll(Collection<?> c) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean addAll(Collection<? extends E> c) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean removeAll(Collection<?> c) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean retainAll(Collection<?> c) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public void clear() {
        throw new UnsupportedOperationException("Not supported");
    }
}

class LinkedListQueue<E> implements Queue<E> {
    private List<E> queue = new LinkedList<>();
    private SpinLock lock = new SpinLock();
    
    @Override
    public boolean add(E e) {
        try {
            lock.acquire();
            queue.add(e);
        } catch (Exception ex) {
        } finally {
            lock.release();
        }
        return true;
    }

    @Override
    public E remove() {
        E val = null;
        try {
            lock.acquire();
            val = queue.remove(0);
        } catch (Exception ex) {
        } finally {
            lock.release();
        }
        return val;
    }

    @Override
    public boolean offer(E e) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public E poll() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public E element() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public E peek() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public int size() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean isEmpty() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean contains(Object o) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public Iterator<E> iterator() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public Object[] toArray() {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public <T> T[] toArray(T[] a) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean remove(Object o) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean containsAll(Collection<?> c) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean addAll(Collection<? extends E> c) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean removeAll(Collection<?> c) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public boolean retainAll(Collection<?> c) {
        throw new UnsupportedOperationException("Not supported");
    }

    @Override
    public void clear() {
        throw new UnsupportedOperationException("Not supported");
    }
}

class PlottingQueuePerformance {
    public static void main(String[] args) {
        int MAX_THREADS = 100;
        int TOTAL_ITEMS = 50000;

        try (FileWriter writer = new FileWriter("PlotData.xml")) {
            writer.write("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n");
            writer.write("<Plot title=\"Queue Fill and Empty Bandwidth with Multiple Threads\">\n");
            writer.write(" <Axes>\n");
            writer.write(
                    " <XAxis min=\"0\" max=\"" + MAX_THREADS + "\">Number of Threads</XAxis>\n");
            writer.write(" <YAxis min=\"0\" max=\"5000000\">Items Per Second</YAxis>\n");
            writer.write(" </Axes>\n");

            // Begin warm up
            List<Integer> dummy = new ArrayList<>();
            for (int i = 0; i < 10000; i++) {
                dummy.add(i);
            }
            dummy = null;
            // End warm up

            // Begin ArrayList Queue Test
            writer.write(" <Sequence title=\"Fill and Empty Bandwidth for ArrayList Queue\" color=\"red\">\n");

            for (int numThreads = 1; numThreads <= MAX_THREADS; numThreads++) {
                System.out.print("Starting ArrayListQueue test with " + numThreads + "/" + MAX_THREADS + " threads...\t");
                Queue<Object> arrayListQueue = new ArrayListQueue<>();
                ArrayList<Thread> threadsThatAdd = new ArrayList<>();
                ArrayList<Thread> threadsThatRemove = new ArrayList<>();
                int ITEMS_PER_THREAD = TOTAL_ITEMS / numThreads;

                // Set up all threads
                for (int i = 0; i < numThreads; i++) {
                    Thread t1 = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            for (int j = 0; j < ITEMS_PER_THREAD; j++) {
                                arrayListQueue.add(null);
                            }
                        }
                    });
                    Thread t2 = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            for (int j = 0; j < ITEMS_PER_THREAD; j++) {
                                arrayListQueue.remove();
                            }
                        }
                    });
                    threadsThatAdd.add(t1);
                    threadsThatRemove.add(t2);
                }

                // Start timer (we don't want to include all the setup above)
                long start = System.nanoTime();

                // Add all elements
                for (int i = 0; i < numThreads; i++) {
                    threadsThatAdd.get(i).start();
                }
                try {
                    for (int i = 0; i < numThreads; i++) {
                        threadsThatAdd.get(i).join();
                    }
                } catch (InterruptedException ex) {}

                // Remove all elements
                for (int i = 0; i < numThreads; i++) {
                    threadsThatRemove.get(i).start();
                }
                try {
                    for (int i = 0; i < numThreads; i++) {
                        threadsThatRemove.get(i).join();
                    }
                } catch (InterruptedException ex) {}

                // End timer
                long end = System.nanoTime();

                double elapsedTimeSeconds = (end - start) / 1e9;
                double itemsPerSecond = (ITEMS_PER_THREAD * numThreads) / elapsedTimeSeconds;
                writer.write(" <DataPoint x=\"" + numThreads + "\" y=\"" + itemsPerSecond + "\"/>\n");
                System.out.println("done.");
            }

            writer.write(" </Sequence>\n");
            // End ArrayList Queue Test

            // Begin LinkedList Queue Test
            writer.write(" <Sequence title=\"Fill and Empty Bandwidth for LinkedList Queue\" color=\"blue\">\n");

            for (int numThreads = 1; numThreads <= MAX_THREADS; numThreads++) {
                System.out.print("Starting LinkedListQueue test with " + numThreads + "/" + MAX_THREADS + " threads...\t");
                Queue<Object> linkedListQueue = new LinkedListQueue<>();
                ArrayList<Thread> threadsThatAdd = new ArrayList<>();
                ArrayList<Thread> threadsThatRemove = new ArrayList<>();
                int ITEMS_PER_THREAD = TOTAL_ITEMS / numThreads;

                // Set up all threads
                for (int i = 0; i < numThreads; i++) {
                    Thread t1 = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            for (int j = 0; j < ITEMS_PER_THREAD; j++) {
                                linkedListQueue.add(null);
                            }
                        }
                    });
                    Thread t2 = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            for (int j = 0; j < ITEMS_PER_THREAD; j++) {
                                linkedListQueue.remove();
                            }
                        }
                    });
                    threadsThatAdd.add(t1);
                    threadsThatRemove.add(t2);
                }

                // Start timer (we don't want to include all the setup above)
                long start = System.nanoTime();

                // Add all elements
                for (int i = 0; i < numThreads; i++) {
                    threadsThatAdd.get(i).start();
                }
                try {
                    for (int i = 0; i < numThreads; i++) {
                        threadsThatAdd.get(i).join();
                    }
                } catch (InterruptedException ex) {}

                // Remove all elements
                for (int i = 0; i < numThreads; i++) {
                    threadsThatRemove.get(i).start();
                }
                try {
                    for (int i = 0; i < numThreads; i++) {
                        threadsThatRemove.get(i).join();
                    }
                } catch (InterruptedException ex) {}

                // End timer
                long end = System.nanoTime();

                double elapsedTimeSeconds = (end - start) / 1e9;
                double itemsPerSecond = (ITEMS_PER_THREAD * numThreads) / elapsedTimeSeconds;
                writer.write(" <DataPoint x=\"" + numThreads + "\" y=\"" + itemsPerSecond + "\"/>\n");
                System.out.println("done.");
            }

            writer.write(" </Sequence>\n");
            // End LinkedList Queue Test

            writer.write("</Plot>\n");

            System.out.println("Successful write.");
        } catch (IOException e) {
            System.err.println("An error occurred.");
            e.printStackTrace();
        }
    }
}