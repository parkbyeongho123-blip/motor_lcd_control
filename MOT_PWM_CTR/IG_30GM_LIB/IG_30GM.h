#ifndef IG_30GM_h
#define IG_30GM_h



#define AVG_COUNT_NUM 20

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>




class IG_30GM
{
private:
    
    int _pin;
    const int _ENCODER_PPR = 13;

    // --- LCD Properties ---
    // These are configured in the init() function
    uint8_t _lcd_addr=0x27; // Will be found by the I2C scanner
    uint8_t _lcd_cols = 16;
    uint8_t _lcd_rows = 2;

    // LCD object will be created dynamically after finding the I2C address
    LiquidCrystal_I2C* _lcd = nullptr;

    // --- Interrupt & RPM Calculation Data ---
    // 'volatile' is crucial here because these are modified by an ISR
    volatile unsigned long _startTime = 0;
    volatile unsigned long _elapsedTime = 0;

    // --- RPM & Pulse Calculation Data ---
    // These are member variables to retain state between calls to lcd_show_rpm()
    double _now_pulsecount = 0.0;
    long double _rpm = 0.0;

    // For moving average of pulse count
    uint8_t _pulse_array_count = 0;
    float _pulse_array[AVG_COUNT_NUM]{}; // Initialize all elements to 0
    float _pulse_avg = 0.0f;

    // For moving average of RPM
    uint8_t _rpm_array_count = 0;
    float _rpm_array[AVG_COUNT_NUM]{}; // Initialize all elements to 0
    float _rpm_avg = 0.0f;
    
    // The ISR function is now a private member
    void _interrupt_handler();

    // Static pointer to the current instance to be used in the static ISR wrapper
    static IG_30GM* _instance;


public:


    
	/**
	 * Constructor
	 *
	 * @param pin	I2C slave address of the LCD display. Most likely printed on the
	 *					LCD circuit board, or look in the supplied LCD documentation.
	 * 
	 */
    IG_30GM(int pin);

    ~IG_30GM();


	/**
	 *
	 * @param interrupt_input	
     * 
     *  -> 펄스로 입력할 핀 넘버
	 */

    void init();

    // Static wrapper function to be passed to attachInterrupt
    static void _isr_wrapper();

    
    void lcd_show_rpm();


};


#endif