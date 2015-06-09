#include "zvalue.hpp"

#include <cstdio>
#include <iostream>


void zobject::freeZObjects(zobjects_holder* holder)
{
    if (holder)
    {
    while (holder->getSize() > 0)
    {
      zvalue z=holder->pop();

      ZVALUE_TO_ZOBJECT((z))->annihilate();
    }
    }
}

zobject::zobject(zobjects_holder* holder, e_type a_type): type(a_type)
{
    if (holder)
    {
      /*
      char s[64];

      sprintf(s, "zobject(): %p", this);

      switch(a_type)
      {
      case EZOBJECT: strcat(s, " EZOBJECT"); break;
      case EZSYMBOL: strcat(s, " EZSYMBOL"); break;
      case EZPAIR: strcat(s, " EZPAIR"); break;
      case EZHASH: strcat(s, " EZHASH"); break;
      case EZENVIRONMENT: strcat(s, " EZENVIRONMENT"); break;
      case EZVECTOR: strcat(s, " EZVECTOR"); break;
      case EZSYMBOLTABLE: strcat(s, " EZSTABLE"); break;
      case EZSYNTREE: strcat(s, " EZSYNTREE"); break;
      }

      Logger::write("LEAK", s);
      */

      holder->push(this);
    }
    if (!holder) {
      //std::cerr << "TUT\n";
    }
}

char* zvector::_to_string() {
    zvector* v = zvector::generate(EMPTY_ZOBJECTS_HOLDER);
    int length_sum = 0;
    int i;
    for (i=0; i<occupied; ++i) {
        char* s = zvalue_to_string(block[i]);
        length_sum += strlen(s)+2; // ", "
        v->add(s);
    }
    if (length_sum>0) length_sum-=2; // last ", "
    char* result = new char[length_sum+4]; // "[ " + "]" + NULL
    strcpy(result, "[ ");
    if (0<v->getSize())
    {
        char *s = (char*)(v->elementAt(0));
        strcat(result, s);
        delete s;
    }
    for (i=1; i<v->getSize(); ++i) {
        strcat(result, ", ");
        char *s = (char*)(v->elementAt(i));
        strcat(result, s);
        delete s;
    }
    strcat(result, "]");
    v->annihilate();
    return result;
}

std::string zvector::_to_parsable_string() {
    std::string result;
    result += "[ ";
    if (0<occupied) {
        result += zvalue_to_parsable_string(block[0]);
    }
    for (int i=1; i<occupied; ++i) {
        result += ", ";
        result += zvalue_to_parsable_string(block[i]);
    }
    result += " ]";
    return result;
}

char* zhash::_to_string() {
#define EZHASH_PRINT(result) \
  { \
    zvector* v = zvector::generate(EMPTY_ZOBJECTS_HOLDER); \
    int length_sum = 0; \
    int block_size = 0; \
    zvalue* block = getAll(block_size); \
    int i; \
    for (i=0; i<block_size; ++i) { \
        if ( (!NULLP(block[i]) && !DELETEDP(block[i])) && (ADDCON(block[i])) ) { \
            char* s = NULL; \
            GETSTRING(s, block[i]) \
            length_sum += strlen(s)+2; /* ", " */ \
            v->add(s); \
        } \
    } \
    if (length_sum>0) length_sum-=2; /* last ", " */ \
    result = new char[length_sum+4]; /* "{ " + "}" + NULL */ \
    strcpy(result, "{ "); \
    if (0<v->getSize()) \
    {\
        char *s = reinterpret_cast<char*>(v->elementAt(0)); \
        strcat(result, s); \
        delete s; \
    }\
    for (i=1; i<v->getSize(); ++i) { \
        strcat(result, ", "); \
        char *s = reinterpret_cast<char*>(v->elementAt(i)); \
        strcat(result, s); \
        delete s; \
    } \
    strcat(result, "}"); \
    v->annihilate(); \
  }

    char* result = NULL;
#define ADDCON(zv) 1
#define GETSTRING(s, zv) s=zvalue_to_string(zv);
    EZHASH_PRINT(result);
#undef ADDCON
#undef GETSTRING
    return result;
}

std::string zhash::_to_parsable_string() {
    return to_parsable_string_modif (false);
}

std::string zhash::to_parsable_string_modif (bool without_int_keys) {
    std::string result;
    int block_size = 0;
    zvalue* block = getAll(block_size);
    for (int i=0; i<block_size; ++i) {
        if ( (!NULLP(block[i]) && !DELETEDP(block[i])) ) {
            result += ", ";
            if (ZPAIRP(block[i])) {
                zpair* p = reinterpret_cast<zpair*>(block[i]);
                zvalue pf = p->getFirst();
                if (! (INTEGERP(pf) && without_int_keys) ) {
                    result += zvalue_to_parsable_string(pf);
                    result += " => ";
                    result += zvalue_to_parsable_string(p->getSecond());
                }
            } else {
                result += "q[conversion_error";
                char tmp[100];
                sprintf(tmp, "%i", i);
                result += tmp;
                result += "] => ";
                result += zvalue_to_parsable_string(block[i]);
            }
        }
    }
    result += " }";
    result.erase(0, 2);
    result.insert(0, "{ ");
    return result;
}

char* zenvironment::_to_string() {
    char* result = NULL;
#define ADDCON(zv) 1
#define GETSTRING(s, zv) s=reinterpret_cast<zpair*>(zv)->to_string();
    EZHASH_PRINT(result);
#undef ADDCON
#undef GETSTRING
    return result;
}

std::string zenvironment::_to_parsable_string() {
    return zhash::_to_parsable_string();
}

char* zsymboltable::_to_string() {
    char* result = NULL;
#define ADDCON(zv) 1
#define GETSTRING(s, zv) s=reinterpret_cast<zsymbol*>(zv)->_to_string();
    EZHASH_PRINT(result);
#undef ADDCON
#undef GETSTRING
    return result;
}

std::string zsymboltable::_to_parsable_string() {
    return std::string("q[ NOT IMPLEMENTED! ]");
}

//  /* old implementation of to_string()
//  case zobject::EZARRAY: {
//      zvector* v = zvector::generate();
//      int length_sum = 0;
//  int i;
//      for (i=0; i<((zarray*)z)->getSize(); ++i) {
//          char* s = zvalue_to_string((*((zarray*)z))[i]);
//          length_sum += strlen(s)+2; // ", "
//          v->add(s);
//      }
//      if (length_sum>0) length_sum-=2; // last ", "
//      result = new char[length_sum+4]; // "[ " + "]" + NULL
//      strcpy(result, "[ ");
//      if (0<v->getSize()) strcat(result, (char*)(v->elementAt(0)) );
//      for (i=1; i<v->getSize(); ++i) {
//          strcat(result, ", ");
//          char *s = (char*)(v->elementAt(i));
//          strcat(result, s);
//          delete s;
//      }
//      strcat(result, "]");
//                         } break;
//  */

std::string zsyntree::_to_parsable_string() {
    std::string result;
    char tmp[100];
    result+="{";
    result+="  q[nr_in_parent] => "; sprintf(tmp, "%i", number_in_parent_tree); result+=tmp;
    result+=", q[last_subtree] => "; sprintf(tmp, "%i", last_subtree); result+=tmp;
    if (is_variant)
    result+=", q[variant] => 1";
    result+=", q[category] => q|";
    if (NULL!=category) {
        if (const char* n=category->get_string()) result+=n;
    }
    result+="|";
    result+=", q[label] => q|";
    if (NULL!=label) {
        if (const char* n=label->get_string()) result+=n;
    }
    result+="|";
    std::string attr = zhash::to_parsable_string_modif (true);
    attr.erase(0, 1); attr.erase(attr.length()-1, 1); //obcinam klamerki
    if (attr.find("=>") != std::string::npos) { //jezeli sa w ogole jakies atrybuty
        result+=", "; result+=attr; //atrybuty sa na tym samym poziomie co category i label
    }
    result += ", q|subtrees| => [ ";
    if (0<=last_subtree) {
        result+= ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(0)))->to_parsable_string();
    }
    for (int i=1; i<=last_subtree; i++) {
        result+=", ";
        result+= ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(i)))->to_parsable_string();
    }
    result+=" ] ";
    result+=" }";
    return result;
}

char* zsyntree::_to_string(bool with_attributes, bool with_subtrees) {
    char* result = NULL;
    char* num = NULL;
    {
        char* tmp = NULL;
        if (-1!=number_in_parent_tree) {
            tmp=itoa(number_in_parent_tree);
            num = new char[strlen(tmp)+3]; // "(" + tmp + ")" + NULL
        } else num = new char[3];
        strcpy(num, "(");
        if (NULL!=tmp) {
            strcat(num, tmp);
            delete tmp;
        }
        strcat(num, ")");
    }
    const char *cat = NULL; const char *lab = NULL;
    int len = strlen(num);
    if (NULL!=category) {
        cat = category->get_string();
        len += strlen(cat);
    }
    if (NULL!=label) {
        lab = label->get_string();
        len += strlen(lab);
    }
    char* attributes = NULL;
    if (with_attributes) {
#define ADDCON(zv) ( ZPAIRP(zv) && !INTEGERP( (ZPAIRC(zv))->getFirst() ) )
#define GETSTRING(s, zv) \
    if (ZOBJECTP(zv) && ZSYNTREEP(zv)) s=ZSYNTREEC(zv)->_to_string(false, false); \
    else s=zvalue_to_string(zv);
        EZHASH_PRINT(attributes);
#undef GETSTRING
#undef ADDCON
        len += strlen(attributes);
    }
    result = new char[len +4]; // num + " " + cat + ":" + label + " " + attributes + NULL
    strcpy(result, num); delete num; num = NULL;
    strcat(result, " ");
    if (NULL!=cat) strcat(result, cat);
    strcat(result, ":");
    if (NULL!=lab) strcat(result, lab);
    strcat(result, " ");
    if (NULL!=attributes) strcat(result, attributes);
    if (with_subtrees && -1L<last_subtree) {
        char* my_self = result; result = NULL;
        zvector* v = zvector::generate(my_holder);
        int length_sum = strlen(my_self);
        int i;
        for (i=0; i<=last_subtree; ++i) {
            char* s = ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(i)))
                ->_to_string(with_attributes, with_subtrees);
            char* ns = new char[strlen(s)*3/2+4]; //pi*oko
            ns[strlen(s)*3/2+3]=0;
            if (i!=last_subtree) strncpy(ns, "+---", 4);
            else strncpy(ns, "`---", 4);
            char* sp = s, *np = ns+4;
            while ('\0'!=*sp) {
                *np = *sp;
                if (10==*sp) {
                    if (i!=last_subtree) strncpy(++np, "|   ", 4);
                    else strncpy(++np, "    ", 4);
                    np+=3;
                }
                ++sp; ++np;
            }
            length_sum += np-ns;
            char* fs = new char[np-ns+1];
            strncpy(fs, ns, np-ns);
            fs[np-ns] = 0;
            v->add(fs);
            delete ns;
            delete s;
        }
        result = new char[length_sum+last_subtree+2];
        strcpy(result, my_self);
        if (0<v->getSize()) {
            strcat(result, "\n");
            char *s = (char*)(v->elementAt(0));
            strcat(result, s);
            delete s;
        }
        for (i=1; i<v->getSize(); ++i) {
            strcat(result, "\n");
            char *s = (char*)(v->elementAt(i));
            strcat(result, s);
            delete s;
        }
        delete my_self;
    }
    delete attributes; attributes = NULL;
    return result;
}

char* itoa(int i)
{
    char buf[16];
    char *s = buf;
    int neg = 0;
    if (i<0) { i*=-1; neg=1; }
    if (0==i) { *s++ = '0'; }
    while (i>0) {
        *s++ = '0' + i%10;
        i/=10;
    }
    char* result = new char[s-buf+1+neg];
    char* r = result;
    if (neg) *r++ = '-';
    s--;
    while (s>=buf) *r++ = *s--;
    *r = 0;
    return result;
}

int standard_zvalue_hash_function(zvalue z)
{
    return reinterpret_cast<zint_t>(z);
}


int the_same_identity_function(zvalue z, zvalue t)
{
    return (z == t);
}


int env_val_hash_function(zvalue z)
{
    assert(ZPAIRP(z));
    return standard_zvalue_hash_function(reinterpret_cast<zpair*>(z)->getFirst());
}


int env_identity_function(zvalue k, zvalue v)
{
    assert(ZPAIRP(v));
    return k == reinterpret_cast<zpair*>(v)->getFirst();
}


int string_hash_function(const char* s)
{
    int h = 29830;
    while (*s) h = (h << 1) ^ ((int) *(s++));

    return (h < 0 ? -h : h);
}


int pchar_hash_function(zvalue z)
{
    if (ZWRAPPERP(z))
    return string_hash_function(reinterpret_cast<const char*>(z));

    return 0;
}

int zsymboltable_zsymbol_hash_function(zvalue z)
{
    assert(ZSYMBOLP(z));
    return string_hash_function(ZSYMBOLC(z)->get_string());
}


int zsymboltable_zsymbol_kv_function(zvalue k, zvalue v)
{
    assert(ZWRAPPERP(k));
    assert(ZSYMBOLP(v));
    return !strcmp(ZUNWRAP(const char*, k),
           ZSYMBOLC(v)->get_string());
}


void zenvironment::deleteAllKeyValue(zvalue s, zvalue d)
{
    int i = 0;
    zvalue* p;
    while ((*(p=get_(s, i))) != NULL_ZVALUE)
    {
    if (((zpair*)*p)->getSecond() == d)
    {
        --occupied;
        *p = DELETED_ZVALUE;
    }
    ++i;
    }
}

void zenvironment::deleteAllKey(zvalue s)
{
    int i = 0;
    zvalue* p;
    while ((*(p=get_(s, i))) != NULL_ZVALUE)
    {
    --occupied;
    *p = DELETED_ZVALUE;
    ++i;
    }
}

zsymboltable::zsymboltable(zobjects_holder* holder):
    zhash(holder, 1024, pchar_hash_function, zsymboltable_zsymbol_hash_function,
      zsymboltable_zsymbol_kv_function), my_holder(holder)
{
    type = EZSYMBOLTABLE;
}


zsymboltable::~zsymboltable() {
    for (int i=0; i<size; ++i) {
        if (!NULLP(block[i]) && !DELETEDP(block[i])) {
        zvalue z = block[i];
            assert(ZSYMBOLP(z));
            ZSYMBOLC(z)->my_zst=NULL_ZVALUE;
        }
    }
}

zsymbol* zsymboltable::get_symbol(zobjects_holder* holder, const char* a_s)
{
    zsymbol* sm = NULL;

    zvalue x = get(ZWRAP(const_cast<char*>(a_s)));
    if (NULLP(x)) {
        sm = zsymbol::generate_raw(holder, a_s);
        sm->my_zst=this;
        put(sm);
    } else {
        sm = ZSYMBOLC(x);
    }

    return sm;
}

zsymbol* zsymboltable::get_symbol(zsymbol* zs)
{
    zsymbol* sm = NULL;

    zvalue x = get(ZWRAP(const_cast<char*>(zs->get_string())));
    if (NULLP(x)) {
        put(zs);
        zs->my_zst=this;
    return zs;
    } else {
    // @todo linijka nizej zakomentowana, nie wiem co to
        //delete zs;
        sm = ZSYMBOLC(x);
    }

    return sm;
}

zsymbol* zsymboltable::remove_symbol(zsymbol* zs) {
    assert(NULL!=zs);
    return remove_symbol(zs->get_string());
}

zsymbol* zsymboltable::remove_symbol(const char* a_s) {
    assert(NULL!=a_s);
    zvalue zv = remove(ZWRAP(const_cast<char*>(a_s)));
    zsymbol* sm = NULL;
    if (!NULLP(zv)) {
        assert(ZSYMBOLP(zv));
        ZSYMBOLC(zv)->my_zst=NULL_ZVALUE;
    }

    return sm;
}

zsyntree::zsyntree(zobjects_holder* holder):zenvironment(holder, 16)
{
    type = EZSYNTREE;

    category = label = reinterpret_cast<zsymbol*>(NULL_ZVALUE);
    equiv_tree = parent_tree = reinterpret_cast<zsyntree*>(NULL_ZVALUE);
    number_in_parent_tree = -1L;
    last_subtree = -1L;
    segment_beg = -1L;
    segment_len = 0;
    is_variant = false;
}


void zsyntree::setAttr(zsymbol* a_attr, zvalue a_val)
{
    change(a_attr, a_val);
}


zvalue zsyntree::getAttr(zsymbol* a_attr)
{
    return fetch(a_attr);
}

void zsyntree::inherit(zsymbol* a_attr, zvalue a_val)
{
    if (NULLP(a_attr))
    return;

    bool was_set = false;
    int start_idx = 0, stop_idx = last_subtree;

    zsyntree* zst;

    for ( ; start_idx <= stop_idx ; ++start_idx)
    {
    zst = ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(start_idx)));

    assert(zst != NULL);

    if (zst->defined(a_attr))
    {
        was_set = true;
        setAttr(a_attr, zst->getAttr(a_attr));

        // @warning - we don't have access to sm_main_subtree_label here
        // if we wanted to chagne sm_main_subtree_label we would have to
        // modify this condition
        if (zst->label != NULL &&
           zst->label->get_string() != NULL &&
           (zst->label->get_string())[0] == '#' &&
           (zst->label->get_string())[1] == '\0')
        break;
    }
    }

    if (!was_set && a_val != DEFAULT_ZVALUE)
    setAttr(a_attr, a_val);
}

int zsyntree::sortByOrder(zhash* a_srefs_hash)
{
    int i, j, k;
    zvalue s, t;
    zvalue v, w, q;
    zvalue temp;

    for (i = last_subtree-1; i >= 0; --i)
    {
    s = fetch(fetch(INTEGER_TO_ZVALUE(i)));
    if (!NULLP(s))
    {
        v = a_srefs_hash->get(s);
        if (!NULLP(v))
        {
        k = i;
        q = ZPAIRC(v)->getSecond();

        for (j = last_subtree; j > i; --j)
        {
            t = fetch(fetch(INTEGER_TO_ZVALUE(j)));
            if (!NULLP(t))
            {
            w = a_srefs_hash->get(t);
            if (!NULLP(w))
            {
                if (ZVALUE_TO_INTEGER(ZPAIRC(w)->getSecond()) <
                   ZVALUE_TO_INTEGER(q))
                {
                k = j;
                break;
                }
            }
            }
        }

        if (k != i)
        {
            temp = fetch(INTEGER_TO_ZVALUE(i));

            int z;
            for (z = i; z < k; ++z)
            {
            change(INTEGER_TO_ZVALUE(z), fetch(INTEGER_TO_ZVALUE(z+1)));

            ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(z)))
                ->number_in_parent_tree = z;
            }

            change(INTEGER_TO_ZVALUE(k), temp);
            ZSYNTREEC(temp)->number_in_parent_tree = k;
        }
        }
    }
    }

    return 0;
}

zsyntree* zsyntree::find(zsymbol* cat, zsymbol* label, bool from_left)
{
    int idx;

    if (last_subtree == -1L)
    return NULL;

    if (from_left)
    {
    if (NULL==cat && NULL==label)
        return ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(0)));

    zsyntree* zst = NULL;
    for (idx = 0; idx <= last_subtree; ++idx)
    {
        zst = ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(idx)));
        if (NULL!=cat && cat!=zst->category)
        {
        zst=NULL;
        continue;
        }
        if (NULL!=label && label!=zst->label)
        {
        zst=NULL;
        continue;
        }
        break;
    }
    return zst;
    }
    else
    {
    if (NULL==cat && NULL==label)
        return ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(last_subtree)));

    zsyntree* zst = NULL;
    for (idx = last_subtree; idx >= 0; --idx)
    {
        zst = ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(idx)));
        if (NULL!=cat && cat!=zst->category)
        {
        zst=NULL;
        continue;
        }
        if (NULL!=label && label!=zst->label)
        {
        zst=NULL;
        continue;
        }
        break;
    }
    return zst;
    }
}

zsyntree* zsyntree::deleteSubtree(zsyntree* a_subtree) {
    assert(NULL!=a_subtree);
    assert(this==a_subtree->parent_tree);
    assert(ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(a_subtree->number_in_parent_tree)))==a_subtree);

    // @todo u¿yæ getNext zamiast fetch
    zvalue ttz = fetch(a_subtree);
    deleteAllKeyValue(ttz, a_subtree);
    deleteAllKeyValue(a_subtree, ttz);

    for (int i = a_subtree->number_in_parent_tree+1; i<=last_subtree; ++i)
    {
        zsyntree* zst = ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(i)));
        change(INTEGER_TO_ZVALUE(i-1), ZOBJECT_TO_ZVALUE(zst));
        zst->number_in_parent_tree=i-1;
    }
    last_subtree--;
    a_subtree->parent_tree = NULL;
    a_subtree->number_in_parent_tree = -1L;

    return a_subtree;
}

void zsyntree::addSubtree(zsyntree* a_subtree, zsymbol* a_label)
{
    insertAsNthSubtree(a_subtree, last_subtree+1);
    a_subtree->label = a_label;
}

void zsyntree::insertAsNthSubtree(zsyntree* a_subtree, int a_n)
{
    assert(NULL != a_subtree);
    assert(NULL == a_subtree->parent_tree);
    assert( -1L == a_subtree->number_in_parent_tree);

    a_subtree->setLabel(reinterpret_cast<zsymbol*>(NULL_ZVALUE));

    int i;

    // shifting subtrees to the right
    for (i = ++last_subtree; i > a_n; --i)
    {
        zsyntree* zst = ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(i-1)));
        change(INTEGER_TO_ZVALUE(i), ZOBJECT_TO_ZVALUE(zst));
        zst->number_in_parent_tree=i;
    }

    change(INTEGER_TO_ZVALUE(i), a_subtree);


    a_subtree->parent_tree = this;
    a_subtree->number_in_parent_tree = a_n;
}

void zsyntree::addSubtree(zsyntree* a_subtree)
{
    addSubtree(a_subtree, reinterpret_cast<zsymbol*>(NULL_ZVALUE));
}

void zsyntree::insertBeside(zsyntree* a_subtree, bool after)
{
    assert(NULL!=parent_tree);
    assert(0<=number_in_parent_tree);

    if (after) parent_tree->insertAsNthSubtree(a_subtree, number_in_parent_tree+1);
    else parent_tree->insertAsNthSubtree(a_subtree, number_in_parent_tree);
}

void zsyntree::replaceWith(zsyntree* a_subtree)
{
    assert(NULL != a_subtree);
    assert(this == a_subtree->parent_tree);

    assert(NULL!=parent_tree);
    assert(0<=number_in_parent_tree);

    a_subtree = deleteSubtree(a_subtree);

    // @todo zmieniæ wskazania %Referencji
    parent_tree->change(INTEGER_TO_ZVALUE(number_in_parent_tree), a_subtree);
    a_subtree->parent_tree = parent_tree;
    a_subtree->number_in_parent_tree = number_in_parent_tree;

    parent_tree = NULL;
    number_in_parent_tree = -1L;

}

#include <string>

std::string& eic_TK17122003(std::string& s) {
    for (size_t i=0; i<s.length(); ++i) {
       switch (s[i]) {
           case '(':
           case ')':
           case '/':
           case ',': s.insert(i, "/"); i++; break;
       }
    }
    return s;
}

#ifdef USE_LOG4CPLUS
#include <log4cplus/logger.h>
#endif

char* zsyntree::zsyntree_to_itf(char* lexeme, char* equiv) {
#ifdef USE_LOG4CPLUS
    log4cplus::Logger zsyntree_to_itf_logger = log4cplus::Logger::getInstance("zsyntree_to_itf");
#else
#define LOG4CPLUS_INFO(a1, a2)
#define LOG4CPLUS_WARN(a1, a2)
#define LOG4CPLUS_DEBUG(a1, a2)
#define LOG4CPLUS_ERROR(a1, a2)
#define LOG4CPLUS_FATAL(a1, a2)
#endif

    zsymboltable* zst = NULL;
    if (NULL!=category) {
        if (!NULLP(reinterpret_cast<zst_ref*>(category)->my_zst)) {
            zst = reinterpret_cast<zsymboltable*>(reinterpret_cast<zst_ref*>(category)->my_zst);
        }
    }
    bool terminal = false;
    if (-1==last_subtree) {
        if (NULL!=category) {
            const char* cat = category->get_string();
            const int len = strlen(cat);
            if (1<=len && '\''==cat[0]) {
                if (2<=len) terminal=true;
            }
        }
    }
    std::string s = "(", tbe = "";
    zvalue z = NULL_ZVALUE;
    if (terminal) {
        s+="T";
        s+=",";
        if (NULL!=equiv) {
            s+="'";
            tbe=equiv;
            s+=eic_TK17122003(tbe);
            s+="'";
        } else if (NULL!=category) { tbe=category->get_string(); s+=eic_TK17122003(tbe); }
        s+=",";
        if (NULL!=zst) {
            if (INTEGERP(z=fetch(zst->get_symbol("Beg")))) {
                char c[] = "         ";
                sprintf(c, "%d", int(ZVALUE_TO_INTEGER(z)));
                tbe=c;
                s+=eic_TK17122003(tbe);
            }
        }
        s+=",";
        if (NULL!=lexeme) { tbe=lexeme; s+=eic_TK17122003(tbe); }
        s+=",";
        //atrybuty beda z nadwezla
    } else {
        char* lex = NULL, *eqi = NULL;
        bool normal = true;

        if (NULL!=category) {
            char* cat = (char*)category->get_string();
            if (NULL!=equiv) {
                cat = new char[4+strlen(equiv)];
                strcpy(cat, "'$");
                strcat(cat, equiv);
                strcat(cat, "'");
            }
            if (!strncmp("'$", cat, 2) && last_subtree == 0) {
                lex = new char[strlen(cat)+1];
                strcpy(lex, cat);
                normal = false;
                s="";
        // @todo co z dolarami, które rzeczywi¶cie pojawiaj± w tek¶cie?
            } else {
                s+="N,";
                tbe=cat;
                s+=eic_TK17122003(tbe);
            }
            if (NULL!=lexeme) delete cat;
        }
        if (normal) {
            s+=",";
            if (NULL!=label) { tbe=label->get_string(); s+=eic_TK17122003(tbe); }
            s+=",";
        }
        if (NULL!=zst) {
            const char* attr[] = { "A", "Neg", "S", "C", "L", "R", "P", "Stopieñ",
                "O", "Inflection", "Tense", "Person", "Num", "Case", "T",
                "Gender", "Degree", "EndInflectio", NULL
            };
            const char** attr_itr = attr;
            char* tmp = NULL;
            while (NULL!=*attr_itr) {
                if (NULL_ZVALUE!=(z=fetch(zst->get_symbol(*attr_itr)))) {
                    if (NULL!=tmp) s+=",";
                    else s+="(";
                    s+="(";
                    s+=*attr_itr;
                    s+=",";
                    tmp = zvalue_to_string(z);
                    tbe = tmp;
                    delete tmp;
                    s+=eic_TK17122003(tbe);
                    s+=")";
                }
                attr_itr++;
            }
            if (NULL!=tmp) s+=")";
        }
        if (normal) {
            if (NULL!=zst) {
                if (ZSYMBOLP(z=fetch(zst->get_symbol("Equiv")))) {
                    //assert(NULL==equiv);
                    eqi = new char[strlen(ZSYMBOLC(z)->get_string())+1];
                    strcpy(eqi, ZSYMBOLC(z)->get_string());
                }
            }
            s+=",";
        }
        int i=-1;
        for (i=0; i<=last_subtree; ++i) {
            zsyntree* st = NULL;
            st = ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(i)));
            do {
                if (0<i && is_variant)
                    break;
                if (normal) {
                    if (i>0) s+=",";
                    else s+="(";
                }
                char* c = st->zsyntree_to_itf(lex?lex:lexeme, eqi?eqi:equiv);
                if (normal) s+=c;
                else {
                    c[strlen(c)-1]=0;
                    s.insert(0, c);
                }
                delete c;
            } while (0);
        }
        if (normal && -1<last_subtree) s+=")";

        if (NULL!=lex) delete lex;
        if (NULL!=eqi) delete eqi;
    }
    s+=")";
    char* itf_form = new char[s.length()+1];
    strcpy(itf_form, s.c_str());
    if (is_variant) { LOG4CPLUS_DEBUG(zsyntree_to_itf_logger, "Jestem wariantem."); }
    LOG4CPLUS_DEBUG(zsyntree_to_itf_logger, s.c_str());
    return itf_form;
}

// - - - - - -| END: zsyntree |- - - - -

char* zvalue_to_string(zvalue z, bool /* full_print */) {
    char* result = NULL;
    if (NULLP(z)) {
        result = new char[sizeof("NULL_ZVALUE")+1];
        strcpy(result, "NULL_ZVALUE");
        return result;
    }
    if (DELETEDP(z)) {
        result = new char[sizeof("DELETED_ZVALUE")+1];
        strcpy(result, "DELETED_ZVALUE");
        return result;
    }
    if (INTEGERP(z)) {
        return itoa(ZVALUE_TO_INTEGER(z));
    }
    // @todo This code is executed even if z is not a zobject.
    //       The problem is the ZOBJECTP() macro.
    if (ZOBJECTP(z)) {
        result = ZVALUE_TO_ZOBJECT(z)->to_string();
        return result;
    }
    // what is it then?
    result = new char[sizeof("UNKNOWN_ZVALUE")+1];
    strcpy(result, "UNKNOWN_ZVALUE");
    return result;
}

std::string zvalue_to_parsable_string(zvalue z) {
    std::string result;
    if (NULLP(z)) {
        result = "NULL_ZVALUE";
        return result;
    }
    if (DELETEDP(z)) {
        result = "DELETED_ZVALUE";
        return result;
    }
    if (INTEGERP(z)) {
        char tmp[100];
        sprintf(tmp, "%i", int(ZVALUE_TO_INTEGER(z)));
        result += tmp;
        return result;
    }
    // @todo This code is executed even if z is not a zobject.
    //       The problem is the ZOBJECTP() macro.
    if (ZOBJECTP(z)) {
        result = ZVALUE_TO_ZOBJECT(z)->to_parsable_string();
        return result;
    }
    // what is it then?
    result = "UNKNOWN_ZVALUE";
    return result;
}

zsymbol* zf_concat(zsymbolfactory* a_sym_fac, zobjects_holder* holder, zsymbol* a, zsymbol* b)
{
    if (NULLP(a)) return b;
    if (NULLP(b)) return a;

    const char* as = ZSYMBOLC(a)->get_string();
    const char* bs = ZSYMBOLC(b)->get_string();
    size_t alen = strlen(as);
    if (alen == 0) return b;
    zsymbol* zs = zsymbol::generate_raw(holder, alen+strlen(bs)+1);
    strcpy(zs->get_block(), as);
    strcpy(zs->get_block()+alen, bs);
    return a_sym_fac->get_symbol(zs);
}

zsymbol* zf_concat_with_space(zsymbolfactory* a_sym_fac, zobjects_holder* holder,
        zsymbol* a, zsymbol* b)
{
    if (NULLP(a)) return b;
    if (NULLP(b)) return a;

    const char* as = ZSYMBOLC(a)->get_string();
    const char* bs = ZSYMBOLC(b)->get_string();
    size_t alen = strlen(as);
    size_t blen = strlen(bs);
    if (alen == 0) return b;
    if (blen == 0) return a;

    zsymbol* zs;
    // @todo universal isspace
    if (as[alen-1] == ' ' || bs[0] == ' ') {
        zs = zsymbol::generate_raw(holder, alen+strlen(bs)+1);
        strcpy(zs->get_block(), as);
        strcpy(zs->get_block()+alen, bs);
    } else {
        zs = zsymbol::generate_raw(holder, alen+strlen(bs)+2);
        strcpy(zs->get_block(), as);
        (zs->get_block())[alen] = ' ';
        strcpy((zs->get_block())+alen+1, bs);
    }
    return a_sym_fac->get_symbol(zs);
}

zsymbol* zf_itos(zsymbolfactory* a_sym_fac, zobjects_holder* holder, zvalue i)
{
    if (!INTEGERP(i)) return NULL;

    char* t = itoa(ZVALUE_TO_INTEGER(i));
    zsymbol* zs = a_sym_fac->get_symbol(holder, t, true);
    return zs;
}

/** BEGIN: for hashes with elements like: zpair(zpair, something) */
int pair_key_val_hash_function(zvalue z)
{
    zpair* zp = reinterpret_cast<zpair*>(reinterpret_cast<zpair*>(z)->getFirst());
    return standard_zvalue_hash_function(zp->getFirst()) +
        standard_zvalue_hash_function(zp->getSecond());
}

int pair_key_key_hash_function(zvalue z)
{
    return standard_zvalue_hash_function(reinterpret_cast<zpair*>(z)->getFirst())
        + standard_zvalue_hash_function(reinterpret_cast<zpair*>(z)->getSecond());
}

int pair_key_identity_function(zvalue k, zvalue v)
{
    zpair* zp = reinterpret_cast<zpair*>(reinterpret_cast<zpair*>(v)->getFirst());
    return (reinterpret_cast<zpair*>(k)->getFirst() == zp->getFirst())
        && (reinterpret_cast<zpair*>(k)->getSecond() == zp->getSecond());
}
/** END: for hashes with elements like: zpair(zpair, something) */

zvector* zf_put_to_vector(zobjects_holder* holder, zvalue first, zvalue second, zvalue third) {
    int elements_count = 3;
    zvalue elements[] = { first, second, third };
    zvector* new_vector = zvector::generate(holder);
    for (int count = 0; count<elements_count; ++count) {
        zvalue z = elements[count];
        if (    NULLP(z)
                || DELETEDP(z) ) {
            //skip
        } else if ( INTEGERP(z) || ZPAIRP(z) || ZSYMBOLP(z) ) {
            new_vector->add(z);
        } else if ( ZVECTORP(z) ) {
            zvector* old_vector = ZVECTORC(z);
            int old_vectors_size = old_vector->getSize();
            for (int c=0; c<old_vectors_size; ++c)
                new_vector->add(old_vector->elementAt(c));
        }
    }
    return new_vector;
}
