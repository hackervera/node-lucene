#include <iostream>
#include <v8.h>
#include <node.h>
//Thanks bnoordhuis and jerrysv from #node.js


#include <sstream>

#include <CLucene.h>
#include "Misc.h"
#include "repl_tchar.h"
#include "StringBuffer.h"

using namespace node;
using namespace v8;
using namespace std;
using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::store;
using namespace lucene::document;
using namespace lucene::search;
using namespace lucene::queryParser;

const static int CL_MAX_DIR = 220;
struct SearchData {
   const char* path;
   const char* score;
};

int arrsize;



SearchData *SearchFilesC(const char* index, const char* fobizzle){

    standard::StandardAnalyzer analyzer;
    char line[80];
    TCHAR tline[80];
    TCHAR* buf;

    IndexReader* reader = IndexReader::open(index);
  
        //printf("Enter query string: ");
        strncpy(line,fobizzle,80);
        //line[strlen(line)-1]=0;
        

        IndexReader* newreader = reader->reopen();
        if ( newreader != reader ){
            _CLLDELETE(reader);
            reader = newreader;
        }
        IndexSearcher s(reader);


        
        STRCPY_AtoT(tline,line,80);
        Query* q = QueryParser::parse(tline,_T("contents"),&analyzer);
        
        buf = q->toString(_T("contents"));
        
        _tprintf(_T("Searching for: %S\n\n"), buf);
        _CLDELETE_LCARRAY(buf);

        uint64_t str = Misc::currentTimeMillis();
        Hits* h = s.search(q);
        uint32_t srch = (int32_t)(Misc::currentTimeMillis() - str);
        str = Misc::currentTimeMillis();
        arrsize = h->length();
        SearchData *search = new SearchData[h->length()];
        for ( size_t i=0; i < h->length(); i++ ){
            Document* doc = &h->doc(i);
            //const TCHAR* buf = doc.get(_T("contents"));
            _tprintf(_T("%d. %S - %f\n"), i, doc->get(_T("path")), h->score(i));
            //const TCHAR* wtfbatman;
            //wtfbatman =  doc->get(_T("path"));
            //search[(int)i].score =  h->score(i);
            //printf("Adding %S %d\n", search[i].path, i);
            char *wtfbbq;
            wtfbbq = new char[100];
            sprintf(wtfbbq,"%S %f", doc->get(_T("path")), h->score(i));
            search[(int)i].path = wtfbbq;
            //sprintf(str,"%S", String::New((char*)doc->get(_T("path")),5));
            //printf("PIZZA %s\n", wtfbbq);
            //sprintf(search[i].path,"%S",(const char*)doc->get(_T("path")));
            //printf("segfault");
            //strcpy(search[i].path,(const char*)doc->get(_T("path")));
        
        }


        printf("\n\nSearch took: %d ms.\n", srch);
        printf("Screen dump took: %d ms.\n\n", (int32_t)(Misc::currentTimeMillis() - str));

        //_CLLDELETE(h);
        //_CLLDELETE(q);

        //s.close();

    //reader->close();
    //_CLLDELETE(reader);
    //printf("Testing %S\n\n", search[0].path);
    return search;
};

void FileDocument(const char* f, Document* doc){

    TCHAR tf[CL_MAX_DIR];
    STRCPY_AtoT(tf,f,CL_MAX_DIR);
    doc->add( *_CLNEW Field(_T("path"), tf, Field::STORE_YES | Field::INDEX_UNTOKENIZED ) );

    FILE* fh = fopen(f,"r");
	if ( fh != NULL ){
		StringBuffer str;
		char abuf[1024];
		TCHAR tbuf[1024];
		size_t r;
		do{
			r = fread(abuf,1,1023,fh);
			abuf[r]=0;
			STRCPY_AtoT(tbuf,abuf,r);
			tbuf[r]=0;
			str.append(tbuf);
		}while(r>0);
		fclose(fh);

		doc->add( *_CLNEW Field(_T("contents"), str.getBuffer(), Field::STORE_YES | Field::INDEX_TOKENIZED) );
	}
}

void indexDocs(IndexWriter* writer, const char* directory) {
    vector<string> files;
    std::sort(files.begin(),files.end());
    Misc::listFiles(directory,files,true);
    vector<string>::iterator itr = files.begin();
    
    // Re-use the document object
    Document doc;
    int i=0;
    while ( itr != files.end() ){
        const char* path = itr->c_str();
        printf( "adding file %d: %s\n", ++i, path );

        doc.clear();
        FileDocument( path, &doc );
        writer->addDocument( &doc );
        ++itr;
    }
};

class Lucene : public ObjectWrap
{

static Persistent<FunctionTemplate> s_ct;
private:
int m_count;
v8::Local<v8::Array> junk;




public:
    
    static void Init(Handle<Object> target)
  {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);

    s_ct = Persistent<FunctionTemplate>::New(t);
    s_ct->InstanceTemplate()->SetInternalFieldCount(1);
    s_ct->SetClassName(String::NewSymbol("Lucene"));

    NODE_SET_PROTOTYPE_METHOD(s_ct, "hello", Hello);
    NODE_SET_PROTOTYPE_METHOD(s_ct, "index", IndexFiles);
    NODE_SET_PROTOTYPE_METHOD(s_ct, "search", SearchFiles);

    target->Set(String::NewSymbol("Lucene"),
                s_ct->GetFunction());
  }
  Lucene() :
    m_count(0)
  {
  }

  ~Lucene()
  {
  }
  static Handle<Value> New(const Arguments& args)
  {
    HandleScope scope;
    Lucene* lucene = new Lucene();
    lucene->Wrap(args.This());
    return args.This();
  }
  static Handle<Value> Hello(const Arguments& args)
  {
    HandleScope scope;
    Lucene* lucene = ObjectWrap::Unwrap<Lucene>(args.This());
    lucene->m_count++;
    Local<String> result = String::New("Hello World");
    return scope.Close(result);
  }
  
  static Handle<Value> IndexFiles(const Arguments& args)
  {
    HandleScope scope;
	IndexWriter* writer = NULL;
	lucene::analysis::WhitespaceAnalyzer an;
	
	if (IndexReader::indexExists(*String::Utf8Value(args[1])) ){
		if ( IndexReader::isLocked(*String::Utf8Value(args[1])) ){
			printf("Index was locked... unlocking it.\n");
			IndexReader::unlock(*String::Utf8Value(args[1]));
		}

		writer = _CLNEW IndexWriter( *String::Utf8Value(args[1]), &an, false);
	}else{
		writer = _CLNEW IndexWriter( *String::Utf8Value(args[1]) ,&an, true);
	}

    //writer->setInfoStream(&std::cout);

    // We can tell the writer to flush at certain occasions
    //writer->setRAMBufferSizeMB(0.5);
    //writer->setMaxBufferedDocs(3);

    // To bypass a possible exception (we have no idea what we will be indexing...)
    writer->setMaxFieldLength(0x7FFFFFFFL); // LUCENE_INT32_MAX_SHOULDBE
    
    // Turn this off to make indexing faster; we'll turn it on later before optimizing
    writer->setUseCompoundFile(false);

	uint64_t str = Misc::currentTimeMillis();

	indexDocs(writer, *String::Utf8Value(args[0]));
	
    // Make the index use as little files as possible, and optimize it
    writer->setUseCompoundFile(true);
    writer->optimize();
	
    // Close and clean up
    writer->close();
	_CLLDELETE(writer);

	printf("Indexing took: %d ms.\n\n", (int32_t)(Misc::currentTimeMillis() - str));IndexWriter(*String::Utf8Value(args[1]), &an, false);
    
    //Lucene* lucene = ObjectWrap::Unwrap<Lucene>(args.This());

    return scope.Close(String::New("foo"));
  }

  static Handle<Value> SearchFiles(const Arguments& args)
  {
    HandleScope scope;
    //Lucene* lucene = ObjectWrap::Unwrap<Lucene>(args.This());

    //lucene->paths->Set(Number::New(0),String::New("foo"));
    SearchData *search = SearchFilesC(*String::Utf8Value(args[0]), *String::Utf8Value(args[1]));
    v8::Local<v8::Array> ar = v8::Array::New(0);
    //global vars ftw
    for (int i = 0; i < arrsize; i++) {
       //strcpy(utf,search[i].path);
        ar->Set(v8::Number::New(i), String::New(search[i].path));
    }

    v8::Context::GetCurrent()->Global()->Set(v8::String::New("fiz"), ar);
    return scope.Close(ar);
    
  }

    
};

Persistent<FunctionTemplate> Lucene::s_ct;

extern "C" {

  static void init (Handle<Object> target)
  {
    Lucene::Init(target);
  }

  NODE_MODULE(lucene_bindings, init);
}
