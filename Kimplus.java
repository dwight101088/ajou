public class Kimplus {
    public static int[] kimplus (int[] word1, int[] word2) {
        int[] word= new int[4];
        for(int i = 0; i < 4; i++){
            word[i] = XOR(word1[i], word2[i]);
        }
        return word;
    }
    public static int XOR(int a, int b){ //GF(3^4)상의 논리합 연산이다. 0+0=0, 0+1=1, 1+2=0...
        int c =- 1;

        if( a==0 ) {
            if(b == 0){
                c = 0;
            }
            if(b == 1){
                c = 1;
            }
            if(b == 2){
                c = 2;
            }
        }
        else if(a == 1) {
            if(b == 2){
                c = 0;
            }
            if(b == 1){
                c = 2;
            }
            if(b == 0){
                c = 0;
            }
        }
        else {
            if(b == 0) {
                c = 2;
            }
            else if(b == 1){
                c = 0;
            }
            else {
                c = 1;
            }
        }
        return c;
    }
}
