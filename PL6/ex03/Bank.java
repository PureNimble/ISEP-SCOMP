package PL6.ex03;
import java.util.ArrayList;
import java.util.List;

public class Bank {
    private static final double TRANSFER_LOWER_LIMIT = 1;
    private static final double TRANSFER_UPPER_LIMIT = 1000000;

    private List<Account> accounts = new ArrayList<>();

    public Bank(int numAccounts) {
        //Initializes bank by creating N accounts with sequential numbers
        for(int i = 1; i <= numAccounts; i++)
            accounts.add(new Account(i));
        System.out.println("Added " + numAccounts + " accounts to bank");
    }

    public Account getAccount(int accountNo) {
        for(Account acc: accounts)
            if(acc.accountNumber == accountNo)
                return acc;

        return null;
    }

    public boolean transferMoney(Account fromAcct,Account toAcct, EurosAmount amount) throws InterruptedException {
        //Validate accounts
        if(fromAcct.accountNumber == toAcct.accountNumber) {
            System.out.println("Cannot transfer between the same account!");
            return false;
        }

        //Validate transfer amount
        if(amount.compareTo(TRANSFER_LOWER_LIMIT) < 0 || amount.compareTo(TRANSFER_UPPER_LIMIT) > 0) {
            System.out.println("Invalid transfer amount!");
            return false;
        }

        //Validate resulting balance in fromAcct
        if (fromAcct.withdraw(amount))
            return toAcct.deposit(amount);

        return false;
    }
}