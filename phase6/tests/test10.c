int putchar(int x);

int main(void)
{
    int *p, x;

    x = 65;
    p = &x;
    putchar(x);
    putchar(*p);
    *p = *p + 32;
    putchar(x);
    putchar(*p);
    putchar(10);
}
