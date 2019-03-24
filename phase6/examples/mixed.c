/* math.c */

int main(void)
{
	int a, b, c, x, y;
	double d, e, z;

    x = 100;
    y = 30;
    z = 2;

    a = x + y + z;
    b = x - y - z;
    c = x * y * z;
    d = x / y + z;
    e = x % y - z;

    printf("%d\n", a);
    printf("%d\n", b);
    printf("%d\n", c);
    printf("%f\n", d);
    printf("%f\n", e);
}
