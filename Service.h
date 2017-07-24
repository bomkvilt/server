#ifndef SERVER_SERVICE_H
#define SERVER_SERVICE_H

#include <boost/enable_shared_from_this.hpp>


#define checkE(EXP)             if (!(EXP))
#define check(EXP)              if (!(EXP))         return
#define checkB(EXP, BODY, RET)  if (!(EXP)) { BODY; return RET; }

#define CASER(ARG) case ARG : return


#define MEM_FF(x)       boost::bind(x)
#define MEM_FF1(x,y)    boost::bind(x, y)
#define MEM_FF2(x,y,z)  boost::bind(x, y,z)
#define MEM_FF3(x,y,z,p)boost::bind(x, y,z,p)

#define MEM_FN(x)       MEM_FF1(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)    MEM_FF2(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)  MEM_FF3(&self_type::x, shared_from_this(),y,z)

#define MEM_FC(x)       MEM_FF1(&self_type::x, this)
#define MEM_FC1(x,y)    MEM_FF2(&self_type::x, this,y)
#define MEM_FC2(x,y,z)  MEM_FF3(&self_type::x, this,y,z)

#define PTR(CLASS)      std::shared_ptr<CLASS>
#define WPTR(CLASS)     std::weak_ptr  <CLASS>

#define DEFINE_SELF(CLASS) \
        typedef CLASS self_type
#define DEFINE_PRS(CLASS) \
        typedef PTR (CLASS)     ptr; \
        typedef WPTR(CLASS)     wptr



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
