package PL6.ex02;

public class Seller extends Thread {
    private TicketStorage tickets;

    Seller(TicketStorage tickets) {
        this.tickets = tickets;
    }

    @Override
    public void run() {
        while (!Thread.interrupted()) {
            try {
                sleep(2000);
                tickets.ticketSelling(this.getId());
            } catch (InterruptedException e) {
                System.out.println("Processo Seller a terminar...");
                return;
            }
        }
    }
}
