package PL6.ex02;

public class Main {
    private static final int SELLERS = 5;
    private static final int TICKETS = 20;
    private static final Thread[] sellers = new Seller[SELLERS];

    public static void main(String[] args) throws InterruptedException {

        TicketStorage storage = new TicketStorage(TICKETS);

        for(int i = 0; i < SELLERS; i++) {
            Seller seller = new Seller(storage);
            sellers[i] = seller;
            sellers[i].start();
        }
    }

    public static void interruptAll() throws InterruptedException {
        System.out.println("Bilhetes esgotados!");

        for(Thread seller: sellers) {
            seller.interrupt();
            seller.join();
        }
        System.out.println("No longer selling tickets. Shutting down...");
        System.exit(0);
    }
}