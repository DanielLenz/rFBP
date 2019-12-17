#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#if !defined __clang__ && __GNUC__ <= 6

  #include <magnetization_oldgcc.hpp>

#else

  #include <magnetization.hpp>

#endif

#include <cmath>
#include <random>

#define PRECISION 1e-4
#define SEED 42

#define isclose(x, y) ( std :: abs((x) - (y)) < PRECISION )

TEST_CASE ( "Test magnetization functions", "[magnetization]" )
{
  std :: mt19937 engine(SEED);
  std :: uniform_real_distribution < double > dist(0., 1.);

  for (int i = 0; i < 100; ++i)
  {
    const double rng_x = dist(engine);
    const double rng_y = dist(engine);
    MagP64 m(dist(engine));
    MagP64 n(dist(engine));
    MagP64 null(0.);

    // test clamp
    const double r11 = mag :: clamp(rng_x, .5, 1.);
    const double r12 = mag :: clamp(rng_x, 1., 1.);

    REQUIRE ( (r11 >= .5 && r11 <= 1.) );
    REQUIRE ( r12 == 1. );

    // test lr
    const double r21 = mag :: lr(rng_x);
    // TODO: increse tests

    REQUIRE ( r21 >= 0. );

    // test sign0
    const double r31 = mag :: sign0(rng_x);
    const double r32 = mag :: sign0(-rng_x);

    REQUIRE ( (r31 == 1L || r31 == -1L) );
    REQUIRE ( r32 == 1L );

    // test isinf
    const bool r41 = mag :: isinf(rng_x);
    const bool r42 = mag :: isinf(INF);
    const bool r43 = mag :: isinf(rng_x / 0.);

    REQUIRE ( r41 == false );
    REQUIRE ( r42 == true );
    REQUIRE ( r43 == true );

    // test signbit
    const bool r51 = mag :: signbit(m);
    const bool r52 = mag :: signbit(-m);

    REQUIRE ( r51 == false );
    REQUIRE ( r52 == true  );

    // test zero
    MagP64 m2(m);
    mag :: zero(m2);

    REQUIRE ( m2.mag     == 0. );
    REQUIRE ( m2.value() == 0. );

    // test abs
    const double r61 = mag :: abs(m);
    const double r62 = mag :: abs(-m);

    REQUIRE ( r61 >= 0. );
    REQUIRE ( r62 >= 0. );

    // test copysign
    MagP64 mm(-m.mag);
    const MagP64 r71 = mag :: copysign( m,  rng_x);
    const MagP64 r72 = mag :: copysign(mm,  rng_x);
    const MagP64 r73 = mag :: copysign( m, -rng_x);
    const MagP64 r74 = mag :: copysign(mm, -rng_x);

    REQUIRE ( r71.mag ==  m.mag );
    REQUIRE ( r72.mag ==  m.mag );
    REQUIRE ( r73.mag == -m.mag );
    REQUIRE ( r74.mag == -m.mag );

    // test sign0
    const long int r81 = mag :: sign0(m);
    const long int r82 = mag :: sign0(-m);

    REQUIRE ( (r81 == 1L || r81 == -1L) );
    REQUIRE ( r81 == -1L );
    REQUIRE ( r82 ==  1L );

    // test logmag2p
    const double r91 = mag :: logmag2p(MagP64(0.));
    const double r92 = mag :: logmag2p(MagP64(-1.));

    REQUIRE ( (r91 == std :: log(0.5)) );
    REQUIRE ( (r92 == INF || r92 == -INF) );

    // test convert
    const MagP64 r101 = mag :: convert < MagP64 >(rng_x);
    const double r102 = mag :: convert(r101);

    REQUIRE ( (isclose(r101.mag, rng_x) && isclose(r101.value(), rng_x)) );
    REQUIRE ( isclose(r102, rng_x) );

    // test couple
    const MagP64 r111 = mag :: couple < MagP64 >(rng_x, rng_y);
    const MagP64 r112 = mag :: couple < MagP64 >(rng_y, rng_x);
    const MagP64 r113 = mag :: couple < MagP64 >(rng_x, 0.);

    REQUIRE ( (isclose(r111.mag, (rng_x - rng_y)/(rng_x + rng_y)) && isclose(r111.value(), (rng_x - rng_y)/(rng_x + rng_y))));
    REQUIRE ( isclose(std :: abs(r111.mag), std :: abs(r112.mag)) );
    REQUIRE ( isclose(std :: abs(r111.value()), std :: abs(r112.value())) );
    REQUIRE ( isclose(r113.mag, 1.) );
    REQUIRE ( isclose(r113.value(), 1.) );

    // test damp
    const MagP64 r121 = mag :: damp(m, n, rng_x);
    const MagP64 r122 = mag :: damp(n, m, 1. - rng_x);
    const MagP64 r123 = mag :: damp(m, n, 0.);
    const MagP64 r124 = mag :: damp(m, n, 1.);

    REQUIRE ( isclose(r121.mag, r122.mag) );
    REQUIRE ( isclose(r121.value(), r122.value()) );
    REQUIRE ( isclose(r123.mag, m.mag) );
    REQUIRE ( isclose(r124.mag, n.mag) );

    // test mtanh
    const MagP64 r131 = mag :: mtanh < MagP64 >(rng_x);
    const MagP64 r132 = mag :: mtanh < MagP64 >(0.);
    const MagP64 r133 = mag :: mtanh < MagP64 >(INF);

    REQUIRE ( (r131.mag >= 0. && r131.mag <= 1.) );
    REQUIRE ( (r131.value() >= 0. && r131.value() <= 1.) );
    REQUIRE ( (isclose(r132.mag, 0.) && isclose(r132.value(), 0.)) );
    REQUIRE ( (isclose(r133.mag, 1.) && isclose(r133.value(), 1.)) );

    // test merf
    const MagP64 r141 = mag :: merf < MagP64 >(rng_x);
    const MagP64 r142 = mag :: merf < MagP64 >(0.);
    const MagP64 r143 = mag :: merf < MagP64 >(INF);

    REQUIRE ( (r141.mag >= 0. && r141.mag <= 1.) );
    REQUIRE ( (r141.value() >= 0. && r141.value() <= 1.) );
    REQUIRE ( (isclose(r142.mag, 0.) && isclose(r142.value(), 0.)) );
    REQUIRE ( (isclose(r143.mag, 1.) && isclose(r143.value(), 1.)) );

    // test bar
    const MagP64 r151 = mag :: bar(m, n);
    const MagP64 r152 = mag :: bar(m, m);
    const MagP64 r153 = mag :: bar(m, null);

    REQUIRE ( (r151.mag >= -1. && r151.mag <= 1.) );
    REQUIRE ( (r151.value() >= -1. && r151.value() <= 1.) );
    REQUIRE ( isclose(r152.mag, 0) );
    REQUIRE ( isclose(r152.value(), 0) );
    REQUIRE ( isclose(r153.mag, m.mag) );
    REQUIRE ( isclose(r153.value(), m.value()) );

    // test log1pxy
    const double r161 = mag :: log1pxy(m, null);
    const double r162 = mag :: log1pxy(MagP64(1.), MagP64(-1.));
    const double r163 = mag :: log1pxy(MagP64(-1.), MagP64(1.));

    REQUIRE ( isclose(r161, std :: log(.5)) );
    REQUIRE ( isclose(r162, r163) );
    REQUIRE ( ((r163 == -INF) || (r163 == INF)) );

    // test mcrossentropy
    const double r171 = mag :: mcrossentropy(m, null);
    const double r172 = mag :: mcrossentropy(null, m);
    const double r173 = mag :: mcrossentropy(null, null);
    const double r174 = mag :: mcrossentropy(m, MagP64(1.));

    REQUIRE ( isclose(r171, std :: log(2)) );
    REQUIRE ( isclose(r172, - std :: log(1. - m.mag*m.mag) * .5 + std :: log(2)) );
    REQUIRE ( isclose(r173, std :: log(2)) );
    REQUIRE ( isclose(r173, r171) );
    REQUIRE ( (r174 == -INF || r174 == INF) );

    // test logZ
    // TODO

    // test auxmix
    // TODO

    // test erfmix
    // TODO

    // test exactmix
    // TODO
  }
}
