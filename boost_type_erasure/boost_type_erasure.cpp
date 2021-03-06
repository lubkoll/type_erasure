#include <printable_types.hpp>
#include <copy_on_write.hpp>

#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/member.hpp>

#include <iostream>


namespace bte = boost::type_erasure;
namespace mpl = boost::mpl;

BOOST_TYPE_ERASURE_MEMBER((has_print), print, 1)

using any_printable = bte::any<
    mpl::vector<
        bte::constructible<bte::_self()>,
        bte::copy_constructible<bte::_self>,
        bte::assignable<bte::_self>,
        bte::destructible<bte::_self>,
        has_print<void ()>
    >
>;

using any_printable_param = bte::any<
    has_print<void ()>,
    bte::_self &
>;

void print_printable (any_printable_param printable)
{ printable.print(); }


#define BOOST_TEST_MODULE TypeErasure

#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(hand_rolled)
{
#if INSTRUMENT_COPIES
    reset_allocations();
#endif

    std::cout << "sizeof(any_printable) = " << sizeof(any_printable) << "\n";

#define ECHO(expr)                                                      \
    do {                                                                \
        std::cout << #expr << ";\nap.print() = ";                       \
        expr;                                                           \
        ap.print();                                                     \
        std::cout << "allocations: " << allocations() << "\n\n";        \
        reset_allocations();                                            \
    } while (false)

    ECHO(hi_printable hi; any_printable ap(hi));
    ECHO(large_printable large; any_printable ap(large));
    ECHO(bye_printable bye; any_printable ap(bye));

    ECHO(hi_printable hi; any_printable ap = hi);
    ECHO(large_printable large; any_printable ap = large);
    ECHO(bye_printable bye; any_printable ap = bye);

    ECHO(hi_printable hi; any_printable tmp = hi; any_printable ap = tmp);
    ECHO(large_printable large; any_printable tmp = large; any_printable ap = tmp);
    ECHO(bye_printable bye; any_printable tmp = bye; any_printable ap = tmp);

#define UNSUPPORTED(expr)                                               \
    do {                                                                \
        std::cout << "NOT SUPPORTED: " << #expr << ";\n\n";             \
    } while (false)

    // Does not compile!
    UNSUPPORTED(hi_printable hi; any_printable ap; ap = hi);
    UNSUPPORTED(large_printable large; any_printable ap; ap = large);
    UNSUPPORTED(bye_printable bye; any_printable ap; ap = bye);

    ECHO(const hi_printable hi{}; any_printable ap(hi));
    ECHO(const large_printable large{}; any_printable ap(large));
    ECHO(const bye_printable bye{}; any_printable ap(bye));

    ECHO(const hi_printable hi{}; any_printable ap = hi);
    ECHO(const large_printable large{}; any_printable ap = large);
    ECHO(const bye_printable bye{}; any_printable ap = bye);

    ECHO(const hi_printable hi{}; any_printable tmp = hi; any_printable ap = tmp);
    ECHO(const large_printable large{}; any_printable tmp = large; any_printable ap = tmp);
    ECHO(const bye_printable bye{}; any_printable tmp = bye; any_printable ap = tmp);

    // Does not compile!
    UNSUPPORTED(const hi_printable hi{}; any_printable ap; ap = hi);
    UNSUPPORTED(const large_printable large{}; any_printable ap; ap = large);
    UNSUPPORTED(const bye_printable bye{}; any_printable ap; ap = bye);

#undef UNSUPPORTED

    ECHO(any_printable ap(hi_printable{}));
    ECHO(any_printable ap(large_printable{}));
    ECHO(any_printable ap(bye_printable{}));

    ECHO(any_printable ap = hi_printable{});
    ECHO(any_printable ap = large_printable{});
    ECHO(any_printable ap = bye_printable{});

#undef ECHO

#define ECHO(expr)                                                      \
    do {                                                                \
        std::cout << #expr << ";\nap.print() = ";                       \
        expr;                                                           \
        std::cout << "allocations: " << allocations() << "\n\n";        \
        reset_allocations();                                            \
    } while (false)

    ECHO(hi_printable hi; print_printable(hi));
    ECHO(large_printable hi; print_printable(hi));
    ECHO(bye_printable hi; print_printable(hi));

#undef ECHO
}

BOOST_AUTO_TEST_CASE(boost_type_erasure_vector)
{
    std::cout << "copied vector<any_printable>{hi_printable, large_printable}" << "\n";

    std::vector<any_printable> several_printables = {
        hi_printable{},
        large_printable{}
    };

    for (auto & printable : several_printables) {
        printable.print();
    }

    std::vector<any_printable> several_printables_copy = several_printables;

    std::cout << "allocations: " << allocations() << "\n\n";
    reset_allocations();
}

BOOST_AUTO_TEST_CASE(boost_type_erasure_vector_copy_on_write)
{
    std::cout << "copied vector<COW<any_printable>>{hi_printable, large_printable}" << "\n";

    std::vector<copy_on_write<any_printable>> several_printables = {
        {hi_printable{}},
        {large_printable{}}
    };

    for (auto & printable : several_printables) {
        const_cast<any_printable &>(*printable).print();
    }

    std::vector<copy_on_write<any_printable>> several_printables_copy = several_printables;

    std::cout << "allocations: " << allocations() << "\n\n";
    reset_allocations();
}
