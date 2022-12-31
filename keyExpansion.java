public class keyExpansion {

    // 9진수 2개로 적혀진 것 처음 key입니다.
    public static int[] intitalKEY = {21, 55, 55, 55, 21, 55, 55, 21, 21, 21, 55, 21, 55, 21, 21, 21};

    public static int[][] word = new int[40][16];  // 16칸이 40개
    public static int r = 0; // 라운드에 따른 Rcon이 몇 번째 인지

    public static void start() {

        int[] intitalTemp = {0, 2, 0, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2,  // 초기의 키를 전부 3진법으로 붚어서 적어 둔 키 입니다.
                0, 2, 0, 1, 1, 2, 1, 2, 1, 2, 1, 2, 0, 2, 0, 1,
                0, 2, 0, 1, 0, 2, 0, 1, 1, 2, 1, 2, 0, 2, 0, 1,
                1, 2, 1, 2, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 1
        };

        int t = 0;
        for (int k = 0; k < 4; k++) {
            for (int i = 0; i < 16; i++) {
                word[k][i] = intitalTemp[t];  // W0 W1 W2 W3 생성
                t++;
            }
        }

        nextKey();
    }
    public static void nextKey() {

        for (int i = 4; i < 40; i++) {
            if (i % 4== 0) {
                word[i] = KimXor.kimXor(word[i - 4], FunG(word[i - 1]));  // 4의 배수 일 때 확장 G함수로 XOR
            }
            else {
                word[i] = KimXor.kimXor(word[i - 1], word[i - 4]);  // 다음 W 생성
            }
        }

    }

    public static int[] FunG(int[] word) { // 확장 g 함수
        /*
        word는 3진수로 이루어진 16칸 배열이다.
        바이트 만큼 한칸 옮기고 3진수 16칸을 9진수 2개 4칸으로 변경
        9진수 2개 4칸을 s-box를 통해 치환한 후 다시 3진수 16칸으로 변경
        Rcon Xor 시킨 값을 리턴한다.
         */


        int[] wPrime = new int[16];
        wPrime = word;

        int temp;
        for (int i = 0; i < 4; i++) {
            temp = word[0];
            for (int k = 14; k >= 0; k--) {
                wPrime[k] = wPrime[k + 1];
            }
            wPrime[15] = temp;
        }


        int[] B = new int[8];
        int temp2 = 0;
        int t = 0;
        for (int i = 0; i < 16; i++) {
            if (i == 0) {
                temp2 = 3 * wPrime[i];
            }
            else if (i % 2 == 1) {
                B[t] = temp2 + wPrime[i];
                t++;
            }
            else {
                temp2 = 3 * wPrime[i];
            }
        }

        box BBB = new box();
        int x = 0;
        int y = 0;
        t = 0;
        int[] four = new int[4];
        for (int i = 0; i < 8; i++) {
            if (i == 0) {
                x = B[i];
            }
            else if (i % 2 == 1) {
                y = B[i];
                four[t] = BBB.S[x][y];
                t++;
            }
            else {
                x = B[i];
            }
        }

        int[] after = new int[16];
        for (int i = 0; i < 4; i++) {
            int ten = four[i] / 10;
            int one = four[i] % 10;
            after[i * 4] = ten / 3;
            after[i * 4 + 1] = ten % 3;
            after[i * 4 + 2] = one / 3;
            after[i * 4 + 3] = one % 3;
        }

        int[] Ar = new int[16];
        int[] Rcon = {58, 1, 3, 10, 30, 88, 45, 5, 16, 50 };
        int[] RconX = new int[4];
        RconX[0] = Rcon[r];
        for (int i = 1; i < 4; i++) {
            RconX[i] = 0;
        }

        int[] RconX16 = new int[16];
        int A, BBBBB;
        A = RconX[0] / 10;
        BBBBB = RconX[0] % 10;

        RconX16[0] = A / 3;
        RconX16[1] = A % 3;
        RconX16[2] = BBBBB / 3;
        RconX16[3] = BBBBB % 3;
        for (int i = 4; i < 16; i++) {
            RconX16[i] = 0;
        }

        for (int i = 0; i < 16; i++) {
            Ar = KimXor.kimXor(after, RconX16);
        }
        r++;


        return Ar;
    }


}
