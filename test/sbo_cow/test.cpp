#include <gtest/gtest.h>

#include "interface.hh"
#include "../mock_fooable.hh"

namespace
{
    using SBOCOW::Fooable;
    using Mock::MockFooable;
    using Mock::MockLargeFooable;

    void death_tests( Fooable& fooable )
    {
#ifndef NDEBUG
        EXPECT_DEATH( fooable.foo(), "" );
        EXPECT_DEATH( fooable.set_value( Mock::other_value ), "" );
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


TEST( TestSBOCOWFooable, Empty )
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


TEST( TestSBOCOWFooable, CopyFromValue_SmallObject )
{
    MockFooable mock_fooable;
    auto value = mock_fooable.foo();
    Fooable fooable( mock_fooable );

    test_interface( fooable, value, Mock::other_value );
}

TEST( TestSBOCOWFooable, CopyFromValue_LargeObject )
{
    MockLargeFooable mock_fooable;
    auto value = mock_fooable.foo();
    Fooable fooable( mock_fooable );

    test_interface( fooable, value, Mock::other_value );
}


TEST( TestSBOCOWFooable, CopyConstruction_SmallObject )
{
    Fooable fooable = MockFooable();
    Fooable other( fooable );
    test_copies( other, fooable, Mock::other_value );
}

TEST( TestSBOCOWFooable, CopyConstruction_LargeObject )
{
    Fooable fooable = MockLargeFooable();
    Fooable other( fooable );
    test_copies( other, fooable, Mock::other_value );
}


TEST( TestSBOCOWFooable, CopyFromValueWithReferenceWrapper_SmallObject )
{
    MockFooable mock_fooable;
    Fooable fooable( std::ref(mock_fooable) );

    test_ref_interface( fooable, mock_fooable, Mock::other_value );
}


TEST( TestSBOCOWFooable, CopyFromValueWithReferenceWrapper_LargeObject )
{
    MockLargeFooable mock_fooable;
    Fooable fooable( std::ref(mock_fooable) );

    test_ref_interface( fooable, mock_fooable, Mock::other_value );
}


TEST( TestSBOCOWFooable, MoveFromValue_SmallObject )
{
    MockFooable mock_fooable;
    auto value = mock_fooable.foo();
    Fooable fooable( std::move(mock_fooable) );

    test_interface( fooable, value, Mock::other_value );
}
TEST( TestSBOCOWFooable, MoveFromValue_LargeObject )
{
    MockLargeFooable mock_fooable;
    auto value = mock_fooable.foo();
    Fooable fooable( std::move(mock_fooable) );

    test_interface( fooable, value, Mock::other_value );
}


TEST( TestSBOCOWFooable, MoveConstruction_SmallObject )
{
    Fooable fooable = MockFooable();
    auto value = fooable.foo();
    Fooable other( std::move(fooable) );

    test_interface( other, value, Mock::other_value );
    death_tests(fooable);
}

TEST( TestSBOCOWFooable, MoveConstruction_LargeObject )
{
    Fooable fooable = MockLargeFooable();
    auto value = fooable.foo();
    Fooable other( std::move(fooable) );

    test_interface( other, value, Mock::other_value );
    death_tests(fooable);
}


TEST( TestSBOCOWFooable, MoveFromValueWithReferenceWrapper_SmallObject )
{
    MockFooable mock_fooable;
    Fooable fooable( std::move(std::ref(mock_fooable)) );

    test_ref_interface( fooable, mock_fooable, Mock::other_value );
}

TEST( TestSBOCOWFooable, MoveFromValueWithReferenceWrapper_LargeObject )
{
    MockLargeFooable mock_fooable;
    Fooable fooable( std::move(std::ref(mock_fooable)) );

    test_ref_interface( fooable, mock_fooable, Mock::other_value );
}


TEST( TestSBOCOWFooable, CopyAssignFromValue_SmallObject )
{
    MockFooable mock_fooable;
    Fooable fooable;

    auto value = mock_fooable.foo();
    fooable = mock_fooable;
    test_interface(fooable, value, Mock::other_value);
}

TEST( TestSBOCOWFooable, CopyAssignFromValue_LargeObject )
{
    MockLargeFooable mock_fooable;
    Fooable fooable;

    auto value = mock_fooable.foo();
    fooable = mock_fooable;
    test_interface(fooable, value, Mock::other_value);
}


TEST( TestSBOCOWFooable, CopyAssignment_SmallObject )
{
    Fooable fooable = MockFooable();
    Fooable other;
    other = fooable;
    test_copies( other, fooable, Mock::other_value );
}

TEST( TestSBOCOWFooable, CopyAssignment_LargeObject )
{
    Fooable fooable = MockLargeFooable();
    Fooable other;
    other = fooable;
    test_copies( other, fooable, Mock::other_value );
}


TEST( TestSBOCOWFooable, CopyAssignFromValueWithReferenceWrapper_SmallObject )
{
    MockFooable mock_fooable;
    Fooable fooable;

    fooable = std::ref(mock_fooable);
    test_ref_interface( fooable, mock_fooable, Mock::other_value );
}

TEST( TestSBOCOWFooable, CopyAssignFromValueWithReferenceWrapper_LargeObject )
{
    MockLargeFooable mock_fooable;
    Fooable fooable;

    fooable = std::ref(mock_fooable);
    test_ref_interface( fooable, mock_fooable, Mock::other_value );
}


TEST( TestSBOCOWFooable, MoveAssignFromValue_SmallObject )
{
    MockFooable mock_fooable;
    Fooable fooable;

    auto value = mock_fooable.foo();
    fooable = std::move(mock_fooable);
    test_interface(fooable, value, Mock::other_value);
}

TEST( TestSBOCOWFooable, MoveAssignFromValue_LargeObject )
{
    MockLargeFooable mock_fooable;
    Fooable fooable;

    auto value = mock_fooable.foo();
    fooable = std::move(mock_fooable);
    test_interface(fooable, value, Mock::other_value);
}


TEST( TestSBOCOWFooable, MoveAssignment_SmallObject )
{
    Fooable fooable = MockFooable();
    auto value = fooable.foo();
    Fooable other;
    other = std::move(fooable);

    test_interface( other, value, Mock::other_value );
    death_tests(fooable);
}

TEST( TestSBOCOWFooable, MoveAssignment_LargeObject )
{
    Fooable fooable = MockLargeFooable();
    auto value = fooable.foo();
    Fooable other;
    other = std::move(fooable);

    test_interface( other, value, Mock::other_value );
    death_tests(fooable);
}


TEST( TestSBOCOWFooable, MoveAssignFromValueWithReferenceWrapper_SmallObject )
{
    MockFooable mock_fooable;
    Fooable fooable;

    fooable = std::move(std::ref(mock_fooable));
    test_ref_interface( fooable, mock_fooable, Mock::other_value );
}

TEST( TestSBOCOWFooable, MoveAssignFromValueWithReferenceWrapper_LargeObject )
{
    MockLargeFooable mock_fooable;
    Fooable fooable;

    fooable = std::move(std::ref(mock_fooable));
    test_ref_interface( fooable, mock_fooable, Mock::other_value );
}


TEST( TestSBOCOWFooable, Cast_SmallObject )
{
    Fooable fooable = MockFooable();

    EXPECT_TRUE( fooable.cast<int>() == nullptr );
    ASSERT_FALSE( fooable.cast<MockFooable>() == nullptr );

    fooable.set_value(Mock::other_value);
    EXPECT_EQ( fooable.cast<MockFooable>()->foo(), Mock::other_value );
}

TEST( TestSBOCOWFooable, Cast_LargeObject )
{
    Fooable fooable = MockLargeFooable();

    EXPECT_TRUE( fooable.cast<int>() == nullptr );
    ASSERT_FALSE( fooable.cast<MockLargeFooable>() == nullptr );

    fooable.set_value(Mock::other_value);
    EXPECT_EQ( fooable.cast<MockLargeFooable>()->foo(), Mock::other_value );
}


TEST( TestSBOCOWFooable, ConstCast_SmallObject )
{
    const Fooable fooable = MockFooable();

    EXPECT_TRUE( fooable.cast<int>() == nullptr );
    ASSERT_FALSE( fooable.cast<MockFooable>() == nullptr );

    EXPECT_EQ( fooable.cast<MockFooable>()->foo(), Mock::value );
}

TEST( TestSBOCOWFooable, ConstCast_LargeObject )
{
    const Fooable fooable = MockLargeFooable();

    EXPECT_TRUE( fooable.cast<int>() == nullptr );
    ASSERT_FALSE( fooable.cast<MockLargeFooable>() == nullptr );

    EXPECT_EQ( fooable.cast<MockLargeFooable>()->foo(), Mock::value );
}

