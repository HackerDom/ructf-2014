import subprocess
import random
import os

x = 0
g = 0
q = 0
a = 1
b = 1
Ha = 2
Hb = 2
hi = 1 #0-7
nu = 2 #0-7
MOD_N = 255

def make_flash(hi, nu):


def RotL(value, shift):
    return ((value << shift) | (value >> (8 - shift))) & MOD_N

def mul8(a, b):
	result = 0
	while a and b:
		if a & 1:
			result += b;
		a >>= 1
		b = (b << 1) & 255;
	return result & 255;

def random_number():
	global x,g,q,a,b,Ha,Hb,hi,nu,MOD_N
	z = g
	r = q
	q = z ^ RotL(x, hi)
	g = r ^ RotL(z, nu)
	x = (mul8(a,z) + b) & MOD_N
	a += Ha
	b += Hb
	current_byte = "%X" % (r & 15)
	return current_byte

def generate_flags(n):
	result = []
	for i in xrange(n):
		s = 'RuCTF_'
		for j in xrange(25):
			s += random_number()
		result.append(s+'=')
	assert len(result) == len(set(result))
	return result

def generate_flags_for_seed(hhi,hnu):
	global x,g,q,a,b,Ha,Hb,hi,nu,MOD_N
	x = 0
	g = 0
	q = 0
	a = 1
	b = 1
	Ha = 2
	Hb = 2
	hi = hhi
	nu = hnu
	open("flags_%d_%d.txt" % (hhi, hnu), 'wb').write('\n'.join(generate_flags(2000)))

def main():
	while True:
		hii = random.randint(1,7)
		nui = random.randint(1,7)
		if not os.path.exists("flags_%d_%d.txt" % (hii,nui)):
			break
	try:
		print 'Compiling with seed %d:%d' % (hhi, hnu)
		subprocess.check_output(['avr-gcc','-Wall','-Os','-DF_CPU=9600000','-Drn_hi=%d' % hhi,'-Drn_nu=%d' % hnu,'-mmcu=attiny13a','-c','main.c','-o','main.o'],stderr=subprocess.STDOUT).split('\n')
		print 'Linking'
		subprocess.check_output(['avr-gcc','-Wall','-Os','-DF_CPU=9600000','-mmcu=attiny13a','-o','main.elf','main.o'],stderr=subprocess.STDOUT).split('\n')
		print 'Cleaning previous hex'
		subprocess.check_output(['rm','-f','main.hex'],stderr=subprocess.STDOUT).split('\n')
		print 'Making new hex'
		subprocess.check_output(['avr-objcopy','-j','.text','-j','.data','-O','ihex','main.elf','main.hex'],stderr=subprocess.STDOUT).split('\n')
		data = subprocess.check_output(['ls', '-la', 'main.hex'])
		print data
		print 'Creating team hex copy'
		data = subprocess.check_output(['cp', 'main.hex', '%d_%d.hex' % (hhi, hnu)])
		print 'Flashing hex'
		data = subprocess.check_output(['avrdude','-c','arduino','-P','/dev/tty.usbmodem1421','-b','19200','-p','t13','-U','flash:w:main.hex:i'],stderr=subprocess.STDOUT).split('\n')
		print 'Writing fuses to increase controller frequency'
		data = subprocess.check_output(['avrdude','-c','arduino','-P','/dev/tty.usbmodem1421','-b','19200','-p','t13','-U','lfuse:w:0x75:m','-U','hfuse:w:0xff:m'],stderr=subprocess.STDOUT).split('\n')
		print 'Generating flags with seed %d:%d' % (hhi, hnu)
		generate_flags_for_seed(hhi, hnu)
		print 'Done'
	except subprocess.CalledProcessError, e:
		print "Flash failed:"
		print e.output

if __name__ == '__main__':
	main()