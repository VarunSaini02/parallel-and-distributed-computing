class RunnableDemo implements Runnable {
   private Thread t;
   private String threadName;

   RunnableDemo( String name) {
      threadName = name;
      System.out.println("Creating " +  threadName );
   }

   @Override
   public void run() {
      System.out.println("Running " +  threadName );
      for(int i = 20; i > 0; i--) {
         System.out.println("Thread: " + threadName + ", " + i);
      }

      System.out.println("Thread " +  threadName + " exiting.");
   }
}

public class ThreadExample1 {

   public static void main(String args[]) {
      Thread t1 = new Thread(new RunnableDemo( "Thread-1"));
      t1.start();

      Thread t2 = new Thread(new RunnableDemo( "Thread-2"));
      t2.start();

      try {
         t1.join();
         t2.join();
      } catch (InterruptedException ex) {
      }
   }
}