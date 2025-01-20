emcc gnuboy.c gnuboy/*.c -o build/index.js -sSINGLE_FILE=1
cat glue.js build/index.js >> build/tmp
mv build/tmp build/index.js
python embed_file.py rom.gbc build/index.js
# cp index.html build/final.html
