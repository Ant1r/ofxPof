//
//  ofUnicodeConverter.h
//  ofxUnicodeExample
//
//  Created by Christopher Baker on 5/25/12.
//  Copyright (c) 2012 School of the Art Institute of Chicago. All rights reserved.
//

#pragma once

#include "utf8.h"
#include "ofUTF8.h"
#include "ofUnicode.h"

//#include "ofUnicodeConverter.h" // for some reason, Poco/UnicodeConverter 
                                // is excluded in the os x/linux version
                                // the header guard should keep us safe.

/*
enum ofTextEncoding {
    OF_TEXT_ENCODING_UTF8 = 0        , // variable width encoding (http://en.wikipedia.org/wiki/UTF-8) backward compatible  w/ ASCII
    OF_TEXT_ENCODING_UTF16           , // 16-bit multi-byte (http://en.wikipedia.org/wiki/UTF-16)
    OF_TEXT_ENCODING_ASCII           , // 7-bit ASCII text encoding (http://en.wikipedia.org/wiki/ASCII)
    OF_TEXT_ENCODING_LATIN_1         , // 8-bit single-byte - (http://en.wikipedia.org/wiki/ISO/IEC_8859-1)
    OF_TEXT_ENCODING_LATIN_9         , // 8-bit single-byte - (http://en.wikipedia.org/wiki/ISO/IEC_8859-15), western chars + €
    OF_TEXT_ENCODING_WINDOWS_1252      // Superset of Latin 1 (ISO 8859-1) http://en.wikipedia.org/wiki/Windows-1252
};
*/

class ofTextConverter {
public:
    // convert between string encodings
//    static string        convert(const ofBuffer& buffer, ofTextEncoding inputEncoding, ofTextEncoding outputEncoding);
//    static string        convert(const void* source, int length,  ofTextEncoding inputEncoding, ofTextEncoding outputEncoding);
//    static string        convert(const string& input, ofTextEncoding inputEncoding, ofTextEncoding outputEncoding);

    // to UTF8
    static ofUTF8String  toUTF8(const ofUTF16String& input);  // utf16-utf8
    
    static ofUTF8String  toUTF8(const ofUniChar& input);      // utf32-utf8
    static ofUTF8String  toUTF8(const ofUniString& input);    // utf32-utf8
    
//    // to UTF16
//    static ofUTF16String toUTF16(const ofUTF8String& input); // utf8->utf16
//    
//    static ofUTF16String toUTF16(const ofUniChar& input);    // utf32->utf16
//    static ofUTF16String toUTF16(const ofUniString& input);  // utf32->utf16

    // to UTF32 / Unicode
    static ofUniString   toUTF32(const ofUTF8String& input);   // utf8->utf32
    static ofUniString   toUTF32(const ofUTF16String& input);  // utf16->utf32

    // to UTF32 / Unicode
    static ofUniString   toUnicode(const ofUTF8String& input);   // utf8->utf32
    static ofUniString   toUnicode(const ofUTF16String& input);  // utf16->utf32

    
    
//    static Poco::UTF8Encoding        utf8_enc;

   // static Poco::TextEncoding& getTextEncoding(ofTextEncoding enc);

};