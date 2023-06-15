package PL6.ex01;

public class Order {
    private int order;

    public Order(int order) {
        this.order = order;
    }
    
    @Override
    public String toString() {
        return String.format("Pedido nº %s:", order);
    }
}