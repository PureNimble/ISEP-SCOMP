package PL6.ex03;

import java.util.concurrent.Semaphore;

public class Account {
    
    private Semaphore semaphore;

    final int accountNumber;
    EurosAmount balance;

    public Account(int accountNumber) {
        this.accountNumber = accountNumber;
        this.balance = new EurosAmount(100000);
        this.semaphore = new Semaphore(1);
    }

    public boolean withdraw(EurosAmount amount) throws InterruptedException {
        semaphore.acquire();
        try {
            balance.subtract(amount);
        } catch (UnsupportedOperationException e) {
            semaphore.release();
            System.out.println(e.getMessage() + " Balance: " + balance + "    Withdrawal amount: " + amount);
            return false;
        }
        semaphore.release();

        System.out.printf("Withdrew %s from account %d. Current balance: %s\n", amount, accountNumber, balance);
        return true;
    }

    public boolean deposit(EurosAmount amount) throws InterruptedException {
        semaphore.acquire();
        balance.add(amount);
        semaphore.release();

        System.out.printf("Deposited %s on account %d. Current balance: %s\n", amount, accountNumber, balance);
        return true;
    }
}