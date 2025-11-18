a = 1
b = 0

for i in range(5000):
	tmp = a
	a = a + b
	b = tmp

	print(a)
