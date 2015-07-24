//////////////////////////////////////////////////////////////////////////////////////////////
/// \file Rotation.cpp
/// \brief Implementation file for a rotation matrix class.
/// \details Light weight rotation class, intended to be fast, and not to provide
///          unnecessary functionality.
///
/// \author Sean Anderson
//////////////////////////////////////////////////////////////////////////////////////////////

#include <lgmath/so3/Rotation.hpp>

#include <stdexcept>

#include <lgmath/so3/Operations.hpp>

namespace lgmath {
namespace so3 {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Default constructor
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation::Rotation() :
  C_ba_(Eigen::Matrix3d::Identity()) {
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Copy constructor
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation::Rotation(const Rotation& C) :
  C_ba_(C.C_ba_) {
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation::Rotation(const Eigen::Matrix3d& C, bool reproj) {

  // Reproject rotation matrix to ensure it is valid
  if (reproj) {
    C_ba_ = so3::vec2rot(so3::rot2vec(C));
  } else {
    C_ba_ = C;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor. The rotation will be C_ba = vec2rot(aaxis_ab)
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation::Rotation(const Eigen::Vector3d& aaxis_ab, unsigned int numTerms) {
  C_ba_ = lgmath::so3::vec2rot(aaxis_ab, numTerms);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor. The rotation will be C_ba = vec2rot(aaxis_ab), aaxis_ab must be 3x1
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation::Rotation(const Eigen::VectorXd& aaxis_ab) {

  // Throw logic error
  if (aaxis_ab.rows() != 3) {
    throw std::logic_error("Tried to initialize a rotation "
                           "from a VectorXd that was not dimension 3");
  }

  // Construct using exponential map
  C_ba_ = lgmath::so3::vec2rot(aaxis_ab);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Assignment operator. Note pass-by-value is intentional.
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation& Rotation::operator=(Rotation C) {
  // Swap (this)'s parameters with the temporary object passed by value
  // The temporary object is then destroyed at end of scope
  std::swap( this->C_ba_, C.C_ba_ );
  return (*this);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the underlying rotation matrix
//////////////////////////////////////////////////////////////////////////////////////////////
const Eigen::Matrix3d& Rotation::matrix() const {
  return this->C_ba_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get the corresponding Lie algebra (axis-angle) using the logarithmic map
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Vector3d Rotation::vec() const {
  return lgmath::so3::rot2vec(this->C_ba_);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get the inverse (transpose) matrix
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation Rotation::inverse() const {
  Rotation temp;
  temp.C_ba_ = C_ba_.transpose();
  return temp;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief In-place right-hand side multiply C_rhs
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation& Rotation::operator*=(const Rotation& C_rhs) {
  C_ba_ = C_ba_*C_rhs.C_ba_;
  return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Right-hand side multiply C_rhs
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation Rotation::operator*(const Rotation& C_rhs) const {
  Rotation temp(*this);
  temp *= C_rhs;
  return temp;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief In-place right-hand side multiply this matrix by the inverse of C_rhs
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation& Rotation::operator/=(const Rotation& C_rhs) {
  this->C_ba_ = this->C_ba_*C_rhs.C_ba_.transpose();
  return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Right-hand side multiply this matrix by the inverse of C_rhs
//////////////////////////////////////////////////////////////////////////////////////////////
Rotation Rotation::operator/(const Rotation& C_rhs) const {
  Rotation temp(*this);
  temp /= C_rhs;
  return temp;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Right-hand side multiply this matrix by the point vector p_a
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Vector3d Rotation::operator*(const Eigen::Vector3d& p_a) const {
  return this->C_ba_ * p_a;
}

} // so3
} // lgmath

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief print transformation
//////////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& out, const lgmath::so3::Rotation& T) {
  out << std::endl << T.matrix() << std::endl;
  return out;
}


