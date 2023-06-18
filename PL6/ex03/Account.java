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
            System.out.println(e.getMessage() + " Dinheiro disponível: " + balance + " Montante da operação: " + amount);
            return false;
        }
        semaphore.release();

        System.out.printf("Tirou %s da conta %d. Dinheiro disponível: %s euros\n", amount, accountNumber, balance);
        return true;
    }

    public boolean deposit(EurosAmount amount) throws InterruptedException {
        semaphore.acquire();
        balance.add(amount);
        semaphore.release();

        System.out.printf("Depositou %s na conta %d. Dinheiro disponível: %s euros\n", amount, accountNumber, balance);
        return true;
    }
}