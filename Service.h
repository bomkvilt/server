#ifndef SERVER_SERVICE_H
#define SERVER_SERVICE_H

#include <boost/enable_shared_from_this.hpp>



#define check(EXP)              if (!(EXP))         return
#define checkC(EXP, BODY, RET)  if (!(EXP)) { BODY; return RET; }

#define CASER(ARG) case ARG : return

#define MEM_FN(x)       boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)  boost::bind(&self_type::x, shared_from_this(),y,z)

#define MEM_FF(x)       boost::bind(x)
#define MEM_FF1(x,y)    boost::bind(x, y)
#define MEM_FF2(x,y,z)  boost::bind(x, y,z)

#define DEFINE_PRS(CLASS) \
        typedef std::shared_ptr<CLASS>     ptr; \
        typedef std::weak_ptr  <CLASS>     wptr



namespace srv {
    using namespace std;


    template<typename _Tp>
    class enable_weak_from_this:
            public enable_shared_from_this<_Tp>
    {
    public:
        weak_ptr<_Tp>
        weak_from_this()
        { return weak_ptr<_Tp>(this->shared_from_this()); }

        weak_ptr<const _Tp>
        weak_from_this() const
        { return weak_ptr<_Tp>(this->shared_from_this()); }
    };
}


#endif //SERVER_SERVICE_H
