Group 8:	Project: Traffic Lights	
	Requirements	
1. System Safety and Reliability		
ID	Requirement	Status
1.1	The system must be powered stably using a 9V adapter regulated to 5VDC via ASM1117	done
1.2	All connections must be properly placed and secured on the breadboard to ensure stable operation	done
		
2. Communication and Data Transmission		
ID	Requirement	Status
2.1	1 Master must use an HC-12 module to wirelessly transmit data to 1 Slave	done
2.2	HC-12 module communicate with STM32 via UART interface	done
2.3	Master and Slave are assigned at channel 88	done
2.4	The communication speed must be at 9600 bps 	done
2.5	The system must maintain stable communication at distances under 50 meters in noisy environments	done
2.6	The HC-12 module must operate at a transmit power of 20 dB (100 mW) to ensure sufficient communication range	done
		
3. Information Display		
		
ID	Requirement	Status
3.1	The system must include a 16x2 I2C LCD to display real time	done
3.2	The LCD must display: real time and mode status	done
3.3	The display must be updated at least every 1 second	done
		
4. Traffic Light Control		
ID	Requirement	Status
4.1	The Master must control Red, Yellow, and Green LEDs using GPIO as well as Slave	done
4.2	In Normal Mode, the Green light must stay on for 18 seconds, Yellow for 3 seconds, and Red for 20 seconds	done
4.4	In Heavy Mode, the Green light must stay on for 45 seconds, Yellow for 5 seconds, and Red for 50 seconds	done
4.5	In Night Mode, the Yellow light blinks every 0,5 second	done
4.6	Control Mode allows traffic light direction change	done
		
5. Real-Time Synchronization		
ID	Requirement	Status
5.1	The system must use a DS3231 module for real-time clock (RTC) functionality	done
5.2	The DS3231 must communicate via I2C, sharing the bus with the LCD	done
5.3	The DS3231 update time every 1 second using interrupt	done
		
6. Operation Mode Switching		
ID	Requirement	Status
6.1	The system must support 5 operation modes controlled via 5 push buttons	done
6.2	Button 1 switches to Normal Mode	done
6.3	Button 2 switches to Heavy Mode	done
6.4	Button 3 switches to Light Mode	done
6.5	Button 4 switches to Control Mode (manual operation)	done
6.6	Button 5 is used to manually toggle traffic light states in Control Mode	done
![image](https://github.com/user-attachments/assets/31178eeb-c6a1-4a0d-af82-550da665985d)
