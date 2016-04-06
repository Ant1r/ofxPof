#include "ofUTF8.h"

//------------------------------------------------------------------
bool ofUTF8::isValid(ofUTF8String txt) {
    try {
        ofUTF8String::iterator end_it = utf8::find_invalid(txt.begin(), txt.end());
        if(end_it != txt.end()) {
            ofUTF8String validPart(txt.begin(),end_it);
            ofLog(OF_LOG_ERROR,"ofUTF8::isValid - detected invalid UTF8 - valid part: " + validPart);
            return false;
        } else {
            return true;
        }
    } catch(const utf8::exception& utfcpp_ex) {
        string err = utfcpp_ex.what();
        ofLog(OF_LOG_ERROR, "ofUTF8::isValid : " + err);
        return false;
    }
}

//------------------------------------------------------------------
bool ofUTF8::isValid(ofUTF8Ptr iter, ofUTF8Ptr end) {
    try {
        bool isValid = utf8::is_valid(iter,end);
        if(!isValid) {
            ofLog(OF_LOG_ERROR,"ofUTF8::isValid - detected invalid UTF8");
            return false;
        } else {
            return true;
        }
    } catch(const utf8::exception& utfcpp_ex) {
        ofLog(OF_LOG_ERROR, "ofUTF8::isValid : " + ofToString(utfcpp_ex.what()));
        return false;
    }

}

//------------------------------------------------------------------
bool ofUTF8::startsWithBOM(ofUTF8String txt) {
    return startsWithBOM(beginPtr(txt),endPtr(txt));
}

//------------------------------------------------------------------
bool ofUTF8::startsWithBOM(ofUTF8Ptr iter, ofUTF8Ptr end) {
    return utf8::starts_with_bom(iter,end);
}

//------------------------------------------------------------------
ofUTF8String ofUTF8::repair(ofUTF8String txt, ofUniChar replacement) {
    ofUTF8String temp;
    try {
        if(replacement == -1) {
            utf8::replace_invalid(txt.begin(), txt.end(), back_inserter(temp));
        } else {
            utf8::replace_invalid(txt.begin(), txt.end(), back_inserter(temp), replacement);
        }
    } catch(const utf8::exception& utfcpp_ex) {
        ofLog(OF_LOG_ERROR, "ofUTF8::repair : " + ofToString(utfcpp_ex.what()));
    }
    return temp;
}

//------------------------------------------------------------------
ofUTF8String& ofUTF8::repairInPlace(ofUTF8String& txt, ofUniChar replacement) {
    txt = repair(txt,replacement);
    return txt;
}
//------------------------------------------------------------------
int ofUTF8::distance(ofUTF8String txt) {
    try {
        return utf8::distance(txt.begin(), txt.end());
    } catch(const utf8::exception& utfcpp_ex) {
        ofLog(OF_LOG_ERROR, "ofUTF8::distance : " + ofToString(utfcpp_ex.what()));
    }
}

//------------------------------------------------------------------
int ofUTF8::distance(ofUTF8Ptr iter, ofUTF8Ptr end) {
    try {
        return utf8::distance(iter, end);
    } catch(const utf8::exception& utfcpp_ex) {
        ofLog(OF_LOG_ERROR, "ofUTF8::distance : " + ofToString(utfcpp_ex.what()));
    }
}

//------------------------------------------------------------------
int ofUTF8::icompare(const ofUTF8String& utf8String0, const ofUTF8String& utf8String1) {
    return Poco::UTF8::icompare(utf8String0,utf8String1);
}

//------------------------------------------------------------------
ofUTF8String ofUTF8::toUpper(const ofUTF8String& str) {
    return Poco::UTF8::toUpper(str);
}

//------------------------------------------------------------------
ofUTF8String& ofUTF8::toUpperInPlace(ofUTF8String& str) {
    return Poco::UTF8::toUpperInPlace(str);
}

//------------------------------------------------------------------
ofUTF8String ofUTF8::toLower(const ofUTF8String& str) {
    return Poco::UTF8::toLower(str);
}

//------------------------------------------------------------------
ofUTF8String& ofUTF8::toLowerInPlace(ofUTF8String& str) {
    return Poco::UTF8::toLowerInPlace(str);
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::beginPtr(const ofUTF8String& input) {
    return input.c_str();
}
//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::endPtr(const ofUTF8String& input) {
    return input.c_str() + input.length();
}
//------------------------------------------------------------------
ofxUTF8Iterator ofUTF8::iterator(const ofUTF8String& input) {
    return ofxUTF8Iterator(beginPtr(input),beginPtr(input),endPtr(input));
}

//------------------------------------------------------------------
ofxUTF8Iterator ofUTF8::begin(const ofUTF8String& input) {
    return iterator(input);
}

//------------------------------------------------------------------
ofxUTF8Iterator ofUTF8::end(const ofUTF8String& input) {
    return ofxUTF8Iterator(endPtr(input),beginPtr(input),endPtr(input));
}


//------------------------------------------------------------------
ofUniChar ofUTF8::getNext(ofUTF8Ptr& iter, ofUTF8Ptr end) {
    try {
        return utf8::next(iter,end);
    } catch(const utf8::exception& utfcpp_ex) {
        ofLog(OF_LOG_ERROR, "ofUTF8::getNext : " + ofToString(utfcpp_ex.what()));
        return -1;
    }
}

//------------------------------------------------------------------
ofUniChar ofUTF8::getPrior(ofUTF8Ptr& iter, ofUTF8Ptr end) {
    try {
        return utf8::prior(iter,end);
    } catch(const utf8::exception& utfcpp_ex) {
        ofLog(OF_LOG_ERROR, "ofUTF8::getPrior : " + ofToString(utfcpp_ex.what()));
        return -1;
    }
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::advance(ofUTF8Ptr& iter, ofUTF8Ptr end, int numToSkip) {
    try {
        utf8::advance(iter,numToSkip,end);
        return iter;
    } catch(const utf8::exception& utfcpp_ex) {
        string err = utfcpp_ex.what();
        ofLog(OF_LOG_ERROR, "ofUTF8::advance : " + err);
        return NULL;
    }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
ofUniChar ofUTF8::get(ofUTF8Ptr iter, ofUTF8Ptr end) {
    return getNext(iter,end);
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::next(ofUTF8Ptr iter, ofUTF8Ptr end) {
    getNext(iter, end); return iter;
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::prior(ofUTF8Ptr iter, ofUTF8Ptr end) {
    getPrior(iter, end); return iter;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
ofUniChar ofUTF8::get(const ofUTF8String& input, ofUTF8Ptr iter) {
    return getNext(iter, endPtr(input));
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::next(const ofUTF8String& input, ofUTF8Ptr iter) {
    next(iter, endPtr(input));
    return iter;
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::prior(const ofUTF8String& input, ofUTF8Ptr iter) {
    prior(iter, endPtr(input));
    return iter;
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::advance(const ofUTF8String& input, ofUTF8Ptr iter, const int numToSkip) {
    return advance(iter, endPtr(input));
}

////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------
ofUniChar ofUTF8::get(ofUTF8Ptr iter) {
    return utf8::unchecked::next(iter); // same as peek
}
//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::next(ofUTF8Ptr iter) {
    utf8::unchecked::next(iter); // we don't return the unicode, just advance
    return iter; // iter is advanced
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::prior(ofUTF8Ptr iter) {
    utf8::unchecked::prior(iter); // we don't return the unicode, just move back
    return iter; // iter is advanced
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::advance(ofUTF8Ptr iter, const int numToSkip) {
    utf8::unchecked::advance(iter,numToSkip); // we don't return the unicode, just advance
    return iter; // iter is advanced
}

//------------------------------------------------------------------
ofUniChar ofUTF8::getNext(ofUTF8Ptr& iter) {
    return utf8::unchecked::next(iter); // increment iter and return value
}

//------------------------------------------------------------------
ofUniChar ofUTF8::getPrior(ofUTF8Ptr& iter) {
    return utf8::unchecked::prior(iter); // increment iter and return value
}

//------------------------------------------------------------------

