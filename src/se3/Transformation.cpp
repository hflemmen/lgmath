//////////////////////////////////////////////////////////////////////////////////////////////
/// \file Transformation.cpp
/// \brief Implementation file for a transformation matrix class.
/// \details Light weight transformation class, intended to be fast, and not to provide
///          unnecessary functionality.
///
/// \author Sean Anderson
//////////////////////////////////////////////////////////////////////////////////////////////

#include <lgmath/se3/Transformation.hpp>

#include <stdexcept>

#include <lgmath/so3/Operations.hpp>
#include <lgmath/se3/Operations.hpp>

namespace lgmath {
namespace se3 {

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Default constructor
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation::Transformation() :
  C_ba_(Eigen::Matrix3d::Identity()), r_ab_inb_(Eigen::Vector3d::Zero()) {
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Move constructor
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation::Transformation(Transformation&& T) :
  C_ba_(std::move(T.C_ba_)), r_ab_inb_(std::move(T.r_ab_inb_)){
  // TODO: Eigen doesn't support move construction, so right now this is mostly the same as a copy...
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation::Transformation(const Eigen::Matrix4d& T) :
    C_ba_(T.block<3,3>(0,0)), r_ab_inb_(T.block<3,1>(0,3)) {
  this->reproject(false); // Trigger a conditional reprojection, depending on determinant
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor. The transformation will be T_ba = [C_ba, -C_ba*r_ba_ina; 0 0 0 1]
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation::Transformation(const Eigen::Matrix3d& C_ba, const Eigen::Vector3d& r_ba_ina) {
  C_ba_ = C_ba;  
  this->reproject(false); // Trigger a conditional reprojection, depending on determinant
  r_ab_inb_ = (-1.0)*C_ba_*r_ba_ina;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor. The transformation will be T_ba = vec2tran(xi_ab)
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation::Transformation(const Eigen::Matrix<double,6,1>& xi_ab, unsigned int numTerms) {
  lgmath::se3::vec2tran(xi_ab, &C_ba_, &r_ab_inb_, numTerms);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor. The transformation will be T_ba = vec2tran(xi_ab), xi_ab must be 6x1
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation::Transformation(const Eigen::VectorXd& xi_ab) {

  // Throw logic error
  if (xi_ab.rows() != 6) {
    throw std::invalid_argument("Tried to initialize a transformation "
                                "from a VectorXd that was not dimension 6");
  }

  // Construct using exponential map
  lgmath::se3::vec2tran(xi_ab, &C_ba_, &r_ab_inb_, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Copy assignment operator. Default implementation.
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation& Transformation::operator=(const Transformation& T) = default;

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Move assignment operator. Manually implemented as Eigen doesn't support moving.
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation& Transformation::operator=(Transformation&& T) {
  // TODO: Eigen doesn't support move construction, so right now this is mostly the same as a copy...
  this->C_ba_ = std::move(T.C_ba_);
  this->r_ab_inb_ = std::move(T.r_ab_inb_);

  return (*this);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Gets basic matrix representation of the transformation
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Matrix4d Transformation::matrix() const {
  Eigen::Matrix4d T_ba = Eigen::Matrix4d::Identity();
  T_ba.topLeftCorner<3,3>() = C_ba_;
  T_ba.topRightCorner<3,1>() = r_ab_inb_;
  return T_ba;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the underlying rotation matrix
//////////////////////////////////////////////////////////////////////////////////////////////
const Eigen::Matrix3d& Transformation::C_ba() const {
  return C_ba_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the "forward" translation r_ba_ina = -C_ba.transpose()*r_ab_inb
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Vector3d Transformation::r_ba_ina() const {
  return (-1.0)*C_ba_.transpose()*r_ab_inb_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the underlying r_ab_inb vector.
//////////////////////////////////////////////////////////////////////////////////////////////
const Eigen::Vector3d& Transformation::r_ab_inb() const {
  return r_ab_inb_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get the corresponding Lie algebra using the logarithmic map
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Matrix<double,6,1> Transformation::vec() const {
  return lgmath::se3::tran2vec(this->C_ba_, this->r_ab_inb_);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get the inverse matrix
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation Transformation::inverse() const {
  Transformation temp;
  temp.C_ba_ = C_ba_.transpose();
  temp.reproject(false); // Trigger a conditional reprojection, depending on determinant
  temp.r_ab_inb_ = (-1.0)*temp.C_ba_*r_ab_inb_;
  return temp;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Get the 6x6 adjoint transformation matrix
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Matrix<double,6,6> Transformation::adjoint() const {
  return lgmath::se3::tranAd(C_ba_, r_ab_inb_);
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Reproject the transformation matrix back onto SE(3). Setting force to false
///        triggers a conditional reproject that only happens if the determinant is of the
///        rotation matrix is poor; this is more efficient than always performing it.
//////////////////////////////////////////////////////////////////////////////////////////////
void Transformation::reproject(bool force) {
  // Note that the translation parameter always belongs to SE(3), but the rotation
  // can incur numerical error that accumulates.
  if (force || fabs(1.0 - this->C_ba_.determinant()) > 1e-6) {
    C_ba_ = so3::vec2rot(so3::rot2vec(C_ba_));
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief In-place right-hand side multiply T_rhs
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation& Transformation::operator*=(const Transformation& T_rhs) {

  // Perform operation
  this->r_ab_inb_ += this->C_ba_ * T_rhs.r_ab_inb_;
  this->C_ba_ = this->C_ba_ * T_rhs.C_ba_;

  // Trigger a conditional reprojection, depending on determinant
  this->reproject(false);

  return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Right-hand side multiply T_rhs
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation Transformation::operator*(const Transformation& T_rhs) const {
  Transformation temp(*this);
  temp *= T_rhs;
  return temp;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief In-place right-hand side multiply this matrix by the inverse of T_rhs
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation& Transformation::operator/=(const Transformation& T_rhs) {

  // Perform operation
  this->C_ba_ = this->C_ba_ * T_rhs.C_ba_.transpose();
  this->r_ab_inb_ += (-1) * this->C_ba_ * T_rhs.r_ab_inb_;

  // Trigger a conditional reprojection, depending on determinant
  this->reproject(false);

  return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Right-hand side multiply this matrix by the inverse of T_rhs
//////////////////////////////////////////////////////////////////////////////////////////////
Transformation Transformation::operator/(const Transformation& T_rhs) const {
  Transformation temp(*this);
  temp /= T_rhs;
  return temp;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Right-hand side multiply this matrix by the homogeneous vector p_a
//////////////////////////////////////////////////////////////////////////////////////////////
Eigen::Vector4d Transformation::operator*(const Eigen::Vector4d& p_a) const {
  Eigen::Vector4d p_b;
  p_b.head<3>() = C_ba_ * p_a.head<3>() + r_ab_inb_ * p_a[3];
  p_b[3] = p_a[3];
  return p_b;
}

} // se3
} // lgmath

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief print transformation
//////////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& out, const lgmath::se3::Transformation& T) {
  out << std::endl << T.matrix() << std::endl;
  return out;
}


