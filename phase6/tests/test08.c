int main(void)
{
    int x, y, z;
    char a, b, c;

    x = 10;
    y = 22;
    z = 34;

    a = x < y;
    b = x > y;
    c = x == y;
    printf("%d %d %d\n", !a, !b, !c);

    x = y / 7;
    z = z * y;
    z = z % (x + 1);
    printf("%d\n", z);
}
