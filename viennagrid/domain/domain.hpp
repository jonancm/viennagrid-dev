#ifndef VIENNAGRID_GEOMETRIC_DOMAIN_HPP
#define VIENNAGRID_GEOMETRIC_DOMAIN_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/config/generic_config.hpp"
#include "viennagrid/domain/topologic_domain.hpp"
#include "viennagrid/domain/metainfo.hpp"
#include "viennagrid/element/element_view.hpp"

namespace viennagrid
{
    struct topologic_config_tag;
    struct vector_type_tag;
    struct metainfo_typelist_tag;
    struct metainfo_container_config_tag;
    
    namespace result_of
    {
        template<typename domain_config>
        struct topologic_config
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, topologic_config_tag>::type::second type;
        };
        
        template<typename domain_config>
        struct vector_type
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, vector_type_tag>::type::second type;
        };
        
        template<typename domain_config>
        struct metainfo_typelist
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, metainfo_typelist_tag>::type::second type;
        };
        
        template<typename domain_config>
        struct metainfo_container_config
        {
            typedef typename viennameta::typemap::result_of::find<domain_config, metainfo_container_config_tag>::type::second type;
        };
        
    }

    
    template<typename vector_type_, typename topologic_domain_type_, typename metainfo_collection_type_>
    class domain_t
    {
        typedef domain_t<vector_type_, topologic_domain_type_, metainfo_collection_type_> self_type;
        
    public:
        
        typedef vector_type_ vector_type;
        typedef topologic_domain_type_ topologic_domain_type;       
        typedef metainfo_collection_type_ metainfo_collection_type;
        
        domain_t() : topologic_domain(create_topologic_domain<topologic_domain_type>()) {}

        ~domain_t()
        {
            viennadata::erase<viennadata::all, viennadata::all>()(*this);
        }


      /** @brief Constructor triggering the refinement of the domain */
      template <typename CellTag, typename OtherDomainType, typename RefinementTag>
      domain_t(refinement_proxy<CellTag, OtherDomainType, RefinementTag> const & proxy) : topologic_domain(create_topologic_domain<topologic_domain_type>())
      {
        detail::refine_impl<CellTag>(proxy.get(), *this, proxy.tag());
      }

        
        typedef typename result_of::element< self_type, viennagrid::vertex_tag >::type vertex_type;
        typedef typename result_of::handle< self_type, viennagrid::vertex_tag >::type vertex_handle_type;
        typedef typename result_of::const_handle< self_type, viennagrid::vertex_tag >::type const_vertex_handle_type;
        
        topologic_domain_type & get_topological_domain() { return topologic_domain; }
        const topologic_domain_type & get_topological_domain() const { return topologic_domain; }
        
        metainfo_collection_type & get_metainfo_collection() { return metainfo_collection; }
        const metainfo_collection_type & get_metainfo_collection() const { return metainfo_collection; }
        
    private:
        
        topologic_domain_type topologic_domain;
        metainfo_collection_type metainfo_collection;
    };
    
    template<typename vector_type_, typename topologic_domain_type_, typename metainfo_collection_type_>
    class domain_t<vector_type_, topologic_domain_type_, metainfo_collection_type_ *>
    {
        typedef domain_t<vector_type_, topologic_domain_type_, metainfo_collection_type_*> self_type;
        
    public:
        
        typedef vector_type_ vector_type;
        typedef topologic_domain_type_ topologic_domain_type;
        typedef metainfo_collection_type_ metainfo_collection_type;
        
        domain_t() : metainfo_collection(0) {}
        domain_t(topologic_domain_type topologic_domain_, metainfo_collection_type & metainfo_collection_) :
            topologic_domain(topologic_domain_), metainfo_collection(&metainfo_collection_) {}
        
        typedef typename result_of::element< self_type, viennagrid::vertex_tag >::type vertex_type;
        typedef typename result_of::handle< self_type, viennagrid::vertex_tag >::type vertex_handle_type;
        typedef typename result_of::const_handle< self_type, viennagrid::vertex_tag >::type const_vertex_handle_type;
        
        topologic_domain_type & get_topological_domain() { return topologic_domain; }
        const topologic_domain_type & get_topological_domain() const { return topologic_domain; }
        
        metainfo_collection_type & get_metainfo_collection() { return *metainfo_collection; }
        const metainfo_collection_type & get_metainfo_collection() const { return *metainfo_collection; }
        
    private:
        
        topologic_domain_type topologic_domain;
        metainfo_collection_type * metainfo_collection;
    };
    
    
    
    
    
    namespace result_of
    {
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type_or_tag>
        struct element< domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag >
        {
            typedef typename element<topologic_domain_type, element_type_or_tag>::type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type_or_tag>
        struct handle< domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag >
        {
            typedef typename handle<topologic_domain_type, element_type_or_tag>::type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type_or_tag>
        struct const_handle< domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag >
        {
            typedef typename const_handle<topologic_domain_type, element_type_or_tag>::type type;
        };
        
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type_or_tag>
        struct element_range< domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag >
        {
            typedef typename element_range< topologic_domain_type, element_type_or_tag>::type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type_or_tag>
        struct const_element_range< domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag >
        {
            typedef typename const_element_range< topologic_domain_type, element_type_or_tag>::type type;
        };
    }
    
    
    
    template<typename element_type_or_tag, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::element_range<domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag>::type elements(domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain)
    {
        return elements<element_type_or_tag>( domain.get_topological_domain() );
    }
    
    template<typename element_type_or_tag, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::const_element_range<domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type_or_tag>::type elements(const domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain)
    {
        return elements<element_type_or_tag>( domain.get_topological_domain() );
    }
    
    
    
    
    
    namespace result_of
    {
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct topologic_domain< domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >
        {
            typedef topologic_domain_type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct container_collection< domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >
        {
            typedef typename topologic_domain_type::container_collection_type type;
        };
        

        
        
        
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct metainfo_collection< domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >
        {
            typedef metainfo_collection_type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct metainfo_collection< domain_t<vector_type, topologic_domain_type, metainfo_collection_type *> >
        {
            typedef metainfo_collection_type type;
        };
        
        
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type, typename metainfo_type>
        struct metainfo_container< domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type, metainfo_type>
        {
            typedef typename metainfo_container<metainfo_collection_type, element_type, metainfo_type >::type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type, typename metainfo_type>
        struct const_metainfo_container< domain_t<vector_type, topologic_domain_type, metainfo_collection_type>, element_type, metainfo_type>
        {
            typedef typename const_metainfo_container<metainfo_collection_type, element_type, metainfo_type >::type type;
        };

        
        template<typename point_container_type>
        struct point_type
        {
            typedef typename viennagrid::metainfo::result_of::value_type<point_container_type>::type type;
        };

        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct point_type< domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >
        {
            typedef vector_type type;
        };
        
        template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
        struct point_type< const domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >
        {
            typedef vector_type type;
        };
        
    }
    
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::topologic_domain< domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >::type & topologic_domain( domain_t< vector_type, topologic_domain_type, metainfo_collection_type > & domain )
    { return domain.get_topological_domain(); }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    const typename result_of::topologic_domain< domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >::type & topologic_domain( const domain_t< vector_type, topologic_domain_type, metainfo_collection_type > & domain )
    { return domain.get_topological_domain(); }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::metainfo_collection< domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >::type & metainfo_collection( domain_t< vector_type, topologic_domain_type, metainfo_collection_type > & domain )
    { return domain.get_metainfo_collection(); }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    const typename result_of::metainfo_collection< domain_t< vector_type, topologic_domain_type, metainfo_collection_type > >::type & metainfo_collection( const domain_t< vector_type, topologic_domain_type, metainfo_collection_type > & domain )
    { return domain.get_metainfo_collection(); }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::metainfo_collection< domain_t< vector_type, topologic_domain_type, metainfo_collection_type* > >::type & metainfo_collection( domain_t< vector_type, topologic_domain_type, metainfo_collection_type* > & domain )
    { return domain.get_metainfo_collection(); }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    const typename result_of::metainfo_collection< domain_t< vector_type, topologic_domain_type, metainfo_collection_type* > >::type & metainfo_collection( const domain_t< vector_type, topologic_domain_type, metainfo_collection_type* > & domain )
    { return domain.get_metainfo_collection(); }
    
    template<typename metainfo_collection_type>
    metainfo_collection_type & metainfo_collection( metainfo_collection_type & collection )
    { return collection; }
    
    template<typename metainfo_collection_type>
    const metainfo_collection_type & metainfo_collection( const metainfo_collection_type & collection )
    { return collection; }
    
    
    
    
    
    template<typename element_type, typename metainfo_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::metainfo_container< domain_t< vector_type, topologic_domain_type, metainfo_collection_type >, element_type, metainfo_type>::type &
        metainfo_container( domain_t< vector_type, topologic_domain_type, metainfo_collection_type > & domain )
    {
        return metainfo_container<element_type, metainfo_type>( metainfo_collection(domain) );
    }
    
    template<typename element_type, typename metainfo_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename result_of::metainfo_container< domain_t< vector_type, topologic_domain_type, metainfo_collection_type >, element_type, metainfo_type>::type const &
        metainfo_container( domain_t< vector_type, topologic_domain_type, metainfo_collection_type > const & domain )
    {
        return metainfo_container<element_type, metainfo_type>( metainfo_collection(domain) );
    }
    
    
    
    
    template<typename metainfo_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    metainfo_type & look_up( domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_t<element_tag, boundary_cell_container_typelist, id_type> & element )
    {
        return look_up<metainfo_type>( metainfo_collection(domain), element );
    }
    
    template<typename metainfo_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    const metainfo_type & look_up( const domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_t<element_tag, boundary_cell_container_typelist, id_type> & element )
    {
        return look_up<metainfo_type>( metainfo_collection(domain), element );
    }
    
    
    template<typename metainfo_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename handle_type>
    metainfo_type & look_up( domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const handle_type & handle )
    {
        return look_up<metainfo_type>( metainfo_collection(domain), dereference_handle(domain, handle) );
    }
    
    template<typename metainfo_type, typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename handle_type>
    const metainfo_type & look_up( const domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const handle_type & handle )
    {
        return look_up<metainfo_type>( metainfo_collection(domain), dereference_handle(domain, handle) );
    }
    
    
    
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type, typename metainfo_type>
    void set( domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_type & element, const metainfo_type & meta_info )
    {
        metainfo::set( metainfo_container<element_type, metainfo_type>(domain.get_metainfo_collection()), element, meta_info );
    }
    
    
    
    
    
    
    template<typename container_type, typename vertex_type>
    typename container_type::value_type & point( container_type & geometric_container, vertex_type const & vertex )
    {
        return viennagrid::metainfo::look_up( geometric_container, vertex );
    }
    
    template<typename container_type, typename vertex_type>
    const typename container_type::value_type & point( container_type const & geometric_container, vertex_type const & vertex )
    {
        return viennagrid::metainfo::look_up( geometric_container, vertex );
    }
    
    template<typename id_type, typename vector_type, typename vertex_type>
    vector_type & point( std::map<id_type, vector_type> & geometric_container, vertex_type const & vertex )
    {
        return viennagrid::metainfo::look_up( geometric_container, vertex );
    }
    
    template<typename id_type, typename vector_type, typename vertex_type>
    const vector_type & point( std::map<id_type, vector_type> const & geometric_container, vertex_type const & vertex )
    {
        return viennagrid::metainfo::look_up( geometric_container, vertex );
    }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type & point( domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vertex_type & vertex )
    {
        return viennagrid::look_up<typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type>( domain, vertex );
    }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    const typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type & point( const domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vertex_type & vertex )
    {
        return viennagrid::look_up<typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type>( domain, vertex );
    }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type & point( domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vertex_handle_type & vertex_handle )
    {
        return viennagrid::look_up<typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type>( domain, dereference_handle(domain, vertex_handle) );
    }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    const typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type & point( const domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::const_vertex_handle_type & vertex_handle )
    {
        return viennagrid::look_up<typename domain_t<vector_type, topologic_domain_type, metainfo_collection_type>::vector_type>( domain, dereference_handle(domain, vertex_handle) );
    }
    
    
    
    
    
    

    
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::container_collection< domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >::type,
                    viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type>
                >::type::handle_type,
                bool
            >
        push_element( domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
    {
        metainfo::increment_size< viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> >(metainfo_collection(domain));
        return inserter(domain)(element);
    }
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::container_collection< domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >::type,
                    element_type
                >::type::handle_type,
                bool
            >
        push_element( domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_type & element)
    {
        metainfo::increment_size< element_type >(metainfo_collection(domain));
        return inserter(domain)(element);
    }   
    
    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type, typename element_type>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::container_collection< domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >::type,
                    element_type
                >::type::handle_type,
                bool
            >
        push_element_noid( domain_t<vector_type, topologic_domain_type, metainfo_collection_type> & domain, const element_type & element)
    {
        metainfo::increment_size< element_type >(metainfo_collection(domain));
        return inserter(domain).template insert<false, true>(element);
    }
    
    
    
    
    
    namespace result_of
    {
        
        template<typename element_tag, typename vector_type, typename handle_tag = viennagrid::storage::pointer_handle_tag,
            typename metainfo_typelist = viennameta::null_type, typename metainfo_container_config = viennagrid::storage::default_container_config>
        struct domain_config
        {
            typedef typename viennagrid::result_of::default_topologic_config<element_tag, handle_tag>::type topologic_config;
            typedef typename viennameta::make_typemap<
                viennagrid::topologic_config_tag, topologic_config,
                viennagrid::vector_type_tag, vector_type,
                viennagrid::metainfo_typelist_tag, metainfo_typelist,
                viennagrid::metainfo_container_config_tag, metainfo_container_config
            >::type type;
        };
        
        
        template<typename domain_config>
        struct domain
        {
            typedef typename viennagrid::result_of::topologic_domain<
                    typename result_of::topologic_config<domain_config>::type
                >::type topologic_domain_type;
                
             typedef typename result_of::vector_type<domain_config>::type vector_type;
                
            typedef typename viennagrid::storage::collection_t<
                    typename viennagrid::result_of::metainfo_container_typemap<
                        typename viennameta::typelist::result_of::push_back<
                            typename result_of::metainfo_typelist< domain_config >::type,
                            viennameta::static_pair<
                                viennagrid::vertex_tag,
                                vector_type
                            >
                        >::type,
                        typename result_of::metainfo_container_config<domain_config>::type,
                        typename result_of::topologic_config<domain_config>::type
                    >::type
                > metainfo_collection_type;  
            
            typedef domain_t<vector_type, topologic_domain_type, metainfo_collection_type> type;
        };
        
        
        
        
        
        template<
            typename domain_type,
            typename element_typelist = 
                typename storage::container_collection::result_of::value_typelist<
                    typename container_collection<domain_type>::type
                >::type,
            typename container_config = 
                storage::default_container_config>
        struct geometric_view
        {
            typedef typename viennagrid::result_of::topologic_view<typename domain_type::topologic_domain_type, element_typelist>::type topologic_view_type;
            typedef typename domain_type::metainfo_collection_type metainfo_collection_type;
            typedef typename viennagrid::domain_t<typename domain_type::vector_type, topologic_view_type, metainfo_collection_type*> type;
        };
    }
    
    

    template<typename vector_type, typename topologic_domain_type, typename metainfo_collection_type>
    struct create_view_helper< domain_t<vector_type, topologic_domain_type, metainfo_collection_type> >
    {
        typedef domain_t<vector_type, topologic_domain_type, metainfo_collection_type> geomatric_view_type;
        
        template<typename domain_type>
        static geomatric_view_type create( domain_type & domain )
        {
            typedef typename geomatric_view_type::topologic_domain_type topologic_view_type;
            return geomatric_view_type(
                viennagrid::create_view<topologic_view_type>( viennagrid::topologic_domain(domain) ),
                domain.get_metainfo_collection()
            );
        }
    };
    
    
    
    template<typename domain_type, typename id_type>
    typename viennagrid::result_of::handle_iterator< typename viennagrid::result_of::element_range<domain_type, typename id_type::value_type::tag>::type >::type
            find_handle(domain_type & domain, id_type id)
    {
        typedef typename id_type::value_type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::handle_iterator<RangeType>::type RangeIterator;
        
        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.handle_begin(); it != range.handle_end(); ++it)
        {
            if ( viennagrid::dereference_handle(domain, *it).id() == id )
                return it;
        }
        
        return range.handle_end();
    }
    
    template<typename domain_type, typename id_type>
    typename viennagrid::result_of::const_handle_iterator< typename viennagrid::result_of::const_element_range<domain_type, typename id_type::value_type::tag>::type >::type
            find_handle(const domain_type & domain, id_type id)
    {
        typedef typename id_type::value_type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::const_element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::const_handle_iterator<RangeType>::type RangeIterator;
        
        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.handle_begin(); it != range.handle_end(); ++it)
        {
            if ( viennagrid::dereference_handle(domain, *it).id() == id )
                return it;
        }
        
        return range.handle_end();
    }

    
}

#endif
