package PL6.ex02;

public class Seller extends Thread {
    private TicketStorage storage;

    public Seller(TicketStorage storage) {
        this.storage = storage;
    }

    @Override
    public void run() {
        while(!Thread.interrupted()) {
            try {
                Ticket ticket = storage.sellTicket();
                System.out.printf("Vendedor nº %d vendeu %s\n", this.getId(), ticket);

                sleep(2000);
            } catch (InterruptedException e) {
                System.out.println("Processo Seller a terminar...");
                return;
            }
        }
        return;
    }
}