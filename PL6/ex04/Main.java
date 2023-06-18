package PL6.ex04;
import java.util.Random;

public class Main {
    static final int TAXIS = 10;
    static final int REQUESTS = 20;
    static final Thread[] requests = new TaxiRequest[REQUESTS];
    static final Thread[] taxis = new Taxi[TAXIS];

    public static void main(String[] args) throws InterruptedException {

        TaxiCentral central = new TaxiCentral();
        for(int i = 0; i < TAXIS; i++) {
            int maxSeats = (new Random().nextInt(6) + 4);

            Taxi taxi = new Taxi(central, maxSeats);
            taxis[i] = taxi;
            taxis[i].start();
        }

        for(int i = 0; i < REQUESTS; i++) {
            int maxSeats = (new Random().nextInt(6) + 4);
            String address = "Rua do Isep";

            TaxiRequest request = new TaxiRequest(central, maxSeats, address);
            requests[i] = request;
            requests[i].start();
        }

        for(Thread request: requests) {
            request.join();
        }
        for(Thread taxi: taxis) {
            taxi.join();
        }
    }
}