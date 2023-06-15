package PL6.ex03;
import java.util.Random;

public class Main {
    static final int ACCOUNTS = 10;
    static final int TRANSFERS = 20;
    static final Thread[] transfers = new Transfer[TRANSFERS];

    public static void main(String[] args) throws InterruptedException {
        //Creates a shared Bank
        Bank bank = new Bank(ACCOUNTS);

        //Initializes RequestTransfer threads with random values
        for(int i = 0; i < TRANSFERS; i++) {
            Account fromAccount = bank.getAccount(new Random().nextInt(ACCOUNTS) + 1);
            Account toAccount = bank.getAccount(new Random().nextInt(ACCOUNTS) + 1);
            EurosAmount amount = new EurosAmount(new Random().nextInt(100000) + 1);

            Transfer transfer = new Transfer(bank, fromAccount, toAccount, amount);
            transfers[i] = transfer;
            transfers[i].start();
        }

        //Waits for active threads
        for(Thread transfer: transfers) {
            transfer.join();
        }

        System.out.println("Finished all transfers. Shutting down...");
    }
}