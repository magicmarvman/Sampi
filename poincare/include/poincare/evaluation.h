#ifndef POINCARE_EVALUATION_H
#define POINCARE_EVALUATION_H

#include <complex.h>
extern "C" {
#include <stdint.h>
}
#include <poincare/preferences.h>
#include <poincare/tree_by_reference.h>
#include <poincare/tree_node.h>

namespace Poincare {

class Expression;
template<typename T>
class Evaluation;

template<typename T>
class EvaluationNode : public TreeNode {
public:
  enum class Type : uint8_t {
    Exception,
    Complex,
    MatrixComplex
  };
  EvaluationNode<T> * childAtIndex(int index) const override { return static_cast<EvaluationNode<T> *>(TreeNode::childAtIndex(index)); }
  virtual Type type() const = 0;
  virtual ~EvaluationNode() = default;
  virtual bool isUndefined() const = 0;
  virtual T toScalar() const { return NAN; }
  virtual Expression complexToExpression(Preferences::ComplexFormat complexFormat) const = 0;
  virtual std::complex<T> trace() const = 0;
  virtual std::complex<T> determinant() const = 0;
};

template<typename T>
class Evaluation : public TreeByReference {
public:
  Evaluation() : TreeByReference() {}
  template<class U> explicit operator U() const {
    // See Expression::operator T() for explanations on this operator
    // TODO add assertions to ensure that we cast only to evaluation subclasses
    // Does not work :static_assert(sizeof(U) == sizeof(Evaluation<T>), "Size mismatch");
    return *reinterpret_cast<U *>(const_cast<Evaluation<T> *>(this));
  }
  EvaluationNode<T> * node() const {
    assert(!TreeByReference::node()->isGhost());
    return static_cast<EvaluationNode<T> *>(TreeByReference::node());
  }

  /* Hierarchy */
  Evaluation<T> childAtIndex(int i) const;
  typename Poincare::EvaluationNode<T>::Type type() const { return node()->type(); }
  bool isUndefined() const { return node()->isUndefined(); }
  T toScalar() const { return node()->toScalar(); }
  Expression complexToExpression(Preferences::ComplexFormat complexFormat) const;
  std::complex<T> trace() const { return node()->trace(); }
  std::complex<T> determinant() const { return node()->determinant(); }
protected:
  Evaluation(EvaluationNode<T> * n) : TreeByReference(n) {}
};

}

#endif
