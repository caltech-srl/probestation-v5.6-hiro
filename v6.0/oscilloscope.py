import pyvisa

rm = pyvisa.ResourceManager()

print(rm.list_resources())

OSC = rm.open_resource('USB0::0x0699::0x0373::C000281::INSTR')

print(OSC.query("*IDN?"))


OSC.write("*RST?")