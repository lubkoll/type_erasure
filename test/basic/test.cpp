#include <gtest/gtest.h>

#include "interface.hh"
#define INSTRUMENT_COPIES
#include "../util.hh"

namespace
{
    const int mock_value = 42;
    const int other_mock_value = 73;

    struct MockFooable
    {
        int foo() const
        {
            return value_;
        }

        void set_value(int value)
        {
            value_ = value;
        }

    private:
        int value_ = mock_value;
    };


    void death_tests( Fooable& fooable )
    {
#ifndef NDEBUG
        EXPECT_DEATH( fooable.foo(), "" );
        EXPECT_DEATH( fooable.set_value( other_mock_value ), "" );
#endif
    }

    void test_interface( Fooable& fooable, int initial_value, int new_value )
    {
        EXPECT_EQ( fooable.foo(), initial_value );
        fooable.set_value( new_value );
        EXPECT_EQ( fooable.foo(), new_value );
    }

    void test_ref_interface( Fooable& fooable, const MockFooable& mock_fooable,
                             int new_value )
    {
        test_interface(fooable, mock_fooable.foo(), new_value);
        EXPECT_EQ( mock_fooable.foo(), new_value );
    }

    void test_copies( Fooable& copy, const Fooable& fooable, int new_value )
    {
        auto value = fooable.foo();
        test_interface( copy, value, new_value );
        EXPECT_EQ( fooable.foo(), value );
        ASSERT_NE( value, new_value );
        EXPECT_NE( fooable.foo(), copy.foo() );
    }
}

TEST( TestFooable, Empty )
{
    Fooable fooable;
    death_tests(fooable);

    Fooable copy(fooable);
    death_tests(copy);

    Fooable move( std::move(fooable) );
    death_tests(move);

    Fooable copy_assign;
    copy_assign = move;
    death_tests(copy_assign);

    Fooable move_assign;
    move_assign = std::move(fooable);
    death_tests(move_assign);
}

TEST( TestFooable, CopyFromValue )
{
    MockFooable mock_fooable;
    auto value = mock_fooable.foo();
    Fooable fooable( mock_fooable );

    test_interface( fooable, value, other_mock_value );
}

TEST( TestFooable, CopyConstruction )
{
    Fooable fooable = MockFooable();
    Fooable other( fooable );
    test_copies( other, fooable, other_mock_value );
}

TEST( TestFooable, CopyFromValueWithReferenceWrapper )
{
    MockFooable mock_fooable;
    Fooable fooable( std::ref(mock_fooable) );

    test_ref_interface( fooable, mock_fooable, other_mock_value );
}

TEST( TestFooable, MoveFromValue )
{
    MockFooable mock_fooable;
    auto value = mock_fooable.foo();
    Fooable fooable( std::move(mock_fooable) );

    test_interface( fooable, value, other_mock_value );
}

TEST( TestFooable, MoveConstruction )
{
    Fooable fooable = MockFooable();
    auto value = fooable.foo();
    Fooable other( std::move(fooable) );

    test_interface( other, value, other_mock_value );
    death_tests(fooable);
}

TEST( TestFooable, MoveFromValueWithReferenceWrapper )
{
    auto expected_heap_allocations = 1u;

    MockFooable mock_fooable;
    CHECK_HEAP_ALLOC( Fooable fooable( std::move(std::ref(mock_fooable)) ),
                      expected_heap_allocations );

    test_ref_interface( fooable, mock_fooable, other_mock_value );
}

TEST( TestFooable, CopyAssignFromValue )
{
    MockFooable mock_fooable;
    Fooable fooable;

    death_tests(fooable);

    auto value = mock_fooable.foo();
    fooable = mock_fooable;
    test_interface(fooable, value, other_mock_value);
}

TEST( TestFooable, CopyAssignment )
{
    Fooable fooable = MockFooable();
    Fooable other;
    other = fooable;
    test_copies( other, fooable, other_mock_value );
}

TEST( TestFooable, CopyAssignFromValueWithReferenceWrapper )
{
    MockFooable mock_fooable;
    Fooable fooable;

    death_tests(fooable);

    fooable = std::ref(mock_fooable);
    test_ref_interface( fooable, mock_fooable, other_mock_value );
}

TEST( TestFooable, MoveAssignFromValue )
{
    MockFooable mock_fooable;
    Fooable fooable;

    death_tests(fooable);

    auto value = mock_fooable.foo();
    fooable = std::move(mock_fooable);
    test_interface(fooable, value, other_mock_value);
}

TEST( TestFooable, MoveAssignment )
{
    Fooable fooable = MockFooable();
    auto value = fooable.foo();
    Fooable other;
    other = std::move(fooable);

    test_interface( other, value, other_mock_value );
    death_tests(fooable);
}

TEST( TestFooable, MoveAssignFromValueWithReferenceWrapper )
{
    MockFooable mock_fooable;
    Fooable fooable;

    death_tests(fooable);

    fooable = std::move(std::ref(mock_fooable));
    test_ref_interface( fooable, mock_fooable, other_mock_value );
}


TEST( TestFooable, HeapAllocations_Empty )
{
    auto expected_heap_allocations = 0u;

    CHECK_HEAP_ALLOC( Fooable fooable,
                      expected_heap_allocations );

    CHECK_HEAP_ALLOC( Fooable copy(fooable),
                      expected_heap_allocations );

    CHECK_HEAP_ALLOC( Fooable move( std::move(fooable) ),
                      expected_heap_allocations );

    CHECK_HEAP_ALLOC( Fooable copy_assign;
                      copy_assign = move,
                      expected_heap_allocations );

    CHECK_HEAP_ALLOC( Fooable move_assign;
                      move_assign = std::move(fooable),
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_CopyFromValue )
{
    auto expected_heap_allocations = 1u;

    MockFooable mock_fooable;
    CHECK_HEAP_ALLOC( Fooable fooable( mock_fooable ),
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_CopyConstruction )
{
    auto expected_heap_allocations = 1u;

    Fooable fooable = MockFooable();
    CHECK_HEAP_ALLOC( Fooable other( fooable );
                      test_copies( other, fooable, other_mock_value ),
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_CopyFromValueWithReferenceWrapper )
{
    auto expected_heap_allocations = 1u;

    MockFooable mock_fooable;
    CHECK_HEAP_ALLOC( Fooable fooable( std::ref(mock_fooable) ),
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_MoveFromValue )
{
    auto expected_heap_allocations = 1u;

    MockFooable mock_fooable;
    CHECK_HEAP_ALLOC( Fooable fooable( std::move(mock_fooable) ),
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_MoveConstruction )
{
    auto expected_heap_allocations = 0u;

    Fooable fooable = MockFooable();
    CHECK_HEAP_ALLOC( Fooable other( std::move(fooable) ),
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_MoveFromValueWithReferenceWrapper )
{
    auto expected_heap_allocations = 1u;

    MockFooable mock_fooable;
    CHECK_HEAP_ALLOC( Fooable fooable( std::move(std::ref(mock_fooable)) ),
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_CopyAssignFromValue )
{
    auto expected_heap_allocations = 1u;

    MockFooable mock_fooable;
    CHECK_HEAP_ALLOC( Fooable fooable;
                      fooable = mock_fooable,
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_CopyAssignment )
{
    auto expected_heap_allocations = 1u;

    Fooable fooable = MockFooable();
    CHECK_HEAP_ALLOC( Fooable other;
                      other = fooable;
                      test_copies( other, fooable, other_mock_value ),
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_CopyAssignFromValuenWithReferenceWrapper )
{
    auto expected_heap_allocations = 1u;

    MockFooable mock_fooable;
    CHECK_HEAP_ALLOC( Fooable fooable;
                      fooable = std::ref(mock_fooable),
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_MoveAssignFromValue )
{
    auto expected_heap_allocations = 1u;

    MockFooable mock_fooable;
    CHECK_HEAP_ALLOC( Fooable fooable;
                      fooable = std::move(mock_fooable),
                      expected_heap_allocations );
}

TEST( TestFooable, HeapAllocations_MoveAssignment )
{
    auto expected_heap_allocations = 0u;

    Fooable fooable = MockFooable();
    CHECK_HEAP_ALLOC( Fooable other;
                      other = std::move(fooable),
                      expected_heap_allocations );
}


TEST( TestFooable, HeapAllocations_MoveAssignFromValueWithReferenceWrapper )
{
    auto expected_heap_allocations = 1u;

    MockFooable mock_fooable;
    CHECK_HEAP_ALLOC( Fooable fooable;
                      fooable = std::move(std::ref(mock_fooable)),
                      expected_heap_allocations );
}
