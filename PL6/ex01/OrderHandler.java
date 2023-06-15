package PL6.ex01;

public class OrderHandler extends Thread {

    private OrderQueue orderQueue;

    public OrderHandler(OrderQueue orderQueue) {
        this.orderQueue = orderQueue;
    }

    @Override
    public void run() {
        while (!Thread.interrupted()) {

            Order order = orderQueue.pollOrder();
            System.out.printf("%s A processar...\n", order);
            try {
                sleep(5000);
            } catch (InterruptedException e) {
                System.out.printf("%s Processamento interrompido.\n", order);
                return;
            }
        }
    }
}