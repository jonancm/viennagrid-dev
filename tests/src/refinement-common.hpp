/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
   
   This is a test file for the ViennaGrid Library IO-System
======================================================================= */


//***********************************************
// Define the input-file format
//***********************************************

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/algorithm/refine.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"

// Helper: Compute volume of tetrahedron
template <typename CellType>
double volume(CellType & cell)
{
  typedef typename CellType::config_type      ConfigType;
  typedef typename viennagrid::result_of::ncell_type<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell_range<CellType, 0>::type       VertexOnCellContainer;
  typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type    VertexOnCellIterator;
  
  VertexOnCellContainer vertices = viennagrid::ncells<0>(cell);
  VertexOnCellIterator vit = vertices.begin();
  
  VertexType & v0 = *vit; ++vit;
  VertexType & v1 = *vit; ++vit;
  VertexType & v2 = *vit; ++vit;
  VertexType & v3 = *vit; ++vit;
  
  return spanned_volume(v0.getPoint(), v1.getPoint(), v2.getPoint(), v3.getPoint());
}

template <typename CellType>
double volume_triangle(CellType & cell)
{
  typedef typename CellType::config_type      ConfigType;
  typedef typename viennagrid::result_of::ncell_type<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell_range<CellType, 0>::type       VertexOnCellContainer;
  typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type    VertexOnCellIterator;
  
  VertexOnCellContainer vertices = viennagrid::ncells<0>(cell);
  VertexOnCellIterator vit = vertices.begin();
  
  VertexType & v0 = *vit; ++vit;
  VertexType & v1 = *vit; ++vit;
  VertexType & v2 = *vit; ++vit;
  
  return spanned_volume(v0.getPoint(), v1.getPoint(), v2.getPoint());
}

// Helper: Remove all refinement tags on a cell
template <typename CellType>
void clear_refinement_tag(CellType & cell)
{
  typedef typename CellType::config_type      ConfigType;
  typedef typename viennagrid::result_of::ncell_type<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell_range<CellType, 1>::type       EdgeOnCellContainer;
  typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type    EdgeOnCellIterator;
  
  EdgeOnCellContainer edges = viennagrid::ncells<1>(cell);
  for(EdgeOnCellIterator eocit  = edges.begin();
                         eocit != edges.end();
                       ++eocit)
  {
    viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit) = false;
  }
  
  viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(cell) = false;
}

// Helper: Remove all refinement tags on a cell
template <typename CellType>
void print_refinement_edges(CellType & cell)
{
  typedef typename CellType::config_type      ConfigType;
  typedef typename viennagrid::result_of::ncell_type<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell_range<CellType, 1>::type       EdgeOnCellContainer;
  typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type    EdgeOnCellIterator;
  
  EdgeOnCellContainer edges = viennagrid::ncells<1>(cell);
  for(EdgeOnCellIterator eocit  = edges.begin();
                         eocit != edges.end();
                       ++eocit)
  {
    if (viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*eocit) == true)
      eocit->print_short();
  }
}


template <typename DomainType>
double domain_volume(DomainType & domain)
{
  typedef typename DomainType::config_type      ConfigType;
  typedef typename ConfigType::cell_tag                  CellTag;
  
  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
  
  double new_volume = 0;
  CellContainer new_cells = viennagrid::ncells<CellTag::topology_level>(domain);
  for (CellIterator new_cit = new_cells.begin();
                    new_cit != new_cells.end();
                  ++new_cit)
  {
    //std::cout << "adding up!" << std::endl;
    double cell_volume = volume(*new_cit);
    assert(cell_volume > 0);
    new_volume += cell_volume;
    //new_cit->print_short();
  }
  
  return new_volume;
}

template <typename DomainType>
double domain_surface(DomainType & domain)
{
  typedef typename DomainType::config_type      ConfigType;
  typedef typename ConfigType::cell_tag                  CellTag;
  typedef typename viennagrid::result_of::ncell_type<ConfigType,
                                            CellTag::topology_level-1>::type      FacetType;
  typedef typename viennagrid::result_of::ncell_type<ConfigType,
                                            CellTag::topology_level>::type        CellType;

  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
                                            
  typedef typename viennagrid::result_of::ncell_range<CellType, CellTag::topology_level-1>::type  FacetOnCellContainer;
  typedef typename viennagrid::result_of::iterator<FacetOnCellContainer>::type                        FacetOnCellIterator;
  
  typedef std::map<FacetType *, std::size_t>  CellFacetMap;
  
  CellFacetMap cell_on_facet_cnt;
  
  CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    FacetOnCellContainer facets = viennagrid::ncells<CellTag::topology_level-1>(*cit);
    for (FacetOnCellIterator focit = facets.begin(); 
                      focit != facets.end();
                    ++focit)
    {
      cell_on_facet_cnt[&(*focit)] += 1;
    }
  }
  
  double domain_surface = 0;
  for (typename CellFacetMap::iterator cfmit = cell_on_facet_cnt.begin();
                                       cfmit != cell_on_facet_cnt.end();
                                     ++cfmit)
  {
    if (cfmit->second == 1)
    {
      domain_surface += volume_triangle(*(cfmit->first));
    }
  }
  
  return domain_surface;
}


template <typename DomainType>
int facet_check(DomainType & domain)
{
  typedef typename DomainType::config_type      ConfigType;
  typedef typename ConfigType::cell_tag                  CellTag;
  typedef typename viennagrid::result_of::ncell_type<ConfigType,
                                            CellTag::topology_level-1>::type      FacetType;
  typedef typename viennagrid::result_of::ncell_type<ConfigType,
                                            CellTag::topology_level>::type        CellType;

  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level-1>::type  FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type         FacetIterator;
                                            
  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
                                            
  typedef typename viennagrid::result_of::ncell_range<CellType, CellTag::topology_level-1>::type  FacetOnCellContainer;
  typedef typename viennagrid::result_of::iterator<FacetOnCellContainer>::type                        FacetOnCellIterator;
  
  typedef std::map<FacetType *, std::size_t>  CellFacetMap;
  
  CellFacetMap cell_on_facet_cnt;
  
  CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    FacetOnCellContainer facets = viennagrid::ncells<CellTag::topology_level-1>(*cit);
    for (FacetOnCellIterator focit = facets.begin(); 
                      focit != facets.end();
                    ++focit)
    {
      cell_on_facet_cnt[&(*focit)] += 1;
    }
  }
  
  for (typename CellFacetMap::iterator cfmit = cell_on_facet_cnt.begin();
                                       cfmit != cell_on_facet_cnt.end();
                                     ++cfmit)
  {
    if (cfmit->second > 2)
    {
      std::cerr << "Topology problem for facet: " << std::endl;
      cfmit->first->print_short();
      
      CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
      for (CellIterator cit = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        std::cout << "Cell: ";
        cit->print_short();
      }
      
      FacetContainer facets = viennagrid::ncells<CellTag::topology_level-1>(domain);
      for (FacetIterator fit = facets.begin();
                        fit != facets.end();
                      ++fit)
      {
        std::cout << "Facet: ";
        fit->print_short();
      }
      
      return EXIT_FAILURE;
    }
  }
  
  return EXIT_SUCCESS;
}


template <typename DomainType>
int surface_check(DomainType & domain_old, DomainType & domain_new)
{
  typedef typename DomainType::config_type      ConfigType;
  typedef typename ConfigType::cell_tag                  CellTag;
  typedef typename viennagrid::result_of::ncell_type<ConfigType,
                                            CellTag::topology_level-1>::type      FacetType;
  typedef typename viennagrid::result_of::ncell_type<ConfigType,
                                            CellTag::topology_level>::type        CellType;

  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level-1>::type  FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type         FacetIterator;
                                            
  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level>::type  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type         CellIterator;
                                            
  
  double old_surface = domain_surface(domain_old);
  double new_surface = domain_surface(domain_new);
  
  if ( (new_surface < 0.9999 * old_surface)
      || (new_surface > 1.0001 * old_surface) )
  {
    CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain_new);
    for (CellIterator cit = cells.begin();
                      cit != cells.end();
                    ++cit)
    {
      std::cout << "Cell: ";
      cit->print_short();
    }
    
    FacetContainer facets = viennagrid::ncells<CellTag::topology_level-1>(domain_new);
    for (FacetIterator fit = facets.begin();
                       fit != facets.end();
                     ++fit)
    {
      std::cout << "Facet: ";
      fit->print_short();
    }
    
    std::cerr << "Surface check failed!" << std::endl;
    std::cerr << "Domain surface before refinement: " << old_surface << std::endl;
    std::cerr << "Domain surface after refinement: " << new_surface << std::endl;
    return EXIT_FAILURE;
  }
    
  return EXIT_SUCCESS;
}

template <typename DomainType>
int volume_check(DomainType & domain_old, DomainType & domain_new)
{
  double old_volume = domain_volume(domain_old);
  double new_volume = domain_volume(domain_new);
  
  if ( (new_volume < 0.9999 * old_volume)
      || (new_volume > 1.0001 * old_volume) )
  {
    std::cerr << "Volume check failed!" << std::endl;
    return EXIT_FAILURE;
  }
    
  return EXIT_SUCCESS;
}

template <typename DomainType>
int sanity_check(DomainType & domain_old, DomainType & domain_new)
{
  if (facet_check(domain_new) != EXIT_SUCCESS)
    return EXIT_FAILURE;  //check for sane topology in new domain
    
  if (surface_check(domain_old, domain_new) != EXIT_SUCCESS)
    return EXIT_FAILURE; //check for same surface
  
  if (volume_check(domain_old, domain_new) != EXIT_SUCCESS)
    return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}
  