#include <iostream>
#include <typeinfo>

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/accessor.hpp"
#include "viennagrid/domain/segmentation.hpp"
#include "viennagrid/domain/element_creation.hpp"

#include "viennagrid/io/vtk_writer.hpp"



int main()
{
  typedef viennagrid::triangular_2d_domain                        domain_type;
  typedef viennagrid::result_of::domain_view< domain_type >::type view_type;
  
  typedef viennagrid::result_of::point_type<domain_type>::type point_type;
  
  typedef viennagrid::result_of::cell_type< domain_type >::type     cell_type;
  typedef viennagrid::result_of::vertex< domain_type >::type     vertex_type;
  typedef viennagrid::result_of::vertex_handle< domain_type >::type     vertex_handle_type;
  typedef viennagrid::result_of::triangle_handle< domain_type >::type   triangle_handle_type;
  typedef viennagrid::result_of::triangle< domain_type >::type          triangle_type;
  
    
  typedef viennagrid::result_of::oriented_3d_hull_segmentation<domain_type>::type segmentation_type;
//   typedef viennagrid::result_of::segmentation<domain_type>::type segmentation_type;
  
  typedef segmentation_type::segment_type segment_type;
  typedef segmentation_type::segment_id_type segment_id_type;
  
  domain_type domain;
  segmentation_type segmentation(domain);
  
  
  vertex_handle_type vh0 = viennagrid::create_vertex( domain, point_type( 0,  0) );
  vertex_handle_type vh1 = viennagrid::create_vertex( domain, point_type(-1, -1) );
  vertex_handle_type vh2 = viennagrid::create_vertex( domain, point_type(-1,  1) );
  vertex_handle_type vh3 = viennagrid::create_vertex( domain, point_type( 1,  1) );
  vertex_handle_type vh4 = viennagrid::create_vertex( domain, point_type( 1, -1) );
  
  triangle_handle_type th0 = viennagrid::create_triangle( domain, vh0, vh1, vh2 );
  triangle_handle_type th1 = viennagrid::create_triangle( domain, vh0, vh2, vh3 );
  triangle_handle_type th2 = viennagrid::create_triangle( domain, vh0, vh3, vh4 );
  triangle_handle_type th3 = viennagrid::create_triangle( domain, vh0, vh4, vh1 );
  
  

  segment_type seg0 = segmentation.create_segment();
  segment_type seg1 = segmentation.create_segment();
  
   
  triangle_type & tri0 = viennagrid::dereference_handle(domain, th0);
  triangle_type & tri1 = viennagrid::dereference_handle(domain, th1);
  triangle_type & tri2 = viennagrid::dereference_handle(domain, th2);
  triangle_type & tri3 = viennagrid::dereference_handle(domain, th3);
  
  
  viennagrid::add_to_segment( seg0, tri0 );
  viennagrid::add_to_segment( seg0, tri1 );
  viennagrid::add_to_segment( seg1, tri2 );
  viennagrid::add_to_segment( seg1, tri3 );
  
//   triangle_handle_type th3 = viennagrid::create_triangle( seg2, vh1, vh4, vh5 );
  
  
  std::deque< double > potential_cell(4);
  potential_cell[0] = 0;
  potential_cell[1] = 1;
  potential_cell[2] = 2;
  potential_cell[3] = 3;
  
  std::deque< double > potential_point(5);
  potential_point[0] = 0;
  potential_point[1] = 1;
  potential_point[2] = 2;
  potential_point[3] = 3;
  potential_point[4] = 4;
  
  
  std::deque< double > potential_cell_segment(4);
  potential_cell_segment[0] = 0;
  potential_cell_segment[1] = 1;
  potential_cell_segment[2] = 2;
  potential_cell_segment[3] = 3;
  
  std::deque< double > potential_point_segment(5);
  potential_point_segment[0] = 4;
  potential_point_segment[1] = 3;
  potential_point_segment[2] = 2;
  potential_point_segment[3] = 1;
  potential_point_segment[4] = 0;
  
  
  
  viennagrid::io::vtk_writer<domain_type, segmentation_type> writer;
  
  writer.add_scalar_data_on_vertices( viennagrid::accessor::dense_container_accessor<vertex_type>(potential_point), "potential_point" );
  writer.add_scalar_data_on_vertices( seg0, viennagrid::accessor::dense_container_accessor<vertex_type>(potential_point_segment), "potential_point_segment" );
  
  writer.add_scalar_data_on_cells( viennagrid::accessor::dense_container_accessor<cell_type>(potential_cell), "potential_cell" );
  writer.add_scalar_data_on_cells( seg1, viennagrid::accessor::dense_container_accessor<cell_type>(potential_cell_segment), "potential_cell_segment" );
  
  writer( domain, segmentation, "test" );
  

    
    return 0;
}