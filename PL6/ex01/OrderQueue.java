package PL6.ex01;

import java.util.LinkedList;

public class OrderQueue {
    private LinkedList<Order> orders;

    public OrderQueue() {
        this.orders = new LinkedList<>();
    }

    public synchronized void addOrder(Order order) {
        orders.add(order);
        System.out.printf("%s Adicionado á fila.\n", order);
        notify();
    }

    public synchronized Order pollOrder() {
        while (hasOrders()) {
            try {
                wait();
            } catch (InterruptedException e) {
                System.out.println("OrderTaker interrompido.\n");
                return null;
            }
        }
        return orders.poll();
    }

    public synchronized boolean hasOrders() {
        return orders.isEmpty();
    }
}