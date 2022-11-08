//keyestudio Multi-purpose Shield V1 - Keyestudio Wiki
#include <OneWire.h>
#include <IRremote.h>
#include <dht11.h>

dht11 DHT;
#define DHT11_PIN 4
#define sw1 2
#define sw2 3
#define led_pass 12
#define led_fail 13
#define led_R 9
#define led_G 10
#define led_B 11
#define buzzer 5
#define IR 8

long past_time = 0;
long test_start = 0;
int led_switch = 0;
int flag = 0;
int result;
int RECV_PIN = 6;

IRrecv irrecv(RECV_PIN);
decode_results RECV_results;

void setup()
{
    
    pinMode(sw1, INPUT);
    pinMode(sw2, INPUT);
    pinMode(led_pass, OUTPUT);
    pinMode(led_fail, OUTPUT);
    pinMode(led_R, OUTPUT);
    pinMode(led_G, OUTPUT);
    pinMode(led_B, OUTPUT);
    pinMode(buzzer, OUTPUT);
    digitalWrite(buzzer, HIGH);

    Serial.begin(9600);
    irrecv.enableIRIn();

}

void loop()
{   

    
    
    long currunt_time = millis();
    //0.5초마다 스위치 상태 확인
    if(currunt_time - past_time >= 500)
    {   
        past_time = currunt_time;

        int sw1_duration = sw1_time();
        int sw2_duration = sw2_time();

        if(flag == 0){
        //지속 시간 확인
            if((sw1_duration && sw2_duration >= 800) && (sw1_duration && sw2_duration <= 1000))
            {   
                start_buzzer();
                digitalWrite(led_pass, LOW);
                digitalWrite(led_fail, LOW);
            }      

        // 1초 이상이면 테스트 스타트
            if(sw1_duration && sw2_duration >= 1000 )
            { 
                flag = 1;
                led_switch = 0;
                test_start = millis();
            }
        }
        //test 진행

        if(flag == 1)
        {
            int IR_val = digitalRead(IR);
            if(IR_val == HIGH)
            { 
                test();
                if((millis() - test_start >= 20000) && flag == 1) flag = 2;
            }
            else delay(100);
        }
        

        //test 종료      
         if(flag == 2)
        {
            test_end();
            flag = 0;
        }
    }
        
    
}

void test()
{  
    
    if (irrecv.decode(&RECV_results))
     {
       Serial.println(RECV_results.value, HEX);
       irrecv.resume(); // Receive the next value
     }
  
    led_rgb(led_switch);
    led_switch++;    
    if(led_switch == 3) 
    {
        led_switch = 0;
    }
     analog();
     LM35();
     DHT11();
    
    
    int light_val = Light();
    

    if((RECV_results.value == 0xFF30CF  ) && (light_val >= 1)) result = 1;
    else if(( RECV_results.value != 0xFF18E7 ) || (light_val < 1) || (irrecv.decode(&RECV_results) == false) ) result = 2;

}

void test_clear()
{   
    digitalWrite(led_pass, HIGH);
    
}

void test_fail()
{
    digitalWrite(led_fail, HIGH);
}

void test_end()
{
   
    led_switch = 7;
    led_rgb(led_switch);

    if(result == 1) test_clear();
    else test_fail();

    start_buzzer();
    delay(500);
    start_buzzer();
    delay(500);
}



long sw1_time()
{
    static unsigned long start_time = 0;
    static bool state;
    if(digitalRead(sw1) != state)
    {
        state = !state;
        start_time = millis();
    } 
    if(state == LOW)
    {
        return millis() - start_time;
    }
    else
    {
        return 0;
    }
}

long sw2_time()
{
    static unsigned long start_time = 0;
    static bool state;
    if(digitalRead(sw2) != state)
    {
        state = !state;
        start_time = millis();
    } 
    if(state == LOW)
    {
        return millis() - start_time;
    }
    else
    {
        return 0;
    }
}

void start_buzzer()
{
   int i;
   for(i = 0; i < 80; i++){
    digitalWrite(buzzer,LOW);// sound
    delay(1);//delay1ms
    digitalWrite(buzzer,HIGH);//not sound
    delay(1);
   }
}

void led_rgb(int led_switch)
{   
    switch(led_switch)
    {           
        case 0: digitalWrite(led_R, HIGH);
                digitalWrite(led_G, LOW);
                break;
        case 1: digitalWrite(led_R, LOW);
                digitalWrite(led_B, HIGH);
                break;
        case 2: digitalWrite(led_B, LOW);
                digitalWrite(led_G, HIGH);
                break;
        default: digitalWrite(led_G, LOW);
                digitalWrite(led_R, LOW);
                digitalWrite(led_B, LOW);
                break;
    }
}

void test_stop()
{
    flag = 2;
}


void analog()
{
int val;
val=analogRead(A0);
Serial.print("analog :");
Serial.println(val);
}

int Light()
{
int val;
val=analogRead(A1);
Serial.print("Light : ");
Serial.println(val);
return val;
}

void LM35()
{
int val;
val=analogRead(A2);
Serial.print("LM32 : ");
Serial.println(val);
}

void DHT11()
{
int chk;
chk = DHT.read(DHT11_PIN);    // READ DATA
switch (chk)
  {
    case DHTLIB_OK:
                Serial.print("OK : ");
                Serial.println(DHT.humidity,1);
                Serial.print("");
                Serial.println(DHT.temperature,1);
                break;
    case DHTLIB_ERROR_CHECKSUM:
                Serial.print("Checksum error        ");
                break;
    case DHTLIB_ERROR_TIMEOUT:
                Serial.print("Time out error        ");
                break;
    default:
                Serial.print("Unknown error         ");
                break;
  }
}
