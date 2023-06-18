package PL6.ex06;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.concurrent.*;

public class Main {
    final static int THREADS = 5;
    final static int PARTITIONS = 10;
    final static int ARRAY_SIZE = 500;
    final static int INTERVAL = ARRAY_SIZE/PARTITIONS;

    public static void main(String[] args) throws ExecutionException, InterruptedException {

        int globalSum = 0;
        int[] array = new int[ARRAY_SIZE];
        initializeArray(array);

        List<Future<Integer>> sums = new ArrayList<>();

        ExecutorService executor = Executors.newFixedThreadPool(THREADS);

        for (int i = 0; i < PARTITIONS; i++) {

            int start = i * INTERVAL, end = start + INTERVAL;
            Future<Integer> sum = executor.submit(new Sum(array, start, end));
            sums.add(sum);
        }

        executor.shutdown();
        executor.awaitTermination(10, TimeUnit.SECONDS);

        System.out.printf("Somas Parciais = [");
        for(Future<Integer> sum: sums) {
            globalSum += sum.get();
            System.out.printf("%d; ", sum.get());
        }
        System.out.printf("]\n");
        System.out.printf("Soma Global: " + globalSum + "\n");

        System.exit(0);
    }

    private static void initializeArray(int[] array) {
        for(int i = 0; i < array.length; i++) {
            array[i] = (new Random()).nextInt(100);
        }
    }
}