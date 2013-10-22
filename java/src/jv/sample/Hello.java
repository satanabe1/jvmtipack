package jv.sample;
public class Hello {
    public int count;
    public Hello(){
        count = 0;
    }
    public void say(String str) {
        System.out.println(str);
        count++;
    }
    public void except(){
        System.out.println("This is exception");
        try{
            throw new Exception();
        }catch(Exception ex){
            ex.printStackTrace();
            System.out.println("catch!!");
        }
    }
    public static void uncaught(){
        System.out.println("This is uncaught");
        throw new RuntimeException();
    }
    public static void main(String[] args) {
        System.out.println("MainStart!");
        Hello hello = new Hello();
        Hello buff = new Hello();
        hello.except();
        //Hello.uncaught();
    }
}
