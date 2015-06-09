#pragma once

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <new>
#include <string>

#include <boost/any.hpp>


typedef void* zvalue;

#ifdef __LP64__
typedef long zint_t;
#else
typedef int zint_t;
#endif


class zobject;

#define NULL_ZVALUE ( reinterpret_cast<zvalue>(NULL)   )
//integer has 01 at the end
#define DELETED_ZVALUE ( reinterpret_cast<zvalue>(0x0002) )
#define DEFAULT_ZVALUE ( reinterpret_cast<zvalue>(0x0004) )
    //@warning this can become a problem because with 4 byte allignment this is a pointer(!)

#define NULLP(z)    ( (z) == NULL_ZVALUE )
#define DELETEDP(z) ( (z) == DELETED_ZVALUE )
#define DEFAULTP(z) ( (z) == DEFAULT_ZVALUE )
#define ZEROP(z)    ( (INTEGERP((z)) && ZVALUE_TO_INTEGER((z)) == 0) )
#define TRUEP(z)    ( !( NULLP((z)) || ZEROP((z)) ) )

#define ZVALUE_TO_INTEGER(z) ( reinterpret_cast<zint_t>(z) >> 1 )
#define INTEGER_TO_ZVALUE(i) ( reinterpret_cast<zvalue>(((i) << 1) | 1) )
#define INTEGERP(z)          ( reinterpret_cast<zint_t>(z) & 1 )

#define ZVALUE_TO_ZOBJECT(z) ( reinterpret_cast<zobject*>(z) )
#define ZOBJECT_TYPE(z)      ( reinterpret_cast<zobject*>(z)->get_type() )

#define ZOBJECTP(z)      ( !(reinterpret_cast<zint_t>(z) & 1) \
                            && !NULLP((z)) && !DELETEDP(z) && !DEFAULTP(z) )
#define ZPAIRP(z)        ( ZOBJECTP(z) && ZOBJECT_TYPE(z) == zobject::EZPAIR )
#define ZSYMBOLP(z)      ( ZOBJECTP(z) && ZOBJECT_TYPE(z) == zobject::EZSYMBOL )
#define ZHASHP(z)        ( ZOBJECTP(z) && ZOBJECT_TYPE(z) == zobject::EZHASH )
#define ZENVIRONMENTP(z) ( ZOBJECTP(z) && ZOBJECT_TYPE(z) == zobject::EZENVIRONMENT )
#define ZVECTORP(z)      ( ZOBJECTP(z) && ZOBJECT_TYPE(z) == zobject::EZVECTOR )
#define ZSYMBOLTABLEP(z) ( ZOBJECTP(z) && ZOBJECT_TYPE(z) == zobject::EZSYMBOLTABLE )
#define ZSYNTREEP(z)     ( ZOBJECTP(z) && ZOBJECT_TYPE(z) == zobject::EZSYNTREE )

#define ZOBJECT_TO_ZVALUE(p) ( reinterpret_cast<zvalue>(p) )
#define ZPAIRC(z)            ( reinterpret_cast<zpair*>(z) )
#define ZSYMBOLC(z)          ( reinterpret_cast<zsymbol*>(z) )
#define ZHASHC(z)            ( reinterpret_cast<zhash*>(z) )
#define ZENVIRONMENTC(z)     ( reinterpret_cast<zenvironment*>(z) )
#define ZVECTORC(z)          ( reinterpret_cast<zvector*>(z) )
#define ZSYNTREEC(z)         ( reinterpret_cast<zsyntree*>(z) )

/**
 * The wrapper it provided in order to use containers (like
 * <code>zvector</code>) for "non-zvalue types".
 *
 *  @warning  We need the 4 byte alignment. Under Borland its -a4 option, but
 *            how to do it under GNU?! It is aligned by gcc to 4 bytes because
 *            of the performance so it should work fine even with <code>
 *                         #define ZWRAPPERP(z) ( ZOBJECTP(z) )
 *            </code> but we can not be sure, so here is a tiny workaround.
 */
#define ZWRAPPERP(Z) ( 1 )
#define ZUNWRAP(t, z) ( reinterpret_cast<t>(reinterpret_cast<void*>(ZVALUE_TO_ZOBJECT(z))) )
#define ZWRAP(p) ( ZOBJECT_TO_ZVALUE(reinterpret_cast<zobject*>(p)) )

/**
 * We do not want to have VFTs in any zobjects.
 * If you need a virtual function then take a look at
 * <code>zobject::to_string()</code> and do it the same way.
 */
#define VIRTUAL_FUNCTION_CALL(return_value, function) \
    zobject* zo = reinterpret_cast<zobject*>(this); \
    switch (zo->type) { \
        case EZOBJECT: return_value=(reinterpret_cast<zobject*>(zo))->function; \
                       break; \
        case EZPAIR: return_value=(reinterpret_cast<zpair*>(zo))->function; \
                     break; \
        case EZSYMBOL: return_value=(reinterpret_cast<zsymbol*>(zo))->function; \
                       break; \
        case EZHASH: return_value=(reinterpret_cast<zhash*>(zo))->function; \
                     break; \
        case EZENVIRONMENT: return_value=(reinterpret_cast<zenvironment*>(zo))->function; \
                            break; \
        case EZVECTOR: return_value=(reinterpret_cast<zvector*>(zo))->function; \
                       break; \
        case EZSYMBOLTABLE: return_value=(reinterpret_cast<zsymboltable*>(zo))->function; \
                            break; \
        case EZSYNTREE: return_value=(reinterpret_cast<zsyntree*>(zo))->function; \
                        break; \
        default: assert(EZOBJECT==zo->type); \
            /* implementation error, unknown subclass of zobject or type not set */ \
    }

/**
 * A way to print any zvalue.
 */
char* zvalue_to_string(zvalue z, bool full_print = false);
std::string zvalue_to_parsable_string(zvalue z);

/**
 * Integer to string.
 *
 * @return  A string (f.e. the number 12 will become a string of the length 3
 *          "12\0").
 * @note    Remember to free the memory!
 */
char* itoa(int i);

class zvector;

typedef zvector zobjects_holder;

#define EMPTY_ZOBJECTS_HOLDER (0)

//---------------------------| zobject |--------------------------------

/**
 * A collectable object.
 * Derive from this class to get instances that will be allocated in
 * the collected heap. When the ollector discovers an inaccessible object
 * derived from "zobject", its destructors will be invoked.
 */
class zobject
{
public:
    static void freeZObjects(zobjects_holder* holder);
protected:
    /**
     * Manual RTTI.
     *
     * @see e_type
     */
    char type;

public:
    /**
     * Manual RTTI.
     *
     * @see type
     */
    enum e_type { EZOBJECT, EZSYMBOL, EZPAIR, EZHASH,
    EZENVIRONMENT, EZVECTOR, EZSYMBOLTABLE, EZSYNTREE};

protected:
    /**
     * @see to_string()
     */
    char* _to_string()
        {
            assert(0); //why there is an instance of this class, maybe the type is wrong...
            char* result = new char[1];
            result[0]=0;
            return result;
        }

    /**
     * @see to_string()
     */
    std::string _to_parsable_string()
        {
            assert(0); //why there is an instance of this class, maybe the type is wrong...
            return std::string("");
        }

    zobject(zobjects_holder* holder, e_type a_type);

public:
    inline void annihilate();
protected:
    ~zobject()
        { }

public:
    /**
     * Manual RTTI.
     *
     * @see type
     */
    e_type get_type() const
        {
            return (e_type)type;
        }

    /**
     * A text representation of this object.
     *
     * @return  Text. You have to release this string.
     */
    inline char* to_string();

    /**
     * A Perl representation :-) of this object.
     *
     * @return  Text.
     */
    inline std::string to_parsable_string();
};

//---------------------------| zpair |--------------------------------

/**
 * A holder for two zvalues.
 */
class zpair: public zobject
{
    friend class zobject;
protected:
    zvalue first;
    zvalue second;

    zpair(zobjects_holder* holder, zvalue a_first, zvalue a_second) :
        zobject(holder, EZPAIR),
        first(a_first),
        second(a_second)
    { }

    ~zpair() { }

    /**
     * @see to_string()
     */
    char* _to_string() {
        char* s_f = zvalue_to_string(first);
        char* s_s = zvalue_to_string(second);
        char* result = new char[4+strlen(s_f)+strlen(s_s)];
        strcpy(result, "(");
        strcat(result, s_f); delete s_f;
        strcat(result, ",");
        strcat(result, s_s); delete s_s;
        strcat(result, ")");
        return result;
    }

    /**
     * @see to_parsable_string()
     */
    std::string _to_parsable_string() {
        std::string result;
        result += "[ ";
        result += zvalue_to_parsable_string(first);
        result += ", ";
        result += zvalue_to_parsable_string(second);
        result += " ]";
        return result;
    }

public:
    /**
     * Used to obtain an instance of zpair.
     */
    static zpair* generate(zobjects_holder* holder, zvalue a_first, zvalue a_second)
        {
            return new zpair(holder, a_first, a_second);
        }

    void setFirst(zvalue z)
        {
            first = z;
        }

    void setSecond(zvalue z)
        {
            second = z;
        }

    zvalue getFirst() const
        {
            return first;
        }

    zvalue getSecond() const
        {
            return second;
        }
};

//---------------------------| zsymbol |--------------------------------

/**
 * A class with a invisible (for the collector) reference to zsymboltable.
 * Used ONLY by <code>zsymbol</code> class.
 */
class zst_ref: public zobject
{
    friend class zsymboltable;
    friend class zobject;
    friend class zsyntree;

protected:
    zvalue my_zst;
    zst_ref(zobjects_holder* holder, e_type a_type):zobject(holder, a_type)
        {
            assert(EZSYMBOL==a_type); //this class is used ONLY by zsymbol
            my_zst = NULL_ZVALUE;
        }
    ~zst_ref()
        {
            my_zst = NULL_ZVALUE;
        }
};

class zsymbolfactory;

/**
 * A string representation class.
 * CAUTION: instances may have different sizes.
 */
class zsymbol: public zst_ref
{
    friend class zsymboltable;
    friend class zobject;

protected:
    /**
     * Stored string.
     * This is just the FIRST character of the string. This whole object
     * is the string.
     *
     * @see get_string()
     *
     * CAUTION: Because of the variable size of instances there
     * can by ONLY ONE field in this class (all other fields
     * are in zst_ref class).
     */
    char block[1];

    zsymbol(zobjects_holder* holder, const char* a_s) :
        zst_ref(holder, EZSYMBOL),
        block()
    {
        strcpy(block, a_s);
    }

    zsymbol(zobjects_holder* holder, size_t /* a_size */) :
        zst_ref(holder, EZSYMBOL),
        block()
    {
        type = EZSYMBOL;
    }

    inline ~zsymbol();

    /**
     * @see to_string()
     */
    char* _to_string()
        {
            char* result = new char[strlen(block)+1];
            strcpy(result, block);
            return result;
        }

    /**
     * @see to_parsable_string()
     */
    std::string _to_parsable_string() {
        std::string result;
        result += "q^";
        result += block;
        result += "^";
        return result;
    }

public:
    /**
     * Used to obtain an instance that does not contain any string.
     * Only the size of the string that may be stored inside this instance.
     *
     * CAUTION: Remember that the last character of a string must be a NULL,
     * and you have to set it manualy.
     * So a_size should be strlen(your_string) + 1
     */
    static zsymbol* generate_raw(zobjects_holder* holder, size_t a_size)
        {
            void* m = new char [sizeof(zst_ref)+a_size];
            return new (m) zsymbol(holder, a_size);
        }

    char* get_block()
        {
            return block;
        }

    /**
     * Used to obtain an instance that contains a copy of given
     * string.
     */
    static zsymbol* generate_raw(zobjects_holder* holder, const char* a_s)
        {
            void* m = new char [sizeof(zst_ref)+strlen(a_s)+1];
            return new (m) zsymbol(holder, a_s);
        }

    const char* get_string() const
        {
            return block;
        }
};

//---------------------------| zvector |--------------------------------

/**
 * Dynamic sized array.
 */
class zvector: public zobject
{
    friend class zobject;
protected:
    int size;
    int occupied;
    zvalue* block;

    zvector(zobjects_holder* holder, int initialSize) :
        zobject(holder, EZVECTOR),
        size(initialSize),
        occupied(0)
        {
            block = new zvalue[initialSize];
            memset(block, 0, size*sizeof(zvalue));
        }

    ~zvector()
        {
            delete [] block;
        }

    void memRequest(int elementCount)
        {
            if (occupied + elementCount > (90*size/100) )
                resize(2*size);
        }

    void resize(int newSize)
        {
            zvalue* oldBlock = block;

            block = new zvalue [size = newSize];
            memset(block, 0, size*sizeof(zvalue));

            for (int i = 0; i < occupied; ++i)
                block[i]=oldBlock[i];

            delete [] oldBlock;
        }

    void move(int step, int position)
        {
            if (step>0)
            memRequest(step);

            int start = (step>0)?occupied-1:position;
            int stop  = (step>0)?position-1:occupied;
            int sign  = (step>0)?-1:1;

            for (int i=start; i!=stop; i+=sign)
            block[step+i]=block[i];

            if (step>0) {
                memset(block+position, 0, step);
            } else {
                memset(block+occupied-step, 0, -step);
            }

            occupied+=step;
        }

    /**
     * @see to_string()
     */
    char* _to_string();

    /**
     * @see to_parsable_string()
     */
    std::string _to_parsable_string();

public:
    static zvector* generate(zobjects_holder* holder, int initialSize = 20)
        {
            if (10>initialSize)
                initialSize = 10;
            return new zvector(holder, initialSize);
        }

    /**
     * <code>val</code> will be added to the end of the vector
     */
    void add(zvalue val)
        {
            memRequest(1);
            block[occupied++]=val;
        }

    /**
     * The same as <code>add<code>.
     */
    void push(zvalue val)
        {
            add(val);
        }

    /**
     * Removes the last element from the vector.
     *
     * @return  The last element.
     */
    zvalue pop()
        {
            const zvalue val = elementAt(occupied-1);
            occupied--;
            return val;
        }

    /**
     * Gets the <code>n</code>+1 element counting from the end.
     * Without any <code>n</code> it gets the last element (n=0).
     * If -1==<code>n</code> it gets the element before the last element.
     *
     * @param   n   A negativ integer.
     * @return  The <code>n</code>+1 element counting form the end.
     */
    zvalue peek(int n=0)
        {
            assert(0>=n);
            return elementAt(occupied-1+n);
        }

    /**
     * A way to access elements in the vector without
     * removing them.
     *
     * @param   aPosition  The position in the vector
     *                     (range [0; getSize()[ ).
     *
     * @return  The element at the position <code>aPosition</code>
     */
    zvalue elementAt(int aPosition) const
        {
            assert( 0<=aPosition && aPosition<occupied );
            return block[aPosition];
        }

    /**
     * @return  Number of elements in the vector.
     */
    int getSize() const
        {
            return occupied;
        }

    void insert(zvalue val, int position)
        {
            assert( 0<=position && position<occupied );
            move(1, position);
            block[position] = val;
        }

    void change(int position, zvalue val)
        {
            assert( 0<=position && position<occupied );
            block[position] = val;
        }

    zvalue remove(int position)
        {
            const zvalue val = elementAt(position);
            move(-1, position+1);
            return val;
        }
};

//---------------------------| zhash |--------------------------------

/**
 * Type of a function that is used by the zhash to put
 * values or to search keys.
 */
typedef int (* f_hash_function)(zvalue);

/**
 * Type of a function that is used by the zhash to
 * decide if the key matches the value.
 */
typedef int (* f_identity_function)(zvalue, zvalue);

/**
 * Hash function based on pointers.
 */
int standard_zvalue_hash_function(zvalue z);

/**
 * Identity hash function based on pointers.
 */
int the_same_identity_function(zvalue z, zvalue t);

/** BEGIN: for hashes with elements like: zpair(zpair, something) */
int pair_key_val_hash_function(zvalue z);
int pair_key_key_hash_function(zvalue z);
int pair_key_identity_function(zvalue k, zvalue v);
/** END: for hashes with elements like: zpair(zpair, something) */

class zhash: public zobject
{
    friend class zobject;
protected:
    bool atomic_mem_allocation;
    int size;
    int occupied;
    int occupied_and_deleted;

    zvalue* block;

    f_hash_function key_hash_function;
    f_hash_function val_hash_function;
    f_identity_function key_val_function;

    zhash(
        zobjects_holder* holder,
        int a_init_size,
        f_hash_function a_key_hash_function=NULL,
        f_hash_function a_val_hash_function=NULL,
        f_identity_function a_key_val_function=NULL
        // bool allocate_atomic_mem=false // unused
    ) :
        zobject(holder, EZHASH),
        size(0),
        occupied(0),
        occupied_and_deleted(0),
        block(NULL),
        key_hash_function(a_key_hash_function),
        val_hash_function(a_val_hash_function),
        key_val_function(a_key_val_function)
    {
        if (key_hash_function == NULL)
        key_hash_function = standard_zvalue_hash_function;

        if (val_hash_function == NULL)
        val_hash_function = key_hash_function;

        if (key_val_function == NULL)
        key_val_function = the_same_identity_function;

        resize(a_init_size);
    }

    ~zhash() {
        delete [] block;
        block=NULL;
    }

    zvalue* put_(zvalue val)
    {
        unsigned int h1 = val_hash_function(val);
        unsigned int h2 = h1 | 0x0001;

        unsigned int ix = h1 % size;

        while (!NULLP(block[ix]) && !DELETEDP(block[ix]))
        (ix += h2) %= size;

        if (NULLP(block[ix]))
        ++occupied_and_deleted;

        block[ix] = val;

        return &block[ix];
    }

    zvalue* get_(zvalue key, int& i) const
    {
        unsigned int h1 = key_hash_function(key);
        unsigned int h2 = h1 | 0x0001;

        unsigned int ix = (h1 + i * h2) % size;

        while (!NULLP(block[ix]))
        {
        if (!DELETEDP(block[ix]) &&
           key_val_function(key, block[ix]))
            return &block[ix];

        ++i;

        (ix += h2) %= size;
        }

        return &block[ix];
    }


    void resize(int a_new_size)
    {
        zvalue* old_block = block;
        int old_size = size;

        block = new zvalue [size = a_new_size];
        memset(block, 0, size*sizeof(zvalue));

        if (NULL!=old_block) {
            int ix;
            for (ix = 0; ix < old_size; ++ix) {
          if (!NULLP(old_block[ix]) && !DELETEDP(old_block[ix]))
                    put_(old_block[ix]);
            }

        occupied_and_deleted = occupied;

            delete [] old_block;
        }
    }

    /**
     * @see to_string()
     */
    char* _to_string();

    /**
     * @see to_parsable_string()
     */
    std::string _to_parsable_string();

    std::string to_parsable_string_modif (bool without_int_keys);

public:
    static zhash* generate(
    zobjects_holder* holder,
    int a_init_size,
    f_hash_function a_key_hash_function=NULL,
    f_hash_function a_val_hash_function=NULL,
    f_identity_function a_key_val_function=NULL)
    {
        return new zhash(holder,
                 a_init_size,
                 a_key_hash_function,
                 a_val_hash_function,
                 a_key_val_function);
    }

    zvalue get(zvalue key) const
    {
        int i = 0;
        return *get_(key, i);
    }

    zvalue getNext(zvalue key, int& i) const
    {
        return *get_(key, i);
    }

    zvalue remove(zvalue key)
    {
    int i = 0;
    zvalue* zv = get_(key, i);
    zvalue v = NULL_ZVALUE;
        if (!NULLP(*zv))
    {
        --occupied;
            v = *zv;
            *zv=DELETED_ZVALUE;
        }
        return v;
    }

    void put(zvalue val)
    {
        if (occupied++ >= size/2)
        resize(2*size);
        else if (occupied_and_deleted >= size-1)
        resize(size);

        put_(val);
    }

    zvalue* getAll(int& block_size) { block_size = size; return block; }
};


int env_val_hash_function(zvalue z);
int env_identity_function(zvalue k, zvalue v);

class zenvironment: public zhash
{
    friend class zobject;

protected:
    zobjects_holder* my_holder;

public:
    zobjects_holder* get_holder()
    {
        return my_holder;
    }

protected:
    zenvironment(zobjects_holder* holder, int a_init_size):
    zhash(holder, a_init_size, standard_zvalue_hash_function,
          env_val_hash_function, env_identity_function), my_holder(holder)
    { type = EZENVIRONMENT; }

    ~zenvironment() { }

    /**
     * @see to_string()
     */
    char* _to_string();

    /**
     * @see to_parsable_string()
     */
    std::string _to_parsable_string();

public:
    static zenvironment* generate(zobjects_holder* holder, int a_init_size)
    {
        return new zenvironment(holder, a_init_size);
    }

    // void annihilate() { delete this; }

    zvalue fetch(zvalue s) const
    {
        zvalue p = get(s);


        if (NULLP(p))
        return NULL_ZVALUE;

        return reinterpret_cast<zpair*>(p)->getSecond();
    }

    zvalue defined(zvalue s) const
    {
        if (NULLP(get(s)))
        return NULL_ZVALUE;

        return INTEGER_TO_ZVALUE(1);
    }

    void change(zvalue s, zvalue d)
    {
        zvalue p = get(s);

        if (NULLP(p)) {
            put(zpair::generate(my_holder, s, d));
        } else {
            reinterpret_cast<zpair*>(p)->setSecond(d);
        }
    }


    void insert(zvalue s, zvalue d)
    {
        put(zpair::generate(my_holder, s, d));
    }

    void deleteAllKeyValue(zvalue s, zvalue d);

    void deleteAllKey(zvalue s);
};

/**
 * A syntactic tree.
 *
 * The following values are kept in it as an zenvironment:
 * - successive subtrees (when key = [0, nb_subtree])
 * - the same subtrees as above but referred by %Whatever instead of number
 * - attributes (when key is a symbol not starting with '%')
 *
 * No tree should appear in more than one (super)tree.
 */
class zsyntree: public zenvironment
{
    friend class zobject;
protected:
    zsyntree(zobjects_holder* holder);
    ~zsyntree() { }

    void insertAsNthSubtree(zsyntree* a_subtree, int a_n);

    /**
     * @see to_string()
     */
    char* _to_string(bool with_attributes=false, bool with_subtrees=false);

    /**
     * @see to_parsable_string()
     */
    std::string _to_parsable_string();


public:
    static zsyntree* generate(zobjects_holder* holder)
    {
        return new zsyntree(holder);
    }

    /**
     * Syntactic category, for example "R", "FR", "GNP".
     */
    zsymbol* category;


    /**
     * Syntactic label, for example "podmiot", "Compl1".
     */
    zsymbol* label;


    /**
     * Equivalent tree, e.g. an english tree for a polish tree and vice versa.
     */
    zsyntree* equiv_tree;


    /**
     * The parent tree, i.e. the tree in which the tree is a
     * (immediate) subtree.
     */
    zsyntree* parent_tree;


    /**
     * Which subtree in the parent tree is this?
     */
    int number_in_parent_tree;


    /**
     * The number of the last subtree
     */
    int last_subtree;

    /**
     * A variant node? I.e. a node whose subnodes are alternative
     * interpretations rather than consituent nodes.
     */
    bool is_variant;

    /**
     * The beginning of the segment that is covered by the tree.
     */
    int segment_beg;


    /**
     * The length of the segment that is covered by the tree.
     */
    int segment_len;


    /**
     * The supporting edge of the root.
     */
    boost::any origin;


    void setCategory(zsymbol* a_category)
    {
        category = a_category;
    }


    zsymbol* getCategory()
    {
        return category;
    }


    void setLabel(zsymbol* a_label)
    {
        label = a_label;
    }

    zsymbol* getLabel()
    {
        return label;
    }

    void makeVariant()
    {
        is_variant = true;
    }

    void setSegmentInfo(int a_segment_beg, int a_segment_len)
    {
        segment_beg = a_segment_beg;
        segment_len = a_segment_len;
    }

    void setOrigin(boost::any a_origin)
    {
        origin = a_origin;
    }

    boost::any getOrigin()
    {
        return origin;
    }

    int sortByOrder(zhash* a_srefs_hash);

    void setAttr(zsymbol* a_attr, zvalue a_val);

    zvalue getAttr(zsymbol* a_attr);


    /**
     * Sets as the value of attribute a_attr:
     * (1) the value of attribute a_attr of the main subnode (if there is
     * a main subnode and a_attr is defined for it),
     * (2) the value of attribute a_attr of the last subnode for which
     * a_attr is defined,
     * (3) a_val if it is not default value.
     */
    void inherit(zsymbol* a_attr, zvalue a_val);

    /**
     * Sets eT as the node's equivalent node  (and vice versa)
     *
     * @param eT Any node (can be NULL)
     */
    void setEquivTree(zsyntree* eT)
    {
        assert(eT != this);

        if (equiv_tree != NULL)
        equiv_tree->equiv_tree = NULL;

        equiv_tree = eT;

        if (eT != NULL)
        {
        if (eT->equiv_tree != NULL)
            eT->setEquivTree(NULL);

        assert(eT->equiv_tree == NULL);
        eT->equiv_tree = this;
        }
    }

    zsyntree* getEquivTree()
    {
        return equiv_tree;
    }

    void addSubtree(zsyntree* a_subtree, zsymbol* a_label);


    void addSubtree(zsyntree* a_subtree);

    /**
     * Looks for a subnode.
     *
     * @param  cat        A category (can be NULL)
     * @param  lab        A label (can be NULL)
     * @param  from_left  Search direction
     *
     * @return  A node with category==cat (if cat was specified) and
     *          label==lab (if lab was specified). If either cat or lab
     *          was specified the first node from left or right (if from_left
     *          was false) is returned.
     *
     *          NULL if no node was found with category==cat (if cat was
     *          specified) and label==lab (if lab was specified). If either
     *          cat or lab was specified it means there are no subnodes
     */
    zsyntree* find(zsymbol* cat, zsymbol* label, bool from_left=true);

    /**
     * Inserts @a a_subtree as a first subtree.
     */
    void insertLeft(zsyntree* a_subtree)
    {
        insertAsNthSubtree(a_subtree, 0);
    }


    /**
     * Inserts @a a_subtree as a last subtree.
     */
    void insertRight(zsyntree* a_subtree)
    {
        addSubtree(a_subtree);
    }

    /**
     * Deletes <code>a_subtree</code> from this tree.
     *
     * @param   a_subtree   A child of this tree to be removed.
     * @return  The removed subtree (<code>a_subtree</code>).
     */
    zsyntree* deleteSubtree(zsyntree* a_subtree);

    /**
     * Inserts <code>a_subtree</code> tree to the parent tree of
     * this tree, just before or after this tree.
     *
     * @param   a_subtree   A tree to be added to the parent tree.
     * @param   after       TRUE if the tree should be added after
     *                      this tree. FALSE if it should be added
     *                      before this.
     */
    void insertBeside(zsyntree* a_subtree, bool after = true);

    /**
     * Replaces this subtree with <code>a_subtree</code> subtree.
     *
     * @param   a_subtree   A tree that should be a subtree of
     *                      the parent tree instead of this tree.
     */
    void replaceWith(zsyntree* a_subtree);

    void detachFromParent() {
        if (NULL!=parent_tree) parent_tree->deleteSubtree(this);
    }

    char* zsyntree_to_string() {
      return _to_string(true, true);
    }

    char* zsyntree_to_itf(char* lexeme = NULL, char* equiv=NULL);

    zsyntree * getSubtree(int index) {
        return ZSYNTREEC(fetch(INTEGER_TO_ZVALUE(index)));
    }

};


class zsymboltable: public zhash
{
    friend class zobject;

protected:
    zobjects_holder* my_holder;

public:
    zobjects_holder* get_holder()
    {
        return my_holder;
    }

protected:
    zsymboltable(zobjects_holder* holder);
     ~zsymboltable();

    /**
     * @see to_string()
     */
    char* _to_string();

    /**
     * @see to_parsable_string()
     */
    std::string _to_parsable_string();


public:
    static zsymboltable* generate(zobjects_holder* holder)
    {
        return new zsymboltable(holder);
    }

    // void annihilate() { delete this; }

    zsymbol* get_symbol(zobjects_holder* holder, const char* a_s);

    zsymbol* get_symbol(const char* a_s) { return get_symbol(my_holder, a_s); }

    zsymbol* get_symbol(zsymbol* zs);

    zsymbol* remove_symbol(zsymbol* zs);

    zsymbol* remove_symbol(const char* a_s);

};


class zsymbolfactory
{
protected:
    zsymboltable* a_zsymboltable;

public:
    zsymbolfactory(zsymboltable* zst)
    {
        assert(NULL!=zst);
        a_zsymboltable = zst;
    }

    ~zsymbolfactory() { a_zsymboltable = NULL; }

    zsymbol* get_symbol(zobjects_holder* holder, const char* a_s, bool delete_oryginal_string)
    {
        zsymbol* zs = a_zsymboltable->get_symbol(holder, a_s);
        if (delete_oryginal_string)
            delete (char*)a_s;
        return zs;
    }

    zsymbol* get_symbol(const char* a_s, bool delete_oryginal_string=false)
    {
        zsymbol* zs = a_zsymboltable->get_symbol(a_s);
        if (delete_oryginal_string)
            delete (char*)a_s;
        return zs;
    }

    zsymbol* get_symbol(zsymbol* zs)
    {
        return a_zsymboltable->get_symbol(zs);
    }

    zobjects_holder* get_holder()
    {
        assert(a_zsymboltable);
        return a_zsymboltable->get_holder();
    }
};

zsymbol* zf_concat(zsymbolfactory* a_sym_fac, zobjects_holder* holder, zsymbol* a, zsymbol* b);

zsymbol* zf_concat_with_space(zsymbolfactory* a_sym_fac, zobjects_holder* holder,
                 zsymbol* a, zsymbol* b);

zsymbol* zf_itos(zsymbolfactory* a_sym_fac, zobjects_holder* holder, zvalue i);

zvector* zf_put_to_vector(zobjects_holder* holder,
              zvalue first,
              zvalue second,
              zvalue third = NULL_ZVALUE);


inline zsymbol::~zsymbol() {
    if (!NULLP(my_zst)) {
        reinterpret_cast<zsymboltable*>(my_zst)->remove(ZWRAP(const_cast<char*>(get_string())));
    }
    my_zst=NULL_ZVALUE;
}

    void zobject::annihilate()
      {
      switch (type) {
      case EZOBJECT:
          delete reinterpret_cast<zobject*>(this);
          break;
      case EZPAIR:
          delete reinterpret_cast<zpair*>(this);
          break;
      case EZSYMBOL:
          reinterpret_cast<zsymbol*>(this)->~zsymbol();
          delete [] reinterpret_cast<char*>(this);
          break;
      case EZHASH:
          delete reinterpret_cast<zhash*>(this);
          break;
      case EZENVIRONMENT:
          delete reinterpret_cast<zenvironment*>(this);
          break;
      case EZVECTOR:
          delete reinterpret_cast<zvector*>(this);
          break;
      case EZSYMBOLTABLE:
          delete reinterpret_cast<zsymboltable*>(this);
          break;
      case EZSYNTREE:
          delete reinterpret_cast<zsyntree*>(this);
          break;
      default: assert(EZOBJECT==type);
          //implementation error, unknown subclass of zobject or type not set
      }
      }

inline char* zobject::to_string() {
    char* result = NULL;
    VIRTUAL_FUNCTION_CALL(result, _to_string());
    return result;
}

inline std::string zobject::to_parsable_string() {
    std::string result;
    VIRTUAL_FUNCTION_CALL(result, _to_parsable_string());
    return result;
}

