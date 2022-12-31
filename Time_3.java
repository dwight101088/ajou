public class Time_3 {//9진수 2개
    static int asdf(int num){
        box ki = new box();
        spladd func= new spladd();
        int[] temp;
        int tmp;


        if(num/30>0){           //만약 3배를 했을 때, 81이 넘는다면,
            temp=func.spl(num);
            temp[0]=temp[1];
            temp[1]=temp[2];
            temp[2]=temp[3];
            temp[3]=0;

            temp= Kimplus.kimplus(temp,ki.kiyak);   //기약다항식을 kimplus한다.

        }


        else {
            //System.out.print(tmp);    //3배를 해도 무방하다면, 3배를 해준다.
            temp = func.spl(num);
            temp[0]=temp[1];
            temp[1]=temp[2];
            temp[2]=temp[3];
            temp[3]=0;
            //System.out.print(tmp);
        }
        tmp=func.add(temp);

        return tmp;
    }
}
