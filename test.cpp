

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

#include <pqxx/pqxx>
#include <stdexcept>

#define WIDTH 8000
#define HEIGHT 5000
#define MARGINS_M 1000
#define TOWNSHIP_ADMINLVL 8
#define AUTOSIZE false

#define DB_USER "herrcrazi"
#define DB_PASSWD "chenco"
#define DB_NAME "kartenn-gis-dev"



// Split a string into an array of substring using one or several delimiter chars
std::vector<std::string> split(std::string str, std::string delim) {
    std::vector<std::string> tokens;
    std::string::size_type last_token_start = 0, i = 0;

    for (i = 0; i < str.size(); ++i)
    {
        if ( delim.find(str[i]) != std::string::npos ) {
            tokens.push_back(str.substr(last_token_start, i - last_token_start));
            last_token_start = i+1;
        }
    }
    tokens.push_back(str.substr(last_token_start, i - last_token_start));

    return tokens;
}


// brisingr !
int main(int argc, const char **argv)
{

    /****************************************/
    /*             Township checks          */
    /****************************************/

    // Check if a township has been provided
    if ( argc < 2) {
        std::cerr << "Error: A township name or postcode must be provided\n";
        std::cerr << "Usage: rendermap <name|postcode> [<size_x> <size_y>]\n";
        exit(-1);
    }

    std::vector<double> map_extent;
    std::string bbox;

    // Determine the total extent of the requested township, with margins
    try
    {
        //Connect to the database
        std::stringstream params; params << "user=" << DB_USER << " password=" << DB_PASSWD << " dbname=" << DB_NAME;
        pqxx::connection db(params.str());
        pqxx::work query(db);

        // Execute query
        std::stringstream query_string;
        query_string << "SELECT ST_Expand(ST_Extent(way), " << MARGINS_M << ") as bbox FROM planet_osm_polygon WHERE admin_level = '" << TOWNSHIP_ADMINLVL << "' AND boundary = 'administrative' AND name LIKE '%" << argv[1] << "%';";
        pqxx::result res = query.exec( query_string.str() );

        std::cout << "Returned " << res.columns() << " columns and " << res.size() << " rows\n";

        // Check the returned response
        if (res.size() > 0 && !res[0][0].is_null())
            bbox = res[0][0].as<std::string>();
        else throw std::runtime_error("Couldn't find this township in the database.");
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        std::cerr << ">> INFO:\n\tdb_user:\t" << DB_USER << "\n\tdb_passwd:\t" << DB_PASSWD << "\n\tdb_name:\t" << DB_NAME << "\n\ttownship:\t" << argv[1] << "\n";
        exit(-2);
    }
    
    // Parse the returned bounding box into a vector of 4 coords as doubles
    bbox = bbox.substr(4, bbox.length() - 5);
    std::cout << "Bounding box: " << bbox << "\n";

    for ( auto &coord : split(bbox, " ,") ) {
        map_extent.push_back(std::stod(coord));
    }



    /****************************************/
    /*          Main map rendering          */
    /****************************************/

    std::cout << "Rendering map...\n";
    
    // Register Mapnik's fonts and plugins
    mapnik::datasource_cache::instance().register_datasources("/usr/lib/mapnik/3.0/input");
    mapnik::freetype_engine::register_font("/usr/share/fonts/truetype/dejavu/DejaVuSansCondensed.ttf");

    // Set the map size and projection
    mapnik::Map m(WIDTH, HEIGHT);
    mapnik::load_map(m, "test.xml");
    
    m.set_srs("+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs");

    // 
    m.zoom_to_box(mapnik::box2d<double>(map_extent[0], map_extent[1], map_extent[2], map_extent[3]));

    std::cout << "Layers :\n";
    for (auto &&layer : m.layers())
    {
        std::cout << "\t" << layer.name() << "\n";        
    }
    std::cout << "Scale: 1:" << m.scale() << "\n";

    //Rendering
    mapnik::image_rgba8 im(WIDTH,HEIGHT);
    mapnik::agg_renderer<mapnik::image_rgba8> ren(m, im);
    ren.apply();
    mapnik::save_to_file(im, "our_world.png");

    std::cout << "Done !\n";

    return 0;
}

