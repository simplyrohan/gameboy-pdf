var lines = [];

function print_msg(msg) {
    lines.push(msg);
    if (lines.length > 25)
        lines.shift();

    for (var i = 0; i < lines.length; i++) {
        var row = lines[i];
        globalThis.getField("console_" + (25 - i - 1)).value = row;
    } 
}

function base64ToUint8Array(base64) {
    // Base64 characters
    const base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const base64Map = Object.fromEntries(base64Chars.split("").map((char, index) => [char, index]));

    // Remove padding characters (`=`) and validate input
    const cleanedBase64 = base64.replace(/=+$/, "");
    if (cleanedBase64.length % 4 === 1) {
        throw new Error("Invalid Base64 string");
    }

    // Compute the output buffer length
    const outputLength = Math.floor((cleanedBase64.length * 3) / 4);
    const buffer = new Uint8Array(outputLength);

    // Decode Base64 string
    let bufferIndex = 0;
    for (let i = 0; i < cleanedBase64.length; i += 4) {
        const encoded1 = base64Map[cleanedBase64[i]] ?? 0;
        const encoded2 = base64Map[cleanedBase64[i + 1]] ?? 0;
        const encoded3 = base64Map[cleanedBase64[i + 2]] ?? 0;
        const encoded4 = base64Map[cleanedBase64[i + 3]] ?? 0;

        const byte1 = (encoded1 << 2) | (encoded2 >> 4);
        const byte2 = ((encoded2 & 15) << 4) | (encoded3 >> 2);
        const byte3 = ((encoded3 & 3) << 6) | encoded4;

        buffer[bufferIndex++] = byte1;
        if (bufferIndex < outputLength) buffer[bufferIndex++] = byte2;
        if (bufferIndex < outputLength) buffer[bufferIndex++] = byte3;
    }

    return buffer;
}

function draw_frame(buffer) {
    // app.alert("Draw")
    let framebuffer = Module.HEAPU8.subarray(buffer, buffer + (160 * 144 * 2));
    for (let y = 0; y < 144; y++) {
        let ascii = "";
        for (let x = 0; x < 160; x++) {
            let color = framebuffer[y * 160 + x];

            // if (color != 0) {
            //     app.alert(color)
            // }

            // Convert 565 to rgb
            let r = (color & 0xF800) >> 8;
            let g = (color & 0x07E0) >> 3;
            let b = (color & 0x001F) << 3;

            // Convert rgb to grayscale
            color = (r + g + b) / 3;


            // Convert grayscale to ascii
            if (color > 240) ascii += " ";
            else if (color > 200) ascii += ".";
            else if (color > 160) ascii += ":";
            else if (color > 120) ascii += "o";
            else if (color > 80) ascii += "O";
            else if (color > 40) ascii += "8";
            else ascii += "@";
        }
        globalThis.getField("field_" + (144 - y - 1)).value = ascii;
    }
}


function move_rom_to_fs() {
    var romdata = "__replace_with_rom__";
    var romsize = __replace_with_rom_size__;

    let stream = FS.open("/" + 'rom.gbc', "w+");
    app.alert(romdata)
    const uint8Buffer = base64ToUint8Array(romdata);

    FS.write(stream, uint8Buffer, 0, romdata.length, 0);
    FS.close(stream);

    app.alert("ROM Moved to FS as rom.gbc");
}
