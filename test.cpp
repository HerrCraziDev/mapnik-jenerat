

#include <mapnik/map.hpp>
#include <mapnik/layer.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/image.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/plugin.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/projection.hpp>
#include <mapnik/proj_transform.hpp>

#define WIDTH 8000
#define HEIGHT 5000

//sfekeglr
main()
{
    std::cout << "Rendering map...\n";
    
    mapnik::datasource_cache::instance().register_datasources("/usr/lib/mapnik/3.0/input");
    mapnik::freetype_engine::register_font("/usr/share/fonts/truetype/dejavu/DejaVuSansCondensed.ttf");

    mapnik::Map m(WIDTH, HEIGHT);
    mapnik::load_map(m, "test.xml");
    
    m.set_srs("+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs");

    // auto baseExtent = mapnik::box2d<double>(-4.5048, 48.39, -4.4712, 48.3811);
    // auto mercExtent = mapnik::box2d<double>(-501503.342579, 6172710.437684, -496458.498712, 6170111.578722);
    // mapnik::proj_transform(mapnik::projection("merc"), mapnik::projection("lonlat"));
    //5387173.17      -498635.86    5385759.42      -494908.99

    //-501503.342579 6172710.437684 -496458.498712 6170111.578722
    //-501013.2,6170139.6 : -497558.1,6172988.6
    m.zoom_to_box(mapnik::box2d<double>(-518535.5, 6147162.1, -503794.3, 6155771.8));

    // m.layers()[0].
    std::cout << "Scale: 1:" << m.scale() << "\n";
    // m.zoom_all();

    //Rendering
    mapnik::image_rgba8 im(WIDTH,HEIGHT);
    mapnik::agg_renderer<mapnik::image_rgba8> ren(m, im);
    ren.apply();
    mapnik::save_to_file(im, "our_world.png");

    std::cout << "Done !\n";
}

