from PIL import Image

# Create font sprites
fontBuf = []
fontImg = Image.open("assets/font-8x12.png").convert("RGBA")

CHAR_W = 8
CHAR_H = 12

def addCharRow(data, offset):
	x = 0
	for i in range(8):
		if i != 0:
			x <<= 1
		if data[offset + i][0]:
			x |= 1
	fontBuf.append(x)

for y in range(8):
	for x in range(16):
		charImg = fontImg.crop((x * CHAR_W, y * CHAR_H, x * CHAR_W + CHAR_W, y * CHAR_H + CHAR_H))
		data = list(charImg.getdata())
		for dy in range(CHAR_H):
			addCharRow(data, dy * CHAR_W)

# Create player sprite
playerBuf = []
playerImg = Image.open("assets/player.png").convert("RGBA")
playerImgData = list(playerImg.getdata())

def convPlayerPixel(px):
	r, g, b, a = px
	if a == 0:
		return 0
	elif g == 255:
		return 1
	elif g == 102:
		return 2
	elif g == 204:
		return 3
	elif r == 0:
		return 4
	elif r == 255:
		return 5
	elif r == 128:
		return 6
	raise Exception("Unexpected color (%d, %d, %d, %d)" % (r, g, b, a))

def addPlayerPixelPair(data, offset):
	p1 = convPlayerPixel(data[offset])
	p2 = convPlayerPixel(data[offset + 1])
	playerBuf.append(p1 | (p2 << 4))

for i in range(int(len(playerImgData) / 2)):
	addPlayerPixelPair(playerImgData, i * 2)

# Write them to files

cf = open("src/data.c", "w")
hf = open("src/data.h", "w")
cf.write("/* Generated file */\n\n")
hf.write("/* Generated file */\n\n")

def writeArray(name, buffer, itemsPerLine):
	count = len(buffer)
	defName = "unsigned char %s[%d]" % (name, count)
	cf.write(defName + " = {")
	hf.write("extern " + defName + ";\n")
	for i in range(count):
		if i == count - 1:
			cf.write(" 0x%02x\n" % buffer[i])
		elif (i % itemsPerLine == 0):
			cf.write("\n\t0x%02x," % buffer[i])
		else:
			cf.write(" 0x%02x," % buffer[i])
	cf.write("};\n")

writeArray("FONT_BITMAP", fontBuf, CHAR_H)
cf.write("\n")
writeArray("PLAYER_BITMAP", playerBuf, 12)

cf.close()
hf.close()
