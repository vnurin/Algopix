I am not offering any license for this project, so according copyright lows nobody else can alter, modify or distribute this project without my permission without being at risk of litigation.
Contact info: Vahagn Nurijanyan, vnurin@gmail.com.

This command line program is for converting from / to any (true-colour, indexed-colour, grayscale) bitmap file of BMP format to atchive either row-by-row or whole image compression / decompression. 
#
This is universal (for any kind of images) image compression technique for storing / transmitting the images. The technique and the compressed file format is based on image regularities(1) discovered in the structure of images. The method of image encoding and compression / the regularities are protected by / described in US patent named "System and Methods for Image/Video Compression", Application / Pub Number: US14/533040 / US20160127734A1, Issue date: 04/23/2019, Patent Number: 10271057. 

This code is for compressing / decompressing an image in single 2D (area) or multiple 1D (row) compressible / compressed units respectively.
1D compressible / compressed units are either indepedent, self containing (the first row, using 1 Modular Method) or they need the previous one (the rest rows, using 2,3,4 Modular Methods) for encoding / decoding to have better compression rate.
For 1D compressed units deltas & minDeltas could have common Huffman Table...
For 1D compressed units it is reasonable to try all four Modular Methods & apply the best one...

* "algopix -1 <24 bit bmp file>" compresses the image into multiple (height number) compressed units 
* "algopix -2 <24 bit bmp file>" compresses the image into single compressed unit 
* "algopix <apx file>" decompresses the image into <24 bit bmp file>


Achieved lossless compression rate is always significantly better and compressing /decompressing speed is faster than for JPEG2000 / JPEG-LS or PNG formats (currently the best lossless image compression formats).

Compression statistics for "rainbow" / "rgb-circles" images respectively (APX is an image file format based on the patented technology):

JPEG2000-LS	3,986 / 44,665 bytes	0.48657227 / 0.95401341 bpp
JPEG-LS		41,007 /  bytes		5.0057373 /  bpp
PNG		2,050 / 14,086 bytes	0.25024414 / 0.30086719 bpp
APX		467 / 2,576 bytes	0.05700684 / 0.05502157 bpp

So, compression ratio for these 2 files for the APX is 421.117773 / 436.214285 respectively! APX is 8.54 / 17.34 times smaller than JPEG2000-LS & 15.92 /  times smaller than JPEG-LS & 4.39 / 5.47 times smaller than PNG (see attached images)!

Here are additional advantages of the technology regarding existing (known) lossless ones:

1. Currently different technologies are used for different kind of images. For example, PNG / GIF format (based on LZ77 / LZW generic data compression technique respectively) is good only for simple (graphical) images and  JPEG2000 / JPEG-LS is good only for complex ones (photos) because it uses completely different compression technique called arithmetic coding. There isn’t one technique suitable for all kind of images. My introduced single technique is an image based general technique and it is universally effective for all kinds of images. The single common code (based on that technique) is equally suitable for compressing / decompressing all types of images - photos; grayscale or graphical ones (in spite in the patent application I introduced also different, dedicated techniques to compress grayscale and indexed-colour images). And the code works for both, row by row & whole image compression modes.
Implementing one and only sequence of algorithms in the single code for any kind of images is critical for embedding the technology into chips to occupy less memory.

2. Compression / Decompression algorithms need to have only 2 rows of image in memory at a time to process. They don't require pre-scanning or keeping in operative memory whole image or any 2D array (having Width*Height quantity of items), resulting efficient operative memory requirement. This is critical for embedding the technique into chips for significantly decreasing the size of required operational memory.

3. Compression / Decompression can be done "on the fly", on a row basis to create compressed autonomous units for each line. It makes possible to [compress, ]transmit & decompress (restore) images simultaneously, row by row and faster (without waiting whole image to be transmitted to decompress).  This is critical for embedding the technology into chips for significantly decreasing the size of required operational and storage memories. Per row compression doesn't need storage to keep whole compressed image: compressed row can be transmitted immediately, releasing the storage for the next one.

4. Different partitions of an image can be compressed/decompressed independently and simultaneously, which makes it highly demanded for multiprocessor or multicore systems. Particularly, compressed unit (block) may be a line (1D block) compressed/decompressed independently or it needs only previous line to process, which makes possible to design fast multicore encoder/decoders to compress [and decompress, if lines are encoded by 1st Procoding Method] each line of an image in independent, parallel threads.

(1) The regularities are also interesting from scientific point of view. Patent application represents huge undiscovered material - it consists of 193 pages (156 pages Description and 37 pages Drawings). Compression is one of the many aspects of utilizing discovered regularities in the image structure and I explored that aspect in the patent application, leaving other aspects unexplored. If image is clearer (closer to its’ natural ideal), its’ compression rate is better, and because of advanced cameras are more sensitive, their captured images have less noise and if images would be compressed by this method, their sizes could be smaller. Image capturing devices become more perfect, which makes the compression rate better...  Besides, discovered image properties can be applied for photo correlation to recover missing image data.
