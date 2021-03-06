%struct_prefix%
{
public:
    // Contructors
    %struct_name% () = default;

    template <typename T,
              typename std::enable_if<
                  !std::is_same< %struct_name%, typename std::decay<T>::type >::value
                  >::type* = nullptr>
    %struct_name% ( T&& value ) :
        handle_ (
            new handle<typename std::decay<T>::type>(
                std::forward<T>( value )
            )
        )
    {}

    %struct_name% ( const %struct_name% & rhs )
        : handle_ ( rhs.handle_ != nullptr ? rhs.handle_->clone() : nullptr )
    {}

    %struct_name% ( %struct_name%&& rhs ) noexcept
        : handle_ ( std::move(rhs.handle_) )
    {}

    // Assignment
    template <typename T>
    %struct_name% & operator= (T&& value)
    {
        %struct_name% temp( std::forward<T>( value ) );
        std::swap(temp, *this);
        return *this;
    }

    %struct_name% & operator= (const %struct_name% & rhs)
    {
        %struct_name% temp(rhs);
        std::swap(temp, *this);
        return *this;
    }

    %struct_name% & operator= (%struct_name% && rhs) noexcept
    {
        handle_ = std::move(rhs.handle_);
        return *this;
    }

    %nonvirtual_members%

private:
    struct handle_base
    {
        virtual ~handle_base () {}
        virtual handle_base * clone () const = 0;

        %pure_virtual_members%
    };

    template <typename T>
    struct handle : handle_base
    {
        template <typename U,
                  typename std::enable_if< !std::is_same< T,
                                                         typename std::decay<U>::type
                                                         >::value
                                           >::type* = nullptr>
        explicit handle( U&& value )
            : value_( std::forward<U>(value) )
        {}

        template <typename U,
                  typename std::enable_if< std::is_same< T,
                                                         typename std::decay<U>::type
                                                         >::value
                                           >::type* = nullptr>
        explicit handle( U&& value )
            : value_( std::forward<U>(value) )
        {}

        virtual handle_base * clone () const
        { 
          return new handle(value_); 
        }

        %virtual_members%

        T value_;
    };

    template <typename T>
    struct handle<std::reference_wrapper<T>> :
        handle<T &>
    {
        handle (std::reference_wrapper<T> ref) :
            handle<T &> (ref.get())
        {}
    };

    std::unique_ptr<handle_base> handle_ = nullptr;
};
