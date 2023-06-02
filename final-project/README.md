# Mini Lightsaber
Embedded Systems Programming;  
University of Denver, ENCE 3231

### Project Idea: 
Build a mini lightsaber using an IMU, LED, and speaker

### Project Requirements: 
- Design a PCB shield for the STM32F407G Discovery Board 
- Build and test a prototype circuit

### Components included: 
- GY-521 Inertial Measurement Unit
- 4 pin RGB LED
- Speaker (connected with aux cord) 
- USB to microUSB adapter

### Folder Structure: 
- Audio Files: lightsaber .wav files 
- Blad Design: designs for the mini blade 
- Hardware Documents: datasheets and other documentation 
- Notes: pictures, videos, presentations, etc. 
- PCB Design: the kiCAD folder as well as gerber, schematics, BOM, etc. 
- STM32 Code: all programs used to test components and build the system 

### Status/Summary:   
Followed "Mutex Embedded" tutorials on youtube to help set up the IMU and play audio files.
IMU and LED test were successful and were successfully tested together (video in notes).
No luck playing an audio file from the USB drive and so no build has combined all three functions yet.

![image](https://github.com/zbayler/ENCE_3221_Class2023/assets/130094926/2c3f363a-7b69-49ff-ab18-70b6b429e39c)
