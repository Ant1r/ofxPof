#include "ofTextConverter.h"


// Like any data, text on a computer is represented by bytes.
// A byte is usually 8 bits and represents the smallest grouping
// of bits that we generally work with.
// 
// Text is representing in a computer by mapping glyphs (or other 
// (control characters), etc to a single integer >= 0.
// 
// The mapping from glyphs to to numbers is called an encoding.
// Many encodings have common mappings (such as ASCII and UNICODE).
// While one encoding may map "ß" to one number, a differnt encoding 
// may represent the "ß" using a different number.
// 
// Unicode is a character mapping that aims to be the standard mapping
// between a glyph (or other control character) and an integer >= 0.
//
// For example:
//  In Unicode, "ß" is represented by the the HEX number 0x00DF (or decimal 
//  number 223).
// 
//  In Mac Roman Encoding, "ß" is represented by the the HEX number 0x00A7 
//  (or decimal number 167)
//
//  By contrast, Mac Roman Encoding represents "ﬂ" (a ligature) with 
//  the HEX number 0x00DF.  Further, Unicode represents "ﬂ" at HEX number
//  0xFB02.
//
// The reason for these different encodings was because many of the older
// systems wanted to ONLY use one byte to represent the most important
// glyphs (and control characters) for their application.  On a modern
// computer, it is no problem to use multiple bytes to represent large numbers,
// but on limited hardware (think, thermal printers), the computational
// overhead of interpreting multi-byte characters was/is more difficult
// than simply agreeing on a common mapping up before reading or writing bytes.
//
// Thus, the first task is to make sure the character mapping between
// a glyph (or other control character) is understood.  Generally,
// the goal is to ultimately use a Unicode character map.
//
// In the following code, the "convert" function (backed by Poco::TextConverter)
// is in charge of remapping one encoding to another.  But, not only
// does it remap characters to new code points, it can also decide how the
// bytes (groups of 8 bits) will be arranged in order to represent glyph
// code points that are too big to fit inside a single byte.  For instance
// the maximum number that can be represented by an unsigned (8-bit) byte is
// 255.  The convert function is limited to storing each code point
// as a series of unsigned chars (which is underneath the std::string object).
//
// Thus, even though UTF16 is usually represented using a _larger_ datatype
// (i.e. wchar -- which can be similar to an unsigned short int), it 
// breaks up the UTF16 into a series of 8 bit bytes.
//
// UTF8, UTF16 and UTF32 are all ways to represent the numbers that represent
// the unicode number for glyphs (and control characters).
// 
// UTF8 uses variable length clusters of 8bit bytes to represent increasingly
// larger unicode numbers.  The first byte in these clusters is either 
// interpreted directly (for code points betwen 0-127) or tells reveals
// how many subsequent bytes will have to be read to get the full number.
//
// UTF16 uses varible length clusters of 16 bits units (2 bytes) to represent
// increasingly larger unicode numbers.  Like UTF8, the first 16 bit unit
// is used to reveal wheather a second 16 bit unit will have to be read
// to get the full number.
// 
// UTF16 is most easily represented by a std::wstring, which is backed by 
// an array of "wchar_t", which is a data type comprised of at least 2 bytes
// and interpreted as a unit.  Alternatively, it can be represented in a 
// std::string as 2 or 4 unsigned chars (i.e. a bytes).  The convert function
// below allows us to convert std::strings that use this encoding into
// a more easily manipulated UTF8 4 byte format.
//
// Generally, it is easier to use the std::wstring to represent UTF16 
// sequences.
// 
// UTF32 is a direct representation of a Unicode code point.  It is backed
// by an unsigned int, which can represent very large values > 0.
//
// In this library, we attempt to differentiate between these different
// schemes by explicitly creating aliases for different string types.
// For instance, an ofUTF8String is simply an alias for std:string.  The
// reason for this alias is simply to be clear what kind of encoding is 
// being used, since a string could have its bytes in any number of ways.
//
// An ofUTF8String is an alias for the std:wstring.
//
// An ofUTF32String is an alias for a basic_string<ofUniChar>, which is a
// string string that is backed by an unsigned int.  Unlike UTF16, which
// has its own dedicated string type (std::wstring), there is no
// such 32bit Unicode String.  There will be in with c++0x.
//
// For more on that, check out this.
// http://members.shaw.ca/akochoi/articles/unicode-processing-c++0x/index.html
// in c++0x, the following is defined:
// typedef string u8string;
// typedef basic_string<unsigned int> u16string;
// typedef basic_string<unsigned int> u32string;
// since it isn't available (easily) yet, we'll just use our own
// there will also be a coverter codecvt class th convert between these
// rather than using POCO or UTF8CPP.  For now, we work around it.
// POCO will support UTF32 functions soon too:
// http://sourceforge.net/apps/trac/poco/milestone/Release%201.5.0
// 
// Anyway, good luck!


////------------------------------------------------------------------
//string ofTextConverter::convert(const ofBuffer& buffer, ofTextEncoding inputEncoding, ofTextEncoding outputEncoding) {
//    return convert(buffer.getBinaryBuffer(), buffer.size(), inputEncoding, outputEncoding);
//}
//
////------------------------------------------------------------------
//string ofTextConverter::convert(const void* source, int length,  ofTextEncoding inputEncoding, ofTextEncoding outputEncoding) {
//    string output;
//    Poco::TextEncoding& ie = getTextEncoding(inputEncoding);
//    Poco::TextEncoding& oe = getTextEncoding(outputEncoding);
//    Poco::TextConverter converter(ie,oe);
//    converter.convert(source, length, output);
//    return output;
//}
//
////------------------------------------------------------------------
//string ofTextConverter::convert(const string& input, ofTextEncoding inputEncoding, ofTextEncoding outputEncoding) {
//    // this returns a std::string -- which is just a sequence of bytes.
//    // the way those bytes is interpreted is up to the user.
//
//    // pass through
//    if(inputEncoding == outputEncoding) return input;
//        
//    string output;
//    Poco::TextEncoding& ie = getTextEncoding(inputEncoding);
//    Poco::TextEncoding& oe = getTextEncoding(outputEncoding);
//    Poco::TextConverter converter(ie,oe);
//    converter.convert(input, output);
//    return output;
//}


// to UTF8
//------------------------------------------------------------------
ofUTF8String ofTextConverter::toUTF8(const ofUTF16String& input) {
    ofUTF8String utf8result;
    try {
        utf8::utf16to8(input.begin(),input.end(), back_inserter(utf8result));
    } catch(const utf8::exception& utfcpp_ex) {
        ofLog(OF_LOG_ERROR, "ofTextConverter::toUTF8 : " + ofToString(utfcpp_ex.what()));
    }
    
    return utf8result;
}

//------------------------------------------------------------------
ofUTF8String ofTextConverter::toUTF8(const ofUniChar& input) {
    
//    ofUTF8String utf8result;
//    try {
//        utf8::utf32to8(&input,&input, back_inserter(utf8result));
//    } catch(const utf8::exception& utfcpp_ex) {
//        ofLog(OF_LOG_ERROR, "ofTextConverter::toUTF8 : " + ofToString(utfcpp_ex.what()));
//    }
//    
//    return utf8result;

    
    
    ofUTF8String txt;
    try {
        utf8::append(input, back_inserter(txt));
    } catch(const utf8::exception& utfcpp_ex) {
        string err = utfcpp_ex.what();
        ofLog(OF_LOG_ERROR, "ofUTF8::append : " + err);
    }
    return txt;
}
//------------------------------------------------------------------
ofUTF8String ofTextConverter::toUTF8(const ofUniString& input) {
    ofUTF8String utf8result;    
    try {
        utf8::utf32to8(input.begin(),input.end(), back_inserter(utf8result));
    } catch(const utf8::exception& utfcpp_ex) {
        ofLog(OF_LOG_ERROR, "ofTextConverter::toUniChar::toUniString : " + ofToString(utfcpp_ex.what()));
    }
    return utf8result;
    
//    ofUTF8String utf8string;
//    for (int i = 0; i < (int)input.size(); i++) utf8string += toUTF8(input[i]);
//    return utf8string;
}

//// to UTF16
////------------------------------------------------------------------
//ofUTF16String ofTextConverter::toUTF16(const ofUTF8String& input) {
//    ofUTF16String utf16result;    
//    try {
//        utf8::utf32to8(&input,&input, back_inserter(utf16result));
//    } catch(const utf8::exception& utfcpp_ex) {
//        ofLog(OF_LOG_ERROR, "ofTextConverter::toUniChar::toUniString : " + ofToString(utfcpp_ex.what()));
//    }
//    return utf16result;
//}
//
////------------------------------------------------------------------
//ofUTF16String ofTextConverter::toUTF16(const ofUniChar& input) {
//    ofUTF16String out;    
//    //Poco::UnicodeConverter::toUTF16(toUTF8(input), out); // well ... in the absence of a real UTF16 class.
//    return out;
//}
////------------------------------------------------------------------
//ofUTF16String ofTextConverter::toUTF16(const ofUniString& input) {
//    ofUTF16String out;    
//    //Poco::UnicodeConverter::toUTF16(toUTF8(input), out); // well ... in the absence of a real UTF16 class.
//    return out;
//}

// to UTF32 / Unicode
//------------------------------------------------------------------
ofUniString ofTextConverter::toUTF32(const ofUTF8String& input) {
    ofUniString utf32result;
    try {
        utf8::utf8to32(ofUTF8::beginPtr(input),ofUTF8::endPtr(input), back_inserter(utf32result));
    } catch(const utf8::exception& utfcpp_ex) {
        ofLog(OF_LOG_ERROR, "ofTextConverter::toUniChar::toUniString : " + ofToString(utfcpp_ex.what()));
    }
    
    return utf32result;
}
//------------------------------------------------------------------
ofUniString ofTextConverter::toUTF32(const ofUTF16String& input) {
    return toUTF32(toUTF8(input));
}

//------------------------------------------------------------------
ofUniString ofTextConverter::toUnicode(const ofUTF8String& input) {return toUTF32(input);}
//------------------------------------------------------------------
ofUniString ofTextConverter::toUnicode(const ofUTF16String& input) {return toUTF32(input);}

//
////------------------------------------------------------------------
//Poco::TextEncoding& ofTextConverter::getTextEncoding(ofTextEncoding enc) {
//    if(enc == OF_TEXT_ENCODING_UTF8) {
//        static Poco::UTF8Encoding utf8_enc;
//        return utf8_enc;
//    } else if(enc == OF_TEXT_ENCODING_UTF16) {
//        static Poco::UTF16Encoding utf16_enc;
//        return utf16_enc;
//    } else if(enc == OF_TEXT_ENCODING_ASCII) {
//        static Poco::ASCIIEncoding ascii_enc;
//        return ascii_enc;
//    } else if(enc == OF_TEXT_ENCODING_LATIN_1) {
//        static Poco::Latin1Encoding latin1_enc;
//        return latin1_enc;
//    } else if(enc == OF_TEXT_ENCODING_LATIN_9) {
//        static Poco::Latin9Encoding latin9_enc;
//        return latin9_enc;
//    } else if(enc == OF_TEXT_ENCODING_WINDOWS_1252) {
//        static Poco::Windows1252Encoding windows1252_enc;
//        return windows1252_enc;
//    } else {
//        ofLog(OF_LOG_WARNING, "ofTextConverter::getEncoding - unknown encoding, returning utf8.");
//        static Poco::UTF8Encoding utf8_enc;
//        return utf8_enc;
//    }
//}


