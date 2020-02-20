import os

files = os.listdir("my/inputs")

for file in files:
	os.system("./simulator < my/inputs/" + file + " > my/outputs/" + file)
