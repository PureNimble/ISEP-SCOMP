package PL6.ex02;

import java.util.concurrent.Semaphore;

public class TicketStorage {

    private int tickets;
    private int remainingTickets;
    private static Semaphore semaphore = new Semaphore(1);

    public TicketStorage(int tickets) {
        this.tickets = tickets;
        this.remainingTickets = tickets;
    }

    public void ticketSelling(long id) {
        try {
            semaphore.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
            System.out.println("Processo Seller a terminar...");
            return;
        }
        if (tickets > 0) {
            tickets--;
            System.out.printf("Vendedor nº %d vendeu bilhete número %d\n", id, remainingTickets - tickets);
        }
        semaphore.release();
    }

    public int getRemainingTickets() {
        return tickets;
    }
}