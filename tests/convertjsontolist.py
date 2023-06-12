
import os, sys, json

if __name__=='__main__':
	if len(sys.argv) != 3:
		print(f"Usage: {sys.argv[0]} input.json output.bin")
		exit(0)

	with open(sys.argv[1]) as f:
		data = json.load(f)

	o = []
	for key in data.keys():
		if len(key) and len(data[key]):
			o.append(bytes(key, "UTF-8"))
			o.append(bytes(data[key], "UTF-8"))
	
	with open(sys.argv[2], "wb") as f:
		f.write(b"\0".join(o))
