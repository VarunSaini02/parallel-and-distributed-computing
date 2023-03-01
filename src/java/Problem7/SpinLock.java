import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicInteger;

public class SpinLock {
    private AtomicInteger lock = new AtomicInteger(0);

    public void acquire() throws TimeoutException, InterruptedException {
        acquire(-1);
    }

    public void acquire(int timeOut) throws TimeoutException, InterruptedException {
        long endTime = System.nanoTime() + ((long)timeOut * (long)1e9);
        while (lock.getAndSet(1) == 1) {
            if (timeOut != -1 && System.nanoTime() > endTime) {
                throw new TimeoutException("Acquire timed out");
            }
            Thread.sleep(1);
        }
    }

    public void release() {
        lock.set(0);
    }
}