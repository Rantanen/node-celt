node-celt
=========
### NodeJS native bindings to libcelt

This module implements bindings for Celt v0.7.0 for Node.js.

    celt = require('node-celt');

    // Create the encoder.
    // Specify 48kHz sampling rate and 10ms frame size.
    // NOTE: The decoder must use the same values when decoding the packets.
    var rate = 48000;
    var frame_size = rate/100;
    var encoder = new celt.CeltEncoder( rate );

    // Encode and decode.
    var encoded = encoder.encode( buffer );
    var decoded = encoder.decode( encoded );

Platform support
----------------

Supported platforms:
- Linux x64
- Darwin x64 (mac osx)
- FreeBSD x64

Add new supported platforms by running ./configure in deps/celt-0.7.1 and
copying the resulting config.h to deps/config/celt-0.7.1/<os>/<arch>.

Use the following flags: --enable-static --disable-shared --with-pic

