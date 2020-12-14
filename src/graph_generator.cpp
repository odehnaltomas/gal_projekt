#include <ogdf/basic/graph_generators.h>
#include <ogdf/basic/simple_graph_alg.h>
#include "graph_generator.h"
#include "utils.h"


GraphGenerator::GraphGenerator() {}

void GraphGenerator::generatePlanarGraph(unsigned int num_nodes, unsigned int num_edges, std::string fileName) {
    Graph g;

    randomPlanarBiconnectedGraph(g, num_nodes, num_edges);
    BoothLueker b;
    if (!b.isPlanar(g)) {
        std::cerr << "Could not generate non-planar graph." << std::endl;
    }

    std::cout << "Generated PLANAR graph '" << fileName;

    GraphGenerator::createLayoutAndSave(g, fileName);
}

void GraphGenerator::generateNonplanarGraph(unsigned int num_nodes, std::string fileName) {
    Graph g;

    completeGraph(g, num_nodes);
    std::cout << "***************************" << std::endl;
    std::cout << "num nodes: " << num_nodes << ", num edges: " << g.numberOfEdges() << std::endl;

    BoothLueker b;
    if (isBiconnected(g)) {
        std::cout << "is no biconnected -> transforming to biconnected" << std::endl;
        makeBiconnected(g);
    }
    if (b.isPlanar(g)) {
        std::cerr << "Could not generate non-planar graph." << std::endl;
        exit(1);
    }

    std::cout << "Generated NON-PLANAR graph '" << fileName;

    GraphGenerator::createLayoutAndSave(g, fileName);
}

void GraphGenerator::createLayoutAndSave(Graph g, std::string fileName) {
    GraphAttributes GA( g, GraphAttributes::nodeGraphics |
                           GraphAttributes::edgeGraphics |
                           GraphAttributes::nodeLabel |
                           GraphAttributes::nodeStyle |
                           GraphAttributes::edgeType |
                            GraphAttributes::edgeLabel |
                           GraphAttributes::edgeArrow |
                           GraphAttributes::edgeStyle ); // Create graph attributes for this graph

    GA.directed() = false;
    int n = 0;
    for (node v : g.nodes) { // iterate through all the node in the graph
        n++;
        GA.fillColor( v ) = Color( "#FFFF00" ); // set node color to yellow

        GA.height( v ) = 20.0; // set the height to 20.0
        GA.width( v ) = 20.0; // set the width to 40.0
        GA.shape(v) = ogdf::Shape::Ellipse;

        string s = to_string(v->index());
        char const *pchar = s.c_str(); //use char const* as target type
        GA.label( v ) = pchar;
    }

    int m = 0;
    for (edge e : g.edges) { // set default edge color and type
        m++;
        GA.bends(e);
        GA.arrowType(e) = ogdf::EdgeArrow::None;
        GA.strokeColor(e) = Color("#0000FF");

        string s = to_string(e->index());
        char const *pchar = s.c_str(); //use char const* as target type
        GA.label( e ) = pchar;
    }

    SugiyamaLayout SL; //Compute a hierarchical drawing of G (using SugiyamaLayout)
    SL.setRanking( new OptimalRanking );
    SL.setCrossMin( new MedianHeuristic );

    OptimalHierarchyLayout *ohl = new OptimalHierarchyLayout;

    SL.setLayout( ohl );
    SL.call( GA );
    GraphIO::write(GA, fileName, GraphIO::writeGML);

    std::cout << "' with " << n << " and " << m << " edges." << std::endl;
   // std::cout << "***************************" << std::endl;
}