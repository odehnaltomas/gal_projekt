#include "graph_generator.h"


GraphGenerator::GraphGenerator() {}

void GraphGenerator::generatePlanarGraph(unsigned int num_nodes, unsigned int num_edges, std::string fileName) {
    Graph g;

    randomPlanarConnectedGraph(g, num_nodes, num_edges);
    GraphGenerator::createLayoutAndSave(g, fileName);
}

void GraphGenerator::generateNonplanarGraph(unsigned int num_nodes, std::string fileName) {
    Graph g;

    randomSimpleGraph(g, num_nodes, (3 * num_nodes - 6));

    int num_tries = 0;
    BoothLueker b;
    while (b.isPlanar(g)) { // This could probably be better done
        if (num_tries > 1000) {
            std::cerr << "Could not generate non-planar graph." << std::endl;
            exit(1);
        }
    }

    GraphGenerator::createLayoutAndSave(g, fileName);
}

void GraphGenerator::createLayoutAndSave(Graph g, std::string fileName) {
    GraphAttributes GA( g, GraphAttributes::nodeGraphics |
                           GraphAttributes::edgeGraphics |
                           GraphAttributes::nodeLabel |
                           GraphAttributes::nodeStyle |
                           GraphAttributes::edgeType |
                           GraphAttributes::edgeArrow |
                           GraphAttributes::edgeStyle ); // Create graph attributes for this graph

    for (node v : g.nodes) { // iterate through all the node in the graph
        GA.fillColor( v ) = Color( "#FFFF00" ); // set node color to yellow

        GA.height( v ) = 20.0; // set the height to 20.0
        GA.width( v ) = 20.0; // set the width to 40.0
        GA.shape(v) = ogdf::Shape::Ellipse;

        string s = to_string(v->index());
        char const *pchar = s.c_str(); //use char const* as target type
        GA.label( v ) = pchar;
    }

    for (edge e : g.edges) // set default edge color and type
    {
        GA.bends(e);
        GA.arrowType(e) = ogdf::EdgeArrow::None;
        GA.strokeColor(e) = Color("#0000FF");
    }

    SugiyamaLayout SL; //Compute a hierarchical drawing of G (using SugiyamaLayout)
    SL.setRanking( new OptimalRanking );
    SL.setCrossMin( new MedianHeuristic );

    OptimalHierarchyLayout *ohl = new OptimalHierarchyLayout;

    SL.setLayout( ohl );
    SL.call( GA );
    GraphIO::write(GA, fileName, GraphIO::writeGML);
}