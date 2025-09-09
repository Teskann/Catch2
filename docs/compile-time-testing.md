<a id="top"></a>
# Compile time testing

Catch2 provides a way to test code at compile time. This is
useful for ensuring the code does not lead to undefined behavior.

> [!IMPORTANT]
> This feature is only available with **C++17** (limited support) or
> **C++20** and above (full support).

The usage is pretty similar to testing at runtime:

| Runtime macro   | Compile time macro           |
|-----------------|------------------------------|
| `SECTION`       | `COMPILE_TIME_SECTION`       |
| `REQUIRE`       | `COMPILE_TIME_REQUIRE`       |
| `REQUIRE_FALSE` | `COMPILE_TIME_REQUIRE_FALSE` |

```c++
TEST_CASE("My compile time test")
{
    COMPILE_TIME_SECTION("Use it the same way as SECTION !")
    {
        std::vector<int> v = {1, 2, 3, 4, 5};
        COMPILE_TIME_REQUIRE( v.size() == 5 );
        COMPILE_TIME_REQUIRE( v[2] == 3 );
    
    }; // <-- Don't forget the semicolon here!
}
```

If you have **C++20**, you can use nested compile time sections:

```C++
TEST_CASE("My other compile time test")
{
    COMPILE_TIME_SECTION("Root section")
    {
        std::vector<int> v = {1, 2, 3, 4, 5};
        COMPILE_TIME_REQUIRE( v.size() == 5 );
        COMPILE_TIME_REQUIRE( v[2] == 3 );
        
        constexpr int x = 10;
    
        COMPILE_TIME_SECTION("Nested section")
        {
            int y = x + 1;  // <- constexpr variables from outside scope are visible
            COMPILE_TIME_REQUIRE( y == 11 );
        };
    };
}
```

All code inside the `COMPILE_TIME_SECTION` will be evaluated at compile time.
If any of the `COMPILE_TIME_REQUIRE` or `COMPILE_TIME_REQUIRE_FALSE` fails,
the test will throw an exception, resulting in a compilation error.

> [!NOTE]
> The code inside the `COMPILE_TIME_SECTION` is never evaluated at runtime.

> ![TIP]
> It is possible to add a `COMPILE_TIME_SECTION` inside a `SECTION`, but
> it is impossible to add a `SECTION` inside a `COMPILE_TIME_SECTION`.

## Debuging compile time tests

### Investigate at compile time

You can check the output of your compiler to find the failing assertion.
The failing line should be highlighter somewhere.

### Investigate at runtime

If you want to debug the code in a `COMPILE_TIME_SECTION`, you can simply replace
`COMPILE_TIME_SECTION` with `SECTION` and the code will be evaluated at runtime
instead.

> [!CAUTION]
> Using `COMPILE_TIME_REQUIRE` or `COMPILE_TIME_REQUIRE_FALSE` inside a
> `SECTION` will result in a non-fatal test failure **since C++20**. This is to
> to ensure that the code is evaluated at compile time, and make sure
> you didn't forget to revert your `SECTION` to a `COMPILE_TIME_SECTION`
> after debugging.
> **This feature is not supported before C++20**

## Compile time test report

**As of C++20**, running a `COMPILE_TIME_SECTION` will add
sections in the test report. These sections run nothing, but they
provide a way to see what was tested at compile time in the test report.

For instance, given the following case:
```C++
TEST_CASE("My other compile time test")
{
    SECTION("Any runtime section")
    {
        COMPILE_TIME_SECTION("First section")
        {
            COMPILE_TIME_SECTION("Nested section 1")
            {
                // ...
            };
        };
        
        COMPILE_TIME_SECTION("Second section")
        {
            // ...
        };
        COMPILE_TIME_SECTION("Third section")
        {
            COMPILE_TIME_SECTION("Nested section 2")
            {
                // ...
            };
        };            
    }
    
    SECTION("Another runtime section")
    {
        SECTION("Nested runtime section")
        {
            // ...
        }
    }    
}
```

The report would look like this:

- ✅ My other compile time test
  - ✅ Any runtime section
    - ✅ First Section [Passed at compile time]
    - ✅ Second section [Passed at compile time]
    - ✅ Third section [Passed at compile time]
  - ✅ Another runtime section
    - ✅ Nested runtime section

> [!NOTE]
> Nested compile time sections are not shown in the report