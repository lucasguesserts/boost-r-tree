// this code is an adaptation of the example provided in the boost documentation:
// https://www.boost.org/doc/libs/1_84_0/libs/geometry/doc/html/geometry/spatial_indexes/rtree_quickstart.html

#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/detail/touches/interface.hpp>
#include <boost/geometry/algorithms/is_empty.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>

#include <boost/geometry/index/predicates.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>

// to store queries results
#include <cstddef>
#include <vector>

// just for output
#include <iostream>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// CHOOSE ONE OF THE FOLLOWING (UNCOMMENT ONE AND COMMENT THE OTHER)
// 1. Boost Data Structures
// 2. Custom Data Structures

// // ------------------------------------------------------------
// // Boost Data Structures
// // ------------------------------------------------------------

// using Point = bg::model::point<double, 3, bg::cs::cartesian>;
// using Box = bg::model::box<Point>;
// using Value = std::pair<Box, unsigned>;
// using RTree = bgi::rtree<Value, bgi::quadratic<16>>;

// ------------------------------------------------------------
// Custom Data Structures
// ------------------------------------------------------------

class Point {
public:
    Point(double x, double y, double z)
        : _x(x)
        , _y(y)
        , _z(z) {}

    // set
    double & x() { return _x; }
    double & y() { return _y; }
    double & z() { return _z; }

    // get
    const double & x() const { return _x; }
    const double & y() const { return _y; }
    const double & z() const { return _z; }
private:
    double _x;
    double _y;
    double _z;
};
BOOST_GEOMETRY_REGISTER_POINT_3D(Point, double, bg::cs::cartesian, x(), y(), z());
// BOOST_GEOMETRY_REGISTER_POINT_3D_CONST(Point, double, bg::cs::cartesian, x(), y(), z()); // using this, the 'set' methods can be removed

class Box {
public:
    Box(const Point & min, const Point & max)
        : _min(min)
        , _max(max) {}

    const Point & min() const { return _min; }
    const Point & max() const { return _max; }
private:
    Point _min;
    Point _max;
};
BOOST_GEOMETRY_REGISTER_BOX(Box, Point, min(), max());

using Value = std::pair<Box, unsigned>;

using RTree = bgi::rtree<Value, bgi::quadratic<16>>;

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------

int main() {

    // create the rtree using default constructor
    RTree rtree;

    // add 8 boxes
    const auto boxes = std::vector{
        Box{Point(0, 0, 0), Point(0 + 4, 0 + 4, 0 + 4)},
        Box{Point(5, 0, 0), Point(5 + 4, 0 + 4, 0 + 4)},
        Box{Point(0, 5, 0), Point(0 + 4, 5 + 4, 0 + 4)},
        Box{Point(5, 5, 0), Point(5 + 4, 5 + 4, 0 + 4)},
        Box{Point(0, 0, 5), Point(0 + 4, 0 + 4, 5 + 4)},
        Box{Point(5, 0, 5), Point(5 + 4, 0 + 4, 5 + 4)},
        Box{Point(0, 5, 5), Point(0 + 4, 5 + 4, 5 + 4)},
        Box{Point(5, 5, 5), Point(5 + 4, 5 + 4, 5 + 4)},
    };
    for (size_t i = 0; i < boxes.size(); ++i) {
        rtree.insert(Value(boxes[i], i));
    }

    // Print all rectangles in the R-tree
    std::cout << std::endl;
    std::cout << "All rectangles in the R-tree:" << std::endl;
    for (const auto & v : rtree) {
        std::cout << bg::dsv(v.first) << " - " << v.second << std::endl;
    }

    // find values intersecting some area defined by a Box
    Box query_box(Point(3, 3, 4), Point(3 + 4, 3 + 4, 4 + 6));
    std::vector<Value> result_s;
    rtree.query(
        bgi::overlaps(query_box),
        std::back_inserter(result_s));

    // display results
    std::cout << std::endl;
    std::cout << "spatial query BoostBox:" << std::endl;
    std::cout << bg::dsv(query_box) << std::endl;
    std::cout << std::endl;
    std::cout << "spatial query result:" << std::endl;
    for (auto const & v : result_s) {
        std::cout << bg::dsv(v.first) << " - " << v.second << std::endl;
        // std::cout << bg::wkt<BoostBox>(v.first) << " - " << v.second << std::endl;
    }

    return 0;
}
