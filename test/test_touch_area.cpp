#include <unity.h>
#include <TouchArea.h>

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_touch_area_contains()
{
    TouchArea area = {10, 10, 100, 50, "Test", "/test.jpg"};
    TEST_ASSERT_TRUE(area.contains(20, 30));
    TEST_ASSERT_FALSE(area.contains(200, 300));
}

void test_find_touched_area_index()
{
    TouchArea list[2] = {
        {0, 0, 100, 100, "One", "/one.jpg"},
        {100, 100, 100, 100, "Two", "/two.jpg"}};

    TEST_ASSERT_EQUAL(0, TouchArea::findIndex(50, 50, list, 2));
    TEST_ASSERT_EQUAL(1, TouchArea::findIndex(150, 150, list, 2));
    TEST_ASSERT_EQUAL(-1, TouchArea::findIndex(300, 300, list, 2));
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_touch_area_contains);
    RUN_TEST(test_find_touched_area_index);
    return UNITY_END();
}

/**
 * For native dev-platform or for some embedded frameworks
 */
int main(void)
{
    return runUnityTests();
}
