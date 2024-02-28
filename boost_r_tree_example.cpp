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

using Storage = std::list<Box>; // it doesn't work with vector

class BoxWrapper {
public:
    BoxWrapper(Storage::const_iterator it)
        : _it(it) {}

    const Storage::const_iterator & it() const { return _it; }

    const Point & min() const { return _it->min(); }
    const Point & max() const { return _it->max(); }
private:
    Storage::const_iterator _it;
};
BOOST_GEOMETRY_REGISTER_BOX(BoxWrapper, Point, min(), max());

using RTree = bgi::rtree<BoxWrapper, bgi::quadratic<16>>;

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------

int main() {

    // create the rtree using default constructor
    RTree rtree;

    // add 8 boxes
    auto boxes = Storage{
        Box{Point(0, 0, 0), Point(0 + 4, 0 + 4, 0 + 4)},
        Box{Point(5, 0, 0), Point(5 + 4, 0 + 4, 0 + 4)},
        Box{Point(0, 5, 0), Point(0 + 4, 5 + 4, 0 + 4)},
        Box{Point(5, 5, 0), Point(5 + 4, 5 + 4, 0 + 4)},
        Box{Point(0, 0, 5), Point(0 + 4, 0 + 4, 5 + 4)},
        Box{Point(5, 0, 5), Point(5 + 4, 0 + 4, 5 + 4)},
        Box{Point(0, 5, 5), Point(0 + 4, 5 + 4, 5 + 4)},
        Box{Point(5, 5, 5), Point(5 + 4, 5 + 4, 5 + 4)},
    };
    for (auto box_it = boxes.begin(); box_it != boxes.end(); ++box_it) {
        rtree.insert(BoxWrapper(box_it));
    }

    // Print all rectangles in the R-tree
    std::cout << std::endl;
    std::cout << "All rectangles in the R-tree:" << std::endl;
    for (const auto & box_it : rtree) {
        std::cout << bg::dsv(box_it) << std::endl;
    }

    // find values intersecting some area defined by a Box
    Box query_box(Point(3, 3, 4), Point(3 + 4, 3 + 4, 4 + 6));
    std::vector<BoxWrapper> result_s;
    rtree.query(
        bgi::overlaps(query_box),
        std::back_inserter(result_s));

    // display results
    std::cout << std::endl;
    std::cout << "spatial query BoostBox:" << std::endl;
    std::cout << bg::dsv(query_box) << std::endl;
    std::cout << std::endl;
    std::cout << "spatial query result:" << std::endl;
    for (auto const & box_it : result_s) {
        std::cout << bg::dsv(box_it) << std::endl;
        // std::cout << bg::wkt<BoostBox>(v.first) << " - " << v.second << std::endl;
    }

    // delete all values from the R-tree
    std::cout << std::endl;
    std::cout << "delete values:" << std::endl;
    for (auto it = boxes.begin(); it != boxes.end();) {
        auto value_to_remove = BoxWrapper(it);
        rtree.remove(value_to_remove);
        it = boxes.erase(it);
    }
    if (boxes.empty()) {
        std::cout << "Boxes are empty" << std::endl;
    } else {
        std::cout << "Boxes are NOT empty" << std::endl;
    }
    if (rtree.empty()) {
        std::cout << "R-tree is empty" << std::endl;
    } else {
        std::cout << "R-tree is NOT empty" << std::endl;
    }

    return 0;
}
