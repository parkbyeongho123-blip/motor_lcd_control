#include <IG_30GM.h>

// Initialize the static instance pointer
IG_30GM* IG_30GM::_instance = nullptr;


// Use member initializer list for initialization instead of assignment in the body.
// This is more efficient and the C++ standard way.
IG_30GM::IG_30GM(int pin) : _pin(pin)
{
    // The static pointer is set to this instance.
    _instance = this;
}



IG_30GM::~IG_30GM()
{
}


void IG_30GM::init() // The 'interrupt_input' parameter is removed as we now use _pin from the constructor.
{

    byte error, address;
    int nDevices;
    


    int interrupt_input = _pin; // Use the member variable
    


    Wire.begin();
    Serial.begin(9600);
    

    
    Serial.println("\nI2C Scanner");
    while (!Serial);             // Leonardo: wait for serial monitor
    Serial.println("Scanning...");  



    nDevices = 0;

    for(address = 1; address < 127; address++ ) 
    {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();



        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address<16) 
            Serial.print("0");

            _lcd_addr = address ; 
            Serial.print(_lcd_addr,HEX);
            Serial.println("  !");

            nDevices++;
        }
        else if (error==4) 
        {
            Serial.print("Unknow error at address 0x");
            if (address<16) 
            Serial.print("0");
            Serial.println(address,HEX);
        }    
    }


    if (nDevices == 0)
    Serial.println("No I2C devices found\n");
    else
    Serial.println("done\n");



    delay(1000);
            // wait 1 seconds for next scan


////////////////////////////////



///////////////// lcd 시작


    
    _lcd = new LiquidCrystal_I2C(_lcd_addr, _lcd_cols, _lcd_rows);




    _lcd->begin();
    _lcd->clear();
    _lcd->home();
    _lcd->print("Loading...");

    
    pinMode(interrupt_input , INPUT_PULLUP);
    

    
    Serial.println("RPM Measurement Start");
    Serial.println("인터럽트 설정 완료. 핀 3의 펄스 수를 세고 있습니다.");

    // Attach the static wrapper function as the ISR.
    // The pin is now correctly using the member variable.
    attachInterrupt(digitalPinToInterrupt(interrupt_input), _isr_wrapper, FALLING);


    
    delay(1000); 

}

// This is the static function that the C-style attachInterrupt can call.
void IG_30GM::_isr_wrapper() 
{
  if (_instance) 
  {
    _instance->_interrupt_handler();
  }
}

// This is the actual instance-specific interrupt handler.
void IG_30GM::_interrupt_handler() 
{
  _elapsedTime = micros() - _startTime;
  _startTime = micros();
}



void IG_30GM::lcd_show_rpm()
{



  _lcd->clear();
  _lcd->home();


  _now_pulsecount = (double)1/((float)_elapsedTime/1000000);





  Serial.print("펄스 주기 (마이크로초): ");
  Serial.println(_elapsedTime);

  Serial.print("1초간 펄스 수: ");
  Serial.println(_now_pulsecount);
  _rpm = _now_pulsecount * 60 / (_ENCODER_PPR)/71;

    
  _pulse_array[_pulse_array_count%AVG_COUNT_NUM] = _now_pulsecount;

  _pulse_array_count++;
  

  _pulse_avg = 0 ;

  for(int i =0 ; i <AVG_COUNT_NUM ; i++)
  {
    _pulse_avg += _pulse_array[i];
  }
  _pulse_avg = _pulse_avg / AVG_COUNT_NUM;




  Serial.print("펄스주기 평균 :");    
  Serial.println(_pulse_avg);











  _rpm_array[_rpm_array_count%AVG_COUNT_NUM] = _rpm;
  _rpm_array_count++;


  _rpm_avg = 0 ;
  for(int i =0 ; i <AVG_COUNT_NUM ; i++)
  {
    _rpm_avg += _rpm_array[i];
  } 
  _rpm_avg = _rpm_avg / AVG_COUNT_NUM;



  Serial.print("RPM avg:");      
  Serial.println(_rpm_avg);




  if (_rpm > 0) {
    
    _lcd->print("ENC_PUL:");   
    _lcd->print(_pulse_avg);
    
    _lcd->setCursor(0,1);

    _lcd->print("RPM : ");
    _lcd->print(_rpm_avg);
  }





  
}