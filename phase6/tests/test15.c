int main(void)
{
    double a, b, c;

    a = 1.1e1;
    b = 2.213;
    c = 3.321;

    printf("%g\n", a + b);
    printf("%g\n", a * b);
    printf("%g\n", c - b);

    c = c / b + a * 3.14159;

    printf("%d\n", a > b);
    printf("%d\n", a == b);
    printf("%d\n", c <= b);

    printf("%d\n", a < b);
    printf("%d\n", a != b);
    printf("%d\n", c >= b);
}
