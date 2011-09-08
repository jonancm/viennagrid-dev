#ifndef VIENNAGRID_DETAIL_LOWER_LEVEL_HOLDER_HPP
#define VIENNAGRID_DETAIL_LOWER_LEVEL_HOLDER_HPP

/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
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


#include <iostream>

#include "viennagrid/forwards.h"
//#include "viennagrid/topology/point.hpp"
//#include "viennagrid/iterators.hpp"
#include "viennagrid/detail/element_orientation.hpp"

#include <vector>

namespace viennagrid
{
  
  
  /************** Level 1: Elements contained by a higher-level element *******/

  template <typename ConfigType,
              typename ElementTag,
              unsigned long levelnum,
              typename handling_tag = typename result_of::subelement_handling<ConfigType, ElementTag,levelnum>::type,
              bool LevelNull = (levelnum == 0)>
  class lower_level_holder  { };

  template <typename ConfigType, typename ElementTag, unsigned long levelnum>
  class lower_level_holder <ConfigType, ElementTag, levelnum, full_handling_tag, false> :
    public lower_level_holder <ConfigType, ElementTag, levelnum - 1>
  {
    //requirements:
    //array of pointers to elements of class 'levelnum' and a integer representing the orientation within the cell relative to the element it points to.
    typedef topology::subelements<ElementTag, levelnum>                         LevelSpecs;
    typedef topology::subelements<typename LevelSpecs::tag, 0>  VertexOnElementSpecs;
    typedef lower_level_holder <ConfigType, ElementTag, levelnum - 1 >      Base;

    typedef element_t<ConfigType, typename LevelSpecs::tag>  LevelElementType;
    typedef element_orientation<VertexOnElementSpecs::num>     ElementOrientationType;

    protected:
      template <typename DomainType>
      void fill_level(DomainType & dom)
      {
        //fill lower level first:
        Base::fill_level(dom);

        //for (long i=0; i<LevelSpecs::num; ++i)
        //  orientations_[i].resize(VertexOnElementSpecs::num);

        topology::subelement_filler<ElementTag, levelnum>::fill(&(elements_[0]),
                                                           &(Base::vertices_[0]),
                                                           &(orientations_[0]),
                                                           dom);
      }

    public:

      lower_level_holder( ) 
      {
        for (long i=0; i < LevelSpecs::num; ++i)
          elements_[i] = NULL;
      };

      lower_level_holder( const lower_level_holder & llh) : Base (llh)
      {
        for (long i=0; i < LevelSpecs::num; ++i)
          elements_[i] = llh.elements_[i];
      }

      /////////////////// access container: ////////////////////
      
      //non-const:
      template <long j>
      typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container(less_tag)
      { 
        return Base::template container<j>();
      }

      template <long j>
      typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container(equal_tag)
      { 
        return &(elements_[0]);
      }

      template <long j>
      typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container()
      { 
        return container<j>( typename level_discriminator<levelnum, j>::result_type() );
      }
      
      //const:
      template <long j>
      const typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container(less_tag) const
      { 
        return Base::template container<j>();
      }

      template <long j>
      const typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container(equal_tag) const
      { 
        return &(elements_[0]);
      }

      template <long j>
      const typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container() const
      { 
        return container<j>( typename level_discriminator<levelnum, j>::result_type() );
      }
      
      

      ////////////////// orientation: ////////////////////
      template <long j>
      ElementOrientationType const &
      local_to_global_orientation(long index, less_tag)
      { 
        return Base::template local_to_global_orientation<j>(index);
      }

      template <long j>
      ElementOrientationType const &
      local_to_global_orientation(long index, equal_tag) 
      { 
        return orientations_[index];
      }

      template <long j>
      ElementOrientationType const &
      local_to_global_orientation(long index)
      { 
        return local_to_global_orientation<j>( index, typename level_discriminator<levelnum, j>::result_type() );
      }

    private: 
      LevelElementType * elements_[LevelSpecs::num];
      ElementOrientationType orientations_[LevelSpecs::num];
  };

  template <typename ConfigType, typename ElementTag, unsigned long levelnum>
  class lower_level_holder <ConfigType, ElementTag, levelnum, no_handling_tag, false> :
    public lower_level_holder < ConfigType, ElementTag, levelnum - 1 >
  {
    //requirements:
    //array of pointers to elements of class 'levelnum' and a integer representing the orientation within the cell relative to the element it points to.
    //typedef typename DomainTypes<ConfigType>::segment_type        SegmentType;
    typedef topology::subelements<ElementTag, levelnum>                                LevelSpecs;
    typedef lower_level_holder < ConfigType, ElementTag, levelnum - 1 >      Base;

    typedef element_t<ConfigType, typename LevelSpecs::tag>  LevelElementType;

    protected:
      template <typename DomainType>
      void fill_level(DomainType & dom)
      {
        //fill lower topological levels only:
        Base::fill_level(dom);
      }

    public:

      lower_level_holder( ) {};

      lower_level_holder( const lower_level_holder & llh) : Base (llh) {}
      
      //////////////////// container ///////////////////////
      
      //non-const:
      template <long j>
      typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container(less_tag)
      { 
        return Base::template container<j>();
      }

      template <long j>
      typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container(equal_tag)
      { 
        typedef typename result_of::subelement_handling<ConfigType, ElementTag, levelnum>::ERROR_HANDLING_OF_ELEMENTS_AT_THIS_TOPOLOGICAL_LEVEL_NOT_PROVIDED   error_type;
        return NULL;
      }

      template <long j>
      typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container()
      { 
        return container<j>( typename level_discriminator<levelnum, j>::result_type() );
      }


      //const:
      template <long j>
      const typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container(less_tag) const
      { 
        return Base::template container<j>();
      }

      template <long j>
      const typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container(equal_tag) const
      { 
        typedef typename result_of::subelement_handling<ConfigType, ElementTag, levelnum>::ERROR_HANDLING_OF_ELEMENTS_AT_THIS_TOPOLOGICAL_LEVEL_NOT_PROVIDED   error_type;
        return NULL;
      }
      
      template <long j>
      const typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container() const
      { 
        return container<j>( typename level_discriminator<levelnum, j>::result_type() );
      }

  };


  //at level 0, i.e. vertex level, recursion ends:
  template <typename ConfigType, typename ElementTag, typename handling_tag>
  class lower_level_holder <ConfigType, ElementTag, 0, handling_tag, true> 
  {
    //array of pointers to elements of class 'levelnum' and a integer representing the orientation within the cell relative to the element it points to.
    //typedef typename DomainTypes<ConfigType>::segment_type               SegmentType;
    typedef topology::subelements<ElementTag, 0>                                      LevelSpecs;

    typedef element_t<ConfigType, typename LevelSpecs::tag>         VertexType;
    typedef typename result_of::point<ConfigType>::type              PointType;

    typedef typename result_of::iterator< element_t<ConfigType, ElementTag>, 0>::type         VertexIterator;

    protected:
      //end recursion:
      template <typename DomainType>
      void fill_level(DomainType & dom) {}

    public:
      lower_level_holder() {};

      lower_level_holder( const lower_level_holder & llh)
      {
        for (long i=0; i < LevelSpecs::num; ++i)
          vertices_[i] = llh.vertices_[i];
      }

      ////////////////// container access: /////////////////////////
      
      //non-const:
      template <long j>
      typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container()
      { 
        return &(vertices_[0]);
      }

      //const:
      template <long j>
      const typename result_of::element_container< element_t<ConfigType, ElementTag>, j, ElementTag::dim>::type *
      container() const
      { 
        return &(vertices_[0]);
      }


    protected:
      VertexType * vertices_[LevelSpecs::num];
  };



}


#endif

