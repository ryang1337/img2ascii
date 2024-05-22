# img2ascii

A simple C program to convert png images to ascii art.

## Usage
Compile using cmake
```
cmake --build build
```

Run the program with the following parameters
```
./build/img2ascii <input_file> <output_file> <output_ascii_image_file> <downsample_height_factor> <downsample_width_factor>

e.g.
./build/img2ascii cat.png cat_modified.png cat_ascii.png 12 6
```

## Requirements
```
libpng 
zlib
```

## Notes
You may need to modify CMakeLists.txt if your libpng and zlib are in different locations.

You can provide `downsample_height_factor` and `downsample_width_factor` to resize the image by those factors.
Since text doesn't take up square spaces, you can provide different values to ensure the resulting
ascii image matches more closely with the original proportions of the image. A height:width ratio of
2:1 is usually good enough.

`output_file` contains the image after transforming it into grayscale and resizing it according to
the factors.
