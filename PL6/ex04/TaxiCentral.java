package PL6.ex04;

import java.util.LinkedList;

public class TaxiCentral {
    private LinkedList<TaxiRequest> requests = new LinkedList<>();

    public synchronized TaxiRequest pollRequest() {
        while (!hasRequests()) {
            try {
                wait();
            } catch (InterruptedException e) {
                System.out.println("TaxiCentral interrompida");
            }
        }
        return requests.poll();
    }

    public synchronized void addRequest(TaxiRequest request) {
        if (requests.add(request)) {
            System.out.printf("%s na lista de espera.\n", request);
            notify();
        } else {
            System.out.printf("Falha ao adicionar pedido %s á lista.\n", request);
        }
    }

    public boolean hasRequests() {
        return !requests.isEmpty();
    }
}