/*******************************************************************\

Module: API to expression classes for bitvectors

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#ifndef CPROVER_UTIL_BITVECTOR_EXPR_H
#define CPROVER_UTIL_BITVECTOR_EXPR_H

/// \file util/bitvector_expr.h
/// API to expression classes for bitvectors

#include "std_expr.h"

/// \brief The byte swap expression
class bswap_exprt : public unary_exprt
{
public:
  bswap_exprt(exprt _op, std::size_t bits_per_byte, typet _type)
    : unary_exprt(ID_bswap, std::move(_op), std::move(_type))
  {
    set_bits_per_byte(bits_per_byte);
  }

  bswap_exprt(exprt _op, std::size_t bits_per_byte)
    : unary_exprt(ID_bswap, std::move(_op))
  {
    set_bits_per_byte(bits_per_byte);
  }

  std::size_t get_bits_per_byte() const
  {
    return get_size_t(ID_bits_per_byte);
  }

  void set_bits_per_byte(std::size_t bits_per_byte)
  {
    set(ID_bits_per_byte, narrow_cast<long long>(bits_per_byte));
  }
};

template <>
inline bool can_cast_expr<bswap_exprt>(const exprt &base)
{
  return base.id() == ID_bswap;
}

inline void validate_expr(const bswap_exprt &value)
{
  validate_operands(value, 1, "bswap must have one operand");
  DATA_INVARIANT(
    value.op().type() == value.type(), "bswap type must match operand type");
}

/// \brief Cast an exprt to a \ref bswap_exprt
///
/// \a expr must be known to be \ref bswap_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref bswap_exprt
inline const bswap_exprt &to_bswap_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_bswap);
  const bswap_exprt &ret = static_cast<const bswap_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \copydoc to_bswap_expr(const exprt &)
inline bswap_exprt &to_bswap_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_bswap);
  bswap_exprt &ret = static_cast<bswap_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \brief Bit-wise negation of bit-vectors
class bitnot_exprt : public unary_exprt
{
public:
  explicit bitnot_exprt(exprt op) : unary_exprt(ID_bitnot, std::move(op))
  {
  }
};

template <>
inline bool can_cast_expr<bitnot_exprt>(const exprt &base)
{
  return base.id() == ID_bitnot;
}

inline void validate_expr(const bitnot_exprt &value)
{
  validate_operands(value, 1, "Bit-wise not must have one operand");
}

/// \brief Cast an exprt to a \ref bitnot_exprt
///
/// \a expr must be known to be \ref bitnot_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref bitnot_exprt
inline const bitnot_exprt &to_bitnot_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_bitnot);
  const bitnot_exprt &ret = static_cast<const bitnot_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \copydoc to_bitnot_expr(const exprt &)
inline bitnot_exprt &to_bitnot_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_bitnot);
  bitnot_exprt &ret = static_cast<bitnot_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \brief Bit-wise OR
class bitor_exprt : public multi_ary_exprt
{
public:
  bitor_exprt(const exprt &_op0, exprt _op1)
    : multi_ary_exprt(_op0, ID_bitor, std::move(_op1), _op0.type())
  {
  }
};

template <>
inline bool can_cast_expr<bitor_exprt>(const exprt &base)
{
  return base.id() == ID_bitor;
}

/// \brief Cast an exprt to a \ref bitor_exprt
///
/// \a expr must be known to be \ref bitor_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref bitor_exprt
inline const bitor_exprt &to_bitor_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_bitor);
  return static_cast<const bitor_exprt &>(expr);
}

/// \copydoc to_bitor_expr(const exprt &)
inline bitor_exprt &to_bitor_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_bitor);
  return static_cast<bitor_exprt &>(expr);
}

/// \brief Bit-wise XOR
class bitxor_exprt : public multi_ary_exprt
{
public:
  bitxor_exprt(exprt _op0, exprt _op1)
    : multi_ary_exprt(std::move(_op0), ID_bitxor, std::move(_op1))
  {
  }
};

template <>
inline bool can_cast_expr<bitxor_exprt>(const exprt &base)
{
  return base.id() == ID_bitxor;
}

/// \brief Cast an exprt to a \ref bitxor_exprt
///
/// \a expr must be known to be \ref bitxor_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref bitxor_exprt
inline const bitxor_exprt &to_bitxor_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_bitxor);
  return static_cast<const bitxor_exprt &>(expr);
}

/// \copydoc to_bitxor_expr(const exprt &)
inline bitxor_exprt &to_bitxor_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_bitxor);
  return static_cast<bitxor_exprt &>(expr);
}

/// \brief Bit-wise AND
class bitand_exprt : public multi_ary_exprt
{
public:
  bitand_exprt(const exprt &_op0, exprt _op1)
    : multi_ary_exprt(_op0, ID_bitand, std::move(_op1), _op0.type())
  {
  }
};

template <>
inline bool can_cast_expr<bitand_exprt>(const exprt &base)
{
  return base.id() == ID_bitand;
}

/// \brief Cast an exprt to a \ref bitand_exprt
///
/// \a expr must be known to be \ref bitand_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref bitand_exprt
inline const bitand_exprt &to_bitand_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_bitand);
  return static_cast<const bitand_exprt &>(expr);
}

/// \copydoc to_bitand_expr(const exprt &)
inline bitand_exprt &to_bitand_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_bitand);
  return static_cast<bitand_exprt &>(expr);
}

/// \brief A base class for shift and rotate operators
class shift_exprt : public binary_exprt
{
public:
  shift_exprt(exprt _src, const irep_idt &_id, exprt _distance)
    : binary_exprt(std::move(_src), _id, std::move(_distance))
  {
  }

  shift_exprt(exprt _src, const irep_idt &_id, const std::size_t _distance);

  exprt &op()
  {
    return op0();
  }

  const exprt &op() const
  {
    return op0();
  }

  exprt &distance()
  {
    return op1();
  }

  const exprt &distance() const
  {
    return op1();
  }
};

template <>
inline bool can_cast_expr<shift_exprt>(const exprt &base)
{
  return base.id() == ID_shl || base.id() == ID_ashr || base.id() == ID_lshr ||
         base.id() == ID_ror || base.id() == ID_rol;
}

inline void validate_expr(const shift_exprt &value)
{
  validate_operands(value, 2, "Shifts must have two operands");
}

/// \brief Cast an exprt to a \ref shift_exprt
///
/// \a expr must be known to be \ref shift_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref shift_exprt
inline const shift_exprt &to_shift_expr(const exprt &expr)
{
  const shift_exprt &ret = static_cast<const shift_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \copydoc to_shift_expr(const exprt &)
inline shift_exprt &to_shift_expr(exprt &expr)
{
  shift_exprt &ret = static_cast<shift_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \brief Left shift
class shl_exprt : public shift_exprt
{
public:
  shl_exprt(exprt _src, exprt _distance)
    : shift_exprt(std::move(_src), ID_shl, std::move(_distance))
  {
  }

  shl_exprt(exprt _src, const std::size_t _distance)
    : shift_exprt(std::move(_src), ID_shl, _distance)
  {
  }
};

/// \brief Cast an exprt to a \ref shl_exprt
///
/// \a expr must be known to be \ref shl_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref shl_exprt
inline const shl_exprt &to_shl_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_shl);
  const shl_exprt &ret = static_cast<const shl_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \copydoc to_shl_expr(const exprt &)
inline shl_exprt &to_shl_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_shl);
  shl_exprt &ret = static_cast<shl_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \brief Arithmetic right shift
class ashr_exprt : public shift_exprt
{
public:
  ashr_exprt(exprt _src, exprt _distance)
    : shift_exprt(std::move(_src), ID_ashr, std::move(_distance))
  {
  }

  ashr_exprt(exprt _src, const std::size_t _distance)
    : shift_exprt(std::move(_src), ID_ashr, _distance)
  {
  }
};

/// \brief Logical right shift
class lshr_exprt : public shift_exprt
{
public:
  lshr_exprt(exprt _src, exprt _distance)
    : shift_exprt(std::move(_src), ID_lshr, std::move(_distance))
  {
  }

  lshr_exprt(exprt _src, const std::size_t _distance)
    : shift_exprt(std::move(_src), ID_lshr, std::move(_distance))
  {
  }
};

/// \brief Extracts a single bit of a bit-vector operand
class extractbit_exprt : public binary_predicate_exprt
{
public:
  /// Extract the \p _index-th least significant bit from \p _src.
  extractbit_exprt(exprt _src, exprt _index)
    : binary_predicate_exprt(std::move(_src), ID_extractbit, std::move(_index))
  {
  }

  extractbit_exprt(exprt _src, const std::size_t _index);

  exprt &src()
  {
    return op0();
  }

  exprt &index()
  {
    return op1();
  }

  const exprt &src() const
  {
    return op0();
  }

  const exprt &index() const
  {
    return op1();
  }
};

template <>
inline bool can_cast_expr<extractbit_exprt>(const exprt &base)
{
  return base.id() == ID_extractbit;
}

inline void validate_expr(const extractbit_exprt &value)
{
  validate_operands(value, 2, "Extract bit must have two operands");
}

/// \brief Cast an exprt to an \ref extractbit_exprt
///
/// \a expr must be known to be \ref extractbit_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref extractbit_exprt
inline const extractbit_exprt &to_extractbit_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_extractbit);
  const extractbit_exprt &ret = static_cast<const extractbit_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \copydoc to_extractbit_expr(const exprt &)
inline extractbit_exprt &to_extractbit_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_extractbit);
  extractbit_exprt &ret = static_cast<extractbit_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \brief Extracts a sub-range of a bit-vector operand
class extractbits_exprt : public expr_protectedt
{
public:
  /// Extract the bits [\p _lower .. \p _upper] from \p _src to produce a result
  /// of type \p _type. Note that this specifies a closed interval, i.e., both
  /// bits \p _lower and \p _upper are included. Indices count from the
  /// least-significant bit, and are not affected by endianness.
  /// The ordering upper-lower matches what SMT-LIB uses.
  extractbits_exprt(exprt _src, exprt _upper, exprt _lower, typet _type)
    : expr_protectedt(
        ID_extractbits,
        std::move(_type),
        {std::move(_src), std::move(_upper), std::move(_lower)})
  {
  }

  extractbits_exprt(
    exprt _src,
    const std::size_t _upper,
    const std::size_t _lower,
    typet _type);

  exprt &src()
  {
    return op0();
  }

  exprt &upper()
  {
    return op1();
  }

  exprt &lower()
  {
    return op2();
  }

  const exprt &src() const
  {
    return op0();
  }

  const exprt &upper() const
  {
    return op1();
  }

  const exprt &lower() const
  {
    return op2();
  }
};

template <>
inline bool can_cast_expr<extractbits_exprt>(const exprt &base)
{
  return base.id() == ID_extractbits;
}

inline void validate_expr(const extractbits_exprt &value)
{
  validate_operands(value, 3, "Extract bits must have three operands");
}

/// \brief Cast an exprt to an \ref extractbits_exprt
///
/// \a expr must be known to be \ref extractbits_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref extractbits_exprt
inline const extractbits_exprt &to_extractbits_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_extractbits);
  const extractbits_exprt &ret = static_cast<const extractbits_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \copydoc to_extractbits_expr(const exprt &)
inline extractbits_exprt &to_extractbits_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_extractbits);
  extractbits_exprt &ret = static_cast<extractbits_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \brief Bit-vector replication
class replication_exprt : public binary_exprt
{
public:
  replication_exprt(constant_exprt _times, exprt _src, typet _type)
    : binary_exprt(
        std::move(_times),
        ID_replication,
        std::move(_src),
        std::move(_type))
  {
  }

  constant_exprt &times()
  {
    return static_cast<constant_exprt &>(op0());
  }

  const constant_exprt &times() const
  {
    return static_cast<const constant_exprt &>(op0());
  }

  exprt &op()
  {
    return op1();
  }

  const exprt &op() const
  {
    return op1();
  }
};

template <>
inline bool can_cast_expr<replication_exprt>(const exprt &base)
{
  return base.id() == ID_replication;
}

inline void validate_expr(const replication_exprt &value)
{
  validate_operands(value, 2, "Bit-wise replication must have two operands");
}

/// \brief Cast an exprt to a \ref replication_exprt
///
/// \a expr must be known to be \ref replication_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref replication_exprt
inline const replication_exprt &to_replication_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_replication);
  const replication_exprt &ret = static_cast<const replication_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \copydoc to_replication_expr(const exprt &)
inline replication_exprt &to_replication_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_replication);
  replication_exprt &ret = static_cast<replication_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \brief Concatenation of bit-vector operands
///
/// This expression takes any number of operands
/// The ordering of the operands is the same as in the SMT-LIB 2 standard,
/// i.e., most-significant operands come first.
class concatenation_exprt : public multi_ary_exprt
{
public:
  concatenation_exprt(operandst _operands, typet _type)
    : multi_ary_exprt(ID_concatenation, std::move(_operands), std::move(_type))
  {
  }

  concatenation_exprt(exprt _op0, exprt _op1, typet _type)
    : multi_ary_exprt(
        ID_concatenation,
        {std::move(_op0), std::move(_op1)},
        std::move(_type))
  {
  }
};

template <>
inline bool can_cast_expr<concatenation_exprt>(const exprt &base)
{
  return base.id() == ID_concatenation;
}

/// \brief Cast an exprt to a \ref concatenation_exprt
///
/// \a expr must be known to be \ref concatenation_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref concatenation_exprt
inline const concatenation_exprt &to_concatenation_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_concatenation);
  return static_cast<const concatenation_exprt &>(expr);
}

/// \copydoc to_concatenation_expr(const exprt &)
inline concatenation_exprt &to_concatenation_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_concatenation);
  return static_cast<concatenation_exprt &>(expr);
}

/// \brief The popcount (counting the number of bits set to 1) expression
class popcount_exprt : public unary_exprt
{
public:
  popcount_exprt(exprt _op, typet _type)
    : unary_exprt(ID_popcount, std::move(_op), std::move(_type))
  {
  }

  explicit popcount_exprt(const exprt &_op)
    : unary_exprt(ID_popcount, _op, _op.type())
  {
  }
};

template <>
inline bool can_cast_expr<popcount_exprt>(const exprt &base)
{
  return base.id() == ID_popcount;
}

inline void validate_expr(const popcount_exprt &value)
{
  validate_operands(value, 1, "popcount must have one operand");
}

/// \brief Cast an exprt to a \ref popcount_exprt
///
/// \a expr must be known to be \ref popcount_exprt.
///
/// \param expr: Source expression
/// \return Object of type \ref popcount_exprt
inline const popcount_exprt &to_popcount_expr(const exprt &expr)
{
  PRECONDITION(expr.id() == ID_popcount);
  const popcount_exprt &ret = static_cast<const popcount_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

/// \copydoc to_popcount_expr(const exprt &)
inline popcount_exprt &to_popcount_expr(exprt &expr)
{
  PRECONDITION(expr.id() == ID_popcount);
  popcount_exprt &ret = static_cast<popcount_exprt &>(expr);
  validate_expr(ret);
  return ret;
}

#endif // CPROVER_UTIL_BITVECTOR_EXPR_H