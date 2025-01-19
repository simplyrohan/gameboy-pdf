echo "[BEGIN] Building gnuboypdf with emscripten"
emcc gnuboypdf.c gnuboy/*.c -o build/index.js -sWASM=0 -sSINGLE_FILE=1 -sEXPORTED_FUNCTIONS=[_main,_loop]

echo "Adding glue code \`pdfboy.js\`"
# cat pdfboy.js >> build/index.js # Glue code
cat pdfboy.js build/index.js >> build/tmp # Glue code
mv build/tmp build/index.js
sed -i 's/console.warn.bind(console)/function (...args) {print_msg(...args)}/g' build/index.js # console isn't implemented in PDF JS API's
sed -i 's/console.log.bind(console)/function (...args) {print_msg(...args)}/g' build/index.js # console isn't implemented in PDF JS API's

echo "Templating and converting to PDF"
python3 generate.py

echo "[DONE] PDF written to \`build/final.pdf\`!"