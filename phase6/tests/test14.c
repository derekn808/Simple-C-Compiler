int putchar(int x);

int main(void)
{
    char i;
    int j;

    i = 65;
    j = 70;

    while (i < j) {
	putchar(i);
	i = i + 1;
    }

    putchar(10);
}
