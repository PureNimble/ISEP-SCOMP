package PL6.ex05;

import java.util.Arrays;

public class Sum implements Runnable {
    int startIndex, endIndex, partialSum;
    int[] array;

    public Sum(int[] array, int startIndex, int endIndex) {
        this.startIndex = startIndex;
        this.endIndex = endIndex;
        this.array = array;
    }

    @Override
    public void run() {

        partialSum = Arrays.stream(array, startIndex, endIndex).sum();
        System.out.printf("Thread %s: Soma [%d, %d[ = %d\n", Thread.currentThread().getId(), startIndex, endIndex, partialSum);
    }
}