package PL6.ex06;

import java.util.Arrays;
import java.util.concurrent.Callable;

public class Sum implements Callable<Integer> {
    int start, end, partialSum;
    int[] array;

    public Sum(int[] array, int start, int end) {
        this.start = start;
        this.end = end;
        this.array = array;
    }

    @Override
    public Integer call() throws Exception {
        partialSum = Arrays.stream(array, start, end).sum();
        System.out.printf("Thread %s: Soma [%d, %d[ = %d\n", Thread.currentThread().getId(), start, end, partialSum);
        return partialSum;
    }
}