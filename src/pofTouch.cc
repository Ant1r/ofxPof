/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofTouch.h"

t_class *poftouch_class;


t_symbol *pofTouch::s_enter, *pofTouch::s_leave, *pofTouch::s_down, *pofTouch::s_up, *pofTouch::s_move,
		*pofTouch::s_cancel; 

void *poftouch_new(t_floatarg w, t_floatarg h, t_floatarg dont_capture, t_floatarg multi)
{
    pofTouch* obj = new pofTouch(poftouch_class, w, h,dont_capture,multi);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->width);
    floatinlet_new(&obj->pdobj->x_obj, &obj->height);
    
    return (void*) (obj->pdobj);
}

void poftouch_free(void *x)
{
	pofTouch* obj = (pofTouch*)(((PdObject*)x)->parent);
	
	delete (pofTouch*)(((PdObject*)x)->parent);
}

void poftouch_draw(void *x, t_float d)
{
	((pofTouch*)(((PdObject*)x)->parent))->do_draw = (d!=0);
}

void poftouch_dont_capture(void *x, t_float nc)
{
	((pofTouch*)(((PdObject*)x)->parent))->capture = (nc==0);
	((pofTouch*)(((PdObject*)x)->parent))->dynamic = (nc==2);
}

void poftouch_multi(void *x, t_float m)
{
	((pofTouch*)(((PdObject*)x)->parent))->multi = (m!=0);
}

void poftouch_shape(void *x, t_symbol *shape)
{
	pofTouch* px = (pofTouch*)(((PdObject*)x)->parent);
	
	if(!strcmp(shape->s_name,"rect")) px->ellipse = false;
	if(!strcmp(shape->s_name,"ellipse")) px->ellipse = true;
}

static void poftouch_anything(void *x, t_symbol *s, int argc, t_atom *argv)
{
    pofTouch* px = (pofTouch*)(((PdObject*)x)->parent);
    
    outlet_anything(px->m_out2, s, argc, argv);
}

void pofTouch::setup(void)
{
	//post("poftouch_setup");
	poftouch_class = class_new(gensym("poftouch"), (t_newmethod)poftouch_new, (t_method)poftouch_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	POF_SETUP(poftouch_class);
	class_addmethod(poftouch_class, (t_method)poftouch_draw, gensym("draw"), A_FLOAT, A_NULL);
	class_addmethod(poftouch_class, (t_method)poftouch_dont_capture, gensym("dont_capture"), A_FLOAT, A_NULL);
	class_addmethod(poftouch_class, (t_method)poftouch_multi, gensym("multi"), A_FLOAT, A_NULL);
	class_addmethod(poftouch_class, (t_method)poftouch_shape, gensym("shape"), A_SYMBOL, A_NULL);
	
	pofTouch::s_enter=gensym("enter");
	pofTouch::s_leave=gensym("leave");
	pofTouch::s_down=gensym("down");
	pofTouch::s_up=gensym("up");
	pofTouch::s_move=gensym("move");
	pofTouch::s_cancel=gensym("cancel");
	
	class_addmethod(poftouch_class, (t_method)poftouch_anything, s_enter , A_GIMME, A_NULL);
	class_addmethod(poftouch_class, (t_method)poftouch_anything, s_leave , A_GIMME, A_NULL);
	class_addmethod(poftouch_class, (t_method)poftouch_anything, s_down	 , A_GIMME, A_NULL);
	class_addmethod(poftouch_class, (t_method)poftouch_anything, s_up	 , A_GIMME, A_NULL);
	class_addmethod(poftouch_class, (t_method)poftouch_anything, s_move	 , A_GIMME, A_NULL);
	class_addmethod(poftouch_class, (t_method)poftouch_anything, s_cancel, A_GIMME, A_NULL);
}

void pofTouch::draw()
{
	if (do_draw) {
		if(ellipse) ofEllipse(0,0,width,height);
		else ofRect(-width/2,-height/2,width,height);
	}
}

bool pofTouch::isInside(int x,int y)
{
	if(ellipse) return (ofDist(2*x/width,2*y/height,0,0) <= 1);
	else return (x >= -width/2 && x < width/2 && y >= -height/2 && y < height/2);
}	

void pofTouch::Send(t_symbol *s, int n, float f1, float f2, float f3)
{
	t_atom ap[4];
	int count = 0;
	 
	if(n > 2) n = 3;
	if(n < 1) n = 1;
	
	SETSYMBOL(&ap[count], s); count++;
	SETFLOAT(&ap[count], f1); count++;
	if(n>1) { SETFLOAT(&ap[count], f2); count++; }
	if(n>2) { SETFLOAT(&ap[count], f3); count++; }
	
	//queueToSelfPd(count, ap);
	poftouch_anything(pdobj, s, count -1, &ap[1]);
}

bool pofTouch::touchMoved(int x, int y, int id)
{
	if(dynamic) {
		if(isInside(x,y)) {
			captured[id]=true;
		
			if((active_id==-1) || multi) {
				active_id=id;
				Send(s_enter, 3, x, y, id);
			}
		} else /*outside*/ {
			if(captured[id]) {
				captured[id] = false;
				if((id == active_id) || multi) {
					Send(s_leave, 3, x, y, id);
					active_id = -1;
				}
			}
		}
	}
	
	if(captured[id]) {	
		if((id == active_id) || multi) {
			Send(s_move, 3, x, y, id);
		}
		return capture;
	} else return false;
}

bool pofTouch::touchDown(int x, int y, int id)
{
	if(isInside(x,y)){
		captured[id]=true;
		
		if((active_id==-1) || multi) {
			active_id=id;
			Send(s_down, 3, x, y, id);
		}
		return capture;
	} else return false;
}

bool pofTouch::touchUp(int x, int y, int id)
{
	if(captured[id]) {
		captured[id] = false;
		if((id == active_id) || multi) {
			Send(s_up, 3, x, y, id);
			active_id = -1;
		}
		return capture;
	} else return false;
}

bool pofTouch::touchDoubleTap(int x, int y, int id){ return false;}

void pofTouch::touchCancel()
{
	if((active_id != -1) || multi) Send(s_cancel, 1, 0);

	std::fill_n(captured, 32, false);
	active_id = -1;

}

