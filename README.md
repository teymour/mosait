# Compilation

## 1. install dependencies 

    $ sudo apt-get install libfreeimage-dev libjpeg8-dev

## 2. compile

    $ make

#Usage

To be able to create your own mosaic, you have to build an image database with the following command:

    $ mosait -c images/ -o images.db

To create a mosait from src_image.jpg, here is the command:

    $ mosait -d images.db -i src_image.jpg -o my_mosaic.jpg

If you need to create lot of images out of the same image database, you'll probably need to optimize your database and orginize it with centroids. The more centroids, the faster will be the mosait creation, but the result may be slightly less good than without centroids. Here is the command to "optimize" the database using 10 centroids:

    $ mosait -O 10 -d images.db
