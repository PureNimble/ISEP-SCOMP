package PL6.ex03;
import java.util.Random;

public class Main {
    static final int ACCOUNTS = 10;
    static final int TRANSFERS = 20;
    static final Thread[] transfers = new Transfer[TRANSFERS];

    public static void main(String[] args) throws InterruptedException {
        Bank bank = new Bank(ACCOUNTS);

        for(int i = 0; i < TRANSFERS; i++) {
            Account fromAccount = bank.getAccount(new Random().nextInt(ACCOUNTS) + 1);
            Account toAccount = bank.getAccount(new Random().nextInt(ACCOUNTS) + 1);
            EurosAmount amount = new EurosAmount(new Random().nextInt(100000) + 1);

            Transfer transfer = new Transfer(bank, fromAccount, toAccount, amount);
            transfers[i] = transfer;
            transfers[i].start();
        }

        for(Thread transfer: transfers) {
            transfer.join();
        }

        System.out.println("Terminaram todas as transferências.");
    }
}



/*
 * 
 *      3) Question: In your solution, can a deadlock occur? If yes, can you solve it by reordering
 *                   the statements?
 * 
 *         Answer: No, because in our solution there are no explicit locks or synchronized blocks being used. 
 *                 Each thread acquires the semaphore permit before executing its critical section and releases 
 *                 it afterward, ensuring exclusive access to the account.
 * 
 */