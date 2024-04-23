#! /bin/bash

sanitize_images() {
    find . -type f -exec sed -i '/^4/d' {} \; 
    find . -type f -exec sed -i 's/^3/1/' {} \;
    find . -type f -exec sed -i 's/^5/3/' {} \;
    find . -type f -exec sed -i 's/^6/4/' {} \;
}

cd train/labels/
sanitize_images

cd ../../

cd valid/labels 
sanitize_images