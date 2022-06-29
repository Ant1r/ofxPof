/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofImage.h"
#include "pofFbo.h"

class pofIm;

class pofImLoader: public ofThread {
	public :
    deque<pofIm*> imagesToLoad;

	void threadedFunction();
    
    void enQueue(pofIm *im) {
    	lock();
    	imagesToLoad.push_back(im);
    	unlock();
    }
    
    unsigned int getLen() {
    	int l;
    	lock();
    	l = imagesToLoad.size();
    	unlock();
    	return l;
    }
    	
} *imLoader = NULL, *imLoaderHTTP = NULL; // two loader tasks : one for online resources, one for on disk ones.
    

class pofIm{

	t_symbol *file;
	int refCount;
	
	static std::map<t_symbol*,pofIm*> images;
	static ofMutex mutex;

	public:
	ofImage im;
	bool loaded, preloaded, needUpdate;
	
	pofIm(t_symbol *f):file(f),refCount(1), loaded(false), preloaded(false), needUpdate(false) {
		images[file] = this;
		im.setAnchorPercent(0.5,0.5);
		im.setUseTexture(false);
	}
	
	~pofIm() {
		images.erase(file);
		//pofBase::textures.erase(file);
	}
	
	void doLoad(void) {
		if(loaded) return;
		if( (!strncmp(file->s_name, "http", strlen("http"))) || ofFile(file->s_name).exists())
            im.load(file->s_name);
		loaded = true;
	}
	
	void load() {
		if(!preloaded) {
			preloaded = true;
			mutex.lock();
			refCount++;
			if(!strncmp(file->s_name, "http", strlen("http"))) imLoaderHTTP->enQueue(this);
			else imLoader->enQueue(this);
			mutex.unlock();
		}
	}
	
	void reload() {
		loaded = false;
		doLoad();
	}

	void loadfile(t_symbol *file) {
		if( (!strncmp(file->s_name, "http", strlen("http"))) || ofFile(file->s_name).exists())
            im.load(file->s_name);
		preloaded = loaded = true;
	}

	bool update() {
		if(!(loaded && im.isAllocated())) return false;
		
		if(!im.isUsingTexture())
		{
            im.setUseTexture(true);
            needUpdate = true;
        }
        if(needUpdate) {
        	im.update();
        	needUpdate = false;
        }
		return true;
	}


	void draw(float x, float y, float w, float h, bool quality) {
		if(!update()) return;
		if(!quality) im.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
		else im.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
		im.draw(x, y, w, h);
	}

	void drawsub(float x, float y, float w, float h, float sx, float sy, float sw, float sh, bool quality) {
		if(!update()) return;
		if(!quality) im.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
		else im.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
		im.drawSubsection(x, y, w, h, sx, sy, sw, sh);
	}

	void bind() {
		if(update()) {
			im.bind();
			pofBase::currentTexture = &im.getTexture();
		}
	}

	void unbind() {
		if(!loaded) return;
		im.unbind();
	}

	static pofIm* getImage(t_symbol *file){
		mutex.lock();
		std::map<t_symbol*,pofIm*>::iterator it;
		it = images.find(file);
		if(it!=images.end()) {
			it->second->refCount++;
			mutex.unlock();
			return it->second;
		}
		else {
			new pofIm(file);
			mutex.unlock();
			return images[file];
		}
	}
		
	static pofIm* findImage(t_symbol *file){
		std::map<t_symbol*,pofIm*>::iterator it;
		it = images.find(file);
		if(it == images.end()) return NULL;
		else return it->second;
	}	
		
	static int letImage(pofIm *image) {
		bool deleted = false;
		mutex.lock();
		image->refCount--;
		if(image->refCount == 0) {
			delete image;
			deleted = true;
		}
		mutex.unlock();
		if(deleted) return 0;
		else return image->refCount;
	}
	
	static int getNumImages()
	{
		return images.size();
	}
};

std::map<t_symbol*,pofIm*> pofIm::images;
ofMutex pofIm::mutex;

//------------------------------------------//

void pofImLoader::threadedFunction() {
	while(isThreadRunning()) {
		if(imagesToLoad.size()) {
			lock();
			pofIm *im = imagesToLoad.front();
			imagesToLoad.pop_front();
			unlock();
			im->doLoad();
			pofIm::letImage(im);
		} else ofSleepMillis(5);
    }
}


//------------------------------------------//

static t_class *pofimage_class;
static t_symbol *s_set, *s_saved, *s_size, *s_monitor, *s_color, 
  *s_save, *s_clear, *s_resize, *s_setcolor, *s_grab, *s_grabfbo,
  *s_crop, *s_reload, *s_loadfile, *s_settype, *s_RGB, *s_RGBA, *s_GRAY;

static void pofimage_set(void *x, t_symbol *f);

#define NEXT_FLOAT_ARG(var) if((argc>0)&&(argv->a_type == A_FLOAT)) { var = atom_getfloat(argv); argv++; argc--; }

static void *pofimage_new(t_symbol *sym,int argc, t_atom *argv)
{
    t_symbol *file=NULL;
    t_symbol *name = NULL;
    t_float w=0,h=0,xa=0,ya=0,sx=0,sy=0,sw=0,sh=0,istexture=0;

	if(argc>0) file = atom_getsymbol(&argv[0]);

    if(argc>1) {
		if(argv[1].a_type == A_SYMBOL) {
			istexture = 1;
			name = atom_getsymbol(&argv[1]); 
		}
		else {
			argc--; argv++;
			NEXT_FLOAT_ARG(w);
			NEXT_FLOAT_ARG(h);
		
			NEXT_FLOAT_ARG(xa);
			NEXT_FLOAT_ARG(ya);
		
			NEXT_FLOAT_ARG(sx);
			NEXT_FLOAT_ARG(sy);
		
			NEXT_FLOAT_ARG(sw);
			NEXT_FLOAT_ARG(sh);
		
			if((argc>0)&&(argv[0].a_type == A_SYMBOL)) name = atom_getsymbol(argv);
		}
	}
	
    pofImage* obj = new pofImage(pofimage_class, file, w, h, xa, ya, sx, sy, sw, sh, istexture, name);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->width);
    floatinlet_new(&obj->pdobj->x_obj, &obj->height);

    obj->pdcanvas = canvas_getcurrent();
	if(file) pofimage_set(&obj->pdobj->x_obj, file);
	
    return (void*) (obj->pdobj);
}

static void pofimage_free(void *x)
{
	pofImage* px= (pofImage*)(((PdObject*)x)->parent);

	px->unreserveAll();
	delete px;
}

static void pofimage_set(void *x, t_symbol *f)
{
	pofImage* px= (pofImage*)(((PdObject*)x)->parent);	
	px->set(f);
}

static void pofimage_reserve(void *x, t_symbol *f)
{
	pofImage* px= (pofImage*)(((PdObject*)x)->parent);
	px->reserve(f);
}

static void pofimage_unreserve(void *x, t_symbol *f)
{
	pofImage* px= (pofImage*)(((PdObject*)x)->parent);

	if(!strcmp(f->s_name, "_all_")) px->unreserveAll();
	else px->unreserve(f);
}

static void pofimage_monitor(void *x, t_float f)
{
	pofImage* px= (pofImage*)(((PdObject*)x)->parent);
	px->monitor = (f != 0);
}

static void pofimage_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
    pofImage* px = (pofImage*)(((PdObject*)x)->parent);
    
    outlet_anything(px->m_out2, s, argc, argv);
}

static void pofimage_getcolor(void *x, t_float X, t_float Y)
{
	pofImage* px= (pofImage*)(((PdObject*)x)->parent);
	if(px->image == NULL) return;
	
	ofImage *image = &px->image->im;
	if(px->image->loaded) {
		t_atom ap[4];
		ofColor color = image->getColor(ofClamp(X, 0, image->getWidth()), ofClamp(Y, 0, image->getHeight()));
		SETFLOAT(&ap[0], color.r/255.0);
		SETFLOAT(&ap[1], color.g/255.0);
		SETFLOAT(&ap[2], color.b/255.0);
		SETFLOAT(&ap[3], color.a/255.0);
		outlet_anything(px->m_out2, s_color, 4, ap);
	} 
}

static void pofimage_setcolors(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofImage* px= (pofImage*)(((PdObject*)x)->parent);
	t_atom ap[4];
	int length;
	// X Y NUM tableR tableG tableB tableA [offsetR offsetG offsetB offsetA]
	if(argc < 6) return;
	SETFLOAT(&ap[0], atom_getfloat(&argv[0]));
	SETFLOAT(&ap[1], atom_getfloat(&argv[1]));
	SETFLOAT(&ap[2], length = atom_getfloat(&argv[2]));
	
	if(argv[3].a_type != A_SYMBOL || argv[4].a_type != A_SYMBOL || argv[5].a_type != A_SYMBOL)
		return;
		
	t_garray *R, *G, *B, *A;
	int lenR, lenG, lenB, lenA;
	int offsetR = 0, offsetG = 0, offsetB = 0, offsetA = 0;
	t_word *vecR, *vecG, *vecB, *vecA = NULL;
	
	if (!(R = (t_garray *)pd_findbyclass(atom_getsymbol(&argv[3]), garray_class)))
		pd_error(x, "%s: no such array", atom_getsymbol(&argv[3])->s_name);
	else if (!garray_getfloatwords(R, &lenR, &vecR))
		pd_error(x, "%s: bad template for tabdump", atom_getsymbol(&argv[3])->s_name);
		
	else if (!(G = (t_garray *)pd_findbyclass(atom_getsymbol(&argv[4]), garray_class)))
		pd_error(x, "%s: no such array", atom_getsymbol(&argv[4])->s_name);
	else if (!garray_getfloatwords(G, &lenG, &vecG))
		pd_error(x, "%s: bad template for tabdump", atom_getsymbol(&argv[4])->s_name);
		
	else if (!(B = (t_garray *)pd_findbyclass(atom_getsymbol(&argv[5]), garray_class)))
		pd_error(x, "%s: no such array", atom_getsymbol(&argv[5])->s_name);
	else if (!garray_getfloatwords(B, &lenB, &vecB))
		pd_error(x, "%s: bad template for tabdump", atom_getsymbol(&argv[5])->s_name);
	else {
		if(argc > 6 && argv[6].a_type == A_SYMBOL) {
			if (!(A = (t_garray *)pd_findbyclass(atom_getsymbol(&argv[6]), garray_class)))
				pd_error(x, "%s: no such array", atom_getsymbol(&argv[6])->s_name);
			else if (!garray_getfloatwords(A, &lenA, &vecA))
				pd_error(x, "%s: bad template for tabdump", atom_getsymbol(&argv[6])->s_name);
			argc-- ;argv++;
		}
		if(argc > 6) offsetR = atom_getfloat(&argv[6]);
		if(argc > 7) offsetG = atom_getfloat(&argv[7]);
		if(argc > 8) offsetB = atom_getfloat(&argv[8]);
		if(argc > 9) offsetA = atom_getfloat(&argv[9]);
		
		t_float *colors = (t_float *)getbytes(length * 4 * sizeof(t_float));
		for(int i = 0 ; i < length ; i++) {
			colors[4 * i      ] = (i + offsetR < lenR) ? vecR[i + offsetR].w_float : 0.0;
			colors[(4 * i) + 1] = (i + offsetG < lenG) ? vecG[i + offsetG].w_float : 0.0;
			colors[(4 * i) + 2] = (i + offsetB < lenB) ? vecB[i + offsetB].w_float : 0.0;
			colors[(4 * i) + 3] = vecA && (i + offsetA < lenA) ? vecA[i + offsetA].w_float : 1.0;
		}
		ap[3].a_type=A_POINTER;
		ap[3].a_w.w_gpointer=(t_gpointer *)colors;
		pofBase::tellGUI(x, s_setcolor, 4, ap);
	}
}

static void pofimage_sub(void *x, t_float sx, t_float sy, t_float sw, t_float sh)
{
	pofImage* px= (pofImage*)(((PdObject*)x)->parent);
	px->subx = sx;
	px->suby = sy;
	px->subwidth = sw;
	px->subheight = sh;
}

static void pofimage_quality(void *x, t_float q)
{
	pofImage* px= (pofImage*)(((PdObject*)x)->parent);
	px->quality = (q != 0);
}

void pofImage::setup(void)
{
	//post("pofimage_setup");
	s_set = gensym("set");
	s_saved = gensym("saved");
	s_size = gensym("size");
	s_monitor = gensym("monitor");
	s_color = gensym("color");
	s_save = gensym("save");
	s_clear = gensym("clear");
	s_resize = gensym("resize");
	s_setcolor = gensym("setcolor");
	s_grab = gensym("grab");
	s_grabfbo = gensym("grabfbo");
	s_crop = gensym("crop");
	s_reload = gensym("reload");
	s_loadfile = gensym("loadfile");
    s_settype = gensym("settype");
    s_RGB = gensym("RGB");
    s_RGBA = gensym("RGBA");
    s_GRAY = gensym("GRAY");
	pofimage_class = class_new(gensym("pofimage"), (t_newmethod)pofimage_new, (t_method)pofimage_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	POF_SETUP(pofimage_class);
	class_addmethod(pofimage_class, (t_method)pofimage_set, s_set, A_SYMBOL, A_NULL);
	class_addmethod(pofimage_class, (t_method)pofimage_reserve, gensym("reserve"), A_SYMBOL, A_NULL);
	class_addmethod(pofimage_class, (t_method)pofimage_unreserve, gensym("unreserve"), A_SYMBOL, A_NULL);
	class_addmethod(pofimage_class, (t_method)pofimage_monitor, gensym("setmonitor"), A_FLOAT, A_NULL);
	class_addmethod(pofimage_class, (t_method)pofimage_getcolor, gensym("getcolor"), A_FLOAT, A_FLOAT, A_NULL);
	class_addmethod(pofimage_class, (t_method)pofimage_setcolors, gensym("setcolors"),	A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)pofimage_sub, gensym("sub"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofimage_class, (t_method)pofimage_quality, gensym("quality"), A_FLOAT, A_NULL);
	
	class_addmethod(pofimage_class, (t_method)tellGUI, s_save,    	A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)tellGUI, s_setcolor,	A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)tellGUI, s_resize,  	A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)tellGUI, s_crop,    	A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)tellGUI, s_clear,   	A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)tellGUI, s_grab,    	A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)tellGUI, s_grabfbo, 	A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)tellGUI, s_reload,    A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)tellGUI, s_loadfile, 	A_GIMME, A_NULL);
    class_addmethod(pofimage_class, (t_method)tellGUI, s_settype, 	A_GIMME, A_NULL);
	
	class_addmethod(pofimage_class, (t_method)pofimage_out, s_size, A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)pofimage_out, s_monitor, A_GIMME, A_NULL);
	class_addmethod(pofimage_class, (t_method)pofimage_out, s_saved, A_GIMME, A_NULL);
    imLoader = new pofImLoader;
    imLoaderHTTP = new pofImLoader;
	imLoader->startThread(true); //, false);    // blocking, non verbose
	imLoaderHTTP->startThread(true); //, false);    // blocking, non verbose
}

void pofImage::release(void)
{
	imLoader->waitForThread(true);
	delete imLoader;
	imLoaderHTTP->waitForThread(true);
	delete imLoaderHTTP;
}

pofImage::~pofImage() 
{ 
	pofBase::treeMutex.lockW();
	if(name) pofBase::textures.erase(name); 
	if(image) pofIm::letImage(image);
	pofBase::treeMutex.unlockW();
}

void pofImage::Update()
{
	float w, h;
	t_atom ap[4];

	if(name) pofBase::textures.erase(name);
	if(monitor) {
		unsigned int len = imLoader->getLen();
		unsigned int lenHTTP = imLoaderHTTP->getLen();
		unsigned int lenTotal = pofIm::getNumImages();
		if((len != loaderLen)||(lenHTTP != loaderLenHTTP)||(lenTotal != imgLen)) {
			loaderLen = len;
			loaderLenHTTP = lenHTTP;
			imgLen = lenTotal;
			SETSYMBOL(&ap[0], s_monitor);
			SETFLOAT(&ap[1], len);
			SETFLOAT(&ap[2], lenHTTP);
			SETFLOAT(&ap[3], lenTotal);
			queueToSelfPd(4, ap);
		}
	}
	
	if(file && (file != displayedFile)) {
		if(image) pofIm::letImage(image);
		//image = pofIm::getImage(file);
		image = pofIm::getImage(makefilename(file, pdcanvas));
		displayedFile = file;
	}
	if(image) {
		image->load();
		if(image->loaded) {
			if(!image->im.isAllocated()) 
			    image->im.allocate(width!=0?width:1, 
			        height!=0?height:width!=0?width:1, 
			        OF_IMAGE_COLOR_ALPHA);
            w = image->im.getWidth();
			h = image->im.getHeight();
			if(name) pofBase::textures[name] = &image->im.getTexture();

		} else w = h = 0;
	} else w = h = 0;
	
	if( (w != imWidth) || (h != imHeight) ) {
		SETSYMBOL(&ap[0], s_size);
		SETFLOAT(&ap[1], w);
		SETFLOAT(&ap[2], h);
		queueToSelfPd(3, ap);
		
		imWidth = w;
		imHeight = h;
	}
}

void pofImage::draw()
{
	float w = width, h = height;
	float sx = subx, sy = suby;
	float sw = subwidth, sh = subheight;

	Update();

	if(image && image->loaded) {
		if(isTexture) image->bind();
		else {
			if(w == 0) {
				if(h != 0) w = imWidth * h / imHeight;
				else w = imWidth;
			}
			if(h == 0) {
				if(w != 0) h = imHeight * w / imWidth;
				else h = imHeight;
			}
			if(sw != 0 || sh != 0 || sx != 0 || sy != 0) {
				if(sx < 0) sx += imWidth;
				if(sy < 0) sy += imHeight;
				if(sw <= 0) sw += imWidth;
				if(sh <= 0) sh += imHeight;
				image->drawsub(0, 0, w, h, sx, sy, sw, sh, quality);
			} else image->draw(0, 0, w, h, quality);
		}
	}
}

void pofImage::postdraw()
{
	if(isTexture) image->unbind();
}

void pofImage::set(t_symbol *f)
{
	file = f ;//makefilename(f, pdcanvas);
}

void pofImage::reserve(t_symbol *f)
{
	t_symbol *file = makefilename(f, pdcanvas);
	if(!file) return;
	
	pofIm* im = pofIm::getImage(file);
	im->load();
	
	std::list<pofIm*>::iterator it = reserved.begin();
	
	while(it != reserved.end()) {
		if(im == *it) break;
		it++;
	}
	
	if(it == reserved.end()) {
		reserved.push_back(im);
	} else pofIm::letImage(im);
}

void pofImage::unreserve(t_symbol *f)
{
	t_symbol *file = makefilename(f, pdcanvas);
	if(!file) return;
	pofIm *im = pofIm::findImage(file);
	if(im) {
		pofIm::letImage(im);
		reserved.remove(im);
	}	
}

void pofImage::unreserveAll()
{
	std::list<pofIm*>::iterator it = reserved.begin();
	
	while(it != reserved.end()) {
		pofIm::letImage(*it);
		it++;
	}
	
	reserved.clear();
}

void pofImage::message(int argc, t_atom *argv)
{
	t_atom ap[4];

	t_symbol *key = atom_getsymbol(argv); 
	argv++; argc--;

	if(image == NULL) return;

	if(key == s_save) {
		if(argc && argv->a_type == A_SYMBOL) {
			t_symbol *filename = atom_getsymbol(argv);
			image->im.save(filename->s_name);
			SETSYMBOL(&ap[0], s_saved);
			SETSYMBOL(&ap[1], filename);
			queueToSelfPd(2, ap);
		}
	} 
	else if(key == s_setcolor) {
		if(argc > 5) {
			t_float X=0.0, Y=0.0, R=0.0, G=0.0, B=0.0, A=1.0;	 	
			X = atom_getfloat(&argv[0]);
			Y = atom_getfloat(&argv[1]);
			R = atom_getfloat(&argv[2]);
			G = atom_getfloat(&argv[3]);
			B = atom_getfloat(&argv[4]);
			A = atom_getfloat(&argv[5]);

			if(image->loaded) {
				image->im.setColor(ofClamp(X, 0, image->im.getWidth()-1), ofClamp(Y, 0, image->im.getHeight()-1), 
					ofColor(R*255.0, G*255.0, B*255.0, A*255.0));
				image->needUpdate = true;
			}
		} else if(argc == 4 && argv[3].a_type == A_POINTER) {
			unsigned int X = (int)atom_getfloat(&argv[0]);
			unsigned int Y = (int)atom_getfloat(&argv[1]);
			unsigned int length = (int)atom_getfloat(&argv[2]);
			t_float *colors = (t_float *)argv[3].a_w.w_gpointer;
			unsigned int i;
			unsigned int W = image->im.getWidth();
			unsigned int H = image->im.getHeight();
			if(X >= W || Y >= H) return;
			for(i = 0; i < length ; i++) {
				image->im.setColor(X, Y, ofColor(
					ofClamp(colors[4 * i], 0, 1) * 255.0, ofClamp(colors[4 * i + 1], 0, 1) * 255.0, 
					ofClamp(colors[4 * i + 2], 0, 1) * 255.0, ofClamp(colors[4 * i + 3], 0, 1) * 255.0));
				X++;
				if(X >= W) {
					X = 0;
					Y++;
					if(Y >= H) break;
				}
			}
			freebytes(colors, 4 * length * sizeof(t_float));
			image->needUpdate = true;
		}
	} 
	else if(key == s_resize) {
		if(argc < 1) return;
		if(argc == 1) image->im.resize(atom_getfloat(&argv[0]), atom_getfloat(&argv[0]));
		else image->im.resize(atom_getfloat(&argv[0]), atom_getfloat(&argv[1]));
	} 
	else if(key == s_crop) {
		if(argc < 4) return;
		image->im.crop(atom_getfloat(&argv[0]), atom_getfloat(&argv[1]), atom_getfloat(&argv[2]), atom_getfloat(&argv[3]));
	} 
	else if(key == s_grab) {
		if(argc < 4) return;
		image->im.grabScreen(atom_getfloat(&argv[0]), atom_getfloat(&argv[1]), atom_getfloat(&argv[2]), atom_getfloat(&argv[3]));
	}
	else if(key == s_clear) {
		float R = 0, G = 0, B = 0, A = 1;
		if(argc > 0) R = atom_getfloat(&argv[0]);
		if(argc > 1) G = atom_getfloat(&argv[1]);
		if(argc > 2) B = atom_getfloat(&argv[2]);
		if(argc > 3) A = atom_getfloat(&argv[3]);
		image->im.setColor(ofColor(R*255.0, G*255.0, B*255.0, A*255.0));
		image->needUpdate = true;
	}
	else if(key == s_grabfbo) {
		if(argc < 1 || argv->a_type != A_SYMBOL) return;
		pofsubFbo* sub = pofsubFbo::get(atom_getsymbol(argv));
		sub->fbo->readToPixels(image->im.getPixels());
		pofsubFbo::let(sub);
		image->needUpdate = true;
	}
	else if(key == s_reload) {
		image->reload();
        image->needUpdate = true;
	}
	else if(key == s_loadfile) {
		if(argc < 1 || argv->a_type != A_SYMBOL) return;
		image->loadfile(atom_getsymbol(argv));
		image->needUpdate = true;
	}
    else if(key == s_settype) {
        if(argc < 1 || argv->a_type != A_SYMBOL) return;
        t_symbol *sym = atom_getsymbol(argv);
        if(sym == s_RGB) image->im.setImageType(OF_IMAGE_COLOR);
        else if(sym == s_RGBA) image->im.setImageType(OF_IMAGE_COLOR_ALPHA);
        else if(sym == s_GRAY) image->im.setImageType(OF_IMAGE_GRAYSCALE);
        image->needUpdate = true;
    }
}
