package PL6.ex05;
import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
    final static int THREADS = 5;
    final static int PARTITIONS = 10;
    final static int ARRAY_SIZE = 500;
    final static int INTERVAL = ARRAY_SIZE/PARTITIONS;

    public static void main(String[] args) {
        int[] array = new int[ARRAY_SIZE];
        initializeArray(array);

        ExecutorService executor = Executors.newFixedThreadPool(THREADS);

        for (int i = 0; i < PARTITIONS; i++) {

            int start = i * INTERVAL, end = start + INTERVAL;
            executor.execute(new Sum(array, start, end));
        }
        executor.shutdown();
    }

    private static void initializeArray(int[] array) {
        for(int i = 0; i < array.length; i++) {
            array[i] = (new Random()).nextInt(100);
        }
    }
}