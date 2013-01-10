#ifndef VIENNAGRID_ALGORITHM_BOUNDARY_HPP
#define VIENNAGRID_ALGORITHM_BOUNDARY_HPP

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

#include <vector>
#include "viennagrid/forwards.hpp"
#include "viennagrid/element/element.hpp"

/** @file boundary.hpp
    @brief Provides the detection and check for boundary n-cells.
*/

namespace viennagrid
{
//   /** @brief A key type for storing boundary information on n-cells, segments and domains */
//   template <typename T>
//   class boundary_key
//   {
//     typedef typename T::ERROR_BOUNDARY_KEY_MUST_BE_USED_WITH_NCELL_OR_DOMAIN_OR_SEGMENT   error_type;
//   };
  
//   /** @brief Specialization for segments */
//   template <typename GeometricContainerType>
//   class boundary_key
//   {
//     public:
//       boundary_key(std::size_t seg_id) : seg_id_(seg_id) {}
//       
//       /** @brief Provide less-than operator for compatibility with std::map */
//       bool operator<(boundary_key const & other) const
//       {
//         return seg_id_ < other.seg_id_;
//       }
//     private:
//       std::size_t seg_id_;
//   };


  /** @brief Specialization for n-cells */
  template <typename GeometricContainerType>
  class boundary_key
  {    
    public:
      boundary_key(GeometricContainerType const & e) : e_(&e)
      {}
          
      /** @brief Provide less-than operator for compatibility with std::map */
      bool operator<(boundary_key const & other) const
      {
        return e_ < other.e_;
      }
      
    private:
      GeometricContainerType const * e_;
  };
  
  
  
//   /** @brief Specialization for n-cells */
//   template<typename element_tag, typename bnd_cell_container_typelist, typename id_tag>
//   class boundary_key< element_t<element_tag, bnd_cell_container_typelist, id_tag> >
//   {
//       typedef element_t<element_tag, bnd_cell_container_typelist, id_tag>  element_type;
//     
//     public:
//       boundary_key(element_type const & e) : e_(&e) {}
//       
//       /** @brief Provide less-than operator for compatibility with std::map */
//       bool operator<(boundary_key const & other) const
//       {
//         return e_ < other.e_;
//         std::cout << "CALL!" << std::endl;
//       }
//     private:
//       element_type const * e_;
//   };  
}

// Configure ViennaData to use a type-based dispatch on the domain

/** @brief Customizations of keys for ViennaData */
namespace viennadata
{
  /** @brief Customizations of keys for ViennaData */
  namespace config
  {
//     template <typename GeometricContainerType>
//     struct key_dispatch<viennagrid::boundary_key< GeometricContainerType > >
//     {
//       typedef type_key_dispatch_tag    tag;
//     };
  }
}
  
namespace viennagrid
{

  /** @brief Helper struct for setting boundary flag of lower level elements of a facet
   * 
   * @tparam dim    Topological dimension the boundary setter is acting on
   */
  
  
  template<typename viennadata_key_type>
  class boundary_setter_functor
  {
  public:
      boundary_setter_functor(viennadata_key_type viennadata_key_) : viennadata_key(viennadata_key_) {}
      
      template<typename element_type>
      void operator()( const element_type & element )
      {
          viennadata::access<viennadata_key_type, bool>(viennadata_key)(element) = true;
      }
  private:
      
      viennadata_key_type viennadata_key;
  };
  
  
  template<typename element_type, typename viennadata_key_type>
  void mark_boundary_cell(element_type & element, viennadata_key_type viennadata_key)
  {
      boundary_setter_functor<viennadata_key_type> setter_functor(viennadata_key);
      viennagrid::for_each_boundary_cell( element, setter_functor );
  }
  
  
  
  
  
  
  
  
  
  

  /** @brief Implementation of boundary detection for the case no facets are available. Issues a hopefully useful compiler error. */
  template <typename DomainSegmentType, typename KeyType>
  void detect_boundary_impl(DomainSegmentType const & seg, KeyType const & key)
  {
    typedef typename DomainSegmentType::ERROR_CANNOT_DETECT_BOUNDARY_BECAUSE_FACETS_ARE_DISABLED        error_type;
  }

  /** @brief Implementation of boundary detection. Should not be called by library users. */
  template <typename ElementTag, typename TopologicContainerType, typename KeyType>
  void detect_boundary_impl(TopologicContainerType const & topologic_domain, KeyType const & key)
  {
    //typedef typename DomainSegmentType::config_type                            ConfigType;
    //typedef typename ElementType:tag                                      ElementTag;
    typedef typename viennagrid::result_of::element<TopologicContainerType, typename ElementTag::facet_tag >::type   FacetType;
    typedef typename viennagrid::result_of::element<TopologicContainerType, ElementTag>::type     CellType;

    typedef typename viennagrid::result_of::const_element_range<TopologicContainerType, typename ElementTag::facet_tag>::type      FacetRange;
    typedef typename viennagrid::result_of::iterator<FacetRange>::type                                           FacetIterator;
      
    typedef typename viennagrid::result_of::const_element_range<TopologicContainerType, ElementTag>::type        CellRange;
    typedef typename viennagrid::result_of::iterator<CellRange>::type                                            CellIterator;

    typedef typename viennagrid::result_of::const_element_range<CellType, typename ElementTag::facet_tag>::type     FacetOnCellRange;
    typedef typename viennagrid::result_of::iterator<FacetOnCellRange>::type                           FacetOnCellIterator;
    
    //iterate over all cells, over facets there and tag them:
    CellRange cells = viennagrid::elements<ElementTag>(topologic_domain);
    for (CellIterator cit = cells.begin();
          cit != cells.end();
          ++cit)
    {
      FacetOnCellRange facets_on_cell = elements<typename ElementTag::facet_tag>(*cit);
      for (FacetOnCellIterator focit = facets_on_cell.begin();
            focit != facets_on_cell.end();
            ++focit)
      {
        bool * data_ptr = viennadata::find<KeyType, bool>(key)(*focit);
        if (data_ptr == NULL)  //Facet has not yet been tagged
        {
          viennadata::access<KeyType, bool>(key)(*focit) = true;
        }
        else
        {
          if (*data_ptr == false)          //mind densely stored data
            *data_ptr = true;
          else          
            viennadata::erase<KeyType, bool>(key)(*focit);
        }        
      }
    }
    
    //iterate over all facets again and tag all lower level topological elements on facets that belong to the boundary:
    FacetRange facets = viennagrid::elements<typename ElementTag::facet_tag>(topologic_domain);
    for (FacetIterator fit = facets.begin();
          fit != facets.end();
          ++fit)
    {
      if (viennadata::find<KeyType, bool>(key)(*fit) != NULL)
      {
        if (viennadata::access<KeyType, bool>(key)(*fit) == true)
            
            mark_boundary_cell( *fit, key );
            
//           boundary_setter<CellTag::dim-2>::apply(*fit,
//                                                             key,
//                                                             typename viennagrid::result_of::bndcell_handling<ConfigType, CellTag, CellTag::dim-2>::type()
//                                                            );
      }
    }
  }



  /** @brief Public interface functions for boundary detection. No need to call it explicitly, since it is called by is_boundary().
   * 
   * @tparam DomainSegmentType    Either a segment or a domain type
   * @tparam KeyType              Type of the key used with ViennaData.
   * @param segment               Either a segment or a domain object
   * @param key                   The key object for ViennaData
   */
  template <typename ElemenTag, typename TopologicContainerType, typename KeyType>
  void detect_boundary(TopologicContainerType const & topologic_domain, KeyType const & key)
  {
    //typedef typename DomainSegmentType::config_type            ConfigType;
    //typedef typename ConfigType::cell_tag                   CellTag;
    //typedef typename result_of::domain<ConfigType>::type                        DomainType;
    //typedef typename result_of::bndcell_handling<ConfigType, DomainType,
    //                                                CellTag::dim-1>::type  HandlingTag;
    
    if (viennadata::access<KeyType, bool>(key)(topologic_domain) == false)
    {
      detect_boundary_impl<ElemenTag>(topologic_domain, key);//, HandlingTag());
      viennadata::access<KeyType, bool>(key)(topologic_domain) = true;
    }
  }


  /** @brief Returns true if a n-cell is located on the boundary of the domain 
   *
   * @param el      The n-cell
   * @param domain  The ViennaGrid domain
   */
  template <typename CellTypeOrTag, typename ElementType, typename TopologicContainerType>
  bool is_boundary(ElementType const & el,
                   TopologicContainerType const & topologic_domain)
  {
    typedef typename result_of::element_tag<CellTypeOrTag>::type CellTag;
    
    typedef boundary_key<TopologicContainerType>    BoundaryKey;
    BoundaryKey key(topologic_domain);
    
    detect_boundary<CellTag>(topologic_domain, key);
    if (viennadata::find<BoundaryKey, bool>(key)(el) != NULL)
    {
      return viennadata::access<BoundaryKey, bool>(key)(el);
    }
    return false;
  }


  /** @brief Returns true if the k-cell provided as first argument is on the boundary of the n-cell provided as second argument
   *
   * @param el      The n-cell
   * @param segment A segment of a domain
   */
  template <typename element_tag_1, typename bnd_cell_container_typelist_1, typename id_tag_1,
            typename element_tag_2, typename bnd_cell_container_typelist_2, typename id_tag_2>
  bool is_boundary(element_t<element_tag_1, bnd_cell_container_typelist_1, id_tag_1> const & el1,
                   element_t<element_tag_2, bnd_cell_container_typelist_2, id_tag_2> const & el2)
  {

    typedef typename viennagrid::result_of::const_element_range<element_t<element_tag_2, bnd_cell_container_typelist_2, id_tag_2>,
                                                              element_tag_1>::type   BoundaryRange;
    typedef typename viennagrid::result_of::iterator<BoundaryRange>::type               BoundaryIterator;
        
    BoundaryRange bnd_cells = viennagrid::elements<element_tag_1>(el2);
    for (BoundaryIterator bit = bnd_cells.begin();
                          bit != bnd_cells.end();
                        ++bit)
    {
      if (&(*bit) == &el1)
        return true;
    }
    
    return false;
  }
  
}

#endif
