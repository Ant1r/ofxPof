/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofTextbox.h"

t_class *pofTextbox_class;

static t_symbol *s_out;

static void pofTextbox_setQuestion(void *x, t_symbol *s, int argc, t_atom *argv)
{
	char *c;
	int l;
	
	pofTextbox* px= (pofTextbox*)(((PdObject*)x)->parent);
	
	if(argc != 0) {
		binbuf_clear(px->bb);
		binbuf_add(px->bb, argc, argv);
		binbuf_gettext(px->bb, &c, &l);
		px->question.assign(c, l);
	} else px->question = "";
}

static void *pofTextbox_new(t_symbol *s, int argc, t_atom *argv)
{
    pofTextbox* obj = new pofTextbox(pofTextbox_class);
    
	obj->bb = binbuf_new();
	pofTextbox_setQuestion(obj->pdobj, NULL, argc, argv);
	
    return (void*) (obj->pdobj);
}

static void pofTextbox_free(void *x)
{
	pofTextbox* px= (pofTextbox*)(((PdObject*)x)->parent);
	binbuf_free(px->bb);
	delete px;
}

static void pofTextbox_show(void *x, t_symbol *s, int argc, t_atom *argv)
{
	char *c;
	int l;
	
	pofTextbox* px= (pofTextbox*)(((PdObject*)x)->parent);
	
	if(argc!=0) {
		binbuf_clear(px->bb);
		binbuf_add(px->bb, argc, argv);
		binbuf_gettext(px->bb, &c, &l);
		px->defAnswer.assign(c, l);
	} else px->defAnswer = "";
	
	px->needShow = true;
}

static void pofTextbox_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
    pofTextbox* px = (pofTextbox*)(((PdObject*)x)->parent);
    
    if(s == s_out) {
		binbuf_clear(px->bb);
		binbuf_text(px->bb, (char*)px->answer.c_str(), px->answer.size());
    	outlet_anything(px->m_out1, s, binbuf_getnatom(px->bb), binbuf_getvec(px->bb));
    }
}

void pofTextbox::setup(void)
{
	//post("pofTextbox_setup");
	s_out = gensym("out");
	pofTextbox_class = class_new(gensym("poftextbox"), (t_newmethod)pofTextbox_new, (t_method)pofTextbox_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	//POF_SETUP(pofTextbox_class);
	class_addmethod(pofTextbox_class, (t_method)pofTextbox_setQuestion, gensym("question"), A_GIMME, A_NULL);
	class_addmethod(pofTextbox_class, (t_method)pofTextbox_show, gensym("show"), A_GIMME, A_NULL);
	class_addmethod(pofTextbox_class, (t_method)pofTextbox_out, s_out, A_GIMME, A_NULL);
}

void pofTextbox::update()
{
	if(!needShow) return;
	
	needShow = false;
	needSend = true;	
#ifndef TARGET_LINUX	
    answer = ofSystemTextBoxDialog(question, defAnswer);
#else
    answer = defAnswer;
#endif
	//answer = ofSystemTextBoxDialog("", "");
#ifndef TARGET_ANDROID
	t_atom ap[2];
	needSend = false;
	SETSYMBOL(&ap[0], s_out);
	queueToSelfPd(1, ap);
#endif	
}

#ifdef TARGET_ANDROID
void pofTextbox::okPressed(/*bool &b*/)
{
	if(needSend) {
		needSend = false;
		t_atom ap[2];
		answer = ofxAndroidGetTextBoxResult();
	
		SETSYMBOL(&ap[0], s_out);
		queueToSelfPd(1, ap);
	}
	//return true;
}
#endif

#ifdef TARGET_ANDROID
void pofTextbox::cancelPressed(/*bool &b*/)
{
	if(needSend) {
		needSend = false;
		t_atom ap[2];
		answer = "";
	
		SETSYMBOL(&ap[0], s_out);
		queueToSelfPd(1, ap);
	}
	//return true;
}
#endif
