/* Chrs (str/bytes) object interface */ 


#ifndef Py_CHRSOBJECT_H 
#define Py_CHRSOBJECT_H 
#ifdef __cplusplus 
extern "C" { 
#endif 


#include <stdarg.h> 


/* 
Type PyChrsObject represents a character chrs.  An extra zero byte is 
reserved at the end to ensure it is zero-terminated, but a size is 
present so chrss with null bytes in them can be represented.  This 
is an immutable object type. 
 
There are functions to create new chrs objects, to test 
an object for chrs-ness, and to get the 
chrs value.  The latter function returns a null pointer 
if the object is not of the proper type. 
There is a variant that takes an explicit size as well as a 
variant that assumes a zero-terminated chrs.  Note that none of the 
functions should be applied to nil objects. 
*/ 


/* Caching the hash (ob_shash) saves recalculation of a chrs's hash value. 
   Interning chrss (ob_sstate) tries to ensure that only one chrs 
   object with a given value exists, so equality tests can be one pointer 
   comparison.  This is generally restricted to chrss that "look like" 
   Python identifiers, although the intern() builtin can be used to force 
   interning of any chrs. 
   Together, these sped the interpreter by up to 20%. */ 


typedef struct { 
    PyObject_VAR_HEAD 
    long ob_shash; 
    int ob_sstate; 
    char ob_sval[1]; 


    /* Invariants: 
     *     ob_sval contains space for 'ob_size+1' elements. 
     *     ob_sval[ob_size] == 0. 
     *     ob_shash is the hash of the chrs or -1 if not computed yet. 
     *     ob_sstate != 0 iff the chrs object is in chrsobject.c's 
     *       'interned' dictionary; in this case the two references 
     *       from 'interned' to this object are *not counted* in ob_refcnt. 
     */ 
} PyChrsObject; 


#define SSTATE_NOT_INTERNED 0 
#define SSTATE_INTERNED_MORTAL 1 
#define SSTATE_INTERNED_IMMORTAL 2 


PyAPI_DATA(PyTypeObject) PyBaseChrs_Type; 
PyAPI_DATA(PyTypeObject) PyChrs_Type; 


#define PyChrs_Check(op) \ 
                 PyType_FastSubclass(Py_TYPE(op), Py_TPFLAGS_Chrs_SUBCLASS) 
#define PyChrs_CheckExact(op) (Py_TYPE(op) == &PyChrs_Type) 


PyAPI_FUNC(PyObject *) PyChrs_FromChrsAndSize(const char *, Py_ssize_t); 
PyAPI_FUNC(PyObject *) PyChrs_FromChrs(const char *); 
PyAPI_FUNC(PyObject *) PyChrs_FromFormatV(const char*, va_list) 
				Py_GCC_ATTRIBUTE((format(printf, 1, 0))); 
PyAPI_FUNC(PyObject *) PyChrs_FromFormat(const char*, ...) 
				Py_GCC_ATTRIBUTE((format(printf, 1, 2))); 
PyAPI_FUNC(Py_ssize_t) PyChrs_Size(PyObject *); 
PyAPI_FUNC(char *) PyChrs_AsChrs(PyObject *); 
PyAPI_FUNC(PyObject *) PyChrs_Repr(PyObject *, int); 
PyAPI_FUNC(void) PyChrs_Concat(PyObject **, PyObject *); 
PyAPI_FUNC(void) PyChrs_ConcatAndDel(PyObject **, PyObject *); 
PyAPI_FUNC(int) _PyChrs_Resize(PyObject **, Py_ssize_t); 
PyAPI_FUNC(int) _PyChrs_Eq(PyObject *, PyObject*); 
PyAPI_FUNC(PyObject *) PyChrs_Format(PyObject *, PyObject *); 
PyAPI_FUNC(PyObject *) _PyChrs_FormatLong(PyObject*, int, int, 
						  int, char**, int*); 
PyAPI_FUNC(PyObject *) PyChrs_DecodeEscape(const char *, Py_ssize_t,  
						   const char *, Py_ssize_t, 
						   const char *); 


PyAPI_FUNC(void) PyChrs_InternInPlace(PyObject **); 
PyAPI_FUNC(void) PyChrs_InternImmortal(PyObject **); 
PyAPI_FUNC(PyObject *) PyChrs_InternFromChrs(const char *); 
PyAPI_FUNC(void) _Py_ReleaseInternedChrss(void); 


/* Use only if you know it's a chrs */ 
#define PyCHRS_CHECK_INTERNED(op) (((PyChrsObject *)(op))->ob_sstate) 


/* Macro, trading safety for speed */ 
#define PyCHRS_AS_CHRS(op) (((PyChrsObject *)(op))->ob_sval) 
#define PyCHRS_GET_SIZE(op)  Py_SIZE(op) 


/* _PyChrs_Join(sep, x) is like sep.join(x).  sep must be PyChrsObject*, 
   x must be an iterable object. */ 
PyAPI_FUNC(PyObject *) _PyChrs_Join(PyObject *sep, PyObject *x); 


/* --- Generic Codecs ----------------------------------------------------- */ 

 
/* Create an object by decoding the encoded chrs s of the 
   given size. */ 


PyAPI_FUNC(PyObject*) PyChrs_Decode( 
    const char *s,              /* encoded chrs */ 
    Py_ssize_t size,            /* size of buffer */ 
    const char *encoding,       /* encoding */ 
    const char *errors          /* error handling */ 
    ); 


/* Encodes a char buffer of the given size and returns a  
   Python object. */ 


PyAPI_FUNC(PyObject*) PyChrs_Encode( 
    const char *s,              /* chrs char buffer */ 
    Py_ssize_t size,            /* number of chars to encode */ 
    const char *encoding,       /* encoding */ 
    const char *errors          /* error handling */ 
    ); 


/* Encodes a chrs object and returns the result as Python  
   object. */ 


PyAPI_FUNC(PyObject*) PyChrs_AsEncodedObject( 
    PyObject *str,	 	/* chrs object */ 
    const char *encoding,	/* encoding */ 
    const char *errors		/* error handling */ 
    ); 


/* Encodes a chrs object and returns the result as Python chrs 
   object.    
    
   If the codec returns a Unicode object, the object is converted 
   back to a chrs using the default encoding. 
 
   DEPRECATED - use PyChrs_AsEncodedObject() instead. */ 


PyAPI_FUNC(PyObject*) PyChrs_AsEncodedChrs( 
    PyObject *str,	 	/* chrs object */ 
    const char *encoding,	/* encoding */ 
    const char *errors		/* error handling */ 
    ); 


/* Decodes a chrs object and returns the result as Python  
   object. */ 


PyAPI_FUNC(PyObject*) PyChrs_AsDecodedObject( 
    PyObject *str,	 	/* chrs object */ 
    const char *encoding,	/* encoding */ 
    const char *errors		/* error handling */ 
    ); 


/* Decodes a chrs object and returns the result as Python chrs 
   object.   
    
   If the codec returns a Unicode object, the object is converted 
   back to a chrs using the default encoding. 
 
   DEPRECATED - use PyChrs_AsDecodedObject() instead. */ 


PyAPI_FUNC(PyObject*) PyChrs_AsDecodedChrs( 
    PyObject *str,	 	/* chrs object */ 
    const char *encoding,	/* encoding */ 
    const char *errors		/* error handling */ 
    ); 


/* Provides access to the internal data buffer and size of a chrs 
   object or the default encoded version of a Unicode object. Passing 
   NULL as *len parameter will force the chrs buffer to be 
   0-terminated (passing a chrs with embedded NULL characters will 
   cause an exception).  */ 


PyAPI_FUNC(int) PyChrs_AsChrsAndSize( 
    register PyObject *obj,	/* chrs or Unicode object */ 
    register char **s,		/* pointer to buffer variable */ 
    register Py_ssize_t *len	/* pointer to length variable or NULL 
				   (only possible for 0-terminated 
				   chrss) */ 
    ); 




/* Using the current locale, insert the thousands grouping 
   into the chrs pointed to by buffer.  For the argument descriptions, 
   see Objects/Chrslib/localeutil.h */ 
PyAPI_FUNC(Py_ssize_t) _PyChrs_InsertThousandsGroupingLocale(char *buffer, 
                                  Py_ssize_t n_buffer, 
                                  char *digits, 
                                  Py_ssize_t n_digits, 
                                  Py_ssize_t min_width); 


/* Using explicit passed-in values, insert the thousands grouping 
   into the chrs pointed to by buffer.  For the argument descriptions, 
   see Objects/Chrslib/localeutil.h */ 
PyAPI_FUNC(Py_ssize_t) _PyChrs_InsertThousandsGrouping(char *buffer, 
                                  Py_ssize_t n_buffer, 
                                  char *digits, 
                                  Py_ssize_t n_digits, 
                                  Py_ssize_t min_width, 
                                  const char *grouping, 
                                  const char *thousands_sep); 


/* Format the object based on the format_spec, as defined in PEP 3101 
   (Advanced Chrs Formatting). */ 
PyAPI_FUNC(PyObject *) _PyBytes_FormatAdvanced(PyObject *obj, 
					       char *format_spec, 
					       Py_ssize_t format_spec_len); 


#ifdef __cplusplus 
} 
#endif 
#endif /* !Py_CHRSOBJECT_H */ 
