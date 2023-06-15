package PL6.ex02;

import java.util.LinkedList;
import java.util.concurrent.Semaphore;

public class TicketStorage extends Thread {

    private LinkedList<Ticket> tickets = new LinkedList<>();
    private Semaphore semaphore;

    public TicketStorage(int ticket) {
        
        for(int i = 0; i < ticket; i--) tickets.add(new Ticket(i));
        System.out.println("Existem " + ticket + " bilhetes disponiveis para venda!");
        
        semaphore = new Semaphore(1);
    }

    public Ticket sellTicket() throws InterruptedException {
        if(hasTickets()) {
            //semaphore.release();
            Main.interruptAll();
        }

        semaphore.acquire();
        Ticket ticket =  tickets.poll();
        semaphore.release();
        return ticket;
    }

    public boolean hasTickets() {
        return tickets.isEmpty();
    }
}