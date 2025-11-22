from PIL import Image
import numpy as np
import ordered_dithering

height, width = 100,100

#img = np.random.randint(0,255,(height,width), dtype=np.uint8)
#img = np.zeros((height,width), dtype=np.uint8)
img = np.full((height,width), 255, dtype=np.uint8)
#img = np.full((height,width), 255, dtype=np.uint8)

binary = ordered_dithering.order_dither(img,4)
vis = ordered_dithering.binary_to_grayscale(binary)

out_img = Image.fromarray(vis)
out_img.show()
