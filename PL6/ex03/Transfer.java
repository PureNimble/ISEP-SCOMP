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
            System.out.println("A iniciar transferência " + getId() + " da conta " + from.accountNumber + " para a conta " + to.accountNumber + " no valor de " + amount);

            if (bank.transferMoney(from, to, amount))
                System.out.printf("Transferência %d concluida com sucesso\n", getId());
            else
                System.out.printf("Transferência %d recusada\n", getId());
        } catch (InterruptedException e) {
            System.out.println("A transferência foi interrompida");
            return;
        }
        return;
    }
}