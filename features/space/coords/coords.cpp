#include <iostream>
#include <string>
#include <print>
#include <vector>
#include <cmath>
#include <array>

const int sectorsize = 20000;

std::array<double, 3> CoordsAbs;

struct CoordsFmt {
    std::string quadrant;
    int sx;
    int sy;
    int sz;
    double x;
    double y;
    double z;
};

CoordsFmt parse_coords(const std::string& input) {
    std::vector<std::string> fields;
    std::string current;

    for (char ch : input) {
        if (ch == ' ' || ch == ':') {
            if (!current.empty()) {
                fields.push_back(current);
                current.clear();
            }
        } else {
            current += ch;
        }
    }

    if (!current.empty()) {
        fields.push_back(current);
    }
    return {fields[0], std::stoi(fields[1]), std::stoi(fields[2]), std::stoi(fields[3]), std::stod(fields[4]), std::stod(fields[5]), std::stod(fields[6])};
}

std::array<double, 3> fmt2abs(const CoordsFmt& input) {
    // logic to convert to absolute xyz goes here
    std::array<double, 3> result;
    int x_sign = 1, y_sign = 1, z_sign = 1;

    if (input.quadrant.starts_with("Beta") || input.quadrant.starts_with("Gamma"))
        x_sign = -1;
    if (input.quadrant.starts_with("Gamma") || input.quadrant.starts_with("Delta"))
        y_sign = -1;
    if (input.quadrant.ends_with("-"))
        z_sign = -1;

    result[0] = x_sign * (input.sx * sectorsize + (input.x + 10000));
    result[1] = y_sign * (input.sy * sectorsize + (input.y + 10000));
    result[2] = z_sign * (input.sz * sectorsize + (input.z + 10000));
    return result;
}

CoordsFmt abs2fmt(const std::array<double, 3>& input) {
    // logic to convert to formatted goes here
    CoordsFmt result;
    if (input[0] >= 0 && input[1] >= 0 && input[2] > 0)
	result.quadrant = "Alpha+";
    else if (input[0] >= 0 && input[1] >= 0 && input[2] <= 0)
	result.quadrant = "Alpha-";
    else if (input[0] < 0 && input[1] >= 0 && input[2] > 0)
	result.quadrant = "Beta+";
    else if (input[0] < 0 && input[1] >= 0 && input[2] <= 0)
	result.quadrant = "Beta-";
    else if (input[0] < 0 && input[1] < 0 && input[2] > 0)
	result.quadrant = "Gamma+";
    else if (input[0] < 0 && input[1] < 0 && input[2] <= 0)
	result.quadrant = "Gamma-";
    else if (input[0] >= 0 && input[1] < 0 && input[2] > 0)
	result.quadrant = "Delta+";
    else if (input[0] >= 0 && input[1] < 0 && input[2] <= 0)
	result.quadrant = "Delta-";


    result.sx = std::abs(input[0] / sectorsize);
    result.sy = std::abs(input[1] / sectorsize);
    result.sz = std::abs(input[2] / sectorsize);

    result.x = std::round((std::fmod(std::abs(input[0]),sectorsize) - 10000)*100) / 100;
    result.y = std::round((std::fmod(std::abs(input[1]),sectorsize) - 10000)*100) / 100;
    result.z = std::round((std::fmod(std::abs(input[2]),sectorsize) - 10000)*100) / 100;

    return result;
}

double dist3d(std::array<double, 3> obj1, std::array<double, 3> obj2) {
    double dist{};
    // calculate distance
    return sqrt(pow(obj2[0] - obj1[0], 2) + pow(obj2[1] - obj1[1], 2) + pow(obj2[2] - obj1[2], 2) * 1.0);
}

std::string showdist(double dist) {
    std::string str{};
    if (dist >= 20000) {
        str = std::to_string(dist/sectorsize) + " Sctrs";
        return str;
    }
    else if (dist < 0.1) {
        str = std::to_string(dist*1000000) + " Kms";
        return str;
    }
    else
        return std::to_string(dist) + " Gms";
}

int main() {

    std::string input_coords_from{};
    std::println("Enter coords from in formatted way: ");
    std::getline(std::cin, input_coords_from);
 
    std::string input_coords_to{};
    std::println("Enter coords to in formatted way: ");
    std::getline(std::cin, input_coords_to);   

    double dist = dist3d(fmt2abs(parse_coords(input_coords_from)), fmt2abs(parse_coords(input_coords_to)));

    std::println("Distance: {} Gms", dist);
    std::println("Distance string: {}", showdist(dist));

    /*
    std::println("Quadrant: {}", parse_coords(input_coords).quadrant);
    std::println("Sector: {}:{}:{}", parse_coords(input_coords).sx, parse_coords(input_coords).sy, parse_coords(input_coords).sz);
    std::println("X Y Z: {} {} {}", parse_coords(input_coords).x, parse_coords(input_coords).y, parse_coords(input_coords).z);
    std::println("Absolute coords: {}:{}:{}", fmt2abs(parse_coords(input_coords)).x, fmt2abs(parse_coords(input_coords)).y, fmt2abs(parse_coords(input_coords)).z);

    CoordsAbs coords_input;
    std::println("Enter absolute coords: ");

//    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin >> coords_input.x >> coords_input.y >> coords_input.z;

    std::println("Formatted coords: {} {}:{}:{} {}:{}:{}", abs2fmt(coords_input).quadrant, abs2fmt(coords_input).sx, abs2fmt(coords_input).sy, abs2fmt(coords_input).sz, abs2fmt(coords_input).x, abs2fmt(coords_input).y, abs2fmt(coords_input).z);
    */
    return 0;
}
