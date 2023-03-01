import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.ArrayList;

class Counter2 implements Runnable {
    static int count = 0;
    private TicketLock lock;

    public Counter2(TicketLock lock) {
        this.lock = lock;
    }

    public void run() {
        for (int k = 0; k < 1000; k++) {
            try {
                lock.acquire();
                count += 1;
            } catch (Exception ex) {
            } finally {
                lock.release();
            }
        }
    }
}

public class TicketLock {
    private AtomicInteger nextTicket = new AtomicInteger(0);
    private AtomicInteger nowServing = new AtomicInteger(0);

    public void acquire() throws TimeoutException, InterruptedException {
        acquire(-1);
    }

    public void acquire(int timeOut) throws TimeoutException, InterruptedException {
        long endTime = System.nanoTime() + ((long)timeOut * (long)1e9);
        int myTicket = nextTicket.getAndIncrement();
        while (nowServing.get() != myTicket) {
            if (timeOut != -1 && System.nanoTime() > endTime) {
                throw new TimeoutException("Acquire timed out");
            }
            Thread.sleep(10);
        }
    }

    public void release() {
        nowServing.getAndIncrement();
    }

    public static void main(String args[]) {
        TicketLock lock = new TicketLock();
        ArrayList<Thread> threads = new ArrayList<>();
        Thread t;
        long start_time, end_time, seconds;

        try {
            lock.acquire();

            start_time = System.nanoTime();
            try {
                lock.acquire(5);
            } catch (TimeoutException ex) {
                System.out.println("Test 1 passed.");
            } finally {
                lock.release();
            }
            end_time = System.nanoTime();
            seconds = (end_time - start_time) / 1000000000;
            if (seconds >= 5)
                System.out.println("Test 2 passed.");
            else
                System.out.println("Test 2 failed.");

        } catch (Exception ex) {
            System.out.println("Test 1 failed. ");
            System.out.println(ex);
        }

        try {
            for (int k = 0; k < 10; k++) {
                t = new Thread(new Counter2(lock));
                t.start();
                threads.add(t);
            }

            for (int k = 0; k < 10; k++)
                threads.get(k).join();
            if (Counter2.count == 10000)
                System.out.println("Test 3 passed.");
            else
                System.out.println("Test 3 failed.");

        } catch (Exception ex) {
        }
    }
}