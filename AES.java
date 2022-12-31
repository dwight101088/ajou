// 9진수 2개가 1바이트 입니다. 평문은 64개
public class AES {
    // 복호화 할 평문
    public static int[] plain = {
            8, 41, 67, 15, 12, 78, 77, 35,
            1, 14, 28, 18, 12, 65, 36, 14
    };

    public static int Nr = 9; // 반복할 라운드

    public static int[][] encrypt() {
        int[][] table;
        table = plain81To16(); // 평문을 16개만 짤라서 2차원 배열로 만들었네용

        for (int round = 0; round < Nr; round++) {
            SubBytes(table);
            ShiftRows(table);
            mixColumns(table);
            AddRoundKey(table, keyExpansion.word[round]);
        }

        SubBytes(table);
        ShiftRows(table);
        AddRoundKey(table, keyExpansion.word[Nr]);

        return table;
    }

    public static void SubBytes(int[][] state) {  // 배열 한 칸에 9진수 두 개로 이루어진 수들의 배열을 받는다.
        box box = new box();

        int x, y;
        for (int i = 0; i < 4; i++) {
            for (int k = 0; k < 4; k++) {
                x = state[i][k] / 10; // 첫번째 9진수
                y = state[i][k] % 10; // 두번째 9진수
                state[i][k] = box.S[x][y]; // 치환
            }
        }

    }

    public static void ShiftRows(int[][] state) { // SubBytes 단계를 거치고 배열에 한 칸에 9진수 2개로 적힌 state
        int temp = 0;

        temp = state[1][0];
        state[1][0] = state[1][1];
        state[1][1] = state[1][2];
        state[1][2] = state[1][3];
        state[1][3] = temp;

        temp = state[2][0];
        state[2][0] = state[2][2];
        state[2][2] = temp;
        temp = state[2][1];
        state[2][1] = state[2][3];
        state[2][3] = temp;

        temp = state[3][0];
        state[3][0] = state[3][3];
        state[3][3] = state[3][2];
        state[3][2] = state[3][1];
        state[3][1] = temp;

    }

    //mixcolum은 아래의 연산을 좌변에 곱한다. 컴퓨팅적으로 적은 연산을 위해 아래의 식을 도출해내 사용하고자 했다.
    //(3,4,1,1)
    //(1,3,4,1)
    //(1,1,3,4)
    //(4,1,1,3)

    public static void mixColumns(int[][] state) {
        int[] tmp = new int[4];
        int[] tmp3 = new int[4];

        spladd func = new spladd();

        for (int i = 0; i < 4; i++) {
            tmp[0] = state[i][0];
            tmp[1] = state[i][1];
            tmp[2] = state[i][2];
            tmp[3] = state[i][3];
            if(tmp[0]>88||tmp[1]>88||tmp[2]>88||tmp[3]>88){ //For error
                System.out.print("flag\n");
            }

            for (int j=0; j < 4; j++) {
                tmp3[i]= Time_3.asdf(tmp[i]);   // 3배+ 81이상일 시, 기약다항식을 kimplus한다.
                //System.out.print(tmp3[i]);
                //System.out.print("\n");
            }

            //state[i][0]=3*[0]+4*[1]+[2]+[3], state[i][1]= ...
            state[i][0]= func.add(Kimplus.kimplus(Kimplus.kimplus(func.spl(tmp3[0]), Kimplus.kimplus(func.spl(tmp3[1]),func.spl(tmp[1]))), Kimplus.kimplus(func.spl(tmp[2]),func.spl(tmp[3]))));
            state[i][1]= func.add(Kimplus.kimplus(Kimplus.kimplus(func.spl(tmp3[1]), Kimplus.kimplus(func.spl(tmp3[2]),func.spl(tmp[2]))), Kimplus.kimplus(func.spl(tmp[3]),func.spl(tmp[0]))));
            state[i][2]= func.add(Kimplus.kimplus(Kimplus.kimplus(func.spl(tmp3[2]), Kimplus.kimplus(func.spl(tmp3[3]),func.spl(tmp[3]))), Kimplus.kimplus(func.spl(tmp[0]),func.spl(tmp[1]))));
            state[i][3]= func.add(Kimplus.kimplus(Kimplus.kimplus(func.spl(tmp3[3]), Kimplus.kimplus(func.spl(tmp3[0]),func.spl(tmp[0]))), Kimplus.kimplus(func.spl(tmp[1]),func.spl(tmp[2]))));
            //System.out.print(state[i][1]);
            //System.out.print("\n");

        }

    }

    public static void AddRoundKey(int[][] state, int[] Roundkey) {
        int[][] arr = new int[4][16];
        int[][] afterArr = new int[4][16];

        for (int i = 0; i < 4; i++) {
            int t = 0;
            for (int k = 0; k < 4; k++) {
                int ten = state[i][k] / 10;
                int one = state[i][k]  % 10;

                arr[i][k * t] = ten / 3;
                arr[i][k * t + 1] = ten % 3;
                arr[i][k * t + 2] = one / 3;
                arr[i][k * t + 3] = one % 3;
                t++;
            }
        }

        for (int i = 0; i < 4; i++) {
            afterArr[i] = KimXor.kimXor(arr[i], Roundkey);
        }

        int t = 0;
        for (int i = 0; i < 4; i++) {
            int temp = 0;
            int aaaa = 0;
            int ten = 0;
            for (int k = 0; k < 16; k++) {
                if (k == 0) {
                    temp = 3 * afterArr[i][k];
                }
                else if (k % 2 == 1) {
                    temp += afterArr[i][k];
                    if (ten == 0) {
                        ten++;
                        aaaa = temp * 10;
                    }else {
                        ten = 0;
                        state[i][t] = aaaa + temp;
                        t = 0;
                    }

                }
                else {
                    temp = 3 * afterArr[i][k];
                }
            }
        }

    }
    public static int[][] plain81To16() {
        int[][] firstTable = new int[4][4];

        int t = 0;
        for (int i = 0; i < 4; i++) {
            for (int k = 0; k < 4; k++) {
                firstTable[i][k] = plain[t]; // 평문 64칸 중 16칸이 테이블로 이동
                t++;
            }
        }


        return firstTable;
    }

    public static void main(String[] args) {

        keyExpansion.start();
        int[][] result = encrypt();

        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++){
                System.out.print(result[i][j] + " ");
            }

        }
    }
}