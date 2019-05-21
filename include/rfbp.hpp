#ifndef RFBP_HPP
#define RFBP_HPP

#include <rfbp.h>

template < class Mag >
double theta_node_update_approx (MagVec < Mag > m, Mag & M, const double * xi, MagVec < Mag > u, Mag & U, const Params < Mag > & params, const long int & nxi, const long int & nm)
{
#ifdef DEBUG
  assert (nxi == nm);
#endif
  static double maxdiff,
                mu    ,
                sigma2;
  static Mag new_U;
#ifdef _OPENMP
#pragma omp barrier
#endif
  new_U = U;
  maxdiff = 0.;
  mu      = 0.;
  sigma2  = 0.;
  double dsigma2, g, p0, pmu, psigma;
  Mag old_m_on(0.),
      new_u(0.),
      new_m(0.);

  static std :: unique_ptr < Mag[] > h;
#ifdef _OPENMP
#pragma omp single
#endif
  h.reset(new Mag[nm]);

  old_m_on = mag :: bar(M, new_U);

#ifdef _OPENMP
#pragma omp barrier
#pragma omp for reduction (+ : mu, sigma2)
  for (long int i = 0L; i < nxi; ++i)
  {
    h[i] = mag :: bar(m[i], u[i]);
    mu  += h[i].value * xi[i];
    sigma2 += (1. - h[i].value * h[i].value) * (xi[i] * xi[i]);
  }
#else
  std :: transform(m, m + nm, u,
                   h.get(), [](const Mag &mi, const Mag &ui)
                   {
                    return mag :: bar(mi, ui);
                   });
  mu     = std :: inner_product(h.get(), h.get() + nm, xi, 0., std :: plus < double >(), [](auto &&hi, const double &xi_i){return hi.value * xi_i;});
  sigma2 = std :: inner_product(h.get(), h.get() + nxi, xi, 0., std :: plus < double >(), [](auto &&hi, const double &xi_i){return (1. - hi.value * hi.value) * (xi_i * xi_i);});
#endif

  dsigma2 = 2. * sigma2;
  new_u   = mag :: merf < Mag >( mu / std :: sqrt(dsigma2) );
  maxdiff = std :: abs( new_U - new_u );

#ifdef _OPENMP
#pragma omp single
#endif
  new_U   = mag :: damp(new_u, new_U, params.damping);
  new_m   = old_m_on % new_U;
  M       = new_m;

  g       = std :: exp(- (mu * mu) / dsigma2) / std :: sqrt(M_PI * dsigma2);

  p0      = 2. * old_m_on.value * g / (1. + old_m_on.value * new_U.value);
  pmu     = p0 * (p0 + mu / sigma2);
  psigma  = p0 * (1. - mu / sigma2 - mu * p0) / dsigma2;

  U = new_U;

#ifdef _OPENMP
#pragma omp for reduction (max : maxdiff)
#endif
  for (long int i = 0L; i < nm; ++i)
  {
    new_u   = mag :: convert < Mag >(mag :: clamp(xi[i] * (p0 + xi[i] * (h[i].value * pmu + xi[i] * (1. - h[i].value * h[i].value) * psigma)),
                                                  -1. + epsilon, 1. - epsilon));
    maxdiff = std :: max(maxdiff, std :: abs(new_u - u[i]) );
    u[i] = mag :: damp(new_u, u[i], params.damping);
    m[i] = h[i] % u[i];
  }

  return maxdiff;
}

template < class Mag >
double theta_node_update_accurate (MagVec < Mag > m, Mag & M, const double * xi, MagVec < Mag > u, Mag & U, const Params < Mag > & params, const long int & nxi, const long int & nm)
{
#ifdef DEBUG
  assert (nxi == nm);
#endif

  static double maxdiff,
                mu     ,
                sigma2 ;
#ifdef _OPENMP
#pragma omp barrier
#endif
  maxdiff = 0.;
  mu      = 0.;
  sigma2  = 0.;
  double tmp;
  Mag old_m_on(0.),
      new_u(0.);
  static Mag new_U;
  new_U = U;
  static std :: unique_ptr < Mag[] > h;
#ifdef _OPENMP
#pragma omp single
#endif
  h.reset(new Mag[nm]);
  old_m_on = mag :: bar(M, new_U);

#ifdef _OPENMP
#pragma omp barrier
#pragma omp for reduction (+ : mu, sigma2)
  for (long int i = 0L; i < nxi; ++i)
  {
    h[i]    = mag :: bar(m[i], u[i]);
    mu     += h[i].value * xi[i];
    sigma2 += (1. - h[i].value * h[i].value) * (xi[i] * xi[i]);
  }
#else
  std :: transform(m, m + nm, u,
                   h.get(), [](const Mag &mi, const Mag &ui)
                   {
                    return mag :: bar(mi, ui);
                   });
  mu     = std :: inner_product(h.get(), h.get() + nm, xi, 0., std :: plus < double >(), [](auto &&hi, const double &xi_i){return hi.value * xi_i;});
  sigma2 = std :: inner_product(h.get(), h.get() + nxi, xi, 0., std :: plus < double >(), [](auto &&hi, const double &xi_i){return (1. - hi.value * hi.value) * (xi_i * xi_i);});
#endif

  new_u = mag :: merf < Mag >( mu / std :: sqrt(2. * sigma2));
#ifdef _OPENMP
#pragma omp single
#endif
  new_U     = mag :: damp(new_u, new_U, params.damping);
  M         = old_m_on % new_U;
  U         = new_U;

#ifdef _OPENMP
#pragma omp for reduction (max : maxdiff)
#endif
  for (long int i = 0L; i < nm; ++i)
  {
    tmp     = std :: sqrt( 2. * (sigma2 - (1. - h[i].value * h[i].value) * xi[i] * xi[i]));
    new_u   = mag :: erfmix(old_m_on, ((mu - h[i].value * xi[i]) + xi[i]) / tmp , ((mu - h[i].value * xi[i]) - xi[i]) / tmp);
    maxdiff = std :: max(maxdiff, std :: abs(new_u - u[i]));
    u[i]    = mag :: damp(new_u, u[i], params.damping);
    m[i]    = h[i] % u[i];
  }

  return maxdiff;
}


template < class Mag >
double theta_node_update_exact (MagVec < Mag > m, Mag & M, const double * xi, MagVec < Mag > u, Mag & U, const Params < Mag > & params, const long int & nxi, const long int & nm)
{
#ifdef DEBUG
  assert (nxi == nm);
#endif
  static double maxdiff,
                pm,
                pp;
  static double **leftC  = nullptr,
                **rightC = nullptr;
  static Mag new_U;
  static std :: unique_ptr < Mag[] > h;
#ifdef _OPENMP
#pragma omp barrier
#endif

#ifdef _OPENMP
#pragma omp single
  {
#endif
    maxdiff = 0.;
    pm = 0.;
    pp = 0.;
    leftC  = new double *[nm + 1L];
    rightC = new double *[nm + 1L];
    new_U = U;
    h.reset(new Mag[nm]);
#ifdef _OPENMP
  }
#endif
    long int z;
    double pz, p;

    z = static_cast < long int >( (nm + 1L) * .5);
    Mag old_m_on(0.),
        new_u(0.),
        mp(0.), mm(0.);
    old_m_on = mag::bar(M, new_U);

    leftC[0] = new double[nm];
    std :: generate_n(leftC[0], nm, [](){return 1.;});
    rightC[nm] = new double[nm];
    std :: generate_n(rightC[nm], nm, [](){return 1.;});

#ifdef _OPENMP
#pragma omp barrier
#pragma omp for
#endif
    for (long int i = 0L; i < nm; ++i)
    {
      h[i]      = mag :: bar(m[i], u[i]);
      leftC[i + 1L]  = new double[i + 2L];
      rightC[i] = new double[nm - i + 1L];
    }

    leftC[1L][0] = (1. - h[0].value * xi[0]) * .5;
    leftC[1L][1] = (1. + h[0].value * xi[0]) * .5;

    for (long int i = 1L; i < nm; ++i)
    {
      leftC[i + 1L][0]      = leftC[i][0]  * (1. - h[i].value * xi[i]) * .5;
      for (long int j = 1L; j < i + 1L; ++j)
        leftC[i + 1L][j]    = leftC[i][j - 1L] * (1. + h[i].value * xi[i]) * .5 + leftC[i][j] * (1. - h[i].value * xi[i]) * .5;
      leftC[i + 1L][i + 1L] = leftC[i][i]  * (1. + h[i].value * xi[i]) * .5;
    }

    rightC[nm - 1L][0] = (1. - xi[nxi - 1L] * h[nm - 1L].value) * .5;
    rightC[nm - 1L][1] = (1. + xi[nxi - 1L] * h[nm - 1L].value) * .5;

    for (long int i = nm - 2L; i >= 0L; --i)
    {
      rightC[i][0]      = rightC[i + 1L][0]            * (1. - h[i].value * xi[i]) * .5;
      for (long int j = 1L; j < nm - i; ++j)
        rightC[i][j]    = rightC[i + 1L][j - 1]        * (1. + h[i].value * xi[i]) * .5 + rightC[i + 1L][j] * (1. - h[i].value * xi[i]) * .5;
      rightC[i][nm - i] = rightC[i + 1L][nm - i - 1L]  * (1. + h[i].value * xi[i]) * .5;
    }

#ifdef DEBUG
  assert (nm % 2L);
#endif

#ifdef _OPENMP
#pragma omp for reduction (+ : pm)
    for (long int i = 0L; i < z;       ++i) pm += rightC[0][i];
#pragma omp for reduction (+ : pp)
    for (long int i = z ; i < nm + 1L; ++i) pp += rightC[0][i];
#else
    pm = std :: accumulate( rightC[0], rightC[0] + z, 0. );
    pp = std :: accumulate( rightC[0] + z, rightC[0] + nm + 1L, 0. );
#endif

    new_u = mag :: couple < Mag >(pp, pm);

#ifdef DEBUG
  assert ( !mag :: isinf(new_u.mag) );
#endif

#ifdef _OPENMP
#pragma omp single
#endif
    new_U = mag :: damp(new_u, new_U, params.damping);
    M     = old_m_on % new_U;
#ifdef _OPENMP
#pragma omp single
#endif
    U     = new_U;

  #ifdef DEBUG
    assert ( !mag :: isinf(M.mag) );
  #endif

  double pp_,
         pm_;
#ifdef _OPENMP
#pragma omp for reduction (max : maxdiff)
#endif
    for (long int i = 0L; i < nm; ++i)
    {
      pm_ = 0.;
      pz = 0.;
      pp_ = 0.;
#ifdef DEBUG
    assert(xi[i] * xi[i] == 1.);
#endif

      for (long int j = 0L; j < nm; ++j)
      {
        p = 0.;
        for (long int k = static_cast < long int >(std :: max(0L, j + i - nm + 1L));
                      k < static_cast < long int >(std :: min(j, i) + 1L);
                      ++k)
          p += leftC[i][k] * rightC[i + 1L][j - k];
        if (j < z - 1L)       pm_ += p;
        else if (j == z - 1L) pz = p;
        else                  pp_ += p;
      }
      mp      = mag :: convert < Mag >(mag :: clamp(pp_ + xi[i] * pz - pm_, -1., 1.));
      mm      = mag :: convert < Mag >(mag :: clamp(pp_ - xi[i] * pz - pm_, -1., 1.));
      new_u   = mag :: exactmix(old_m_on, mp, mm);
      maxdiff = static_cast < double >(std :: max(maxdiff, std :: abs(new_u - u[i])));
      u[i]    = mag :: damp(new_u, u[i], params.damping);
      m[i]    = h[i] % u[i];
#ifdef DEBUG
    assert ( !mag :: isinf(u[i].mag) );
#endif
    }

  #ifdef _OPENMP
  #pragma omp single
    {
  #endif
      for (long int i = 0L; i < nm; ++i)
      {
        delete[] leftC[i];
        delete[] rightC[i];
      }
      delete[] leftC;
      delete[] rightC;
  #ifdef _OPENMP
    }
  #endif
    return maxdiff;
}



template < class Mag >
double free_energy_theta(const MagVec < Mag > m, const Mag & M, const double * xi, const MagVec < Mag > u, const Mag & U, const long int & nxi, const long int & nm)
{
#ifdef DEBUG
  assert (nm == nxi);
#endif
  std :: unique_ptr < Mag[] > h(new Mag[nm]);
  const auto old_m_on = mag :: bar(M, U);

// #ifdef _OPENMP
// #pragma omp barrier
// #pragma omp for reduction (+ : mu, sigma)
//   for (long int i = 0L; i < nxi; ++i)
//   {
//     h[i]   = mag :: bar(m[i], u[i]);
//     mu    += h[i] * xi[i];
//     sigma += (1. - h[i].value * h[i].value) * (xi[i] * xi[i]);
//   }
// #else

  std :: transform(m, m + nm, u,
                   h.get(), [](const Mag &mi, const Mag &ui)
                   {
                     return mag :: bar(mi, ui);
                   });
  const double mu    = std :: inner_product(h.get(), h.get() + nm,  xi, 0., std :: plus < double >(), [](auto &&hi, const double &xi_i){return hi.value * xi_i;});
  const double sigma = std :: inner_product(h.get(), h.get() + nxi, xi, 0., std :: plus < double >(), [](auto &&hi, const double &xi_i){return (1. - hi.value * hi.value) * (xi_i * xi_i);});
// #endif

  const auto b = mag :: merf < Mag >( mu / std :: sqrt( 2. * sigma ) );
  double f     = -mag :: log1pxy(old_m_on, b);

#ifdef DEBUG
  assert( !mag :: isinf(f) );
#endif

// #ifdef _OPENMP
// #pragma omp for reduction (+ : f)
//   for (long int i = 0L; i < nm; ++i) f += mag :: log1pxy(h[i], u[i]);
// #else
  f = std :: inner_product(h.get(), h.get() + nm, u, f, std :: plus < double >(), [](auto &&hi, const Mag &ui){return mag :: log1pxy(hi, ui);});
// #endif

  return f;
}

template < class Mag >
double free_energy_theta_exact(MagVec < Mag > m, const Mag & M, const double * xi, MagVec < Mag > u, const Mag & U, const long int & nm)
{
  long int z;
  double pm, pp, f;
  pm = 0.;
  pp = 0.;
  f  = 0.;
  double ** leftC = nullptr;
  leftC = new double*[nm];

  Mag old_m_on(0.),
      b(0.);

  std :: unique_ptr < Mag[] > h(new Mag[nm]);
  old_m_on = mag :: bar(M, U);

  z = static_cast < long int >( (nm + 1L) * .5);

// #ifdef _OPENMP
// #pragma omp barrier
// #pragma omp for
//   for (long int i = 0L; i < nm; ++i)
//   {
//     h[i] = mag :: bar(m[i], u[i]);
//     leftC[i] = new double[i + 2L];
//     // leftC[i] = std :: make_unique < double[] > (i + 2L);
//   }
// #else
  std :: transform(m, m + nm, u,
                   h.get(), [](const Mag &mi, const Mag &ui)
                   {
                    return mag :: bar(mi, ui);
                   });
  for (long int i = 0L; i < nm; ++i)
    leftC[i] = new double[i + 2L];
// #endif

  leftC[0][0] = (1. - h[0].value * xi[0]) * .5;
  leftC[0][1] = (1. + h[0].value * xi[0]) * .5;

  for (long int i = 1L; i < nm; ++i)
  {
    leftC[i][0] = leftC[i - 1L][0] * (1. - (h[i].value * xi[i])) * .5;
    for (long int j = 1L; j < i + 1L; ++j)
      leftC[i][j] = leftC[i - 1L][j - 1L] * (1. + (h[i].value * xi[i])) * .5 + leftC[i - 1L][j] * (1. - (h[i].value * xi[i])) * .5;
    leftC[i][i + 1L] = leftC[i - 1L][i] * (1. + (h[i].value * xi[i])) * .5;
  }
#ifdef DEBUG
  assert (nm % 2L);
#endif

// #ifdef _OPENMP
// #pragma omp for reduction (+ : pm)
//   for (long int i = 0; i < z;      ++i) pm += leftC[nm - 1L][i];
// #pragma omp for reduction (+ : pp)
//   for (long int i = z; i < nm + 1; ++i) pp += leftC[nm - 1L][i];
// #else
  pm = std :: accumulate(leftC[nm - 1L],     leftC[nm - 1L] + z,      0.);
  pp = std :: accumulate(leftC[nm - 1L] + z, leftC[nm - 1L] + nm + 1, 0.);
// #endif
  b  = mag :: couple < Mag >(pp, pm);
  f -= mag :: log1pxy(old_m_on, b);

#ifdef DEBUG
  assert ( !mag :: isinf(f) );
#endif

// #ifdef _OPENMP
// #pragma omp for reduction (+ : f)
//   for (long int i = 0; i < nm; ++i) f += mag :: log1pxy(h[i], u[i]);
// #else
  f = std :: inner_product(h.get(), h.get() + nm, u, f, std :: plus < double >(), [](auto &&h, const Mag &u){return mag :: log1pxy(h, u);});
// #endif

// #ifdef _OPENMP
// #pragma omp single
//   {
// #endif
    for (int i = 0; i < nm; ++i) delete[] leftC[i];
    delete[] leftC;
// #ifdef _OPENMP
//   }
// #endif

  return f;
}

template < class Mag >
double m_star_update(Mag & m_j_star, Mag & m_star_j, Params < Mag > & params) //old entro_node_update
{
#ifdef _OPENMP
#pragma omp barrier
#endif
  double diff;
  Mag old_m_j_star = mag :: bar(m_j_star, m_star_j),
      new_m_star_j(0.),
      new_m_j_star(0.);
  if (mag :: isinf(params.r))  // to check
      new_m_star_j = (old_m_j_star != 0.)                                  ?
                   (mag :: copysign(params.tan_gamma, old_m_j_star.value)) :
                   Mag(0.) ;
  else new_m_star_j = mag :: arrow( (old_m_j_star ^ params.tan_gamma), params.r ) ^ params.tan_gamma;

  diff         = std :: abs(new_m_star_j - m_star_j);
  new_m_star_j = mag :: damp(new_m_star_j, m_star_j, params.damping);
  new_m_j_star = old_m_j_star % new_m_star_j;
#ifdef _OPENMP
#pragma omp barrier
#endif
  m_j_star = new_m_j_star;
  m_star_j = new_m_star_j;

  return diff;
}


template < class Mag >
double iterate(Cavity_Message < Mag > & messages, const Patterns & patterns, Params < Mag > & params)
{
  const long int size = messages.M + messages.N * messages.K;
  long int i, j, k;
  double maxdiff = 0.;
  Mag z(0.);
  static std :: unique_ptr < long int[] > randperm;
  static std :: unique_ptr < double[] > ones;
#ifdef _OPENMP
#pragma omp single
  {
#endif
    randperm.reset(new long int[size]);
    ones.reset(new double[messages.K]);
#ifdef _OPENMP
  }
#endif

#ifdef _OPENMP
#pragma omp barrier
#pragma omp for
  for (long int i = 0L; i < messages.K; ++i) ones[i] = 1L;
#else
  std :: fill_n(ones.get(), messages.K, 1L);
#endif

#ifdef _OPENMP
#pragma omp for
  for (long int i = 0L; i < size; ++i) randperm[i] = i;
#else
  std :: iota(randperm.get(), randperm.get() + size, 0L);
#endif

#ifdef _OPENMP
#pragma omp single
  {
#endif
    std :: mt19937 eng;
    std :: shuffle(randperm.get(), randperm.get() + size, eng);
#ifdef _OPENMP
  }
#endif

  auto tnu1 = accuracy < Mag >[params.accuracy1];
  auto tnu2 = accuracy < Mag >[params.accuracy2];

  for (long int a = 0L; a < size; ++a)
  {
#ifdef _OPENMP
#pragma omp barrier
#endif
    if (randperm[a] < messages.M)
    {
      z = Mag(0.);
      for (k = 0L; k < messages.K; ++k)
        maxdiff = std :: max(maxdiff, tnu1(messages.m_j_star[k],       messages.m_in[randperm[a]][k], patterns.input[randperm[a]], messages.weights[randperm[a]][k], messages.m_no[randperm[a]][k], params, patterns.Ncol, messages.N));
      maxdiff   = std :: max(maxdiff, tnu2(messages.m_in[randperm[a]], messages.m_on[randperm[a]],    ones.get(),                  messages.m_ni[randperm[a]],       z,                             params, messages.K,    messages.K));
    }
    else
      if (!(params.r == 0. || params.tan_gamma.value == 0.) )
      {
        j = static_cast < long int >(randperm[a] - messages.M);
        k = static_cast < long int >(static_cast < double >(j) / messages.N);
        i = static_cast < long int >(j % messages.N);
        maxdiff = std::max(maxdiff, m_star_update(messages.m_j_star[k][i], messages.m_star_j[k][i], params));
      }
  }
  return maxdiff;
}



template < class Mag >
bool converge( Cavity_Message < Mag > & messages, const Patterns & patterns, Params < Mag > & params)
{
  bool ok = false;

#ifdef VERBOSE
  const auto start_time = what_time_is_it_now();
#endif

  for (long int it = 0L; it < params.max_iters; ++it)
  {
#ifdef _OPENMP
#pragma omp barrier
#endif
    const double diff = iterate(messages, patterns, params);

#ifdef VERBOSE
#ifdef _OPENMP
#pragma omp single
#endif
      std :: cout << "\r\x1B[K[it=" << it << " Delta=" << diff << " lambda=" << params.damping << "]";
#endif // verbose

    if (diff < params.epsil)
    {
      ok = true;

#ifdef VERBOSE
#ifdef _OPENMP
#pragma omp single
#endif
      std :: cout << std :: endl << "ok" << std :: endl;
#endif // verbose
      break;
    }
  }

#ifdef VERBOSE
#ifdef _OPENMP
#pragma omp single
#endif
  std :: cout << ( !ok ? "\nfailed\n" : "")
              << "elapsed time = "
              << duration(start_time)
              << " milliseconds"
              << std :: endl;
#endif
  return ok;
}


long int * nonbayes_test (long int ** const sign_m_j_star, const Patterns & patterns, const long int & K)
{
  long int* predicted_labels = new long int[patterns.Nrow];
  double s, s2, trsf0;

#ifdef _OPENMP
#pragma omp barrier
#pragma omp for
#endif
  for (long int i = 0L; i < patterns.Nrow; ++i)
  {
    s = std :: accumulate(sign_m_j_star, sign_m_j_star + K,
                          0., [&](const double &val, const auto wk)
                          {
                            trsf0 = static_cast < double >(std :: inner_product( wk, wk + patterns.Ncol, patterns.input[i], 0., std :: plus < double >(), [](const auto &wki, const auto &pi) { return wki * pi; }));
                            s2    = static_cast < double >(std :: inner_product( wk, wk + patterns.Ncol, patterns.input[i], 0., std :: plus < double >(), [](const auto &wki, const auto &pi) { return (1L - wki * wki) * (pi * pi); }));
                            return val + std :: erf( trsf0 / std :: sqrt(2. * s2) );
                          });
    predicted_labels[i] = static_cast < long int >(sign(s));
  }

  return predicted_labels;
}

template < class Mag >
long int error_test(Cavity_Message < Mag > & messages, const Patterns & patterns)
{
  long int ** bin_weights = messages.get_weights();
  auto lbls = nonbayes_test(bin_weights, patterns, messages.K);
  static long int t;
  t = 0.;
#ifdef _OPENMP
#pragma omp barrier
#pragma omp for reduction (+ : t)
  for (long int i = 0L; i < patterns.Nrow; ++i)
    t += static_cast < long int >( lbls[i] != patterns.output[i] );
#else
  t = std :: inner_product(lbls, lbls + patterns.Nrow, patterns.output, 0L, std :: plus < long int >(), [](const long int &lbli, const long int &oi){ return static_cast < long int >( lbli != oi );});
#endif

#ifdef _OPENMP
#pragma omp single
#endif
  delete[] lbls;

  return t;
}

template < class Mag >
double free_energy(const Cavity_Message < Mag > & messages, const Patterns & patterns, const Params < Mag > & params)
{
  static double f;
  f = 0.;
  Mag z(0.);

  static std :: unique_ptr < double[] > ones;
#ifdef _OPENMP
#pragma omp single
#endif
  ones.reset(new double[messages.K]);
  std :: unique_ptr < Mag[] > v(new Mag[messages.M]);

#ifdef _OPENMP
#pragma omp barrier
#pragma omp for
  for (long int i = 0L; i < messages.K; ++i) ones[i] = 1.;
#else
  std :: fill_n(ones.get(), messages.K, 1.);
#endif

#ifdef _OPENMP
#pragma omp for reduction (+ : f) collapse(2)
#endif
  for (long int i = 0L; i < messages.M; ++i)
    for (long int j = 0L; j < messages.K; ++j)
      f += free_energy_theta(messages.m_j_star[j], messages.m_in[i][j], patterns.input[i], messages.weights[i][j], messages.m_no[i][j], patterns.Ncol, messages.N);

#ifdef _OPENMP
#pragma omp for reduction (+ : f)
#endif
  for (long int i = 0L; i < messages.M; ++i)
    f += free_energy_theta_exact(messages.m_in[i], messages.m_on[i], ones.get(), messages.m_ni[i], z, messages.K);

#ifdef _OPENMP
#pragma omp for reduction (+ : f) collapse(2)
#endif
  for (long int i = 0L; i < messages.K; ++i)
    for (long int j = 0L; j < messages.N; ++j)
    {
      for (long int k = 0L; k < messages.M; ++k) v[k] = messages.weights[k][i][j];
      f -= mag :: logZ(messages.m_star_j[i][j], v.get(), messages.M);
      f -= log2_over_2;
      f += mag :: log1pxy(params.tan_gamma, -params.tan_gamma) * .5;
      auto old_m_j_star = mag :: bar(messages.m_j_star[i][j], messages.m_star_j[i][j]);
      f += mag :: log1pxy(old_m_j_star, messages.m_star_j[i][j]);

      f += mag :: mcrossentropy( mag :: arrow(old_m_j_star ^ params.tan_gamma, params.r + 1.),
                                              old_m_j_star ^ params.tan_gamma);
    }
  return f / (messages.N * messages.K);
}

#ifdef STATS

template < class Mag >
double compute_S (const Cavity_Message < Mag > & messages, const Params < Mag > & params)
{
  static double S;
  S = 0.;

#ifdef _OPENMP
#pragma omp barrier
#pragma omp for reduction (+ : S) collapse(2)
#endif
  for (long int i = 0L; i < messages.K; ++i)
    for (long int j = 0L; j < messages.N; ++j)
    {
      auto old_m_j_star = mag :: bar(messages.m_star_j[i][j], messages.m_j_star[i][j]);
      S += ( ( old_m_j_star ^ mag :: arrow( (old_m_j_star ^ params.tan_gamma), params.r ) ) % params.tan_gamma ).value;
    }

  return S / (messages.N * messages.K);
}

template < class Mag >
double compute_q_bar (const Cavity_Message < Mag > & messages, const Params < Mag > & params)
{
  static double q_bar;
  q_bar = 0.;

#ifdef _OPENMP
#pragma omp barrier
// #pragma omp declare reduction (sum_mags : Mag : omp_out = omp_out + omp_in ) initializer(omp_priv=0)
// #pragma omp for reduction (sum_mags : q_bar) collapse(2)
#pragma omp for reduction (+ : q_bar) collapse(2)
#endif
  for (long int i = 0L; i < messages.K; ++i)
    for (long int j = 0L; j < messages.N; ++j)
    {
      auto old_m_j_star = mag :: bar(messages.m_j_star[i][j], messages.m_star_j[i][j]);
      auto mx           = mag :: arrow( old_m_j_star ^ params.tan_gamma, params.r + 1.);
      q_bar            += (mx ^ mx).value;
    }
  return q_bar / (messages.N * messages.K);
}

template < class Mag >
double compute_q (const Cavity_Message < Mag > & messages, const long int & nm_j_star, const long int & nm_j_star_col)
{
  static double q;
  q = 0.;

#ifdef _OPENMP
#pragma omp barrier
#pragma omp for collapse(2) reduction (+ : q)
#endif
  for (long int i = 0L; i < nm_j_star; ++i)
    for (long int j = 0L; j < nm_j_star_col; ++j)
      q += messages.m_j_star[i][j].value * messages.m_j_star[i][j].value;

  return q / (messages.N * messages.K);
}

template < class Mag >
void mags_symmetry (const Cavity_Message < Mag > & messages, double * overlaps)
{
  static double* qs;
  qs = new double[messages.K];

#ifdef _OPENMP
#pragma omp barrier
#pragma omp for
  for (long int i = 0L; i < messages.K; ++i) qs[i] = 0.;
#else
  std :: fill_n(qs, messages.K, 0.);
#endif

#ifdef _OPENMP
#pragma omp for reduction(+: qs[:messages.K])
#endif
  for (long int it = 0L; it < messages.K * messages.N; ++it)
  {
    std::ldiv_t dv = std :: div(it, messages.N);
    qs[dv.quot] += messages.m_j_star[dv.quot][dv.rem].value * messages.m_j_star[dv.quot][dv.rem].value;
  }

#ifdef _OPENMP
#pragma omp for
#endif
  for (long int i = 0L; i < messages.K; ++i) qs[i] = std :: sqrt(qs[i]);

#ifdef _OPENMP
#pragma omp for
#endif
  for (long int i = 0L; i < messages.K; ++i) overlaps[ i * messages.K + i ] = 1.;

#ifdef _OPENMP
#pragma omp for
#endif
  for (long int k1 = 0L; k1 < messages.K; ++k1)
    for (long int k2 = k1 + 1L; k2 < messages.K; ++k2)
    {
      const double s = std :: inner_product(messages.m_j_star[k1], messages.m_j_star[k1] + messages.N,
                                            messages.m_j_star[k2],
                                            0., std :: plus < double >(),
                                            [](const Mag &a, const Mag &b)
                                            {
                                             return a.value * b.value;
                                            }) / (qs[k1] * qs[k2]);
      overlaps[k1*messages.K + k2] = s;
      overlaps[k2*messages.K + k1] = s;
   }
#ifdef _OPENMP
#pragma omp single
#endif
  delete[] qs;
  return; /* qs/N; */
}

#endif // STATS


#ifndef __clang__

#if __GNUC__ <= 6

template < class Mag, typename std :: enable_if < std :: is_same < Mag, MagP64 > :: value > :: type * >
void set_outfields(const Cavity_Message < Mag > & message, const long int * output, const double & beta)
{
  static double t;
  t = std :: tanh(beta * .5);

#ifdef _OPENMP
#pragma omp for
#endif
  for (long int i = 0L; i < message.M; ++i) message.m_on[i] = MagP64(output[i] * t);
}

template < class Mag, typename std :: enable_if < std :: is_same < Mag, MagT64 > :: value > :: type * >
void set_outfields(const Cavity_Message < Mag > & message, const long int * output, const double & beta)
{
  static double t;
  t = std :: tanh(beta * .5);

#ifdef _OPENMP
#pragma omp for
#endif
  for (long int i = 0L; i < message.M; ++i) message.m_on[i] = MagT64(std :: atanh(output[i] * t));
}


#else

template < class Mag >
void set_outfields(const Cavity_Message < Mag > & message, const long int * output, const double & beta)
{
  static double t;
  t = std :: tanh(beta * .5);

  if constexpr ( std :: is_same < Mag, MagP64 > :: value )
  {
#ifdef _OPENMP
#pragma omp for
#endif
    for (long int i = 0L; i < message.M; ++i) message.m_on[i] = MagP64(output[i] * t);
  }
  else
  {
#ifdef _OPENMP
#pragma omp for
#endif
    for (long int i = 0L; i < message.M; ++i) message.m_on[i] = MagT64(std :: atanh(output[i] * t));
  }
}

#endif // old gcc

#endif // __clang__

template < class Mag >
long int ** focusingBP(const long int & K, const Patterns & patterns, const long int & max_iters, const long int & max_steps, const long int & seed, const double & damping, const std :: string & accuracy1, const std :: string & accuracy2, const double & randfact, const FocusingProtocol & fprotocol, const double & epsil, int nth, std :: string outfile, std :: string outmessfiletmpl, std :: string initmess, const bool & bin_mess )
{
  __unused bool ok;
  long int it = 1;
  const  long int M  = patterns.Nrow,
                  N  = patterns.Ncol;
  long int        errs;
#ifdef STATS
  double S, q, betaF, Sint, q_bar;
  std :: unique_ptr < double[] > mags(new double[K * K]);
#endif
  std :: ofstream os;

  Cavity_Message < Mag > messages;
  Params < Mag > params(max_iters, damping, epsil, static_cast<double>(NAN), 0., 0., accuracy1, accuracy2);
  long int **weights;
#ifdef DEBUG
  if (N <= 0L)                                                                      error_Npositive(N);
  if (K <= 0L)                                                                      error_Kpositive(K);
  if (max_iters < 0L)                                                               error_maxiters(max_iters);
  if (max_steps < 0L)                                                               error_maxsteps(max_steps);
  if (damping < 0.  || damping >= 1.)                                               error_damping(damping);
  if (randfact < 0. || randfact >= 1.)                                              error_randfact(randfact);
  if (accuracy1 != "exact" && accuracy1 != "accurate" && accuracy1 != "none")       error_accuracy1(accuracy1);
  if (accuracy2 != "exact" && accuracy2 != "accurate" && accuracy2 != "none")       error_accuracy2(accuracy2);
  if (accuracy1 == "exact" && !(N % 2L))                                            error_Nexact(N);
  if (accuracy2 == "exact" && !(K % 2L))                                            error_Kexact(K);
#endif
  if ( std :: is_same < Mag, MagT64 > :: value && !file_exists( std :: string(PWD) + "atanherf_interp.max_16.step_0.0001.first_1.dat") )
    error_atanherf_file();

#ifdef _OPENMP
#pragma omp parallel num_threads(nth)
  { // start parallel section
#endif

#ifdef _OPENMP
#pragma omp single
  {
#endif

    messages        = (initmess.empty())                           ?
                      Cavity_Message<Mag>(M, N, K, randfact, seed) :
                      Cavity_Message<Mag>(initmess, bin_mess)      ;

    outfile         = outfile.empty()                                                                                                                           ?
                      "results_BPCR_N" + std :: to_string(N) + "_K" + std :: to_string(K) + "_M" + std :: to_string(M) + "_s" + std :: to_string(seed) + ".txt" :
                      outfile;

    outmessfiletmpl = outmessfiletmpl.empty()                                                                                                                          ?
                      "messages_BPCR_N" + std :: to_string(N) + "_K" + std :: to_string(K) + "_M" + std :: to_string(M) + "_gamma_s" + std :: to_string(seed) + ".txt" :
                      outmessfiletmpl;
#ifdef _OPENMP
  } // parallel section
#endif
#ifdef _OPENMP
#pragma omp single
#endif
  if (!outfile.empty())
  {
#ifdef VERBOSE
    std :: cout << "writing outfile '" << outfile << "'" << std :: endl;
#endif // verbose
    os.open(outfile);
#ifdef STATS
    os << "pol y beta S q q_bar betaF S_int E" << std :: endl;
#else
    os << "it pol y beta ok E" << std :: endl;
#endif
    os.close();
  }

  if ( !initmess.empty() )
  {
    errs = error_test(messages, patterns);
#ifdef VERBOSE
#ifdef _OPENMP
#pragma omp single
#endif
    std :: cout << "initial errors = " << errs << std :: endl;
#endif
  }

#if defined VERBOSE && defined STATS
  if (K > 1L)
  {
#ifdef _OPENMP
#pragma omp single
#endif
    std :: cout << "mags overlaps =" << std :: endl;
    mags_symmetry(messages, mags.get());
#ifdef _OPENMP
#pragma omp single
    {
#endif
      for (long int i = 0L; i < K; ++i)
      {
        for(long int j = 0L; j < K; ++j)
          std :: cout << std :: setprecision(6) << std :: fixed << mags[i * K + j] << " ";
        std :: cout << std :: endl;
      }
      std :: cout << std :: endl;
#ifdef _OPENMP
    }
#endif
  }
#endif // end verbose

  for (long int i = 0L; i < fprotocol.Nrep; ++i)
  {
#ifdef _OPENMP
#pragma omp barrier
#endif

#ifdef DEBUG
    if (!mag :: isinf(fprotocol.beta[i])) error_infinite(fprotocol.beta[i]);
#endif

#ifdef _OPENMP
#pragma omp sections
    {
      #pragma omp section
      params.tan_gamma = mag :: mtanh < Mag >(fprotocol.gamma[i]);
      #pragma omp section
      params.r         = static_cast < double >(fprotocol.n_rep[i] - 1L);
      #pragma omp section
      params.beta      = fprotocol.beta[i];
    }
#else
    params.tan_gamma = mag :: mtanh < Mag >(fprotocol.gamma[i]);
    params.r         = static_cast < double >(fprotocol.n_rep[i] - 1L);
    params.beta      = fprotocol.beta[i];
#endif
#ifdef _OPENMP
#pragma omp barrier
#endif
    set_outfields(messages, patterns.output, fprotocol.beta[i]);
#ifdef _OPENMP
#pragma omp barrier
#endif
    ok = converge(messages, patterns, params);

#if defined VERBOSE && defined STATS
    if (K > 1L)
    {
#ifdef _OPENMP
#pragma omp single
#endif
      std :: cout << "mags overlaps =" << std :: endl;
      mags_symmetry(messages, mags.get());
#ifdef _OPENMP
#pragma omp single
      {
#endif
        for (long int i = 0L; i < K; ++i)
        {
          for(long int j = 0L; j < K; ++j)
            std :: cout << std :: setprecision(6) << std :: fixed << mags[i * K + j] << " ";
          std :: cout << std :: endl;
        }
        std :: cout << std :: endl;
#ifdef _OPENMP
      }
#endif
    }
#endif // end verbose

    errs = error_test(messages, patterns);

#ifdef STATS
    S     = compute_S(messages, params);
    q     = compute_q(messages, K, N);
    q_bar = compute_q_bar(messages, params);
    betaF = free_energy(messages, patterns, params);
    Sint  = - betaF - fprotocol.gamma[i] * S;

#if defined VERBOSE && defined STATS
#ifdef _OPENMP
#pragma omp single
#endif
    std :: cout <<  "it="    << it
                << " pol="   << params.tan_gamma
                << " y="     << fprotocol.n_rep[i]
                << " beta="  << fprotocol.beta[i]
                << " (ok="   << ok
                << ") S="    << S
                << " betaF=" << betaF
                << " S_int=" << Sint
                << " q="     << q
                << " q_bar=" << q_bar
                << " E="     << errs
                << std :: endl;
#endif
    if (ok && !outfile.empty())
    {
#ifdef _OPENMP
#pragma omp single
      {
#endif
        os.open(outfile, std :: ios_base :: app);
#ifdef STATS
        os << params.tan_gamma   << " "
           << fprotocol.n_rep[i] << " "
           << fprotocol.beta[i]  << " "
           << S                  << " "
           << q                  << " "
           << q_bar              << " "
           << betaF              << " "
           << Sint               << " "
           << errs
           << std :: endl;
#else
        os << it                << " "
           << params.tan_gamma  << " "
           << fprotocol.nrep[i] << " "
           << fprotocol.beta[i] << " "
           << ok                << " "
           << errs
           << std :: endl;
#endif
      os.close();
#ifdef _OPENMP
    }
#endif
    }

#ifdef VERBOSE
#ifdef _OPENMP
#pragma omp single
#endif
    std :: cout <<  "it="   << it
                << " pol="  << params.tan_gamma
                << " y="    << fprotocol.n_rep[i]
                << " beta=" << fprotocol.beta[i]
                << " (ok="  << ok
                << ") E="   << errs
                << std :: endl;
#endif
#endif // STATS
    if (errs == 0.)
      break;
#ifdef _OPENMP
#pragma omp single
#endif
    ++it;
    if (it > max_steps)  break;
  }
  weights = messages.get_weights();
#ifdef _OPENMP
  } // parallel section
#endif
  if (!outmessfiletmpl.empty()) messages.save_messages(outmessfiletmpl, params);
  return weights;
}

#endif // RFBP_HPP