%struct_prefix%
{
public:
    // Contructors
    %struct_name% () = default;

    template <typename T,
              typename std::enable_if<
                  !std::is_same< %struct_name%, typename std::decay<T>::type >::value
                  >::type* = nullptr>
    %struct_name% ( T&& value ) noexcept ( std::is_rvalue_reference<T>::value &&
                                           std::is_nothrow_move_constructible<typename std::decay<T>::type>::value ) :
        handle_ (
            new Handle<typename std::decay<T>::type>(
                std::forward<T>( value )
            )
        )
    {}

    %struct_name% ( const %struct_name% & rhs )
        : handle_ ( rhs.handle_ ? rhs.handle_->clone() : nullptr )
    {}

    %struct_name% ( %struct_name%&& rhs ) noexcept
        : handle_ ( std::move(rhs.handle_) )
    {}

    // Assignment
    template <typename T,
              typename std::enable_if<
                  !std::is_same< %struct_name%, typename std::decay<T>::type >::value
                  >::type* = nullptr>
    %struct_name%& operator= (T&& value) noexcept ( std::is_rvalue_reference<T>::value &&
                                                    std::is_nothrow_move_constructible<typename std::decay<T>::type>::value )
    {
        %struct_name% temp( std::forward<T>( value ) );
        std::swap(temp, *this);
        return *this;
    }

    %struct_name%& operator= (const %struct_name%& rhs)
    {
        %struct_name% temp(rhs);
        std::swap(temp, *this);
        return *this;
    }

    %struct_name%& operator= (%struct_name%&& rhs) noexcept
    {
        handle_ = std::move(rhs.handle_);
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

    %nonvirtual_members%

private:
    struct HandleBase
    {
        virtual ~HandleBase () {}
        virtual HandleBase * clone () const = 0;

        %pure_virtual_members%
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

        virtual HandleBase* clone () const
        { 
          return new Handle(value_);
        }

        %virtual_members%

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
