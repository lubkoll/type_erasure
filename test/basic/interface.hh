#ifndef BASIC_FOOABLE_HH
#define BASIC_FOOABLE_HH

#include <cassert>
#include <memory>
#include <utility>


namespace Basic {
    
    class Fooable
    {
    public:
        // Contructors
        Fooable () = default;
    
        template <typename T,
                  typename std::enable_if<
                      !std::is_same< Fooable, typename std::decay<T>::type >::value
                      >::type* = nullptr>
        Fooable ( T&& value ) :
            handle_ (
                new Handle<typename std::decay<T>::type>(
                    std::forward<T>( value )
                )
            )
        {}
    
        Fooable ( const Fooable & rhs )
            : handle_ ( rhs.handle_ ? rhs.handle_->clone() : nullptr )
        {}
    
        Fooable ( Fooable&& rhs ) noexcept
            : handle_ ( std::move(rhs.handle_) )
        {}
    
        // Assignment
        template <typename T,
                  typename std::enable_if<
                      !std::is_same< Fooable, typename std::decay<T>::type >::value
                      >::type* = nullptr>
        Fooable& operator= (T&& value)
        {
            Fooable temp( std::forward<T>( value ) );
            std::swap(temp, *this);
            return *this;
        }
    
        Fooable& operator= (const Fooable& rhs)
        {
            Fooable temp(rhs);
            std::swap(temp, *this);
            return *this;
        }
    
        Fooable& operator= (Fooable&& rhs) noexcept
        {
            handle_ = std::move(rhs.handle_);
            return *this;
        }
    
        int foo ( ) const
        {
                assert(handle_); return handle_->foo( );
        }
        void set_value ( int value )
        {
                assert(handle_); handle_->set_value(value );
        }
    
    private:
        struct HandleBase
        {
            virtual ~HandleBase () {}
            virtual HandleBase * clone () const = 0;
    
            virtual int foo ( ) const = 0;
            virtual void set_value ( int value ) = 0;
        };
    
        template <typename T>
        struct Handle : HandleBase
        {
            template <typename U,
                      typename std::enable_if<
                          !std::is_same< T, typename std::decay<U>::type >::value
                                               >::type* = nullptr>
            explicit Handle( U&& value )
                : value_( value )
            {}
    
            template <typename U,
                      typename std::enable_if<
                          std::is_same< T, typename std::decay<U>::type >::value
                                               >::type* = nullptr>
            explicit Handle( U&& value )
                : value_( std::forward<U>(value) )
            {}
    
            virtual HandleBase* clone () const
            { 
              return new Handle(value_);
            }
    
            virtual int foo ( ) const {
                return value_.foo( );
            }
            virtual void set_value ( int value ) {
                value_.set_value(value );
            }
    
            T value_;
        };
    
        template <typename T>
        struct Handle< std::reference_wrapper<T> > : Handle<T&>
        {
            Handle (std::reference_wrapper<T> ref)
                : Handle<T&> (ref.get())
            {}
        };
    
        std::unique_ptr<HandleBase> handle_;
    };

}
#endif

