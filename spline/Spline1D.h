/*
 * This file is part of the Cpp utils distribution (https://github.com/feodorp/Cpp
 * Copyright (C) 2022 Feodor Pisnitchenko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _Spline1D_h
#define _Spline1D_h

#include <Eigen/Dense>
#include <math.h>

namespace Spline {

    template<typename _T, int _Size> struct CriticalPointArray;
    template<typename _T, int _Size> std::ostream& operator<<(std::ostream&, const CriticalPointArray<_T,_Size>&);

    template<typename _T, int _Size>
    struct CriticalPointArray
    {
        public:
            typedef _T Scalar;
            enum { Size = _Size };
            typedef Eigen::Array<Scalar,2*Size,1> PointArray;

            CriticalPointArray() : _array(PointArray::Zero()), _length(Eigen::Index(0)) {}

            inline static const CriticalPointArray Zero() { return CriticalPointArray(); }
            inline void setZero() { (*this) = Zero(); }

            inline bool full() const { return _length < Size; }
            inline Eigen::Index size() const { return _length; }
            inline Eigen::Index& size() { return _length; }
            inline const Scalar x(Eigen::Index i) const { return _array(i); }
            inline const Scalar y(Eigen::Index i) const { return _array(i+Size); }
            inline decltype(auto) x() const { return _array.segment(0,_length); }
            inline decltype(auto) y() const { return _array.segment(Size,_length); }

            friend std::ostream& operator<< <_T,_Size>(std::ostream&, const CriticalPointArray<_T,_Size>&);

            inline void Save(const Scalar& x, const Scalar& y)
            {
                // return;
                if (_length < Size) {
                    if (_length == 0) {
                        _array(_length)      = x;
                        _array(_length+Size) = y;
                    } else if (_length == 1) {
                        if (_array(0+Size) < y) {
                            _array(1)      = _array(0);
                            _array(1+Size) = _array(0+Size);
                            _array(0)      = x;
                            _array(0+Size) = y;
                        } else {
                            _array(1)      = x;
                            _array(1+Size) = y;
                        }
                    } else {
                        if (_array(_length-1+Size) < y) {
                            auto min_elem = std::lower_bound(_array.data()+Size,_array.data()+Size+_length, y, std::greater<Scalar>());
                            std::copy_backward(min_elem,_array.data()+Size+_length, _array.data()+Size+_length+1);
                            std::copy_backward(min_elem-Size,_array.data()+_length, _array.data()+_length+1);
                            *(min_elem - Size) = x;
                            *min_elem          = y;
                        } else {
                            _array(_length)      = x;
                            _array(_length+Size) = y;
                        }
                    }
                    ++_length;
                } else if (_array(_length-2+Size) < y ) {
                    auto min_elem = std::lower_bound(_array.data()+Size,_array.data()+Size+_length-1, y, std::greater<Scalar>());
                    std::copy_backward(min_elem,_array.data()+Size+_length-1, _array.data()+Size+_length);
                    std::copy_backward(min_elem-Size,_array.data()+_length-1, _array.data()+_length);
                    *(min_elem - Size) = x;
                    *min_elem          = y;
                } else if (_array(_length-1+Size) < y ) {
                    _array(_length-1)      = x;
                    _array(_length-1+Size) = y;
                }
            }
        private:
            PointArray _array;
            Eigen::Index _length;
    };

    template <typename _Scalar, int _Size = Eigen::Dynamic>
    class Spline
    {
        public:
            enum { BreaksSize = _Size };
            enum { CoefsSize = BreaksSize == Eigen::Dynamic ? Eigen::Dynamic : BreaksSize - 1 };

            typedef _Scalar Scalar;
            typedef Eigen::Array<Scalar,BreaksSize,1> Points;

            template<int _N> using MaximaArray = CriticalPointArray<Scalar,_N>;

            // default constructor
            Spline() : _num_breaks(0), _breaks(), _coeffs() {}

            // explicit constructor
            template<typename ArrayTypeX, typename ArrayTypeY>
            Spline(const Eigen::ArrayBase<ArrayTypeX>& x, const Eigen::ArrayBase<ArrayTypeY>& y)
            : _num_breaks(), _breaks(), _coeffs()
            {
                _AssertSize(x,y);
                // cast function has no cost if cast to same type
                _SetSpline(x.template cast<Scalar>(),y.template cast<Scalar>());
            }

            // copy constructor
            Spline(const Spline& spline) : _num_breaks(spline._num_breaks), _breaks(spline._breaks), _coeffs(spline._coeffs) {}

            ~Spline() {}


            template<typename ArrayTypeX, typename ArrayTypeY>
            inline void set(const Eigen::ArrayBase<ArrayTypeX>& x, const Eigen::ArrayBase<ArrayTypeY>& y)
            {
                _AssertSize(x,y);
                // cast function has no cost if cast to same type
                _SetSpline(x.template cast<Scalar>(),y.template cast<Scalar>());
            }
            inline int num_breaks() const { return _num_breaks; }
            inline decltype(auto) breaks() const { return _breaks.head(_num_breaks); }
            inline decltype(auto) coefs() const { return _coeffs.topRows(_num_breaks-1); }
            inline Scalar operator()(Scalar x)
            {
                const Scalar* pos = std::upper_bound(_breaks.data(), _breaks.data()+_num_breaks, x);
                const Eigen::Index it = std::max(Eigen::Index(std::distance(_breaks.data(),pos))-Eigen::Index(1),Eigen::Index(0)) ;
                const Scalar h = x - _breaks(it);
                return ((_coeffs(it,0)*h+_coeffs(it,1))*h+_coeffs(it,2))*h+_coeffs(it,3);
            }

            inline const Scalar operator()(const Scalar x, const Eigen::Index& it) const
            {
                const Scalar h = x - _breaks(it);
                return ((_coeffs(it,0)*h+_coeffs(it,1))*h+_coeffs(it,2))*h+_coeffs(it,3);
            }

            template<int _N, typename = std::enable_if_t<(_N > 0)>>
            inline MaximaArray<_N> maxima() {
                MaximaArray<_N> _maxima;
                maxima(_maxima);
                return _maxima;
            }

            template<int _N, typename = std::enable_if_t<(_N > 0)>>
            inline void maxima(MaximaArray<_N>& _maxima) {
                assert((_num_breaks > 0) && "To obtain spline maxima it must be created first with interpolation points.");

                _maxima.setZero();
                Scalar x;
                for (Eigen::Index i = 0; i < _num_breaks - 1; ++i) {
                    if (_coeffs(i,0) != Scalar(0.0)) {
                        x  = _coeffs(i,1)*_coeffs(i,1)-Scalar(3.0)*_coeffs(i,0)*_coeffs(i,2);
                        if ( x > Scalar(0.0) ) {
                            if (_coeffs(i,1) >= Scalar(0.0)) {
                                if (_coeffs(i,0) < Scalar(0.0)) {
                                    x = _breaks(i) - (_coeffs(i,1)+std::sqrt(x))/(Scalar(3.0)*_coeffs(i,0));
                                    if (x < _breaks(i+1))
                                        // _SaveMaximum(_maxima,x,(*this)(x,i));
                                        _maxima.Save(x,(*this)(x,i));
                                }
                            } else {
                                x = _coeffs(i,1) + std::sqrt(x);
                                if ( (x >= Scalar(0.0) && _coeffs(i,0) < Scalar(0.0))
                                        || (x <= Scalar(0.0) && _coeffs(i,0) > Scalar(0.0))) {
                                    x = _breaks(i) - x/(Scalar(3.0)*_coeffs(i,0));
                                    if (x < _breaks(i+1))
                                        // _SaveMaximum(_maxima,x,(*this)(x,i));
                                        _maxima.Save(x,(*this)(x,i));
                                }
                            }
                        }
                    } else if (_coeffs(i,1) < Scalar(0.0) && _coeffs(i,2) >= Scalar(0.0)) {
                        x = _breaks(i) - Scalar(0.5)*_coeffs(i,2)/_coeffs(i,1);
                        if (x < _breaks(i+1))
                            // _SaveMaximum(_maxima,x,(*this)(x,i));
                            _maxima.Save(x,(*this)(x,i));
                    }
                }

                // check boundaries
                if ( _coeffs(0,2) < Scalar(0.0) ) {
                    _maxima.Save(_breaks(0),_coeffs(0,3));
                }

                x = _breaks(_num_breaks-1) - _breaks(_num_breaks-2);
                if ( (Scalar(3.0)*_coeffs(_num_breaks-2,0)*x
                        + Scalar(2.0)*_coeffs(_num_breaks-2,1))*x
                        + _coeffs(_num_breaks-2,2) > Scalar(0.0) ) {
                    _maxima.Save(_breaks(_num_breaks-1),(*this)(_breaks(_num_breaks-1),_num_breaks-2));
                }
            }

        private:
            typedef Eigen::Array<Scalar,BreaksSize,1,Eigen::ColMajor> BreaksVector;
            typedef Eigen::Array<Scalar,CoefsSize,4,Eigen::ColMajor> CoefsVector;

            Eigen::Index _num_breaks;
            BreaksVector _breaks;
            CoefsVector _coeffs;

            template<typename ArrayTypeX, typename ArrayTypeY>
            inline void _AssertSize(const Eigen::ArrayBase<ArrayTypeX>& x, const Eigen::ArrayBase<ArrayTypeY>& y)
            {
                if constexpr (!std::is_same_v<ArrayTypeX,ArrayTypeY>)
                    static_assert(int(ArrayTypeX::SizeAtCompileTime) == int(ArrayTypeY::SizeAtCompileTime),
                            "x and y-coordenates vectors of interpolation points must have same size.");
                if constexpr (ArrayTypeX::SizeAtCompileTime == Eigen::Dynamic) {
                    assert( (y.size() == x.size()) && "x and y-coordenates vectors of interpolation points must have same size.");
                    assert( (x.size() > 1) && " Number of interpolation points is less then 2.");
                } else {
                    static_assert(ArrayTypeX::SizeAtCompileTime > 1, " Number of interpolation points is less then 2.");
                }
                if constexpr (BreaksSize != Eigen::Dynamic) {
                    if constexpr (ArrayTypeX::SizeAtCompileTime != Eigen::Dynamic) {
                        static_assert(int(BreaksSize) >= int(ArrayTypeX::SizeAtCompileTime),
                                "Spline size must be Eigen::Dynamic or bigger than number of interpolation points.");
                    } else {
                        assert( (BreaksSize >= x.size()) && "Use Spline with Dynamic size. Ex.: Spline<float> Spl(x,y);");
                    }
                }
            }

            template<typename ArrayTypeX, typename ArrayTypeY>
            inline void _SetSpline(const Eigen::ArrayBase<ArrayTypeX>& x, const Eigen::ArrayBase<ArrayTypeY>& y)
            {
                _num_breaks = x.size();
                if constexpr (BreaksSize == Eigen::Dynamic) {
                    if (_breaks.size() < x.size()) {
                        _breaks.resize(_num_breaks);
                        _coeffs.resize(_num_breaks - 1,4);
                    }
                }

                const Eigen::Index n = _num_breaks;
                assert(((x.segment(1,n-1) > x.segment(0,n-1)).all()) && "Break points must be in assending order." );

                // check if x is sorted

                if ( n == 2) {
                    _breaks(0) = x(0);
                    _breaks(1) = x(1);
                    _coeffs(0,0) = (y(1)-y(0))/(x(1)-x(0));
                    _coeffs(0,1) = y(0);
                    return;
                }

                // auxiliar
                _breaks.head(n-1) = x.segment(1,n-1) - x.segment(0,n-1);                                   // Dx
                _coeffs.col(2).head(n-1)    = (y.segment(1,n-1) - y.segment(0,n-1)) / _breaks.head(n-1);   // Dy/Dx

                // RHS
                _coeffs.col(1)(0) = Scalar(0.0);
                _coeffs.col(1).segment(1,n-2) = Scalar(3.0) * (_coeffs.col(2).segment(1,n-2) - _coeffs.col(2).segment(0,n-2));

                // main diagonal (with boundary conditions)
                _coeffs.col(3).segment(1,n-4) = 2.0 * (x.segment(3,n-4) - x.segment(1,n-4));
                _coeffs.col(3)(0) = 2.0 * (x(2) - x(0));
                _coeffs.col(3)(n-3) = 2.0 * (x(n-1) - x(n-3));

                // sub-diagonal
                _coeffs.col(0).head(n-3) = _breaks.segment(1,n-3);

                // solve symmetric positive defined tridiagonal system
                Scalar d, l;
                l = _coeffs.col(0)(0);
                _coeffs.col(0)(0) /=  _coeffs.col(3)(0);
                _coeffs.col(1)(1) /=  _coeffs.col(3)(0);
                for (Eigen::Index i = 1; i < n-2; ++i ) {
                    d = Scalar(1.0) / (_coeffs.col(3)(i) - _coeffs.col(0)(i-1)*l);
                    _coeffs.col(1)(i+1) = (_coeffs.col(1)(i+1) - _coeffs.col(1)(i)*l)*d;
                    l = _coeffs.col(0)(i);
                    _coeffs.col(0)(i) = _coeffs.col(0)(i)*d;
                }
                for (Eigen::Index i = n-4; i >= 0; i-- ) {
                    _coeffs.col(1)(i+1) = _coeffs.col(1)(i+1) - _coeffs.col(0)(i)*_coeffs.col(1)(i+2);
                }

                // coefficients
                _coeffs.col(0).head(n-2) = (_coeffs.col(1).segment(1,n-2) - _coeffs.col(1).segment(0,n-2)) /
                                          (Scalar(3.0) * _breaks.head(n-2));
                _coeffs.col(0)(n-2) = - _coeffs.col(1)(n-2) /
                                        (Scalar(3.0) * _breaks(n-2));

                _coeffs.col(2).head(n-2) = _coeffs.col(2).head(n-2) -
                                        (Scalar(2.0)*_coeffs.col(1).segment(0,n-2) + _coeffs.col(1).segment(1,n-2)) *
                                        _breaks.head(n-2) / Scalar(3.0);
                _coeffs.col(2)(n-2) = _coeffs.col(2)(n-2) -
                                     Scalar(2.0)*_coeffs.col(1)(n-2) *
                                     _breaks(n-2) / Scalar(3.0);

                _coeffs.col(3).head(n-1) = y.head(n-1);
                _breaks.head(n) = x;
            }

    };

    // deduction guide
    template<typename ArrayType>
        Spline(const Eigen::ArrayBase<ArrayType>&, const Eigen::ArrayBase<ArrayType>&) ->
            Spline<typename ArrayType::Scalar, ArrayType::SizeAtCompileTime>;

    template<typename ArrayTypeX, typename ArrayTypeY>
        Spline(const Eigen::ArrayBase<ArrayTypeX>&, const Eigen::ArrayBase<ArrayTypeY>&) ->
            Spline<std::common_type_t<typename ArrayTypeX::Scalar,typename ArrayTypeY::Scalar>, (ArrayTypeX::SizeAtCompileTime == ArrayTypeY::SizeAtCompileTime ? ArrayTypeX::SizeAtCompileTime : Eigen::Dynamic)>;

    template<typename Scalar, int Size>
        Spline(const Spline<Scalar,Size>&) -> Spline<Scalar,Size>;

    template<typename _T, int _Size>
    std::ostream& operator<<(std::ostream& out, const CriticalPointArray<_T,_Size>& _array)
    {
        if (_Size)
            for (int i=0; i < _Size; ++i )
                out << _array._array(i) << " " << _array._array(i+_Size) << std::endl;
        return out;
    }

}
#endif
