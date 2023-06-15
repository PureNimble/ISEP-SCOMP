package PL6.ex01;

public class OrderTaker extends Thread {
    private OrderQueue orderQueue;
    private static int orderCount = 1;

    public OrderTaker(OrderQueue orderQueue) {
        this.orderQueue = orderQueue;
    }

    @Override
    public void run() {
        while (!Thread.interrupted()) {
            Order order = new Order(orderCount++);
            try {
                sleep(2000);
            } catch (InterruptedException e) {
                System.out.printf("%s OrderTaker interrompido.\n", order);
                return;
            }
            orderQueue.addOrder(order);
        }
    }
}