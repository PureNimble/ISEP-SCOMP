package PL6.ex03;
import java.util.ArrayList;
import java.util.List;

public class Bank {
    private static final double TRANSFER_LOWER_LIMIT = 1;
    private static final double TRANSFER_UPPER_LIMIT = 1000000;

    private List<Account> accounts = new ArrayList<>();

    public Bank(int numAccounts) {
        for(int i = 1; i <= numAccounts; i++)
            accounts.add(new Account(i));
        System.out.println("Adicionou " + numAccounts + " contas ao banco");
    }

    public Account getAccount(int accountNo) {
        for(Account acc: accounts)
            if(acc.accountNumber == accountNo)
                return acc;

        return null;
    }

    public boolean transferMoney(Account fromAcct,Account toAcct, EurosAmount amount) throws InterruptedException {
        if(fromAcct.accountNumber == toAcct.accountNumber) {
            System.out.println("Não é possível fazer uma transferência para a mesma conta!");
            return false;
        }

        if(amount.compareTo(TRANSFER_LOWER_LIMIT) < 0 || amount.compareTo(TRANSFER_UPPER_LIMIT) > 0) {
            System.out.println("Montante inválido!");
            return false;
        }
        
        if (fromAcct.withdraw(amount))
            return toAcct.deposit(amount);

        return false;
    }
}