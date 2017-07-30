#ifndef SERVER_SERVICE_H
#define SERVER_SERVICE_H

#include <boost/enable_shared_from_this.hpp>

/**************************************|  |**************************************/

#define check(EXP)      if(!(EXP))
#define checkR(EXP)     check(EXP)      return
#define checkB(EXP)     check(EXP)      break
#define checkC(EXP)     check(EXP)      continue

/**************************************| Switch-case |**************************************/

#define SWITCH(V)  switch(V)
#define CASE(ARG)  case ARG :
#define CASER(ARG) CASE(ARG) return
#define CASEB(ARG) CASE(ARG) break
#define DEFAULT    default :

//---------------------------

template <typename _Tp, typename _Tq, size_t _Np, size_t _Nq>
bool Comparator(_Tp (&l)[_Np], _Tq (&r)[_Nq]) {
    checkR(_Np == _Nq) false;
    for (size_t i = 0; i < _Np; ++i)
        checkR(l[i] == r[i]) false;
    return true;
};

template <typename _Tp, typename _Tq, size_t _Np>
bool Comparator(_Tp (&l)[_Np], _Tq r)
{ return l == r; };

template <typename _Tp, typename _Tq, size_t _Nq>
bool Comparator(_Tp l, _Tq (&r)[_Nq])
{ return l == r; };

template <typename _Tp, typename _Tq>
bool Comparator(_Tp l, _Tq r)
{ return l == r; };

#define SWITCHM(V)  switch (0) { \
                    default: auto& _VAL_ = V; bool _ALL_ = false;
#define CASEM(ARG)  } if (Comparator(_VAL_,ARG) || _ALL_) { _ALL_ = true;
#define CASERM(ARG) CASEM(ARG) return
#define CASEBM(ARG) CASEM(ARG) break
#define DEFAULTM    } {


/**************************************| Functions bind |**************************************/

#define MEM_FN(x)       boost::bind(x)
#define MEM_FN1(x,y)    boost::bind(x, y)
#define MEM_FN2(x,y,z)  boost::bind(x, y,z)
#define MEM_FN3(x,y,z,p)boost::bind(x, y,z,p)

#define MEM_FS(x)       MEM_FN1(&self_type::x, shared_from_this())
#define MEM_FS1(x,y)    MEM_FN2(&self_type::x, shared_from_this(),y)
#define MEM_FS2(x,y,z)  MEM_FN3(&self_type::x, shared_from_this(),y,z)

#define MEM_FT(x)       MEM_FN1(&self_type::x, this)
#define MEM_FT1(x,y)    MEM_FN2(&self_type::x, this,y)
#define MEM_FT2(x,y,z)  MEM_FN3(&self_type::x, this,y,z)

/**************************************| smart ptrs |**************************************/

#define PTR(CLASS)      std::shared_ptr<CLASS>
#define WPTR(CLASS)     std::weak_ptr  <CLASS>

#define DEFINE_SELF(CLASS) \
        typedef CLASS self_type
#define DEFINE_PRS(CLASS) \
        typedef PTR (CLASS)     ptr; \
        typedef WPTR(CLASS)     wptr

/**************************************|  |**************************************/

namespace srv {

    template<typename _Tp>
    class enable_weak_from_this:
            public std::enable_shared_from_this<_Tp>
    {
    public:
        std::weak_ptr<_Tp>
        weak_from_this()
        { return std::weak_ptr<_Tp>(this->shared_from_this()); }

        std::weak_ptr<const _Tp>
        weak_from_this() const
        { return std::weak_ptr<_Tp>(this->shared_from_this()); }
    };
}


#endif //SERVER_SERVICE_H
