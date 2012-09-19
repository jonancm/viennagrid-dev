#ifndef VIENNAMETA_UTILS_HPP
#define VIENNAMETA_UTILS_HPP


namespace viennameta
{
    // basic operations
    template<typename type1, typename type2>
    struct _equal
    {
        static const bool value = false;
    };
    
    template<typename type>
    struct _equal<type,type>
    {
        static const bool value = true;
    };
    
    template<typename type1, typename type2>
    struct _not_equal
    {
        static const bool value = !_equal<type1, type2>::value;
    };
    
    template<bool condition, typename type1, typename type2>
    struct _if
    {
        typedef type1 type;
    };
    
    template<typename type1, typename type2>
    struct _if<false, type1, type2>
    {
        typedef type2 type;
    };
    
    template<typename _type>
    struct _identity
    {
        typedef _type type;
    };
    
    
    // error generator
    template<typename _type, bool error>
    struct _identity_errcheck
    {
        typedef _type type;
    };

    template<typename _type>
    struct _identity_errcheck<_type, false>
    {};

    
    // an empty type
    class null_type {};
    
    // some special types
    class out_of_range {};
    class not_found {};
    
    
    // a static type pair
    template<typename first_, typename second_>
    struct static_pair
    {
        typedef first_ first;
        typedef second_ second;
    };
    
    
    // a tag wrapper
    template<typename foo>
    class tag {};
}


#endif