%struct_prefix%
{
    public:
    // Contructors
    %struct_name% () = default;

    template <typename T,
              typename std::enable_if<
                  !std::is_same< %struct_name%, typename std::decay<T>::type >::value
                  >::type* = nullptr>
    %struct_name% (T&& value)
    {
        handle_ = clone_impl( std::forward<T>(value), buffer_ );
    }

    %struct_name% (const %struct_name%& rhs)
    {
        if (rhs.handle_)
            handle_ = rhs.handle_->clone_into(buffer_);
    }

    %struct_name% (%struct_name%&& rhs) noexcept
    {
        swap(rhs.handle_, rhs.buffer_);
    }

    // Assignment
    template <typename T,
              typename std::enable_if<
                  !std::is_same< %struct_name%, typename std::decay<T>::type >::value
                  >::type* = nullptr>
    %struct_name%& operator= (T&& value)
    {
        reset();
        handle_ = clone_impl(std::forward<T>(value), buffer_);
        return *this;
    }

    %struct_name%& operator= (const %struct_name%& rhs)
    {
        %struct_name% temp(rhs);
        swap(temp.handle_, temp.buffer_);
        return *this;
    }

    %struct_name%& operator= (%struct_name%&& rhs) noexcept
    {
        %struct_name% temp(std::move(rhs));
        swap(temp.handle_, temp.buffer_);
        return *this;
    }

    ~%struct_name% ()
    {
        reset();
    }

    %nonvirtual_members%

    private:
        using buffer = std::array<unsigned char, 24>;

    struct HandleBase
    {
        virtual ~HandleBase () {}
        virtual HandleBase * clone_into (buffer & buf) const = 0;
        virtual bool heap_allocated () const = 0;
        virtual void destroy () = 0;

        %pure_virtual_members%
    };

    template <typename T, bool HeapAllocated>
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

        virtual HandleBase* clone_into (buffer& buf) const
        {
            return clone_impl(value_, buf);
        }

        virtual bool heap_allocated () const
        {
            return HeapAllocated;
        }

        virtual void destroy ()
        {
            if (HeapAllocated)
                delete this;
            else
                this->~Handle();
        }

        %virtual_members%

                T value_;
    };

    template <typename T, bool HeapAllocated>
    struct Handle<std::reference_wrapper<T>, HeapAllocated> : Handle<T&, HeapAllocated>
    {
        Handle (std::reference_wrapper<T> ref) :
            Handle<T&, HeapAllocated> (ref.get())
        {}
    };

    template <typename T>
    static HandleBase* clone_impl (T&& value, buffer& buf)
    {
        using PlainType = typename std::decay<T>::type;
        using BufferHandle = Handle<PlainType, false>;

        void* buf_ptr = &buf;
        std::size_t buf_size = sizeof(buf);
        buf_ptr = std::align( alignof(BufferHandle),
                              sizeof(BufferHandle),
                              buf_ptr, buf_size);
        if (buf_ptr) {
            new (buf_ptr) BufferHandle( std::forward<T>(value) );
            return static_cast<HandleBase*>(buf_ptr);
        } else {
            return new Handle<PlainType, true>( std::forward<T>(value) );
        }
    }

    void swap (HandleBase*& rhs_handle, buffer& rhs_buffer)
    {
        const bool this_heap_allocated =
                !handle_ || handle_->heap_allocated();
        const bool rhs_heap_allocated =
                !rhs_handle || rhs_handle->heap_allocated();

        if (this_heap_allocated && rhs_heap_allocated) {
            std::swap(handle_, rhs_handle);
        } else if (this_heap_allocated) {
            const std::ptrdiff_t offset = handle_offset(rhs_handle, rhs_buffer);
            rhs_handle = handle_;
            buffer_ = rhs_buffer;
            handle_ = handle_ptr(char_ptr(&buffer_) + offset);
        } else if (rhs_heap_allocated) {
            const std::ptrdiff_t offset = handle_offset(handle_, buffer_);
            handle_ = rhs_handle;
            rhs_buffer = buffer_;
            rhs_handle = handle_ptr(char_ptr(&rhs_buffer) + offset);
        } else {
            const std::ptrdiff_t this_offset =
                    handle_offset(handle_, buffer_);
            const std::ptrdiff_t rhs_offset =
                    handle_offset(rhs_handle, rhs_buffer);
            std::swap(buffer_, rhs_buffer);
            handle_ = handle_ptr(char_ptr(&buffer_) + this_offset);
            rhs_handle = handle_ptr(char_ptr(&rhs_buffer) + rhs_offset);
        }
    }

    void reset ()
    {
        if (handle_)
            handle_->destroy();
    }

    template <typename T>
    static unsigned char* char_ptr (T* ptr)
    {
        return static_cast<unsigned char*>(static_cast<void*>(ptr));
    }

    static HandleBase* handle_ptr (unsigned char* ptr)
    {
        return static_cast<HandleBase*>(static_cast<void*>(ptr));
    }

    static std::ptrdiff_t handle_offset (HandleBase* handle, buffer& buf)
    {
        assert(handle);
        unsigned char* const char_handle = char_ptr(handle);
        unsigned char* const char_buffer = char_ptr(&buf);
        return char_handle - char_buffer;
    }

    HandleBase* handle_ = nullptr;
    buffer buffer_;
};
