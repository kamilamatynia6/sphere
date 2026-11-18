// CONTAINERS AND ALGORITHMS - TASKS
// N = 30 (indywidualny numer), promien sfery R = 8.0 + 0.01 * N = 8.3 m
//
// 1. Konwersja punktow z ukladu kartezjanskiego do sferycznego
// 2. Wybor punktow lezacych blizej niz 1.0 m od sfery o promieniu R
// 3. Z punktow z p. 2 wybor: a) punktow nad sfera, b) punktow wewnatrz sfery
// 4. Zapis punktow z 3a i 3b do dwoch osobnych plikow
// 5. Odleglosci punktow z p. 2 od sfery
// 6. 3 punkty najblizej sfery
// 7. Rzutowanie punktow z p. 2 na sfere
// 8. Konwersja punktow z p. 7 z powrotem do ukladu kartezjanskiego i zapis do pliku
// 9. Wyswietlenie wynikow z p. 4 i p. 8 w CloudCompare

#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <iomanip>

#include "geometry.hpp"

// punkt we wspolrzednych sferycznych
struct SphericalPoint
{
    double r{0.0};      // odleglosc od poczatku ukladu
    double theta{0.0};  // kat od osi Z (0 .. pi)
    double phi{0.0};    // kat w plaszczyznie XY od osi X (-pi .. pi)
};

SphericalPoint cartesianToSpherical(const geometry::Point3d& p)
{
    const double r{std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z)};
    const double theta{std::acos(p.z / r)};
    const double phi{std::atan2(p.y, p.x)};
    return SphericalPoint{r, theta, phi};
}

geometry::Point3d sphericalToCartesian(const SphericalPoint& s)
{
    return geometry::Point3d{
        s.r * std::sin(s.theta) * std::cos(s.phi),
        s.r * std::sin(s.theta) * std::sin(s.phi),
        s.r * std::cos(s.theta)
    };
}

void writePointsToFile(const std::vector<SphericalPoint>& points, const std::filesystem::path& pathToFile)
{
    std::ofstream file{pathToFile};
    file << std::fixed << std::setprecision(6);
    file << "x,y,z\n";
    for (const auto& s: points)
    {
        const auto p{sphericalToCartesian(s)};
        file << p.x << "," << p.y << "," << p.z << "\n";
    }
    std::cout << "Zapisano " << points.size() << " punktow do pliku " << pathToFile << std::endl;
}

int main()
{
    std::filesystem::path pathToPointsIn3d{"points_in_3d_30.txt"};
    const auto pointsIn3D {geometry::readPointsIn3DFromFile(pathToPointsIn3d)};
    std::cout << "Liczba wczytanych punktow: " << pointsIn3D.size() << std::endl;

    const int N{30};
    const double sphereRadius{8.0 + 0.01 * N};
    const double maxDistance{1.0};
    std::cout << "Promien sfery R = " << sphereRadius << " m" << std::endl;

    // 1. konwersja do wspolrzednych sferycznych
    std::vector<SphericalPoint> sphericalPoints(pointsIn3D.size());
    std::ranges::transform(pointsIn3D, sphericalPoints.begin(), cartesianToSpherical);

    // 2. punkty blizej niz 1.0 m od sfery
    std::vector<SphericalPoint> nearSphere;
    std::ranges::copy_if(sphericalPoints, std::back_inserter(nearSphere),
        [sphereRadius, maxDistance](const SphericalPoint& s)
        {
            return std::abs(s.r - sphereRadius) < maxDistance;
        });
    std::cout << "Punkty blizej niz " << maxDistance << " m od sfery: " << nearSphere.size() << std::endl;

    // 3a. punkty nad sfera (na zewnatrz)
    std::vector<SphericalPoint> aboveSphere;
    std::ranges::copy_if(nearSphere, std::back_inserter(aboveSphere),
        [sphereRadius](const SphericalPoint& s) { return s.r > sphereRadius; });

    // 3b. punkty wewnatrz sfery
    std::vector<SphericalPoint> insideSphere;
    std::ranges::copy_if(nearSphere, std::back_inserter(insideSphere),
        [sphereRadius](const SphericalPoint& s) { return s.r <= sphereRadius; });

    std::cout << "Nad sfera: " << aboveSphere.size() << ", wewnatrz sfery: " << insideSphere.size() << std::endl;

    // 4. zapis 3a i 3b do dwoch plikow
    writePointsToFile(aboveSphere, "points_above_sphere.txt");
    writePointsToFile(insideSphere, "points_inside_sphere.txt");

    // 5. odleglosci punktow z p. 2 od sfery
    std::vector<double> distances(nearSphere.size());
    std::ranges::transform(nearSphere, distances.begin(),
        [sphereRadius](const SphericalPoint& s) { return std::abs(s.r - sphereRadius); });
    std::cout << "Najmniejsza odleglosc: " << std::ranges::min(distances) << " m, "
              << "najwieksza: " << std::ranges::max(distances) << " m" << std::endl;

    // 6. 3 punkty najblizej sfery
    std::vector<SphericalPoint> closest{nearSphere};
    std::ranges::partial_sort(closest, closest.begin() + 3,
        [sphereRadius](const SphericalPoint& a, const SphericalPoint& b)
        {
            return std::abs(a.r - sphereRadius) < std::abs(b.r - sphereRadius);
        });

    std::cout << "\n3 punkty najblizej sfery:\n";
    for (int i{0}; i < 3; i++)
    {
        const auto p{sphericalToCartesian(closest[i])};
        std::cout << p.x << " " << p.y << " " << p.z
                  << "  odleglosc: " << std::abs(closest[i].r - sphereRadius) << " m\n";
    }
    std::cout << std::endl;

    // 7. rzutowanie na sfere: katy bez zmian, r = R
    std::vector<SphericalPoint> projected(nearSphere.size());
    std::ranges::transform(nearSphere, projected.begin(),
        [sphereRadius](const SphericalPoint& s) { return SphericalPoint{sphereRadius, s.theta, s.phi}; });

    // 8. powrot do ukladu kartezjanskiego i zapis do pliku
    writePointsToFile(projected, "points_projected_on_sphere.txt");

    return EXIT_SUCCESS;
}