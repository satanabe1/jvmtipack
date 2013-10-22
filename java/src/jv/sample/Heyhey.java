package jv.sample;

public class Heyhey {

	public static void main(String[] args) {
		Japanese japanese = new Japanese(15, "Taro", "Saito", "saitotaro");
		British british = new British(23, "John", "Ken", "Nutsuo");
		japanese.yah();
		british.hey();
		String mazekoze = japanese.getFirstName() + british.getLastName();
		System.out.println(mazekoze);
	}
}

class Person {
	private String firstName;
	private String lastName;
	protected int age;

	public Person(int age, String firstName, String lastName) {
		this.firstName = firstName;
		this.lastName = lastName;
		this.age = age;
	}

	public String getFirstName() {
		return firstName;
	}

	public String getLastName() {
		return lastName;
	}

	public int getAge() {
		return age;
	}
}

class Japanese extends Person {
	private String bloodGroup;

	public Japanese(int age, String firstName, String lastName,
			String bloodGroup) {
		super(age, firstName, lastName);
		this.bloodGroup = bloodGroup;
	}

	public String yah() {
		return "Watashi ha," + getLastName() + getFirstName()
				+ " Desu. Ketsueki gata ha " + bloodGroup + " Desu";
	}
}

class British extends Person {
	private String middleName;

	public British(int age, String firstName, String middleName, String lastName) {
		super(age, firstName, lastName);
		this.middleName = middleName;
	}

	public String hey() {
		return "Hey! My name is " + getFirstName() + "-" + middleName + "-"
				+ getLastName() + "!";

	}
}
