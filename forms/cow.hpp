%struct_prefix%
{
public:
    // Contructors
    %struct_name% () = default;

    template <typename T,
              typename std::enable_if<
                  !std::is_same< %struct_name%, typename std::decay<T>::type >::value
                  >::type* = nullptr>
    %struct_name% (T&& value) :
        handle_ (
            std::make_shared< Handle<typename std::decay<T>::type> >(
                std::forward<T>(value)
            )
        )
    {}

    // Assignment
    template <typename T,
              typename std::enable_if<
                  !std::is_same< %struct_name%, typename std::decay<T>::type >::value
                  >::type* = nullptr>
    %struct_name%& operator= (T&& value)
    {
        %struct_name% temp( std::forward<T>(value) );
        std::swap(temp.handle_, handle_);
        return *this;
    }

    template <typename T>
    T* cast()
    {
        return dynamic_cast<T*>( handle_.get() );
    }

    template <typename T>
    const T* cast() const
    {
        return dynamic_cast<const T*>( handle_.get() );
    }

    %nonvirtual_members%

private:
    struct HandleBase
    {
        virtual ~HandleBase () {}
        virtual std::shared_ptr<HandleBase> clone () const = 0;

        %pure_virtual_members%
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

        virtual std::shared_ptr<HandleBase> clone () const
        {
            return std::make_shared<Handle>(value_);
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
