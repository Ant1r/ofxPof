//
//  ofxUTF8CPP.h
//  ofxUTFCPPExample
//
//  Created by Christopher Baker on 5/23/12.
//  Copyright (c) 2012 School of the Art Institute of Chicago. All rights reserved.
//

#pragma once

#include "ofUnicode.h"
#include "utf8.h"

class ofUTF8 {
public:
     
    ////////////////////////////////////////////////////////////////////
    // UTF8 MANIPULATION TOOLS /////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    // append unicode char to UTF8 string
    //static ofUTF8String   append(ofUTF8String txt, ofUniChar unicode);
    //static ofUTF8String   append(ofUTF8String txt, ofUniString unicode);
    //static ofUTF8String&  appendInPlace(ofUTF8String& txt, ofUniChar unicode);

    // check to see if a UTF8 string is valid
    static bool     isValid(string txt);
    static bool     isValid(ofUTF8Ptr iter, ofUTF8Ptr end);
    
    // check to see if a string starts with a UTF8 BOM (byte order mark)
    static bool     startsWithBOM(ofUTF8String txt);
    static bool     startsWithBOM(ofUTF8Ptr iter, ofUTF8Ptr end);
    
    // attempt to repair a broken UTF8 string
    static ofUTF8String   repair(string txt, ofUniChar replacement = -1);
    static ofUTF8String&  repairInPlace(string& txt, ofUniChar replacement = -1);
    
    // calculate the number of UTF8 chars (string.length() returns the number of bytes)
    static int      distance(ofUTF8String txt);
    static int      distance(ofUTF8Ptr iter, ofUTF8Ptr end);
    
    // case insensitive comparison of UTF8 strings
    static int      icompare(const ofUTF8String& utf8String0, const ofUTF8String& utf8String1);
    
    // unicode-based case conversion
	static ofUTF8String   toUpper(const ofUTF8String& str);
	static ofUTF8String&  toUpperInPlace(ofUTF8String& str);
	static ofUTF8String   toLower(const ofUTF8String& str);
	static ofUTF8String&  toLowerInPlace(ofUTF8String& str);
    
    ////////////////////////////////////////////////////////////////////
    // UTF8 ITERATION TOOLS ////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    
    // custom iterator helper
    static ofUTF8Ptr beginPtr(const ofUTF8String& input); // gets the start pointer to a string (input.c_str())
    static ofUTF8Ptr endPtr(const ofUTF8String& input);
    
    // custom bi-directional iterator
    static ofxUTF8Iterator iterator(const ofUTF8String& input);
    static ofxUTF8Iterator begin(const ofUTF8String& input);
    static ofxUTF8Iterator end(const ofUTF8String& input);
    
    // Checked Iterators - No need to check input 
    ////////////////////////////////////////////////////////////////////
    static ofUniChar   getNext(ofUTF8Ptr& iter, ofUTF8Ptr end);  // will increment iter and return value
    static ofUniChar   getPrior(ofUTF8Ptr& iter, ofUTF8Ptr end); // will increment iter and return value
    static ofUTF8Ptr   advance(ofUTF8Ptr& iter, ofUTF8Ptr end, int numToSkip = 1); // skips N unicode chars
    //------------------------------------------------------------------
    //------------------------------------------------------------------
    static ofUniChar  get(ofUTF8Ptr iter, ofUTF8Ptr end);  // reads the unichar at the current iter position
    static ofUTF8Ptr  next(ofUTF8Ptr iter, ofUTF8Ptr end); // moves the iterator to the next unicode start
    static ofUTF8Ptr  prior(ofUTF8Ptr iter, ofUTF8Ptr start); // moves the iterator to the previous unicode start
    //------------------------------------------------------------------
    //------------------------------------------------------------------
    static ofUniChar  get(const ofUTF8String& input, ofUTF8Ptr iter = NULL);  // reads the unichar at the current iter position
    static ofUTF8Ptr  next(const ofUTF8String& input, ofUTF8Ptr iter = NULL); // moves the iterator to the next unicode start
    static ofUTF8Ptr  prior(const ofUTF8String& input, ofUTF8Ptr iter = NULL); // moves the iterator to the previous unicode start
    static ofUTF8Ptr  advance(const ofUTF8String& input, ofUTF8Ptr iter = NULL, int numToSkip = 1); // skips N unicode chars
    
    // Unchecked Iterators - Should check input string for valid UTF8 first 
    ////////////////////////////////////////////////////////////////////
    static ofUniChar  get(ofUTF8Ptr iter);  // reads the unichar at the current iter position
    static ofUTF8Ptr  next(ofUTF8Ptr iter); // moves the iterator to the next unicode start
    static ofUTF8Ptr  prior(ofUTF8Ptr iter); // moves the iterator to the previous unicode start
    static ofUTF8Ptr  advance(ofUTF8Ptr iter, int numToSkip = 1); // skips N unicode chars
    
    static ofUniChar  getNext(ofUTF8Ptr& iter); // moves the iterator to the next unicode start and returns it
    static ofUniChar  getPrior(ofUTF8Ptr& iter); // moves the iterator to the previous unicode start and returns it

    
};