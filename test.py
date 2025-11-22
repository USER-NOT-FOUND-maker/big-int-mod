from sys import set_int_max_str_digits

set_int_max_str_digits(0)

a = 0
b = 1

for i in range(500):
	a,b = b, a + b
	print(b)
