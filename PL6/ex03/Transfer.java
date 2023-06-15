package PL6.ex03;

import java.util.Random;

public class Transfer extends Thread {
    private Bank bank;
    private Account from;
    private Account to;
    private EurosAmount amount;

    public Transfer(Bank bank, Account from, Account to, EurosAmount amount) {
        this.bank = bank;
        this.from = from;
        this.to = to;
        this.amount = amount;
    }


    @Override
    public void run() {
        try {
            sleep(new Random().nextInt(5000));
            System.out.println("Initiating transfer " + getId() + " from account " + from.accountNumber + " to account " + to.accountNumber + " by " + amount);

            if (bank.transferMoney(from, to, amount))
                System.out.printf("Transfer %d successful\n", getId());
            else
                System.out.printf("Transfer %d rejected\n", getId());
        } catch (InterruptedException e) {
            System.out.println("Transfer was interrupted");
            return;
        }
        return;
    }
}