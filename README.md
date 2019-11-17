# Torpedo Mk I

In this folder you can find files with the program and schematics of the control board for a 3D printed torpedo. You can find it for download at Thingiverse:

https://www.thingiverse.com/thing:3944261

The electronics is very simple, and it is based on Arduino. It uses a Reed switch for arming a firing the torpedo. This is a magnetically activated swicth. As it is located inside the torpedo, you may need a magnet with a minimum magnetic field. We use old hard drive magnets, which are quite powerful.

Once build and programmed, the electronics is very simple to use. You have to arm the torpedo first by exciting the Reed switch. Then, for firing, you hace to excite it for a second time. For aborintg, you might want to excite it for a third time, while the motor is running. The motor activation time is sotred in the Arduino's EEPROM. While in any of the previous system states (arming, firing, waiting) the system shuts down automatically after a period of 60 secons of inactivity.

The arming and firing can be oeprated in two different ways:

A) Firing for a given amount of time. Arm the torpedo by placing and holding a magnet for the initialisation time (1 sec) plus the firing time without removing the magnet. The systems gives feedback at the beginning, after the initialisation time, and at the end of the process. The feedback is via a LED light and a very small activation of the motor. After removal of the magnet the firing time is stored in the EEPROM and can be used in subsequent time. Place the magnet a second time to fire it.

B) Firing for a pre-programmed amount of time. Arm the torpedo by placing and holding a magnet for a period shorter than the initialisation time (1 sec) without removing the magnet. The systems gives feedback at the beginning and the end. Place the magnet a second time to fire it.
