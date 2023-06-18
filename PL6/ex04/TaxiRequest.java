package PL6.ex04;

import java.util.Random;

public class TaxiRequest extends Thread{
    TaxiCentral taxiCentral;

    int numPassengers;
    String address;

    public TaxiRequest(TaxiCentral taxiCentral, int numPassengers, String address) {
        this.numPassengers = numPassengers;
        this.address = address;
        this.taxiCentral = taxiCentral;
    }

    @Override
    public void run() {
        try {
            sleep(new Random().nextInt(5000));
            System.out.printf("Pedido %s para %d passageiros feito com sucesso\n", getId(), numPassengers);

            taxiCentral.addRequest(this);
        } catch (InterruptedException e) {
            System.out.println("TaxiRequest interrompido");
            return;
        }
        return;
    }

    @Override
    public String toString() {
        return String.format("Pedido nº %d Passageiros: %d", getId(), numPassengers);
    }
}