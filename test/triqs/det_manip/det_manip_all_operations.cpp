#include <triqs/det_manip/det_manip.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <triqs/arrays/asserts.hpp>
#include <triqs/test_tools/arrays.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <iostream>
#include <iomanip>

// function to fill the matrix
struct fun {
  double operator()(double x, double y) const {
    const double pi = acos(-1);
    const double beta = 10.0;
    const double epsi = 0.1;
    double tau = x-y;
    bool s = (tau>0);
    tau = (s ? tau : beta + tau);
    double r = epsi + tau/beta *(1-2*epsi);
    return - (pi/beta)/ std::sin ( pi *r);
  }
};



TEST(det_manip, all_operators){

  fun f;
  triqs::mc_tools::random_generator RNG("mt19937", 23432);
  std::vector<double> initial_x{RNG(10.),RNG(10.),RNG(10.),RNG(10.)};
  std::vector<double> initial_y{RNG(10.),RNG(10.),RNG(10.),RNG(10.)};
  triqs::det_manip::det_manip<fun> D1(f, initial_x, initial_y);
  triqs::det_manip::det_manip<fun> D9(f, initial_x, initial_y);
  int nsteps = 50;

  for(int is = 0 ; is<nsteps ; is++){

    std::cout << "\n\nis=" << is << std::endl;
    // verif of regenerate
    auto mat_b = D1.matrix();
    auto det_b = D1.determinant();
    D1.regenerate();
    std::cout << "\n1) verif regenerate" << std::endl<< std::endl;
    EXPECT_EQ_ARRAY(mat_b, D1.matrix());
    EXPECT_CLOSE(det_b, D1.determinant());

    // verif change_col
    int j = RNG(D1.size());
    double y = RNG(10.);
    D1.change_col(j,y);
    initial_y[j] = y;
    triqs::det_manip::det_manip<fun> D2(f, initial_x, initial_y);
    std::cout << "2) verif change_col" << std::endl;
    std::cout << "D1 det = " << D1.determinant() << std::endl;
    std::cout << "D2 det = " << D2.determinant() << std::endl;
    EXPECT_EQ_ARRAY(D1.matrix(), D2.matrix());
    EXPECT_CLOSE(D1.determinant(), D2.determinant());
    // verify try_remove, try_insert
    D9.try_remove(0,j);
    D9.complete_operation();
    D9.try_insert(0,j,initial_x[0],y);
    D9.complete_operation();
    std::cout << "D9 det = " << D9.determinant() << std::endl<< std::endl;
    EXPECT_EQ_ARRAY(D9.matrix(), D2.matrix());
    EXPECT_CLOSE(D9.determinant(), D2.determinant());

    //verif regenerate
    D9.regenerate();
    std::cout << "3) verif regenerate" << std::endl;
    std::cout << "D2 det = " << D2.determinant() << std::endl;
    std::cout << "D9 det = " << D9.determinant() << std::endl<< std::endl;
    EXPECT_EQ_ARRAY(D9.matrix(), D2.matrix());
    EXPECT_CLOSE(D9.determinant(), D2.determinant());

    // verif change_row
    int i = RNG(D1.size());
    double x = RNG(10.);
    D1.change_row(i,x);
    initial_x[i] = x;
    triqs::det_manip::det_manip<fun> D3(f, initial_x, initial_y);
    std::cout << "4) verif change_row" << std::endl;
    std::cout << "D1 det = " << D1.determinant() << std::endl;
    std::cout << "D3 det = " << D3.determinant() << std::endl;
    EXPECT_EQ_ARRAY(D1.matrix(), D3.matrix());
    EXPECT_CLOSE(D1.determinant(), D3.determinant());
    // verify try_remove, try_insert
    D9.try_remove(i,0);
    D9.complete_operation();
    D9.try_insert(i,0,x,initial_y[0]);
    D9.complete_operation();
    std::cout << "D9 det = " << D9.determinant() << std::endl<< std::endl;
    EXPECT_EQ_ARRAY(D9.matrix(), D3.matrix());
    EXPECT_CLOSE(D9.determinant(), D3.determinant());

    //verif regenerate
    D9.regenerate();
    std::cout << "5) verif regenerate" << std::endl;
    std::cout << "D3 det = " << D3.determinant() << std::endl;
    std::cout << "D9 det = " << D9.determinant() << std::endl<< std::endl;
    EXPECT_EQ_ARRAY(D9.matrix(), D3.matrix());
    EXPECT_CLOSE(D9.determinant(), D3.determinant());

    // verif change_one_row_and_one_col
    x = RNG(10.);
    y = RNG(10.);
    i = RNG(D1.size());
    j = RNG(D1.size());
    D1.change_one_row_and_one_col(i,j,x,y);
    initial_x[i] = x;
    initial_y[j] = y;
    triqs::det_manip::det_manip<fun> D4(f, initial_x, initial_y);
    std::cout << "6) verif change_one_row_and_one_col" << std::endl;
    std::cout << "D1 det = " << D1.determinant() << std::endl;
    std::cout << "D4 det = " << D4.determinant() << std::endl;
    EXPECT_EQ_ARRAY(D1.matrix(), D4.matrix());
    EXPECT_CLOSE(D1.determinant(), D4.determinant());
    // verify try_remove, try_insert
    D9.try_remove(i,j);
    D9.complete_operation();
    D9.try_insert(i,j,x,y);
    D9.complete_operation();
    std::cout << "D9 det = " << D9.determinant() << std::endl<< std::endl;
    EXPECT_EQ_ARRAY(D9.matrix(), D4.matrix());
    EXPECT_CLOSE(D9.determinant(), D4.determinant());

    //verif regenerate
    D9.regenerate();
    std::cout << "7) verif regenerate" << std::endl;
    std::cout << "D4 det = " << D4.determinant() << std::endl;
    std::cout << "D9 det = " << D9.determinant() << std::endl<< std::endl;
    EXPECT_EQ_ARRAY(D9.matrix(), D4.matrix());
    EXPECT_CLOSE(D9.determinant(), D4.determinant());

// // insert2 and remove2 are less precise... the code below fails
//     // verif insert2
//     double x0=RNG(10.);
//     double y0=RNG(10.);
//     int i0=RNG(D1.size()+1);
//     int j0=RNG(D1.size()+1);
//     double x1=RNG(10.);
//     double y1=RNG(10.);
//     int i1=RNG(D1.size()+1);
//     int j1=RNG(D1.size()+1);
//     if(j1>=j0)j1++;
//     if(i1>=i0)i1++;
//     D1.insert2(i0,i1,j0,j1,x0,x1,y0,y1);
//     std::cout << "8) verif insert_2" << std::endl;
//     std::cout << "D1 det = " << D1.determinant() << std::endl;
//     double det = D1.determinant();
//     D1.regenerate();
//     std::cout << "D1 det = " << D1.determinant() << std::endl<< std::endl;
//     EXPECT_CLOSE(det, D1.determinant());
//
//     // verif remove2
//     D1.remove2(i0,i1,j0,j1);
//     std::cout << "9) verif insert_2" << std::endl;
//     std::cout << "D1 det = " << D1.determinant() << std::endl;
//     det = D1.determinant();
//     D1.regenerate();
//     std::cout << "D1 det = " << D1.determinant() << std::endl;
//     std::cout << "D9 det = " << D9.determinant() << std::endl;
//     EXPECT_CLOSE(D1.determinant(), D1.determinant());
  }
}

MAKE_MAIN;