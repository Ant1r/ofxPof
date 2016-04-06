#include "ofUnicode.h"


//------------------------------------------------------------------
bool ofUnicode::isValid(const ofUniChar& unichar) {
    return !((unichar > 0x0010ffffu) || (unichar >= 0xd800u && unichar <= 0xdfffu));
}

//------------------------------------------------------------------
bool ofUnicode::isPrintable(const ofUniChar& unichar) {
    Poco::Unicode::CharacterProperties props;
	Poco::Unicode::properties(unichar, props);
    
    return props.type != Poco::Unicode::UCP_CONTROL;
}

//------------------------------------------------------------------
bool ofUnicode::isCntrl(const ofUniChar& unichar) {
    // True if unichar is:
    //  - ISO 8-bit control character (U+0000..U+001f and U+007f..U+009f)
    //  - UCP_CONTROL
    //  - UCP_FORMAT (Cf)
    //  - UCP_LINE_SEPARATOR
    //  - UCP_PARAGRAPH_SEPARATOR
    // This definition shared with ICU's isCntrl function 
    // http://icu-project.org/apiref/icu4c/uchar_8h.html#a1295bd387a68fe6df79fedce367c18dd
    

    Poco::Unicode::CharacterProperties props;
	Poco::Unicode::properties(unichar, props);
    
    return props.type == Poco::Unicode::UCP_CONTROL              ||
           props.type == Poco::Unicode::UCP_FORMAT               ||
           props.type == Poco::Unicode::UCP_LINE_SEPARATOR       ||
           props.type == Poco::Unicode::UCP_PARAGRAPH_SEPARATOR;
    
}

//------------------------------------------------------------------
bool ofUnicode::isTitle(const ofUniChar& unichar) {
    Poco::Unicode::CharacterProperties props;
	Poco::Unicode::properties(unichar, props);
    
    return props.type == Poco::Unicode::UCP_TITLE_CASE_LETTER;
}

//------------------------------------------------------------------
bool ofUnicode::isSpace(const ofUniChar& unichar) {
    return Poco::Unicode::isSpace(unichar);
}

//------------------------------------------------------------------
bool ofUnicode::isDigit(const ofUniChar& unichar) {
    return Poco::Unicode::isDigit(unichar);
}

//------------------------------------------------------------------
bool ofUnicode::isPunct(const ofUniChar& unichar) {
    return Poco::Unicode::isPunct(unichar);
}

//------------------------------------------------------------------
bool ofUnicode::isAlpha(const ofUniChar& unichar) {
    return Poco::Unicode::isAlpha(unichar);
}

//------------------------------------------------------------------
bool ofUnicode::isAlphaNumeric(const ofUniChar& unichar) {
    return isAlpha(unichar) || isDigit(unichar);
}

//------------------------------------------------------------------
bool ofUnicode::isLower(const ofUniChar& unichar) {
    return Poco::Unicode::isLower(unichar);
}

//------------------------------------------------------------------
bool ofUnicode::isUpper(const ofUniChar& unichar) {
    return Poco::Unicode::isUpper(unichar);
}

//------------------------------------------------------------------
ofUniChar ofUnicode::toLower(const ofUniChar& unichar) {
    return Poco::Unicode::toLower(unichar);
}

//------------------------------------------------------------------
ofUniChar ofUnicode::toUpper(const ofUniChar& unichar) {
    return Poco::Unicode::toUpper(unichar);
}

//------------------------------------------------------------------
ofUniChar& ofUnicode::toLowerInPlace(ofUniChar& unichar) { 
    unichar = toLower(unichar); 
    return unichar;
}
//------------------------------------------------------------------
ofUniChar& ofUnicode::toUpperInPlace(ofUniChar& unichar) { 
    unichar = toUpper(unichar); 
    return unichar;
}

//------------------------------------------------------------------
ofUniString ofUnicode::toLower(ofUniString& unichar) {
    ofUniString out;
    ofUniString::iterator it = unichar.begin();
    while(it != unichar.end()) {out.push_back(toLower(*(it++)));}
    return out;
}

//------------------------------------------------------------------
ofUniString ofUnicode::toUpper(ofUniString& unichar) {
    ofUniString out;
    ofUniString::iterator it = unichar.begin();
    while(it != unichar.end()) {out.push_back(toUpper(*(it++)));}
    return out;
}

//------------------------------------------------------------------
ofUniString& ofUnicode::toLowerInPlace(ofUniString& unichar) {
    ofUniString::iterator it = unichar.begin();
    while(it != unichar.end()) {toUpperInPlace(*(it++));}
    return unichar;
}

//------------------------------------------------------------------
ofUniString& ofUnicode::toUpperInPlace(ofUniString& unichar) {
    ofUniString::iterator it = unichar.begin();
    while(it != unichar.end()) {toLowerInPlace(*(it++));}
    return unichar;
}


