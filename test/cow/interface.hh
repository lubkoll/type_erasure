#ifndef COW_FOOABLE_HH
#define COW_FOOABLE_HH

#include <cassert>
#include <memory>
#include <utility>
#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>


#if defined(_MSC_VER) && _MSC_VER == 1800
#define noexcept
#endif


namespace COW {
    
    class Fooable
    {
    public:
        // Contructors
        Fooable () = default;
    
        template <typename T,
                  typename std::enable_if<
                      !std::is_same< Fooable, typename std::decay<T>::type >::value
                      >::type* = nullptr>
        Fooable (T&& value) noexcept ( std::is_rvalue_reference<T>::value &&
                                             std::is_nothrow_move_constructible<typename std::decay<T>::type>::value ) :
            handle_ (
                std::make_shared< Handle<typename std::decay<T>::type> >(
                    std::forward<T>(value)
                )
            )
        {}
    
        // Assignment
        template <typename T,
                  typename std::enable_if<
                      !std::is_same< Fooable, typename std::decay<T>::type >::value
                      >::type* = nullptr>
        Fooable& operator= (T&& value) noexcept ( std::is_rvalue_reference<T>::value &&
                                                        std::is_nothrow_move_constructible<typename std::decay<T>::type>::value )
        {
            Fooable temp( std::forward<T>(value) );
            std::swap(temp.handle_, handle_);
            return *this;
        }
    
        template <typename T>
        T* cast()
        {
            assert(handle_);
            Handle<T>* handle = dynamic_cast<Handle<T>*>( handle_.get() );
            if(handle)
                return &handle->value_;
            return nullptr;
        }
    
        template <typename T>
        const T* cast() const
        {
            assert(handle_);
            const Handle<T>* handle = dynamic_cast<const Handle<T>*>( handle_.get() );
            if(handle)
                return &handle->value_;
            return nullptr;
        }
    
        int foo ( ) const
        {
                assert(handle_);
                return read().foo( );
        }
        void set_value ( int value )
        {
                assert(handle_);
                write().set_value(value );
        }
    
    private:
        struct HandleBase
        {
            virtual ~HandleBase () {}
            virtual std::shared_ptr<HandleBase> clone () const = 0;
    
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
            explicit Handle( U&& value ) noexcept
                : value_( value )
            {}
    
            template <typename U,
                      typename std::enable_if<
                          std::is_same< T, typename std::decay<U>::type >::value
                                               >::type* = nullptr>
            explicit Handle( U&& value ) noexcept ( std::is_rvalue_reference<U>::value &&
                                                    std::is_nothrow_move_constructible<typename std::decay<U>::type>::value )
                : value_( std::forward<U>(value) )
            {}
    
            virtual std::shared_ptr<HandleBase> clone () const
            {
                return std::make_shared<Handle>(value_);
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
    
        const HandleBase& read () const
        {
            return *handle_;
        }
    
        HandleBase& write ()
        {
            if (!handle_.unique())
                handle_ = handle_->clone();
            return *handle_;
        }
    
        std::shared_ptr<HandleBase> handle_;
    };

}
#endif

