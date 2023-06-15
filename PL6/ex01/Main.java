package PL6.ex01;

public class Main {

    private static final int HANDLERS = 18;
    private static final int TAKERS = 10;
    private static final Thread[] handlers = new Thread[HANDLERS];
    private static final Thread[] takers = new Thread[TAKERS];

    public static void main(String[] args) throws InterruptedException {
        
        OrderQueue queue = new OrderQueue();

        for (int i = 0; i < TAKERS; i++) {
            OrderTaker orderTaker = new OrderTaker(queue);
            takers[i] = orderTaker;
            takers[i].start();
        }

        for (int i = 0; i < HANDLERS; i++) {
            OrderHandler orderHandler = new OrderHandler(queue);
            handlers[i] = orderHandler;
            handlers[i].start();
        }

        Thread.sleep(10000);

        for (Thread taker : takers) {
            System.out.printf("A interromper %s...\n", taker);
            taker.interrupt();
            taker.join();
        }

        for (Thread handler : handlers) {
            System.out.printf("A interromper %s...\n", handler);
            handler.interrupt();
            handler.join();
        }

        System.out.println("Fim de todos os processamentos");
    }
}