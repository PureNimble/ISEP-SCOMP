package PL6.ex02;

public class Main {
    private static final int TICKETS = 15;
    private static final int SELLERS = 5;
    private static final Thread[] sellers = new Seller[SELLERS];

    public static void main(String[] args) throws InterruptedException {
        
        TicketStorage tickets = new TicketStorage(TICKETS);

        for(int i = 0; i < SELLERS; i++) {
            Seller ticket = new Seller(tickets);
            sellers[i] = ticket;
            sellers[i].start();
        }

        Thread.sleep(10000);

        for (Thread seller : sellers) {
            System.out.printf("A interromper %s...\n", seller);
            seller.interrupt();
            seller.join();
        }

        if (tickets.getRemainingTickets() > 0) {
            System.out.printf("Ficaram %s bilhetes por vender...\n", tickets.getRemainingTickets());
        } else System.out.printf("Bilhetes esgotados!\n");
    }
}