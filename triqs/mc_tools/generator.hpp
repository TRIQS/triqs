//  (C) Copyright 2008-10 Lukas Gamper <gamperl -at- gmail.com>
//                        Brigitte Surer <surerb -at- phys.ethz.ch>
//                        Bela Bauer <bauerb -at- phys.ethz.ch>
//
//                        downgraded by O. Parcollet for compilation with icc11 & boost 1.45
//
//  Use, modification, and distribution are subject to the Boost Software 
//  License, Version 1.0. (See at <http://www.boost.org/LICENSE_1_0.txt>.)

#include <vector>
#include <algorithm>
#include <boost/function.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/exceptions.hpp>

namespace boost{
    template<typename R> class generator {
        public:
            static std::size_t const buffer_size = 1000;
            template<typename Generator> generator(Generator const & gen, std::size_t size = buffer_size)
                : 
                 buf(make_shared<buffer>(gen, size))
                , cur(buf->cur)
                , end(buf->end())
            {}
            inline R const operator()() {
                if (cur == end)
                    buf->fill();
                return *cur++;
            }
            generator(generator<R> const & rhs)
                : 
                 buf(new buffer (*rhs.buf))
                , cur(buf->cur)
                , end(buf->end())
            {} 
            generator<R> & operator=(generator<R> rhs) {
                swap(*this, rhs);
                return *this;
            }
        private:
            class buffer: public std::vector<R> {
                public:
                     friend class generator<R>;
                    template<typename Generator> buffer(Generator const & gen, std::size_t size)
                        : std::vector<R>(size)
                        , engine(make_shared<Generator>(gen))
                        , cur(this->end())
                        , fill_helper(lambda::bind(std::generate<typename buffer::iterator, Generator &>, lambda::_1, lambda::_2, ref(*reinterpret_cast<Generator *>(engine.get()))))
                        , clone(lambda::bind(buffer::template type_keeper<Generator>, lambda::_1, lambda::_2))
                    {}
                    template<typename Generator> buffer(reference_wrapper<Generator> gen, std::size_t size)
                        : std::vector<R>(size)
                        , cur(this->end())
                        , fill_helper(lambda::bind(std::generate<typename buffer::iterator, Generator &>, lambda::_1, lambda::_2, gen))
                    {} 
                    buffer(buffer const & rhs)
                        : std::vector<R>(rhs)
                        , cur(this->begin() + (rhs.cur - rhs.begin()))
                        , fill_helper(rhs.fill_helper)
                        , clone(rhs.clone)
                    {
                        if (rhs.engine)
                            clone(*this, rhs);
                    }
                    inline void fill() {
                        fill_helper(this->begin(), this->end());
                        cur = this->begin();
                    }
                private:
                    template<typename Generator> static void type_keeper(buffer & lhs, buffer const & rhs) {
                        lhs.engine = make_shared<Generator>(*reinterpret_cast<Generator *>(rhs.engine.get()));
                        lhs.fill_helper = lambda::bind(std::generate<typename buffer::iterator, Generator &>, lambda::_1, lambda::_2, ref(*reinterpret_cast<Generator *>(lhs.engine.get())));
                    }
                    shared_ptr<void> engine;
                    typename std::vector<R>::const_iterator cur;
                    function<void(typename std::vector<R>::iterator, typename std::vector<R>::iterator)> fill_helper;
                    function<void(buffer &, buffer const &)> clone;
            };

            shared_ptr<buffer> buf;
            typename buffer::const_iterator & cur;
            typename buffer::const_iterator end;
        };
}
