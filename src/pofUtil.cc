/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofUtil.h"
#include "ofxZipPass.h"
#include "ofURLFileLoader.h"

static t_class *pofutil_class;
static t_symbol *s_download, *s_out, *s_unzip, *s_done, *s_error;
static t_symbol *s_rmfile,*s_rmdir,*s_mkdir,*s_renamedir,*s_movefile,*s_copyfile;


static void *pofutil_new(t_symbol *s, int argc, t_atom *argv)
{
	pofUtil* obj = new pofUtil(pofutil_class);

	canvas_getargs(&obj->pargc, &obj->pargv);
	obj->pdcanvas = canvas_getcurrent();

	if((argv->a_type == A_SYMBOL)&&(atom_getsymbol(argv) == gensym("sync")))
		obj->sync = true;

	return (void*) (obj->pdobj);
}

static void pofutil_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);

	if((argc>1) && argv->a_type == A_SYMBOL)
		outlet_anything(px->m_out1, atom_getsymbol(argv), argc-1, argv+1);
}

static void pofutil_getdir(void *x)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	t_atom ap;

	SETSYMBOL(&ap, canvas_getdir(px->pdcanvas));
	outlet_anything(px->m_out1, gensym("getdir"), 1, &ap);
}

static void pofutil_getdatapath(void *x)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	t_atom ap;

	SETSYMBOL(&ap, gensym(ofToDataPath("", true).c_str()));
	outlet_anything(px->m_out1, gensym("getdatapath"), 1, &ap);
}

static void pofutil_time(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	t_binbuf *bb = binbuf_new();
	char *buf;
	int buflen;
	t_atom ap;

	binbuf_add(bb, argc, argv);
	binbuf_gettext(bb, &buf, &buflen);

	string str(buf, buflen);
	if(buflen>0) SETSYMBOL(&ap, gensym(ofGetTimestampString(str).c_str()));
	else SETSYMBOL(&ap, gensym(ofGetTimestampString().c_str()));

	binbuf_free(bb);
	freebytes(buf, buflen);

	outlet_anything(px->m_out1, gensym("time"), 1, &ap);
}

static void pofutil_exists(void *x, t_symbol *path)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	ofFile file(path->s_name);
	t_atom at;

	if(!file.exists()) SETSYMBOL(&at, gensym("no"));
	else if(file.isDirectory()) SETSYMBOL(&at, gensym("dir"));
	else if(file.isFile()) SETSYMBOL(&at, gensym("file"));
	else SETSYMBOL(&at, gensym("other")); // ??

	outlet_anything(px->m_out1, gensym("exists"), 1, &at);
}

static void pofutil_fileinfo(void *x, t_symbol *path)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	ofFile file(path->s_name);
	t_atom at[4];

	if(!file.exists()) {
		SETSYMBOL(&at[0], gensym("error"));
		outlet_anything(px->m_out1, gensym("fileinfo"), 1, at);
		return;
	}

	if(file.isDirectory()) SETSYMBOL(&at[0], gensym("dir"));
	else if(file.isFile()) SETSYMBOL(&at[0], gensym("file"));
	else SETSYMBOL(&at[0], gensym("other")); // ??

	SETFLOAT(&at[1], std::filesystem::last_write_time(file) / 86400); // days from 1970
	SETFLOAT(&at[2], std::filesystem::last_write_time(file) % 86400); // seconds in the day

	if(file.isFile()) {
		SETFLOAT(&at[3], file.getSize());
		outlet_anything(px->m_out1, gensym("fileinfo"), 4, at);
	}
	else outlet_anything(px->m_out1, gensym("fileinfo"), 3, at);
}

static void pofutil_dollarg(void *x)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	if(px->pargc) outlet_anything(px->m_out1, gensym("dollarg"), px->pargc, px->pargv); 
}

static void pofutil_listdir(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	t_symbol *s_dir;

	if((argc>0)&&(argv->a_type == A_SYMBOL)) {
		s_dir = atom_getsymbol(argv); 
		ofDirectory dir(s_dir->s_name);
		argc-- ; argv++;

		while((argc>0)&&(argv->a_type == A_SYMBOL)) {
			dir.allowExt(atom_getsymbol(argv)->s_name);
			argc-- ; argv++;
		}

		int n = dir.listDir();
		dir.sort();
		t_atom *at = (t_atom *)getbytes((n+1) * sizeof(t_atom) );
		SETSYMBOL(at, s_dir);
		for(int i=0 ; i<n ; i++) SETSYMBOL(&at[i+1], gensym(dir.getName(i).c_str()));
		outlet_anything(px->m_out1, gensym("listdir"), n+1, at);
	}
}

static void pofutil_setstring(void *x, t_symbol *id, t_symbol *value)
{
	pofBase::setString(id->s_name, value->s_name);
}

static void pofutil_stringfind(void *x, t_symbol *src, t_symbol *sub)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	t_atom at;
	SETFLOAT(&at, (int)string(src->s_name).find(sub->s_name));
	outlet_anything(px->m_out1, gensym("stringfind"), 1, &at);
}

static void pofutil_dirbaseext(void *x, t_symbol *file)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	t_atom at[3];

	SETSYMBOL(&at[0], gensym(ofFilePath::getEnclosingDirectory(file->s_name).c_str()));
	SETSYMBOL(&at[1], gensym(ofFilePath::getBaseName(file->s_name).c_str()));
	SETSYMBOL(&at[2], gensym(ofFilePath::getFileExt(file->s_name).c_str()));
	
	outlet_anything(px->m_out1, gensym("dirbaseext"), 3, at);
}

static void pofutil_fileops(void *x, t_symbol *command, int argc, t_atom *argv, bool sync)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	float errcode = 0;

	t_atom at[4];
	SETSYMBOL(&at[0], s_out);
	SETSYMBOL(&at[1], command);
	SETSYMBOL(&at[2], s_error);
	SETFLOAT(&at[3], errcode);

	if(command == s_rmfile) {
		if((argc>0) && (argv->a_type == A_SYMBOL)) {
			if(ofFile::removeFile(atom_getsymbol(argv)->s_name, 
				false)) // bRelativeToData
			SETSYMBOL(&at[2], s_done);
		}
	}
	else if(command == s_rmdir) {
		if((argc>0) && (argv->a_type == A_SYMBOL)) {
			if(ofDirectory::removeDirectory(atom_getsymbol(argv)->s_name, 
				true, 		//deleteIfNotEmpty
				false)) 	// bRelativeToData
			SETSYMBOL(&at[2], s_done);
		}
	}
	else if(command == s_mkdir) {
		if((argc>0) && (argv->a_type == A_SYMBOL)) {
			if(ofDirectory::createDirectory(atom_getsymbol(argv)->s_name, 
				false, 	// bRelativeToData
				true))	// recursive
			SETSYMBOL(&at[2], s_done);
		}
	}
	else if(command == s_renamedir) {
		if((argc>1) && (argv->a_type == A_SYMBOL) && ((argv+1)->a_type == A_SYMBOL)) {
			if(ofDirectory(atom_getsymbol(argv)->s_name).renameTo(atom_getsymbol(argv+1)->s_name, 
				false, 	// bRelativeToData
				true)) 	//overwrite
			SETSYMBOL(&at[2], s_done);
		}
	}
	else if(command == s_movefile) {
		if((argc>1) && (argv->a_type == A_SYMBOL) && ((argv+1)->a_type == A_SYMBOL)) {
			if(((argc<3) || (atom_getfloat(argv+2) == 0)) && ofFile::doesFileExist(atom_getsymbol(argv+1)->s_name, false))
				goto end; // error:don't overwrite
			if(ofFile(atom_getsymbol(argv)->s_name).moveTo(atom_getsymbol(argv+1)->s_name,
					false,											// bRelativeToData
					argc>2 ? atom_getfloat(argv+2) != 0 : false))	//overwrite
				SETSYMBOL(&at[2], s_done);
		}
	}
	else if(command == s_copyfile) {
		if((argc>1) && (argv->a_type == A_SYMBOL) && ((argv+1)->a_type == A_SYMBOL)) {
			if((ofFile(atom_getsymbol(argv)->s_name) == ofFile(atom_getsymbol(argv+1)->s_name)) // silently skip if same path
			|| (ofFile(atom_getsymbol(argv)->s_name).copyTo(atom_getsymbol(argv+1)->s_name,
					false,											// bRelativeToData
					argc>2 ? atom_getfloat(argv+2) != 0 : false)))	//overwrite
				SETSYMBOL(&at[2], s_done);
		}
	}

	end:
	
	if(sync) pofutil_out(x, NULL, 3, at + 1);
	else px->queueToSelfPd(4, at);
}

//--------------------- sys thread (for asynchronous operations : mkdir rmdir renamedir rmfile...) ----------------
class pofSysThread : public ofThread {
	public :
	void *obj;
	t_symbol *command;
	t_binbuf *binbuf;
	bool running;
	bool error;

	pofSysThread(void *x, t_symbol *s, int ac, t_atom *av) : 
 		obj(x), command(s), binbuf(NULL), running(false){
		binbuf = binbuf_new();
		binbuf_add(binbuf, ac, av);
	}

	~pofSysThread() {
		binbuf_free(binbuf);
	}

	void threadedFunction(){
		running = true;
		int argc = binbuf_getnatom(binbuf);
		t_atom *argv = binbuf_getvec(binbuf);

		pofutil_fileops(obj, command, argc, argv, false);
		running = false;
		return;
	}
};

static void pofutil_thread(void *x, t_symbol *s, int argc, t_atom *argv)
{
	t_atom at;
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);

	if(px->sync) pofutil_fileops(x, s, argc, argv, true);
	else {
		if(px->systhread != NULL && px->systhread->running) {
			pd_error(x, "pofutil %s : async thread is already running !", s->s_name);
			SETSYMBOL(&at, gensym("error_running"));
			outlet_anything(px->m_out1, s, 1, &at);
			return;
		}

		if(px->systhread) delete px->systhread;

		px->systhread = new pofSysThread(x, s, argc, argv);
		px->systhread->startThread();
	}
}

static void pofutil_sync(void *x, t_float sync)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	if(sync != 0) px->sync = true;
	else px->sync = false;
}

//----------------- Unzip ------------------------

class pofUnzipper : public ofThread {
	public :
	void *obj;
	string file;
	string path;
	string passwd;
	bool running;
	bool error;

	pofUnzipper(void *x, string _file, string _path, string _passwd) : 
		obj(x), file(_file), path(_path), passwd(_passwd), running(false){
	}
	
	~pofUnzipper() {
	}
	
	void threadedFunction(){
		float errcode = 0;
		running = true;
		t_atom at[4];
		SETSYMBOL(&at[0], s_out);
		SETSYMBOL(&at[1], s_unzip);

		ofxUnzipPass zip(file, passwd);
		if(zip.isOk()) {
			errcode = 1;
			if(zip.unzipTo(path)) SETSYMBOL(&at[2], s_done);
			else SETSYMBOL(&at[2], s_error);
		}
		else SETSYMBOL(&at[2], s_error);
		
		SETFLOAT(&at[3], errcode);
	
		((pofUtil*)obj)->queueToSelfPd(4,at);
		running = false;
		return;
	}
};

static void pofutil_unzip(void *x, t_symbol *zipfile, t_symbol *path, t_symbol *password)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	zipfile = makefilename(zipfile, px->pdcanvas);

	if(px->unzipper != NULL && px->unzipper->running) return;
	if(px->unzipper) delete px->unzipper;
	
	if (!password || !*password->s_name) password = gensym("");
	if(password->s_name[0]=='@') password = gensym(pofBase::getString(password->s_name + 1).c_str());

	px->unzipper = new pofUnzipper(px, zipfile->s_name, path->s_name, password->s_name);
	px->unzipper->startThread();
}

/* FileDownload : */
static void pofutil_download_tick(void *x);

class pofFileDownloader : public ofThread {
	public :
	void *obj;
	string url;
	string file;
	bool done;
	bool error;
	string redirection;
	t_clock *x_clock;

	pofFileDownloader(void *x, string _url, string _file) : obj(x), url(_url), file(_file), done(false), error(false), redirection("") {
		x_clock = clock_new(x, (t_method)pofutil_download_tick);
		clock_delay(x_clock, 500);
	}
	
	virtual ~pofFileDownloader() {
		clock_free(x_clock);
	}

	void threadedFunction(){
		try {
			ofHttpResponse response = ofSaveURLTo(url, file);
			if (response.status == 200) done = true;
			else error = true;
			return;
		} catch (const Exception& exc) {
			ofLogError("pofUtil_download") << "handleRequest(): "+ exc.displayText();
			error = true;
			return;
		}
		error = true;
		return;
	}

	float getMb() {
		if(ofFile(file).exists())
		return (ofFile(file).getSize()/(float)1000000);
		else return 0.0;
	}
};

static void pofutil_download_tick(void *x)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	t_atom at[2];
	
	if(px->fileDownloader == NULL) return;
	if(px->fileDownloader->done) {
		if(px->fileDownloader->redirection.size() != 0) {
			SETSYMBOL(at, gensym("redirect"));
			SETSYMBOL(at + 1, gensym(px->fileDownloader->redirection.c_str()));
			outlet_anything(px->m_out1, s_download, 2, at);
		}
		SETSYMBOL(at, s_done);
		outlet_anything(px->m_out1, s_download, 1, at);
		delete px->fileDownloader;
		px->fileDownloader = NULL;
	} else if(px->fileDownloader->error){
		SETSYMBOL(at, s_error);
		outlet_anything(px->m_out1, s_download, 1, at);
		delete px->fileDownloader;
		px->fileDownloader = NULL;
	} else {
		SETSYMBOL(at, gensym("Mb"));
		SETFLOAT(at + 1, px->fileDownloader->getMb());
		outlet_anything(px->m_out1, s_download, 2, at);
		clock_delay(px->fileDownloader->x_clock, 500);
	}
}

static void pofutil_download(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	t_atom at;
	t_symbol *url, *path, *pre_url;

	if(argc && (argv->a_type == A_SYMBOL)) {
		url = atom_getsymbol(argv); 
		argc-- ; argv++;
	} else return;

	if(argc && (argv->a_type == A_SYMBOL)) {
		path = atom_getsymbol(argv); 
		argc-- ; argv++;
	} else return;

	if(argc && (argv->a_type == A_SYMBOL)) {
		pre_url = atom_getsymbol(argv); 
		argc-- ; argv++;
	} else pre_url = gensym("");

	if(pre_url->s_name[0] == '@') pre_url = gensym(pofBase::getString(&(pre_url->s_name[1])).c_str());

	string urlStr = string(pre_url->s_name) + string(url->s_name);

	if(px->fileDownloader != NULL) {
		SETSYMBOL(&at, gensym("error_running"));
		outlet_anything(px->m_out1, s_download, 1, &at);
		return;
	}

	(px->fileDownloader = new pofFileDownloader(x, urlStr, path->s_name))->startThread();

	SETSYMBOL(&at, gensym("start"));
	outlet_anything(px->m_out1, s_download, 1, &at);
}

static void pofutil_abort_download(void *x)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);
	t_atom at;

	if(px->fileDownloader) {
		px->fileDownloader->waitForThread(true);
		delete px->fileDownloader;
		px->fileDownloader = NULL;
		SETSYMBOL(&at, gensym("aborted"));
		outlet_anything(px->m_out1, s_download, 1, &at);
	}
}

static void pofutil_free(void *x)
{
	pofUtil* px= (pofUtil*)(((PdObject*)x)->parent);

	if(px->fileDownloader) {
		px->fileDownloader->waitForThread(true);
		delete px->fileDownloader;
	}
	if(px->unzipper) {
		px->unzipper->waitForThread(true);
		delete px->unzipper;
	}
	delete px;
}

void pofUtil::setup(void)
{
	//post("pofutil_setup");
	s_download = gensym("download");
	s_out = gensym("out");
	s_unzip = gensym("unzip");
	s_done = gensym("done");
	s_error = gensym("error");

	s_rmfile =  gensym("rmfile");
	s_rmdir =  gensym("rmdir");
	s_mkdir =  gensym("mkdir");
	s_renamedir =  gensym("renamedir");
	s_movefile =  gensym("movefile");
	s_copyfile =  gensym("copyfile");
	
	pofutil_class = class_new(gensym("pofutil"), (t_newmethod)pofutil_new, (t_method)pofutil_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);

	class_addmethod(pofutil_class, (t_method)pofutil_out, s_out, A_GIMME, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_getdir, gensym("getdir"), A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_getdatapath, gensym("getdatapath"), A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_time, gensym("time"), A_GIMME, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_dollarg, gensym("dollarg"), A_NULL);

	class_addmethod(pofutil_class, (t_method)pofutil_thread, s_rmfile, A_GIMME, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_thread, s_rmdir, A_GIMME, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_thread, s_mkdir, A_GIMME, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_thread, s_renamedir, A_GIMME, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_thread, s_movefile, A_GIMME, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_thread, s_copyfile, A_GIMME, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_sync, gensym("sync"), A_FLOAT, A_NULL);

	class_addmethod(pofutil_class, (t_method)pofutil_listdir, gensym("listdir"), A_GIMME, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_exists, gensym("exists"), A_SYMBOL, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_fileinfo, gensym("fileinfo"), A_SYMBOL, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_dirbaseext, gensym("dirbaseext"), A_SYMBOL, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_setstring, gensym("setstring"), A_SYMBOL, A_SYMBOL, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_stringfind, gensym("stringfind"), A_SYMBOL, A_SYMBOL, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_unzip, gensym("unzip"), A_SYMBOL, A_SYMBOL, A_DEFSYMBOL, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_download, s_download, A_GIMME, A_NULL);
	class_addmethod(pofutil_class, (t_method)pofutil_abort_download, gensym("abort_download"), A_NULL);
}


