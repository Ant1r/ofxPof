/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofText.h"
#include "pofFont.h"

static t_class *poftext_class;
static t_symbol *s_out, *s_size;

void *poftext_new(t_symbol *font, t_float xanchor, t_float yanchor, t_float space, t_float spacing)
{
    if(space == 0) space = 1;
    if(spacing == 0) spacing = 1;
    
    pofText* obj = new pofText(poftext_class, font, xanchor, yanchor, space, spacing);
    
    obj->bb = binbuf_new();
    obj->mustUpdate = true;
    return (void*) (obj->pdobj);
}

void poftext_free(void *x)
{
	pofText* px= (pofText*)(((PdObject*)x)->parent);
	binbuf_free(px->bb);
	delete px;
}

void poftext_text(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofText* px= (pofText*)(((PdObject*)x)->parent);
	
	string spaceStr="";
	std::ostringstream tmpStr;
	bool added;
	
	while(argc>0) {
		added = true;
		if(argv->a_type == A_SYMBOL) { 
			tmpStr << spaceStr << atom_getsymbol(argv)->s_name; 
		}
		else if(argv->a_type == A_FLOAT) tmpStr << spaceStr << atom_getfloat(argv);
		else added = false;
		
		if(added) spaceStr=" ";
		argc-- ; argv++;
	}
	
	px->mutex.lock();
	px->str = tmpStr.str();
	px->mustUpdate = true;
	px->mutex.unlock();
}

void poftext_set(void *x, t_symbol *newfont)
{
	pofText* px= (pofText*)(((PdObject*)x)->parent);
	px->font = newfont;
	px->mustUpdate = true;
}

void poftext_anchor(void *x, t_float xanchor, t_float yanchor)
{
	pofText* px= (pofText*)(((PdObject*)x)->parent);
	px->xanchor = xanchor;
	px->yanchor = yanchor;
}

void poftext_spacing(void *x, t_float space, t_float spacing)
{
	pofText* px= (pofText*)(((PdObject*)x)->parent);
	px->spaceSize = space;
	px->letterSpacing = spacing;
	px->mustUpdate = true;
}

void poftext_width(void *x, t_float w)
{
	pofText* px= (pofText*)(((PdObject*)x)->parent);
	px->width = w;
	px->mustUpdate = true;
}

void poftext_lineHeight(void *x, t_float h)
{
	pofText* px= (pofText*)(((PdObject*)x)->parent);
	px->lineHeight = h;
	px->mustUpdate = true;
}

void poftext_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofText* px= (pofText*)(((PdObject*)x)->parent);

	if((argc>1) && argv->a_type == A_SYMBOL)
		outlet_anything(px->m_out2, atom_getsymbol(argv), argc-1, argv+1);
}

void pofText::setup(void)
{
	//post("poftext_setup");
	s_size = gensym("size");
	s_out = gensym("out");
	poftext_class = class_new(gensym("poftext"), (t_newmethod)poftext_new, (t_method)poftext_free,
		sizeof(PdObject), 0, A_SYMBOL, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	POF_SETUP(poftext_class);
	class_addmethod(poftext_class, (t_method)poftext_text, gensym("text"), A_GIMME, A_NULL);
	class_addmethod(poftext_class, (t_method)poftext_set, gensym("set"), A_SYMBOL, A_NULL);
	class_addmethod(poftext_class, (t_method)poftext_anchor, gensym("anchor"), A_FLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(poftext_class, (t_method)poftext_spacing, gensym("spacing"), A_FLOAT, A_FLOAT, A_NULL);
	class_addmethod(poftext_class, (t_method)poftext_out, s_out, A_GIMME, A_NULL);
	class_addmethod(poftext_class, (t_method)poftext_width, gensym("width"), A_FLOAT, A_NULL);
	class_addmethod(poftext_class, (t_method)poftext_lineHeight, gensym("lineheight"), A_FLOAT, A_NULL);
}

static void computeString(const string &inStr, string &outStr, ofTrueTypeFont *font, float w)
{
	string tmpLine;
	outStr.clear();
	unsigned int index = 0;
	int lastSpace = -1; // in tmpLine
	bool wasNL = true;
	
	#define PUSH_TMP(newline) do { \
		if(newline) tmpLine.push_back('\n'); \
		outStr += tmpLine; tmpLine.clear(); wasNL = true; lastSpace = -1;\
	} while(false)
	#define printdebug false
	
	while(index < inStr.size()) {
		if(inStr[index] == ' ') {
			if(!wasNL) {
				lastSpace = tmpLine.size();
				if(printdebug) cout << " spaceAt:" <<lastSpace<<" ";
				tmpLine.push_back(inStr[index]);
				if( (font->getStringBoundingBox(tmpLine, 0, 0)).width > w) {
					tmpLine.erase(tmpLine.end()-1); // remove last char
					if(printdebug) cout <<endl<<"space wrapping ; lastSpace="<<lastSpace<<" tmpLine: "<<tmpLine<<endl;
					PUSH_TMP(true);
				}
			}
		}
		else if(inStr[index] == '\n') {
			if(!wasNL) {
				if(printdebug) cout <<endl<<"nl wrapping :"<<tmpLine<<endl;
				PUSH_TMP(true);
			} else if(printdebug) cout << " eat nl...";
		}
		else { // normal char :
			tmpLine.push_back(inStr[index]);
			if(printdebug) cout << inStr[index];
			if( (font->getStringBoundingBox(tmpLine, 0, 0)).width > w && lastSpace != -1 ) {
				string tmp = tmpLine.substr(lastSpace + 1, tmpLine.size() - lastSpace - 1);
				if(printdebug) cout << endl<<"wrapping ; lastSpace="<<lastSpace<<" removing: "<<tmp<<endl;
				try {
					tmpLine.erase(lastSpace, tmpLine.size() - lastSpace);
				} catch(int e) { cout<<"***erase exception !***"<<endl;}
				PUSH_TMP(!wasNL);
				tmpLine = tmp;					
			}
			wasNL = false;
		}
		
		index++;
	}
	
	PUSH_TMP(false);
}

void pofText::draw()
{
	ofTrueTypeFont *offont = pofFont::getFont(font);
	float lH;
	bool update = false;
	
	if(offont == NULL) return;
	
	lH = offont->getLineHeight(); // backup normal lineHeigth
	
	offont->setSpaceSize(spaceSize);
	offont->setLetterSpacing(letterSpacing);
	if(lineHeight!=0) offont->setLineHeight(lineHeight);
	
	if(mustUpdate) {
		mustUpdate = false;
		mutex.lock();
		copyStr = str;
		mutex.unlock();

		update = true;
		if(width != 0) computeString(copyStr, computedStr, offont, width);
		else computedStr = copyStr;
	}
	
	bound = offont->getStringBoundingBox(computedStr, 0, 0);
	
	offont->drawString(computedStr,bound.width*(-xanchor-1)/2 - bound.x,bound.height*(yanchor-1)/2 - bound.y);
	
	offont->setLineHeight(lH); // restore font lineHeight

	// send bounds
	if((oldBound != bound)||update) {
		oldBound = bound;	
		t_atom ap[6];

		SETSYMBOL(&ap[0], s_out);
		SETSYMBOL(&ap[1], s_size);
		SETFLOAT(&ap[2], bound.width);
		SETFLOAT(&ap[3], bound.height);
		SETFLOAT(&ap[4], bound.x);
		SETFLOAT(&ap[5], bound.y);
		queueToSelfPd(6, ap);
	}

}

