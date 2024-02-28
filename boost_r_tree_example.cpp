// this code is an adaptation of the example provided in the boost documentation:
// https://www.boost.org/doc/libs/1_84_0/libs/geometry/doc/html/geometry/spatial_indexes/rtree_quickstart.html

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>

#include <boost/geometry/index/rtree.hpp>

// to store queries results
#include <vector>

// just for output
#include <iostream>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

using Point = bg::model::point<double, 2, bg::cs::cartesian>;
using Box = bg::model::box<Point>;
using Value = std::pair<Box, unsigned>;
using RTree = bgi::rtree<Value, bgi::quadratic<16>>;

int main() {

    // create the rtree using default constructor
    RTree rtree;

    // create some values
    for (unsigned i = 0; i < 10; ++i) {
        // create a Box
        Box b(
            Point(i + 0.0, i + 0.0),
            Point(i + 2.0, i + 2.0));
        // insert new Value
        rtree.insert(std::make_pair(b, i));
    }

    // Print all rectangles in the R-tree
    std::cout << std::endl;
    std::cout << "All rectangles in the R-tree:" << std::endl;
    for (const auto & v : rtree) {
        std::cout << bg::dsv(v.first) << " - " << v.second << std::endl;
    }

    // find values intersecting some area defined by a Box
    Box query_box(Point(1, 1), Point(3, 3));
    std::vector<Value> result_s;
    auto has_area_intersection = [&query_box](const Value& v) {
        Box R;
        bg::intersection(v.first, query_box, R);
        return bg::area(R) > 0;
    };
    rtree.query(bgi::intersects(query_box) && bgi::satisfies(has_area_intersection), std::back_inserter(result_s));

    // display results
    std::cout << std::endl;
    std::cout << "spatial query Box:" << std::endl;
    std::cout << bg::dsv(query_box) << std::endl;
    std::cout << std::endl;
    std::cout << "spatial query result:" << std::endl;
    for (Value const & v : result_s) {
        std::cout << bg::dsv(v.first) << " - " << v.second << std::endl;
        // std::cout << bg::wkt<Box>(v.first) << " - " << v.second << std::endl;
    }

    return 0;
}
