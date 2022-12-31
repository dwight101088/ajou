public class KimXor {
    public static int[] kimXor(int[] word1, int[] word2) { // 각 칸이 3진법인 16배열을 XOR
        int[] WORD = new int[16];
        for (int i = 0; i < 16; i++) {
            WORD[i] = XOR(word1[i], word2[i]);
        }

        return WORD;
    }

    public static int XOR(int a, int b) { // 3진법 Xor 진리표
        int c = -1;

        if (a == 2) {
            if (b == 2) {
                c = 1;
            }
            else if (b == 0) {
                c = 0;
            }
            else if (b == 1) {
                c = 2;
            }
        }
        else if ( a == 0) {
            if (b == 2) {
                c = 0;
            }
            else if (b == 0) {
                c = 2;
            }
            else if (b == 1) {
                c = 1;
            }
        }
        else if (a == 1) {
            if (b == 2) {
                c = 2;
            }
            else if (b == 0) {
                c = 1;
            }
            else if (b == 1) {
                c = 0;
            }
        }

        return c;
    }
}

