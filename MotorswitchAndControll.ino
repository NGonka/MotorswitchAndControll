
const long buad = 9600;
const String version= "0.1";

boolean debug=false;
boolean sendSwitch=true;
String msg="";
int Number;
boolean MotorMode =false;
int last_MotorSpeed=0;


//pin setup
int BRK = 8;
int DIR = 7;
int PWM_out = 9;
int VPROPI = A3;		// current sense pin 500mV/1A
int motorSwitch = 5;	// Set Pin HIGH for Motormode; LOW for Generatormode
int Sleep=4;



void Brake_active()
{
	digitalWrite(BRK,HIGH);
	
}

void Brake_deactive()
{
	digitalWrite(BRK,LOW);
}


void forward(int speed)
{
	Brake_active();					//Set Brake for better Performance according to DRV8801 Truth Table on page: https://www.pololu.com/product/2136
	digitalWrite(DIR,HIGH);
	analogWrite(PWM_out,speed);
}

void backward(int speed)
{
	Brake_active();					//Set Brake for better Performance according to DRV8801 Truth Table on page: https://www.pololu.com/product/2136
	digitalWrite(DIR,LOW);
	analogWrite(PWM_out,speed);
}

void Brake()
{
	
	digitalWrite(PWM_out,LOW);
	//backward(Brakespeed);
	Brake_active();
}

void Brake_release()
{
	digitalWrite(PWM_out,LOW);
	Brake_deactive();
}

void SleepMode(boolean Mode)
{
	if (Mode)
	{
		digitalWrite(Sleep,LOW);
	}
	else
	{
		digitalWrite(Sleep,HIGH);
		delay(2);
	}
}

void switchOff()
{
	backward(0);
	Brake_release();	
	SleepMode(true);
	digitalWrite(motorSwitch,LOW);
}

void setup()
{
	TCCR1B = TCCR1B & 0b11111000 | 0x01; // sets Timer1 (Pin 9 and 10) to 31300Hz
	pinMode(BRK,OUTPUT);
	pinMode(DIR,OUTPUT);
	pinMode(PWM_out,OUTPUT);
	pinMode(motorSwitch,OUTPUT);
	pinMode(Sleep,OUTPUT);
	SleepMode(true);
	
	Serial.begin(buad);
	Serial.println("Loadcontroller, Motorswitch and Control. Ready");
	//Serial.println("Input Speed only from 0 to 128");
	Serial.print("Version: ");
	Serial.println(version);

}

void loop()
{
	if(Serial.available()>0)
	{
		msg="";
		while(Serial.available()>0)
		{
			msg+=char(Serial.read());
			delay(10);
		}
		sendSwitch=true;
		if(debug)
		{
			Serial.print("msg1: ");
			Serial.println(msg);
		}
	}
	
	Number= msg.substring(1).toInt();
	msg=msg.substring(0,1);
	msg.toUpperCase();
	
	
	
	
	
	if (debug&&sendSwitch)
	{
		Serial.print("Number: ");
		Serial.print(Number);
		Serial.print(", msg substring: ");
		Serial.println(msg);
		sendSwitch=false;
	}
	
	if (msg.equals("M"))
	{	
		SleepMode(false);
		MotorMode=true;
		digitalWrite(motorSwitch,HIGH);
		int Motorspeed=constrain(Number,0,255);
		
		if (Motorspeed!=0)
		{
			Brake_active();
			forward(Motorspeed);
			last_MotorSpeed=Motorspeed;
		}
		else
		{
			//Brake(last_MotorSpeed);
			//last_MotorSpeed=0;
			Brake();
		}
		
		if(debug&&(MotorMode==true))
		{
			Serial.print("Motorspeed: ");
			Serial.println(Motorspeed);
			Serial.println(("Motormode On."));
			
		}
		else if (debug&&(MotorMode==false))
		{
			Serial.println(("Motormode Off."));
		}
		msg="";
	}
	else if(msg.equals("B"))
	{
		SleepMode(false);
		MotorMode=true;
		digitalWrite(motorSwitch,HIGH);
		int Motorspeed=constrain(Number,0,255);
		
		if (Motorspeed!=0)
		{
			Brake_active();
			backward(Motorspeed);
			last_MotorSpeed=Motorspeed;
		}
		else
		{
			//Brake(last_MotorSpeed);
			//last_MotorSpeed=0;
			Brake();
		}
		msg="";
	}
	else if (msg.equals("R"))
	{
		Brake_release();
	}
	else if (msg.equals("G"))
	{
		switchOff();
		MotorMode=false;
	}
	
msg="";
}
