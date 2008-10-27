/*======================================================================

This file is part of the elastix software.

Copyright (c) University Medical Center Utrecht. All rights reserved.
See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notices for more information.

======================================================================*/

/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTransform.h,v $
  Language:  C++
  Date:      $Date: 2008-06-29 12:58:58 $
  Version:   $Revision: 1.64 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAdvancedTransform_h
#define __itkAdvancedTransform_h

#include "itkTransform.h"
#include "itkMatrix.h"

namespace itk
{
  
/** \class AdvancedTransform
 * \brief Transform maps points, vectors and covariant vectors from an input
 * space to an output space.
 *
 * This abstract class define the generic interface for a geometrical 
 * transformation from one space to another. The class provides methods
 * for mapping points, vectors and covariant vectors from the input space 
 * to the output space. 
 *
 * Given that transformation are not necessarily invertible, this basic
 * class does not provide the methods for back transformation. Back transform
 * methods are implemented in derived classes where appropriate.
 * 
 * \par Registration Framework Support
 * Typically a Transform class has several methods for setting its 
 * parameters. For use in the registration framework, the parameters must
 * also be represented by an array of doubles to allow communication
 * with generic optimizers. The Array of transformation parameters is set using
 * the SetParameters() method.
 *
 * Another requirement of the registration framework is the computation
 * of the Jacobian of the transform T. In general, an ImageToImageMetric
 * requires the knowledge of this Jacobian in order to compute the metric
 * derivatives. The Jacobian is a matrix whose element are the partial
 * derivatives of the transformation with respect to the array of parameters
 * mu that defines the transform, evaluated at a point p: dT/dmu(p).
 * 
 * If penalty terms are included in the registration, the transforms also
 * need to implement other derivatives of T. Often, penalty terms are functions
 * of the spatial derivatives of T. Therefore, e.g. the SpatialJacobian dT/dx
 * and the SpatialHessian d^2T/dx_idx_j require implementation. The
 * GetValueAndDerivative() requires the d/dmu of those terms. Therefore,
 * we additionally define GetJacobianOfSpatialJacobian() and
 * GetJacobianOfSpatialHessian().
 *
 * \ingroup Transforms
 *
 */
template <class TScalarType,
  unsigned int NInputDimensions = 3, 
  unsigned int NOutputDimensions = 3>
class ITK_EXPORT AdvancedTransform
  : public Transform< TScalarType, NInputDimensions, NOutputDimensions >
{
public:
  /** Standard class typedefs. */
  typedef AdvancedTransform           Self;
  typedef Transform                   Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;
  
  /** New method for creating an object using a factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( AdvancedTransform, Transform );

  /** Dimension of the domain space. */
  itkStaticConstMacro( InputSpaceDimension, unsigned int, NInputDimensions );
  itkStaticConstMacro( OutputSpaceDimension, unsigned int, NOutputDimensions );

  /** Typedefs from the Superclass. */
  typedef typename Superclass::ScalarType           ScalarType;
  typedef typename Superclass::ParametersType       ParametersType;
  typedef typename Superclass::JacobianType         JacobianType;
  typedef typename Superclass::InputVectorType      InputVectorType;
  typedef typename Superclass::OutputVectorType     OutputVectorType;
  typedef typename Superclass
    ::InputCovariantVectorType                      InputCovariantVectorType;
  typedef typename Superclass
    ::OutputCovariantVectorType                     OutputCovariantVectorType;
  typedef typename Superclass::InputVnlVectorType   InputVnlVectorType;
  typedef typename Superclass::OutputVnlVectorType  OutputVnlVectorType;
  typedef typename Superclass::InputPointType       InputPointType;
  typedef typename Superclass::OutputPointType      OutputPointType;

  /** Types for the (Spatial)Jacobian/Hessian. */
  typedef std::vector< unsigned long >              NonZeroJacobianIndicesType;
  typedef Matrix< ScalarType,
    InputSpaceDimension, OutputSpaceDimension >     SpatialJacobianType;
  typedef std::vector< SpatialJacobianType >        JacobianOfSpatialJacobianType;
  typedef std::vector<
    Matrix< ScalarType,
    InputSpaceDimension, OutputSpaceDimension > >   SpatialHessianType; // think about this type, should be a 3D native type
  typedef std::vector< SpatialHessianType >         JacobianOfSpatialHessianType;
  typedef typename SpatialJacobianType::InternalMatrixType  InternalMatrixType;

  /** This returns a sparse version of the Jacobian of the transformation.
   *
   * The Jacobian is expressed as a vector of partial derivatives of the
	 * transformation components with respect to the parameters \$\bm{\mu}\$ that
   * define the transformation \$\bm{T}\$, evaluated at a point \$\bm{p}\$.
   *
   * \f[
      J=\left[ \begin{array}{cccc}
      \frac{\partial T_{1}}{\partial \mu_{1}}(\bm{p}) & 
      \frac{\partial T_{1}}{\partial \mu_{2}}(\bm{p}) & 
      \cdots &
      \frac{\partial T_{1}}{\partial \mu_{m}}(\bm{p}) \\
      \frac{\partial T_{2}}{\partial \mu_{1}}(\bm{p}) & 
      \frac{\partial T_{2}}{\partial \mu_{2}}(\bm{p}) & 
      \cdots &
      \frac{\partial T_{2}}{\partial \mu_{m}}(\bm{p}) \\
      \vdots & \vdots & \ddots & \vdots \\
      \frac{\partial T_{d}}{\partial \mu_{1}}(\bm{p}) & 
      \frac{\partial T_{d}}{\partial \mu_{2}}(\bm{p}) & 
      \cdots &
      \frac{\partial T_{d}}{\partial \mu_{m}}(\bm{p})
      \end{array}\right],
   * \f]
   * with \$m\$ the number of parameters, i.e. the size of \$\mu\$, and \$d\$
   * the dimension of the image.
   */
  virtual void GetJacobian(
    const InputPointType & ipp,
    JacobianType & j,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const;

  /** The GetJacobian from the superclass.
   * It is needed to override this method, otherwise the VS2008 compiler gets
   * confused. It complained: "Function does not take 1 arguments".
   */
  virtual const JacobianType & GetJacobian( const InputPointType & ) const;

	/** Compute the spatial Jacobian of the transformation.
   *
   * The spatial Jacobian is expressed as a vector of partial derivatives of the
   * transformation components with respect to the spatial position \$\bm{x}\$,
   * evaluated at a point \$\bm{p}\$.
   *
   * \f[
      sJ=\left[ \begin{array}{cccc}
      \frac{\partial T_{1}}{\partial x_{1}}(\bm{p}) & 
      \frac{\partial T_{1}}{\partial x_{2}}(\bm{p}) & 
      \cdots &
      \frac{\partial T_{1}}{\partial x_{m}}(\bm{p}) \\
      \frac{\partial T_{2}}{\partial x_{1}}(\bm{p}) & 
      \frac{\partial T_{2}}{\partial x_{2}}(\bm{p}) & 
      \cdots &
      \frac{\partial T_{2}}{\partial x_{m}}(\bm{p}) \\
      \vdots & \vdots & \ddots & \vdots \\
      \frac{\partial T_{d}}{\partial x_{1}}(\bm{p}) & 
      \frac{\partial T_{d}}{\partial x_{2}}(\bm{p}) & 
      \cdots &
      \frac{\partial T_{d}}{\partial x_{m}}(\bm{p})
      \end{array}\right],
   * \f]
   * with \$m\$ the number of parameters, i.e. the size of \$\mu\$, and \$d\$
   * the dimension of the image.
   */
  virtual void GetSpatialJacobian(
    const InputPointType & ipp,
    SpatialJacobianType & sj ) const;
  
  /** Compute the spatial Hessian of the transformation.
   *
   * The spatial Hessian is the vector of matrices of partial second order
   * derivatives of the transformation components with respect to the spatial
   * position \$\bm{x}\$, evaluated at a point \$\bm{p}\$.
   *
   * \f[
      sH=\left[ \begin{array}{cc}
      \frac{\partial^2 T_{i}}{\partial x_{1} \partial x_{1}}(\bm{p}) & 
      \frac{\partial^2 T_{i}}{\partial x_{1} \partial x_{2}}(\bm{p}) \\
      \frac{\partial^2 T_{i}}{\partial x_{1} \partial x_{2}}(\bm{p}) &
      \frac{\partial^2 T_{i}}{\partial x_{2} \partial x_{2}}(\bm{p}) \\
      \end{array}\right],
   * \f]
   * with i the i-th component of the transformation.
   */
  virtual void GetSpatialHessian(
    const InputPointType & ipp,
    SpatialHessianType & sh ) const;

  /** Compute the Jacobian of the spatial Jacobian of the transformation.
   *
   * The Jacobian of the spatial Jacobian is the derivative of the spatial
   * Jacobian to the transformation parameters \$\bm{\mu}\$, evaluated at
   * a point \$\bm{p}\$.
   */
  virtual void GetJacobianOfSpatialJacobian(
    const InputPointType & ipp,
    JacobianOfSpatialJacobianType & jsj,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const;

  /** Compute both the spatial Jacobian and the Jacobian of the
   * spatial Jacobian of the transformation.
   */
  virtual void GetJacobianOfSpatialJacobian(
    const InputPointType & ipp,
    SpatialJacobianType & sj,
    JacobianOfSpatialJacobianType & jsj,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const;

  /** Compute the Jacobian of the spatial Hessian of the transformation.
   *
   * The Jacobian of the spatial Hessian is the derivative of the spatial
   * Hessian to the transformation parameters \$\bm{\mu}\$, evaluated at
   * a point \$\bm{p}\$.
   */
  virtual void GetJacobianOfSpatialHessian(
    const InputPointType & ipp,
    JacobianOfSpatialHessianType & jsh,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const;

  /** Compute both the spatial Hessian and the Jacobian of the
   * spatial Hessian of the transformation.
   */
  virtual void GetJacobianOfSpatialHessian(
    const InputPointType & ipp,
    SpatialHessianType & sh,
    JacobianOfSpatialHessianType & jsh,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices ) const;

protected:
  AdvancedTransform();
  AdvancedTransform( unsigned int Dimension, unsigned int NumberOfParameters );
  virtual ~AdvancedTransform() {};

private:

  AdvancedTransform(const Self&); // purposely not implemented
  void operator=(const Self&);    // purposely not implemented

}; // end class AdvancedTransform

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_AdvancedTransform(_, EXPORT, x, y) namespace itk { \
  _(3(class EXPORT AdvancedTransform< ITK_TEMPLATE_3 x >)) \
  namespace Templates { typedef AdvancedTransform< ITK_TEMPLATE_3 x > AdvancedTransform##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkAdvancedTransform+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkAdvancedTransform.txx"
#endif

#endif
