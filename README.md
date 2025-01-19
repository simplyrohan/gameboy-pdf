# gameboy-pdf
A port of GNUBoy inspired by doompdf

# Workflow
 - Using emscripten compile a JS Module that drives GNUBoy (plus interaction functions to get and set data)
 - In the same JS file, glue the GNUBoy scripts with [the PDF APIs](https://opensource.adobe.com/dc-acrobat-sdk-docs/library/jsapiref/JS_API_AcroJS.html) (glue code is in `pdfboy.js`)
 - Using `generate.py` with reference to the [PDF spec](https://opensource.adobe.com/dc-acrobat-sdk-docs/pdfstandards/PDF32000_2008.pdf) create the front-end and link the glued JS file along with hooks on all buttons, inputs, and outputs