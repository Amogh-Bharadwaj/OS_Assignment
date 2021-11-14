f = open("sample.txt", "w")

for i in range(1000000):
    f.write(str(i)+"\n")

f.close()