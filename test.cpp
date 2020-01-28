

#include <mapnik/map.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/image.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/plugin.hpp>
#include <mapnik/datasource_cache.hpp>


#define WIDTH 1600
#define HEIGHT 700


main()
{
    mapnik::datasource_cache::instance().register_datasources("/usr/lib/mapnik/3.0/input");

    mapnik::Map m(WIDTH, HEIGHT);
    mapnik::load_map(m, "test.xml");
    m.zoom_all();
    mapnik::image_rgba8 im(WIDTH,HEIGHT);
    mapnik::agg_renderer<mapnik::image_rgba8> ren(m, im);
    ren.apply();
    mapnik::save_to_file(im, "our_world.png");
}

