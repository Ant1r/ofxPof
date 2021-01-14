/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofTexts.h"
#include "pofFonts.h"

static t_class *poftexts_class;
static t_symbol *s_out, *s_size;

void *poftexts_new(t_symbol *font, t_float size, t_float xanchor, t_float yanchor, /*t_float space,*/ t_float spacing)
{
    //if(spacing == 0) spacing = 1;
    if(size < 1) size = 1;
    pofTexts* obj = new pofTexts(poftexts_class, font, size, xanchor, yanchor, /*space,*/ spacing);
    obj->pdcanvas = canvas_getcurrent();
    obj->mustUpdate = true;
    return (void*) (obj->pdobj);
}

void poftexts_free(void *x)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	delete px;
}

void poftexts_text(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	
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

void poftexts_set(void *x, t_symbol *newfont)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	px->font = newfont;
	px->mustUpdate = true;
}

void poftexts_size(void *x, t_float s)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	if(s < 1) s = 1;
	px->size = s;
	//px->mustUpdate = true;
}

void poftexts_anchor(void *x, t_float xanchor, t_float yanchor)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	px->xanchor = xanchor;
	px->yanchor = yanchor;
}

void poftexts_spacing(void *x, t_float spacing)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	px->letterSpacing = spacing;
	px->mustUpdate = true;
}

void poftexts_center(void *x, t_float center)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	px->center = (center != 0);
	px->mustUpdate = true;
}

void poftexts_under(void *x, t_float height, t_float width, t_float y)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	px->underHeight = height;
	px->underWidth = width;
	px->underY = y;
	//px->mustUpdate = true;
}

void poftexts_width(void *x, t_float w)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	px->width = w;
	px->mustUpdate = true;
}

void poftexts_lineHeight(void *x, t_float h)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	px->lineHeight = h;
	px->clipChanged = true;
}

void poftexts_readfile(void *x, t_symbol *f)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	t_symbol* file = makefilename(f, px->pdcanvas);

	if(file == NULL || !ofFile(file->s_name).exists()) {
		post("poftexts_readfile error :file %s doesn't exists.",f->s_name);
		return;
	}
	string tmpStr = ofBufferFromFile(file->s_name).getText();
	
	px->mutex.lock();
	px->str = tmpStr;
	px->mustUpdate = true;
	px->mutex.unlock();
}

void poftexts_cliplines(void *x, t_float max, t_float offset)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);
	if(max < 0) max = 0;
	if(offset < 0) offset = 0;
	px->maxLines = max;
	px->lineOffset = offset;
	px->clipChanged = true;
}

void poftexts_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofTexts* px= (pofTexts*)(((PdObject*)x)->parent);

	if((argc>1) && argv->a_type == A_SYMBOL)
		outlet_anything(px->m_out2, atom_getsymbol(argv), argc-1, argv+1);
}

void pofTexts::setup(void)
{
	//post("poftexts_setup");
	s_size = gensym("size");
	s_out = gensym("out");
	poftexts_class = class_new(gensym("poftexts"), (t_newmethod)poftexts_new, (t_method)poftexts_free,
		sizeof(PdObject), 0, A_SYMBOL, A_FLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	POF_SETUP(poftexts_class);
	class_addmethod(poftexts_class, (t_method)poftexts_text, gensym("text"), A_GIMME, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_set, gensym("set"), A_SYMBOL, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_size, gensym("size"), A_FLOAT, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_anchor, gensym("anchor"), A_FLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_spacing, gensym("spacing"), A_FLOAT, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_center, gensym("center"), A_FLOAT, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_under, gensym("under"), A_FLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_out, s_out, A_GIMME, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_width, gensym("width"), A_FLOAT, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_lineHeight, gensym("lineheight"), A_FLOAT, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_readfile, gensym("readfile"), A_SYMBOL, A_NULL);
	class_addmethod(poftexts_class, (t_method)poftexts_cliplines, gensym("cliplines"), A_DEFFLOAT, A_DEFFLOAT, A_NULL);
}

void pofTexts::draw()
{
	pofFonts *poffont = pofFonts::getFont(font);
	ofxFontStash *offont = poffont->offont;
	bool update = false;
	int numLines;
	bool wordsWereCropped;
	float finalsize;

	if(offont == NULL) return;
	if(!offont->isLoaded()) return;

	offont->setCharacterSpacing(letterSpacing);
	offont->setLineHeight(lineHeight);
	finalsize = size * poffont->scale;
	if(lastFinalsize != finalsize) {
		lastFinalsize = finalsize;
		mustUpdate = true;
	}
	
	if(mustUpdate) {
		string copyStr;

		mutex.lock();
		mustUpdate = false;
		copyStr = str;
		mutex.unlock();
		update = true;
		
		lines = offont->computeMultiLines(
			str,				/*string*/
			finalsize,			/*size*/
			width,				/*column width*/
			numLines,			/*get back the number of lines*/
			&wordsWereCropped 	/* this bool will b set to true if the box was to small to fit all text*/
		);
		totalLines = numLines;//lines.size();
	}

	if(update || clipChanged) {
		clipChanged = false;
		update = true;
		bound = offont->drawMultiLines(
			lines,		/*lines*/
			finalsize,	/*size*/
			0, 0,		/*where*/
			width, 		/*column width*/
			numLines,	/*get back the number of lines*/
			true,		/* if true, we wont draw (just get bbox back) */
			maxLines,	/* max number of lines to draw, crop after that */
			center, lineOffset
		);
	}

	offont->drawMultiLines(
		lines,		/*lines*/
		finalsize,	/*size*/
		bound.width*(-xanchor-1)/2 - bound.x,bound.height*(yanchor-1)/2 - bound.y,	/*where*/
		width, 		/*column width*/
		numLines,	/*get back the number of lines*/
		false,		/* if true, we wont draw (just get bbox back) */
		maxLines,	/* max number of lines to draw, crop after that */
		center, lineOffset, underHeight, underWidth, underY
	 );

	if((oldBound != bound)||update) {
		oldBound = bound;
		t_atom ap[6];

		SETSYMBOL(&ap[0], s_out);
		SETSYMBOL(&ap[1], s_size);
		SETFLOAT(&ap[2], bound.width);
		SETFLOAT(&ap[3], bound.height);
		SETFLOAT(&ap[4], numLines);
		SETFLOAT(&ap[5], totalLines);
		queueToSelfPd(6, ap);
	}
}


