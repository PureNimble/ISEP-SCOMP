package PL6.ex02;

public class Ticket {
    int ticket;

    public Ticket(int ticket) {
        this.ticket = ticket;
    }
    
    @Override
    public String toString() {
        return String.format("Bilhte nº %s", ticket);
    }
}