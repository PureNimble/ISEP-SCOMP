package PL6.ex04;
import java.util.Random;

public class Taxi extends Thread {

    TaxiCentral taxiCentral;
    int maxSeats;

    public Taxi(TaxiCentral taxiCentral, int maxSeats) {
        this.taxiCentral = taxiCentral;
        this.maxSeats = maxSeats;
    }

    @Override
    public void run() {
        try {
            sleep(new Random().nextInt(5000));
            System.out.printf("O taxi nº %s está disponível com a capacidade de %d lugares\n", getId(), maxSeats);

            TaxiRequest request = taxiCentral.pollRequest();
            System.out.printf("O taxi nº %s recebeu um pedido para %d pessoas\n", getId(), request.numPassengers);

            if (request.numPassengers > maxSeats) {
                System.out.printf("Número de passageiros superior á capacidade máxima do taxi %s, comunicando a central...\n", getId());
                TaxiRequest newTaxiRequest = new TaxiRequest(taxiCentral, request.numPassengers - maxSeats, request.address);
                newTaxiRequest.start();
            }
        } catch (InterruptedException e) {
            System.out.println("Taxi interrompido");
            return;
        }
        return;
    }
}