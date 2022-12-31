public class spladd {
    public int add(int[] a) {  // 3진수 4개를 9비트로 합친다
        int num;
        num = a[0] * 30 + a[1] * 10 + a[2] * 3 + a[3];
        if(num > 88) {
            System.out.print(num);
            System.out.print(" ff\n");
        }

        return num;
    }
    public int[] spl(int num) { // 반대
        int[] temp = new int[4];
        temp[0] = num / 30;
        num %= 30;

        temp[1] = num / 10;
        num %= 10;
        temp[2] = num / 3;
        temp[3] = num % 3;
        return temp;
    }
}
